/******************************************************************************/
/**  COPYRIGHT (C) 2005- Opulan, INC. ALL RIGHTS RESERVED.                   **/
/**--------------------------------------------------------------------------**/
/** This software embodies materials and concepts which are proprietary and  **/
/** confidential to Opulan, Inc.                                             **/
/**--------------------------------------------------------------------------**/
/**  Module name: ipmux_fe.c                                                                        **/
/******************************************************************************/
#include <config.h>
#include <common.h>
#include <malloc.h>
#include <net.h>
#include <command.h>
#include <asm/io.h>
#include <configs/opipmux.h>

#if defined(CONFIG_IPMUX_2)

#include "ipmux_2_dmac.h"

/*************** for debug ***************/
/*
#define DEBUGDMA 1
#define DEBUGDMA_DETAIL 1
#define DEBUGDMA_DETAIL_RX 1
#define DEBUGDMA 1
#define DEBUGDMA_DETAIL 1
#define DEBUGDMA_DETAIL_RX 1
#define DEBUGDMA_DETAIL_TX 1
#define DEBUGDMA_DETAIL_BD_TX 1
#define DEBUGDMA_DETAIL_BD_RX 1
*/

/*  write bd table until writing success */
/*#define BD_WRITE_UNTIL_OK 1*/

extern UINT16 debug_rx ; /*0:no debug; 1:print tcp,icmp 2:print simple info; 3:print mem of all packet header; 4:tftpb */
extern UINT16 debug_tx ;

/* gel dma : 64 rx fifos/64 tx fifos  */
/*******************************
        buffer arrange
        |    |   rx buffer0
                .
                .
                .
        |    |   rx buffern
        |    |   tx buffer0
                .
                .
                .
        |    |   tx buffern
********************************/

//#define DMAC_DEBUG
#ifdef DMAC_DEBUG
#define dbg_dma_printf printf
#else
#define dbg_dma_printf 
#endif

#define DMABUF_NUM_RX  64           /* wwei mark */
#define DMABUF_NUM_TX  64           /* wwei mark */

#define DMABUF_LEN  2048            /* wwei mark */

#define DMABUF_TX_OFFSET   (DMABUF_NUM_RX*DMABUF_LEN)

/* bd table format */
#define MAX_WAIT_USEC 0x2000

#define BD_VALID_BIT    0x00001000      /* zw mark */ /* valid bit mask in BD */
#define BD_VALID_SHIFT  12              /* zw mark */ /* valid bit shift in BD */
#define BD_END_BIT      0x00000800      /* zw mark */ /* end flag mask in BD */
#define BD_END_SHIFT    28              /* wwei mark */ /* end flag shift in BD */
#define BD_BLOCKID_BIT  0x000007FF      /* wwei mark */ /* buffer address(block id) mask in BD */

#define BD_LENGTH_BIT   0x7FF      /* zw mark */ /* data length mask in BD */
#define BD_LENGTH_SHIFT 17              /* wwei mark */
#define PCK_MAXLEN_MASK BD_LENGTH_BIT
UINT16 bd_id_rx=0;
UINT16 bd_id_tx=0;



/* indirect ctrl */
#define BDTBL_ID_SHIFT 16                 

#define IND_ACC_TYPE_WRITE  0x1<<30         
#define IND_ACC_TYPE_READ   0x0 <<30         
#define IND_ACC_BUSY        0x1<<31         

#define BDTBL_INDEX    0<<BDTBL_ID_SHIFT  

/*  #define BUF_TO_ZERO 1   */
#ifdef BUF_TO_ZERO
    #define KSEG0_TO_KSEG1(x) (x+0xa0000000)
UINT8** Buffers;
#else
    #define KSEG0_TO_KSEG1(x) ((x)>=0xa0000000?(x):((x)+0x20000000))
volatile UINT32 Buffers[DMABUF_NUM_RX+DMABUF_NUM_TX][DMABUF_LEN/4];    /* 128 buffer, each 2048 byte */
#endif

#define BUF_ADDR(bd_idx) (KSEG0_TO_KSEG1((UINT32)Buffers + (bd_idx)*DMABUF_LEN))
//#define RX_BUF_ADDR(bd_idx) (KSEG0_TO_KSEG1((UINT32)Buffers + (bd_idx)*DMABUF_LEN))
//#define TX_BUF_ADDR(bd_idx) (KSEG0_TO_KSEG1((UINT32)Buffers + DMABUF_TX_OFFSET+ (bd_idx)*DMABUF_LEN ))
#if 0
#define RX_BUF_BLOCKID(bd_idx) (bd_idx)
#define TX_BUF_BLOCKID(bd_idx) (bd_idx+DMABUF_NUM_RX)
#endif 

#define BUF_BLOCKID(bd_idx) (bd_idx)  /*zw*/

/* begin added by jiangmingli for atheros switch */
#ifdef CONFIG_IPMUX_ATHEROS_PKT_HEADTAG
volatile UINT8 RxBufTmp[DMABUF_LEN];    /* each 2048 byte */

PKT_HEADER_ATHEROS_s g_stAtherosHeader;
UINT8 aucBroadcastMac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
/* begin modified by jiangmingli for new atheros packet head struct */ 
UINT8 g_ucFePortid = 0x1e;
/* end modified by jiangmingli for new atheros packet head struct */
#endif
/* end added by jiangmingli for atheros switch */


/* prototype declare */
//static UINT32 read_bd_rx(UINT16 bd_index);
static void  read_bd(tbl_acc_data  * p_bd_data,UINT16 bd_index);
static UINT16 find_valid_rx(void);
static void print_config(void);
static void test_gel_sendpack(void);
static void unable_dma_fe(void);
static void init_dma_fe(void);
static void print_bd_data(UINT32 bd_data, UINT16 bd_index);
//static void init_bd_rx(void);
static void init_bd(void);
static void inc_bdid_tx(void);
static void inc_bdid_rx(void);
#ifdef DEBUGDMA
void print_bd_current_rx(void);
void testBD(void);
void testgel(void);
void print_net(void);
#endif

void print_bd_one_rx(UINT16 bd_index,int check_valid);
void ntoh_range(UINT32 *addr,UINT32 len);
void test_gel_debug(void);
void opipmux_gel_reset(void);
void opipmux_gel_tx_loopbackto_rx(void);
/*void opipmux_gel_set_loopback(void);*/
int opipmux_gel_init(void);
int debug_fe_loopback(void);
int opipmux_gel_initialize(bd_t *bis);

extern ulong get_timer_usec(ulong base);
extern void debug_pck_send(UINT8 *pck,int len,UINT16 bd_id);
extern void debug_pck_Rcv(unsigned char *pck,int len,unsigned short bd_id);

/* code */
#ifdef DEBUGDMA
UINT32 get_rx_buf_addr(UINT16 bd_index)
{
    return(UINT32)(RX_BUF_ADDR(bd_index));
}
#endif

#ifdef BD_WRITE_UNTIL_OK
static void write_bd_rx_check(UINT32 data,UINT16 bd_index)
{

    static int rewrite_bd_num=0;
    UINT32 bd_data=0;

    rewrite_bd_num++;
    bd_data=read_bd(bd_index);

    while (data!=bd_data) {
        rewrite_bd_num++;
        write_bd_rx(data, bd_index);
        bd_data=read_bd_rx(bd_index);
    }

    if (rewrite_bd_num>=3) {
        printf(" \n***rw rx %d %d times!\n",bd_index,rewrite_bd_num);
    }
    rewrite_bd_num=0;

}
#endif

/**************************************************************************
*
*  bd_one_rx_valid
*
* DESCRIPTION:
*      get the VALID bit of the bd data.
*
* INPUT:
*      bd_index  - the bd index
*
* OUTPUT:
*      null
*
* RETURNS:
*      the VALID bit of the bd data.
*
* SEE ALSO:
*/
static UINT32 bd_one_rx_valid(UINT16 bd_index)  /*zw*/
{
    tbl_acc_data bd_data;

    read_bd(&bd_data,bd_index);

    return(((&bd_data)->acc_data1)& BD_VALID_BIT);
}

/**************************************************************************
*
*  find_valid_rx
*
* DESCRIPTION:
*      get the next valid bd.
*
* INPUT:
*      null
*
* OUTPUT:
*      null
*
* RETURNS:
*      the index of the next valid bd.
*
* SEE ALSO:
*/

static UINT16 find_valid_rx(void)   /*zw*/
{

    UINT32 i ;
    UINT16 old_id_rx;
    old_id_rx=bd_id_rx;
    for (i=bd_id_rx;i<(DMABUF_NUM_RX+bd_id_rx);i++) {

        if (bd_one_rx_valid((UINT16)(i%DMABUF_NUM_RX))) {
            bd_id_rx=(UINT16)(i%DMABUF_NUM_RX);
#ifdef DEBUGMODE
            if (old_id_rx!=bd_id_rx) {
                printf("!!!!!!!!bd table wrong,old bd_id_r=%x  new bd_id_r=%x\n",old_id_rx,bd_id_rx);
            }
#endif
            return(1); /*find valid bd*/
        }
    }

    return(0);        /*no valid bd*/
}


#ifdef DEBUGDMA
void print_bd_current_rx(void)
{
    print_bd_one_rx(bd_id_rx,0);
}

void print_bd_one_rx(UINT16 bd_index,int check_valid)
{
    UINT32 bd_data;

    bd_data=read_bd(bd_index);

    if ( ((bd_data & BD_VALID_BIT)!=0 && check_valid) || (!check_valid) ) {
        print_bd_data(bd_data, bd_index);
    }
}
#endif

/**************************************************************************
*
*  write_bd_tx
*
* DESCRIPTION:
*      write the data to DMA buffer descripter.
*
* INPUT:
*      data     - the bd data
*      bd_index - the bd index
*
* OUTPUT:
*       null
*
* RETURNS:
*       null
*
* SEE ALSO:
*/
static void write_bd(tbl_acc_data *data,UINT16 bd_index)    /** checked **/
{
    volatile gmac_dmal_reg *p_gel_dma = (volatile gmac_dmal_reg *)OP_IPMUX2_DMA_ADDR;
    UINT32 i=0;

    if(((bd_index==DMABUF_NUM_TX-1))||(bd_index==DMABUF_NUM_TX+DMABUF_NUM_RX-1) ){      /*(bd_index==DMABUF_NUM_TX-1)||*/
        data->acc_data1=data->acc_data1 | BD_END_BIT;                                                                   /* add the end flag to data  @@@@ bit43*/
    }

    p_gel_dma->DMAC_TBL_ACC_DATA_1 = data->acc_data1 ;                               
    p_gel_dma->DMAC_TBL_ACC_DATA_0 = data->acc_data0 ;                               

    p_gel_dma->DMAC_TBL_ACC_CTRL =\
                                  IND_ACC_BUSY | IND_ACC_TYPE_WRITE | BDTBL_INDEX | bd_index;   /* zw 923 support  MSG table */

    opipmux_sync();

    i=get_timer(0);
    while (p_gel_dma->DMAC_TBL_ACC_CTRL & IND_ACC_BUSY) {
        if (get_timer_usec(i)>MAX_WAIT_USEC) {
            printf("!!!Write TX bd table timeout, CTRL_TBL 0x%x = 0x%x. \n", &(p_gel_dma->DMAC_TBL_ACC_CTRL) ,p_gel_dma->DMAC_TBL_ACC_CTRL);
            break;
        }

        udelay(1);
        i++;
    }

}


/**************************************************************************
*
*  read_bd_tx
*
* DESCRIPTION:
*      read data of the indicated bd.
*
* INPUT:
*      bd_index  - the bd index
*
* OUTPUT:
*      null
*
* RETURNS: 
*      data of the indicated bd.
*
* SEE ALSO:
*/
static void  read_bd(tbl_acc_data  * p_bd_data,UINT16 bd_index)   /*checked*/
{

      volatile gmac_dmal_reg *p_gel_dma = (volatile gmac_dmal_reg *)OP_IPMUX2_DMA_ADDR;
      UINT32 i=0;
	
      p_gel_dma->DMAC_TBL_ACC_CTRL =\
                                  IND_ACC_BUSY | IND_ACC_TYPE_READ | BDTBL_INDEX | bd_index;
      opipmux_sync();

      i=get_timer(0);
      while (p_gel_dma->DMAC_TBL_ACC_CTRL & IND_ACC_BUSY) {
        if (get_timer_usec(i)>MAX_WAIT_USEC) {
            printf("!!!READ TX bd table timeout, CTRL_TBL 0x%x = 0x%x. \n", &(p_gel_dma->DMAC_TBL_ACC_CTRL) ,p_gel_dma->DMAC_TBL_ACC_CTRL);
            break;
        }

        udelay(1);
        i++;
      }

    p_bd_data ->acc_data1= p_gel_dma->DMAC_TBL_ACC_DATA_1 ;                                
    p_bd_data ->acc_data0= p_gel_dma->DMAC_TBL_ACC_DATA_0 ;  

}


/**************************************************************************
*
*  read_bd_tx
*
* DESCRIPTION:
*      parse and print the data of the indicated bd.
*
* INPUT:
*      bd_index  - the bd index
*
* OUTPUT:
*      the parsed result of the bd data.
*
* RETURNS:
*      null.
*
* SEE ALSO:
*/

static void print_bd_data(UINT32 bd_data, UINT16 bd_index)
{
    printf("    bd %d data =0x%x, F=%d, Length=%d, blockid=%d end=%x\n",bd_index,bd_data,\
           (bd_data & BD_VALID_BIT)>>BD_VALID_SHIFT, (bd_data & BD_LENGTH_BIT)>>BD_LENGTH_SHIFT, \
           bd_data & BD_BLOCKID_BIT, (bd_data & BD_END_BIT) >> BD_END_SHIFT) ;
}

static void print_bd_one_tx(UINT16 bd_index,int check_valid)
{

}

void print_bd_rx(int printAll)
{

}

void print_bd_tx(int printAll)
{


}

#ifdef DEBUGDMA
1111
void testBD(void)
{
    UINT32 i,bd_data;

    printf("    test gel Rx bd :  write all bd i*10 -> read back ******\n");
    for (i=0;i<DMABUF_NUM_RX;i++) {
        write_bd_rx(10*i,i);

        bd_data=read_bd_rx(i);

        if ((bd_data & BD_LENGTH_BIT)!=10*i) {
            printf("    !!!err:bd %x should be  %x, now data =%x, F=%x, Length=%x\n",i, 10*i,bd_data,\
                   (bd_data & BD_VALID_BIT)>0x11,bd_data & BD_LENGTH_BIT) ;
        }

    }

    for (i=0;i<DMABUF_NUM_RX;i++) {
        bd_data=read_bd_rx(i);
        if ((bd_data & BD_LENGTH_BIT)!=10*i) {
            printf("    !!!err:bd %x data =%x, F=%x, Length=%x\n",i,bd_data,\
                   (bd_data & BD_VALID_BIT)>0x11,bd_data & BD_LENGTH_BIT) ;
        }
    }

    printf("    test gel rx bd over ******\n");


    printf("    test gel tx bd :  write all bd i*10+5 -> read back ******\n");
    //print_bd_tx();
    for (i=0;i<DMABUF_NUM_TX;i++) {
        write_bd(10*i+5,i);
    }

    for (i=0;i<DMABUF_NUM_TX;i++) {
        bd_data=read_bd(i);
        if ((bd_data & BD_LENGTH_BIT)!=10*i+5) {
            printf("    !!!err:bd %x data =%x, F=%x, Length=%x\n",i,bd_data,\
                   (bd_data & BD_VALID_BIT)>0x11,bd_data & BD_LENGTH_BIT) ;
        }
    }
    printf("    rest gel tx bd over ******\n");
    init_bd_rx();

}


void testgel(void)
{

    UINT32 i;

    printf("/****** test gel : init mac ******/");
    opipmux_gel_init();
    print_config();

    printf("------------------test for dma---------------\n");
    printf("    test gel rx bd:  write all bd i*10 -> read back ******\n");

    printf("------------------test for send data-------------\n");
    for (i=0;i<10;i++) {
        test_gel_sendpack();
        udelay(1000);
    }

    printf("------------------test for receive and send back data-------------\n");
    print_net();
}
#endif

static void init_bd(void)    /*zw 923*/
{
    UINT16 i;
    tbl_acc_data *bd_data;
    static  tbl_acc_data  BD_TMP;
    bd_data=&BD_TMP;

    for (i=0;i<DMABUF_NUM_RX;i++) {
        /* must init tx lenth to 1 */
        bd_data->acc_data0 = BUF_ADDR(i);   /*it is just a tmp addr -(UINT32)Buffers) */
	 bd_data->acc_data1 =	0x1;         /*tmp */  
        write_bd(bd_data,i);
      }
    for (i=DMABUF_NUM_RX;i<DMABUF_NUM_RX+DMABUF_NUM_TX;i++) {
        /* must init tx lenth to 1 */
        bd_data->acc_data0 = BUF_ADDR(i);   /*it is just a tmp addr  */
	 bd_data->acc_data1 =	0x1;   /*tmp */  
        write_bd(bd_data,i);
     }

}

/**************************************************************************
*
*  opipmux_gel_send
*
* DESCRIPTION:
*      send packet data to fe port.
*
* INPUT:
*      dev      - the fe device
*      packet   - the packet data pointer
*      length   - the length of the packet data
*          -
*
* OUTPUT:
*      null
*
* RETURNS:
*      0    -  send packet data failed
*      1    -  send packet data successed
* SEE ALSO:
*/
int opipmux_gel_send(struct eth_device* dev, volatile void *packet, int length)   /*zw*/
{
     int i;
     int j=0; 
     tbl_acc_data   p_bd_data;
     UINT8 *pucPkt = NULL;
     length=length+4;
     volatile gmac_dmal_reg *p_gel_dma = (volatile gmac_dmal_reg *)OP_IPMUX2_DMA_ADDR; 
     char tmp_char;
    /* begin added by jiangmingli for atheros switch */
#ifdef  CONFIG_IPMUX_ATHEROS_PKT_HEADTAG
 1111111111
        pucPkt = (char *)TX_BUF_ADDR(bd_id_tx);
        /* dmac + smac */
        memcpy(pucPkt, (char *)packet, 12);
        /* atheros header */
       if (0 == memcmp(aucBroadcastMac, (char *)packet, 6))
          {
        /* begin modified by jiangmingli for new atheros packet head struct */
        //g_stAtherosHeader.usBroadcast = 1;
            g_stAtherosHeader.usPortNum = 0x1e;
        /* end modified by jiangmingli for new atheros packet head struct */
        }
       /* begin modified by jiangmingli for new atheros packet head struct */
         else
               {
                  g_stAtherosHeader.usPortNum = 1 << g_ucFePortid;
                }
          /* end modified by jiangmingli for new atheros packet head struct */
          memcpy(pucPkt + 12, (char *)&g_stAtherosHeader, 2);
         /* begin modified by jiangmingli for new atheros packet head struct */
         //g_stAtherosHeader.usBroadcast = 0;
         /* end modified by jiangmingli for new atheros packet head struct */
         /* data */
          memcpy(pucPkt + 14, packet + 12, length - 12);
          length = length + 2;
#else
   /* wait for dma ready for send  */

         read_bd(&p_bd_data,bd_id_tx) ;

         i=get_timer(0);
         while ((&p_bd_data)->acc_data1& BD_VALID_BIT){    
	         read_bd(&p_bd_data,bd_id_tx) ;
                if (get_timer_usec(i)>MAX_WAIT_USEC) {
                     printf("!!!TX wait timeout\n");
                    return 0;
                    }
           udelay(1);
            i++;
         }
	 
   #ifdef DMAC_DEBUG
     if(((&p_bd_data)->acc_data0)!=BUF_ADDR(bd_id_tx)){
           printf("SEND$$$$$ (&p_bd_data)->acc_data1= 0x%x\n",(&p_bd_data)->acc_data1);
           printf("SEND$$$$$ (&p_bd_data)->acc_data0= 0x%x\n",(&p_bd_data)->acc_data0);
	   }
      else{
		printf("tx ok\n");      
		}
   #endif
       memcpy((char *)(BUF_ADDR(bd_id_tx)),(char *)packet,length);	   /** copy packet to the buffer in dram**/
   #ifdef DMAC_DEBUG
       dbg_dma_printf("\ndebug:     pck send ; buffer addr : 0x%x ; tx : 0x%x ; packlen : 0x%x\n",BUF_ADDR(bd_id_tx),bd_id_tx,length+16);
       print_mem(BUF_ADDR(bd_id_tx),length);
   #endif

#endif
    /* end added by jiangmingli for atheros switch */

         opipmux_sync();

       tmp_char = *(volatile char *)(BUF_ADDR(bd_id_tx) + length); /**ensure ddr controller write finish**/
//       if(0 == *(char *)BUF_ADDR(bd_id_tx)) ;
	   if (debug_tx>0)  printf("tmp_char = 0x%x\n",tmp_char);
//    ntoh_range((UINT32 *)(BUF_ADDR(bd_id_tx)),length);   /*xwang : endian */
   	   
         length = (length ) & PCK_MAXLEN_MASK;                /* max len = 2047   */
         (&p_bd_data)->acc_data1=length|BD_VALID_BIT;   /*set dma status*/

         write_bd(&p_bd_data,bd_id_tx);                        /*wrtie bd to start dma transfer*/

             /* dma will transmit the packet */
          opipmux_sync();

         /* wait for send complete   */
 
          read_bd(&p_bd_data,bd_id_tx) ;

       if(!((&p_bd_data)->acc_data1& BD_VALID_BIT)){
             dbg_dma_printf("debug:     dma send OK\n\n");
         }
		
#ifdef DMAC_DEBUG
   if(((&p_bd_data)->acc_data0)!=BUF_ADDR(bd_id_tx)){
             printf("SEND$$$$$ (&p_bd_data)->acc_data1= 0x%x\n",(&p_bd_data)->acc_data1);
             printf("SEND$$$$$ (&p_bd_data)->acc_data0= 0x%x\n",(&p_bd_data)->acc_data0);
		}
	else{
		printf("tx ok\n");      
		}
#endif   

         inc_bdid_tx();              /*after dma transfer ,increase the bd index*/

        return(1);
}

static void inc_bdid_tx(void)     /*zw*/
{
    /* transmit ok  */
    bd_id_tx++;
    if (bd_id_tx>=(DMABUF_NUM_TX+DMABUF_NUM_RX)) {    /* tx number could't beyond NO_OF_FIFOS */
        bd_id_tx=DMABUF_NUM_RX;
    }
}

static void inc_bdid_rx(void)
{
    /* receive ok   */
    bd_id_rx++;
    if (bd_id_rx>=DMABUF_NUM_RX) {                              /* tx number could't beyond NO_OF_FIFOS */
        bd_id_rx=0;
    }
}

void print_mem(char *p,UINT32 len)
{
    UINT16 i;

    printf("    ram addr = %x , data len = %x",p,len);

    for (i=0;i<len;i++) {
        if (0==i%16) {
            printf("\n  0x%02x : ",(p+i));
        }
        printf("%02x",*(UINT8 *)(p+i));
        printf(" ");
    }

    printf("\n\n");

}

int spaToBdZero(void)
{
    int i=0;
    char pck[64];
    memset(pck,0,64);
    while(bd_id_tx!=0)
    {
        opipmux_gel_send(NULL,pck, 64);
        i++;
    }
    return i;
}

int debug_fe_loopback(void)
{
    return(0);
}

#ifdef DEBUGDMA
void print_net(void)
{
    int length;
    UINT32 bd_data;

    print_config();

    printf("wait for net packet ... \n");
    for (;;) {

        if (!find_valid_rx()) {
            continue;
        }

        bd_data=read_bd_rx(bd_id_rx);
        /* read packet from dma buffer  */
        length = bd_data & PCK_MAXLEN_MASK;
        length = length >> BD_LENGTH_SHIFT;

        printf("\nget packet : bd %d data =0x%x, F=%d, Length=%d\n",\
               bd_id_rx,bd_data,(bd_data & BD_VALID_BIT)>0x11,length) ;

        /*Pass packet up to the protocol layers. This route is defined in file net.c*/
        //printf("net order byte\n");
        ntoh_range((UINT32 *)(RX_BUF_ADDR(bd_id_rx)),length);
        print_mem((char *)(RX_BUF_ADDR(bd_id_rx)),length);

        /*send this packet back*/
        /*
        printf("  0x20-0x24 to be modified\n");
        *(char *)(RX_BUF_ADDR(bd_id_rx)+0x20)=0xaa;
        *(char *)(RX_BUF_ADDR(bd_id_rx)+0x21)=0xbc;
        *(char *)(RX_BUF_ADDR(bd_id_rx)+0x22)=0xdd;
        *(char *)(RX_BUF_ADDR(bd_id_rx)+0x23)=0xee;
        *(char *)(RX_BUF_ADDR(bd_id_rx)+0x24)=bd_id_tx;
        */
        printf("    send this packet back \n");
        opipmux_gel_send(NULL,(char *)(RX_BUF_ADDR(bd_id_rx)),length-4);

        /* clear the RD */
        bd_data = bd_data & (~BD_VALID_BIT);
        write_bd_rx(bd_data, bd_id_rx);

        bd_id_rx++;

        if (bd_id_rx>=DMABUF_NUM_GEL_RX) {
            bd_id_rx=0;
        }
    }
}
#endif

void test_gel_debug(void)
{
    printf("for debug\n");
}

void ntoh_range(UINT32 *addr,UINT32 len)
{
    UINT32 i;

#ifdef BIGENDIAN
    return ;
#endif
    for (i=0;i<len;i+=4) {
        *(UINT32 *)((char *)addr+i)=ntohl(*(UINT32 *)((char *)addr+i));
    }
}


/**************************************************************************
*
*  opipmux_gel_recv
*
* DESCRIPTION:
*      receive packet data from fe port and handle to protocol level.
*
* INPUT:
*      dev      - the fe device
*
* OUTPUT:
*      null
*
* RETURNS:
*      0    -  receive packet data successed
*      -1   -  Nothing has been received
* SEE ALSO:
*/
static int opipmux_gel_recv(struct eth_device* dev){   /*zw*/

        int length;

        tbl_acc_data  p_bd_data;

        UINT8 *pucPkt = NULL;
#ifdef CONFIG_IPMUX_ATHEROS_PKT_HEADTAG
       PKT_HEADER_ATHEROS_s *pstAtherosHeader = NULL;
#endif

      if (0==find_valid_rx()){
	   return(-1);
	   }

      for (;;) 
	   {
             opipmux_sync();

             read_bd(&p_bd_data,bd_id_rx);
		
#ifdef DMAC_DEBUG		   
	 if(((&p_bd_data)->acc_data0)!=BUF_ADDR(bd_id_rx)){
             printf("RCV$$$$$ (&p_bd_data)->acc_data1= 0x%x\n",(&p_bd_data)->acc_data1);
             printf("RCV$$$$$ (&p_bd_data)->acc_data0= 0x%x\n",(&p_bd_data)->acc_data0);
		}
	   else{
		printf("rx ok\n");      
		}
#endif	
             if (!((&p_bd_data)->acc_data1 & BD_VALID_BIT)) {          
                return(-1);                            /* Nothing has been received */
               }		   
          length = ((&p_bd_data)->acc_data1 & PCK_MAXLEN_MASK) ; /*get packet lenth from rx BD*/


 
        /* begin added by jiangmingli for atheros switch */
 #ifdef CONFIG_IPMUX_ATHEROS_PKT_HEADTAG
        pucPkt = (char *)RX_BUF_ADDR(bd_id_rx);
        pstAtherosHeader = (PKT_HEADER_ATHEROS_s *)(pucPkt + 12);
        /* begin modified by jiangmingli for new atheros packet head struct */
        //g_stAtherosHeader.usPortNum = pstAtherosHeader->usPortNum;
        g_ucFePortid = pstAtherosHeader->usPortNum;
        /* end modified by jiangmingli for new atheros packet head struct */
        /* dmac + smac */
        memcpy(RxBufTmp, pucPkt, 12);
        /* data */
        memcpy(RxBufTmp + 12, pucPkt + 14, length - 4 - 14);
        NetReceive(RxBufTmp, length - 6);
#else

      #ifdef DMAC_DEBUG
	dbg_dma_printf("debug:     pck rcved ; buffer addr : 0x%x ; rx : 0x%x ; packlen : 0x%x\n",BUF_ADDR(bd_id_rx),bd_id_rx,length-4);
	print_mem((char *)BUF_ADDR(bd_id_rx), length - 4);
     #endif
   
       NetReceive((char *)BUF_ADDR(bd_id_rx), length - 4);
   
 #endif
        /* end added by jiangmingli for atheros switch */

   #ifdef DMAC_DEBUG
        printf("rx netrecv ok\n");
   #endif

        (&p_bd_data)->acc_data1= (&p_bd_data)->acc_data1& (~BD_VALID_BIT);  /*set packet rcved flag*/

       write_bd(&p_bd_data, bd_id_rx);


        inc_bdid_rx();         /*increase rc index*/

    }

#ifdef DMAC_DEBUG
 printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!rx over!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
#endif
   
    return(0);
}

/**************************************************************************
*
*  opipmux_gel_reset
*
* DESCRIPTION:
*      reset the fe port.
*
* INPUT:
*      null
*
* OUTPUT:
*      null
*
* RETURNS:
*      null
*
* SEE ALSO:
*/
void opipmux_gel_reset(void)  
{
    unable_dma_fe();
	
}

/**************************************************************************
*
*  opipmux_gel_tx_loopbackto_rx
*
* DESCRIPTION:
*      set the fe port into internal loopback.
*
* INPUT:
*      null
*
* OUTPUT:
*      null
*
* RETURNS:
*      null
*
* SEE ALSO:
*/
void opipmux_gel_tx_loopbackto_rx(void)
{


}
#if 0
void opipmux_gel_tx_loopbackto_rx(void)
{
    volatile gmac_ctrl_reg *greg = (volatile gmac_ctrl_reg *)OP_IPMUX_GEL_ADDR;
    UINT32 val;

    /* program mac config1 register leve tx and rx disabled */
    val = GMAC_CONFIG_1(    0,  /*soft_reset*/
                            0,  /*sim_reset*/
                            0,  /*reset_rx_cntl*/
                            0,  /*reset_tx_cntl*/
                            0,  /*reset_rx_func*/
                            0,  /*reset_tx_func*/
                            1,  /*loopback*/
                            1,  /*rx_flow_cntl*/
                            1,  /*tx_flow_cntl*/
                            0,  /*rx_enable*/
                            0   /*tx_enable*/
                       ) ;
    greg->mac_config1=val;

}
#endif


/**************************************************************************
*
*  opipmux_gel_set_loopback
*
* DESCRIPTION:
*      set the fe port into extern loopback.
*
* INPUT:
*      null
*
* OUTPUT:
*      null
*
* RETURNS:
*      null
*
* SEE ALSO:

void opipmux_gel_set_loopback(void)
{
    volatile UINT32 *p = (volatile UINT32 *)0xbf00c000;   

    *p=1;
}*/

#ifdef DEBUGDMA
/*
    TO BE DETERMAINED need configure three mode:
        1. Speed mode:  0: 10/100 mbs 1: 1000 mbs
        2. full duplex / half duplex
        3. Tx and Rx enable
*/
static void print_config(void)
{
    DECLARE_GLOBAL_DATA_PTR;
    //volatile gmac_ctrl_reg *greg = (volatile gmac_ctrl_reg *)OP_IPMUX_GEL_ADDR;
    volatile gmac_dmal_reg *p_gel_dma = (volatile gmac_dmal_reg *)OP_IPMUX_GEL_DMA_ADDR;

    printf("    [addr config]\n");
    printf("                IP="); print_IPaddr(gd->bd->bi_ip_addr); puts(" \t");
    printf("(in use:"); print_IPaddr(gd->bd->bi_ip_addr); puts(")\n");
    printf("        mac= %2x-%2x-%2x-%2x-%2x-%2x \t",gd->bd->bi_enetaddr[0],\
           gd->bd->bi_enetaddr[1],gd->bd->bi_enetaddr[2],gd->bd->bi_enetaddr[3],\
           gd->bd->bi_enetaddr[4],gd->bd->bi_enetaddr[5]);
    printf("(in use:%2x%2x%2x%2x%2x%2x)\n",NetOurEther[0],\
           NetOurEther[1],NetOurEther[2],NetOurEther[3],NetOurEther[4],NetOurEther[5]);

    printf("    [gmac config]\n");
    //printf("        cfg1=%x\n",greg->mac_config1);    wwei 
    //printf("        cfg2=%x\n",greg->mac_config2);    wwei 

    printf("    [dma config]\n");
    printf("        RxBaddr kseg0=%x kseg1=%x\n", p_gel_dma->BASEADDR, RX_BUF_ADDR(0));
    printf("        TxBaddr kseg1=%x\n", BUF_ADDR(0));

    printf("    [other config]\n");
    /*printf("        loopback:%x\n",*((UINT32 *)0xbf003420));       wwei mark */


}
#endif

/**************************************************************************
*
*  init_dma_fe
*
* DESCRIPTION:
*      init the dma for fe port.
*
* INPUT:
*      null
*
* OUTPUT:
*      null
*
* RETURNS:
*      null
*
* SEE ALSO:
*/
static void enable_dma(void)   /*zw*/
{
    volatile gmac_dmal_reg *p_gel_dma = (volatile gmac_dmal_reg *)OP_IPMUX2_DMA_ADDR;

   p_gel_dma->DMAC_MSG_CFG=0x1;   /*enable msg dma*/
  
  //    p_gel_dma->DMAEN=0x3;       /***interrupt ****if need ,add it*/

}

/**************************************************************************
*
*  unable_dma_fe
*
* DESCRIPTION:
*      unable the dma for fe port.
*
* INPUT:
*      null
*
* OUTPUT:
*      null
*
* RETURNS:
*      null
*
* SEE ALSO:
*/
static void unable_dma_fe(void)   /*zw 923*/
{
    volatile gmac_dmal_reg *p_gel_dma = (volatile gmac_dmal_reg *)OP_IPMUX2_DMA_ADDR;

    p_gel_dma->DMAC_MSG_CFG=0x0;      /*msg dma disable*/
    p_gel_dma->DMAI_EN_R=0x0;               /*all interupt disable*/
  
}


/**************************************************************************
*
*  opipmux_gel_init
*
* DESCRIPTION:
*      init the fe port.
*
* INPUT:
*      null
*
* OUTPUT:
*      null
*
* RETURNS:
*      1    - init the fe port successfully.
*
* SEE ALSO:
*/
int opipmux_gel_init(void)
{

    volatile gmac_dmal_reg *p_gel_dma = (volatile gmac_dmal_reg *)OP_IPMUX2_DMA_ADDR;
    UINT32 i;
     tbl_acc_data  * p_bd_data;
    /******** init dma ********/
#ifdef BUF_TO_ZERO
    Buffers=(UINT8**)0x0;
    TxBuf=(UINT8**)0x0;
#endif
    for (i = 0; i < DMABUF_NUM_RX+DMABUF_NUM_TX; i++) {
        memset((char *)BUF_ADDR(i),i,DMABUF_LEN);
    }

   p_gel_dma->BASEADDR= 0xa0000000; /* 0xa0000000;*/  /*(UINT32)Buffers;*/

   bd_id_rx=0;
   bd_id_tx=64;

    /******** init gel ********/
    unable_dma_fe();                 /*  disable DMA  */

    p_gel_dma->DMAC_INIT=0x1;               /* enable initial bd */
	  
    init_bd();                                       /*  init bd tables */
//    run_command("md 0xa1e303c4 4",0);
  *((unsigned int*)MSG_CFG_RGS) =(*(unsigned int*)MSG_CFG_RGS)|0x3;  /** :enable MSG rx and TX**/

   while(p_gel_dma->DMAC_INIT&(0x00010000)); /*wait for DMAC initial done */

  enable_dma();                              /*  enable dma   */

    /* begin added by jiangmingli for atheros switch */
#ifdef CONFIG_IPMUX_ATHEROS_PKT_HEADTAG
111111111111111
    /* begin modified by jiangmingli for new atheros packet head struct */
    g_stAtherosHeader.usVersionHi = 2;
    g_stAtherosHeader.usPriority = 3;
    g_stAtherosHeader.usPackType = 0;
    g_stAtherosHeader.usFromCpu = 1;
    g_stAtherosHeader.usPortNum = 0x1e;    
    /* end modified by jiangmingli for new atheros packet head struct */
#endif
    /* end added by jiangmingli for atheros switch */
   dbg_dma_printf("debug:     RX buffers start from:0x%x\n",BUF_ADDR(0));
   dbg_dma_printf("debug:     TX buffers start from:0x%x\n",BUF_ADDR(64));
   dbg_dma_printf("debug:     bd_id_rx= 0x%x\n",bd_id_rx);
   dbg_dma_printf("debug:     bd_id_tx= 0x%x\n",bd_id_tx);
   dbg_dma_printf("debug:     opipmux_gel_inited...\n");

    return(1);

}

static void test_gel_sendpack(void)
{
    char *pack[1000];

    memset(pack,0xEF,1000);

    opipmux_gel_send(NULL,pack,60);
}


void spa(char *p,UINT16 length)
{
    opipmux_gel_send(NULL,p,length);
}


static void opipmux_gel_halt(struct eth_device* dev){
}

int opipmux_gmac_initialize(bd_t *bis){
    return (opipmux_gel_initialize(bis));
}


int opipmux_gel_initialize(bd_t *bis){
    struct eth_device* dev;

    dev = (struct eth_device*) malloc(sizeof *dev);

    memset(dev, 0, sizeof *dev);

    sprintf(dev->name, "OPULAN FE");

    dev->iobase = 0;
    dev->priv   = 0;
    dev->init   = opipmux_gel_init;
    dev->halt   = opipmux_gel_halt;
    dev->send   = opipmux_gel_send;
    dev->recv   = opipmux_gel_recv;

    eth_register(dev);

    return(1);
}

#endif 
