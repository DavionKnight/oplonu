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

#if defined(CONFIG_IPMUX_CLT) || defined(CONFIG_IPMUX_ONU)|| defined(CONFIG_ONU_2)

#include "ipmuxclt_fe.h"

/*************** for debug ***************/
/*
//#define DEBUGDMA 1
//#define DEBUGDMA_DETAIL 1
//#define DEBUGDMA_DETAIL_RX 1
//#define DEBUGDMA 1
//#define DEBUGDMA_DETAIL 1
//#define DEBUGDMA_DETAIL_RX 1
//#define DEBUGDMA_DETAIL_TX 1
//#define DEBUGDMA_DETAIL_BD_TX 1
//#define DEBUGDMA_DETAIL_BD_RX 1
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
#define DMABUF_NUM_GEL_RX  64           /* wwei mark */
#define DMABUF_NUM_GEL_TX  64           /* wwei mark */

#define DMABUF_LEN_GEL  2048            /* wwei mark */

#define DMABUF_TX_OFFSET   (DMABUF_NUM_GEL_RX*DMABUF_LEN_GEL)

/* bd table format */
#define MAX_WAIT_USEC 0x2000

#if 0

#define BD_VALID_BIT   0x04000000      /* wwei mark */  /* valid bit mask in BD */
#define BD_VALID_SHIFT 26              /* wwei mark */  /* valid bit shift in BD */
#define BD_END_BIT     0x02000000      /* wwei mark */  /* end flag mask in BD */
#define BD_END_SHIFT   25              /* wwei mark */  /* end flag shift in BD */
#define BD_BLOCKID_BIT 0x000000FF      /* wwei mark */  /* buffer address(block id) mask in BD */

#define BD_LENGTH_BIT  0x01FFC000      /* wwei mark */ /* data length mask in BD */
#define BD_LENGTH_SHIFT 14             /* wwei mark */
#define PCK_MAXLEN_MASK BD_LENGTH_BIT
UINT16 bd_id_rx=0;
UINT16 bd_id_tx=0;

#else

#define BD_VALID_BIT    0x20000000      /* wwei mark */ /* valid bit mask in BD */
#define BD_VALID_SHIFT  29              /* wwei mark */ /* valid bit shift in BD */
#define BD_END_BIT      0x10000000      /* wwei mark */ /* end flag mask in BD */
#define BD_END_SHIFT    28              /* wwei mark */ /* end flag shift in BD */
#define BD_BLOCKID_BIT  0x000007FF      /* wwei mark */ /* buffer address(block id) mask in BD */

#define BD_LENGTH_BIT   0x0FFE0000      /* wwei mark */ /* data length mask in BD */
#define BD_LENGTH_SHIFT 17              /* wwei mark */
#define PCK_MAXLEN_MASK BD_LENGTH_BIT
UINT16 bd_id_rx=0;
UINT16 bd_id_tx=0;

#endif

/* indirect ctrl */
#define BDTABLE_ID_SHIFT 6                  /* wwei mark */

#define IND_ACC_TYPE_WRITE  0x1<<30         /* wwei mark */
#define IND_ACC_TYPE_READ   0  <<30         /* wwei mark */
#define IND_ACC_BUSY        0x1<<31         /* wwei mark */

#define TX_BDTABLE_ID 0<<BDTABLE_ID_SHIFT  /* wwei mark */
#define RX_BDTABLE_ID 0<<BDTABLE_ID_SHIFT  /* wwei mark */

/*  #define BUF_TO_ZERO 1   */
#ifdef BUF_TO_ZERO
    #define KSEG0_TO_KSEG1(x) (x+0xa0000000)
UINT8** Buffers;
#else
    #define KSEG0_TO_KSEG1(x) ((x)>=0xa0000000?(x):((x)+0x20000000))
volatile UINT32 Buffers[DMABUF_NUM_GEL_RX+DMABUF_NUM_GEL_TX][DMABUF_LEN_GEL/4];    /* 128 buffer, each 2048 byte */
#endif

#define RX_BUF_ADDR(bd_idx) (KSEG0_TO_KSEG1((UINT32)Buffers + (bd_idx)*DMABUF_LEN_GEL))
#define TX_BUF_ADDR(bd_idx) (KSEG0_TO_KSEG1((UINT32)Buffers + (bd_idx)*DMABUF_LEN_GEL + DMABUF_TX_OFFSET))

#define RX_BUF_BLOCKID(bd_idx) (bd_idx)
#define TX_BUF_BLOCKID(bd_idx) (bd_idx+DMABUF_NUM_GEL_RX)

/* begin added by jiangmingli for atheros switch */
#ifdef CONFIG_IPMUX_ONU_A_F13_BC
volatile UINT8 RxBufTmp[DMABUF_LEN_GEL];    /* each 2048 byte */
#else
#ifdef CONFIG_IPMUX_ATHEROS_PKT_HEADTAG
volatile UINT8 RxBufTmp[DMABUF_LEN_GEL];    /* each 2048 byte */

PKT_HEADER_ATHEROS_s g_stAtherosHeader;
UINT8 aucBroadcastMac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
/* begin modified by jiangmingli for new atheros packet head struct */ 
UINT8 g_ucFePortid = 0x1e;
/* end modified by jiangmingli for new atheros packet head struct */
#endif
#endif
/* end added by jiangmingli for atheros switch */

extern unsigned short g_phy_speed;

/* prototype declare */
static UINT32 read_bd_rx(UINT16 bd_index);
static UINT32 read_bd_tx(UINT16 bd_index);
static UINT16 find_valid_rx(void);
static void print_config(void);
static void test_gel_sendpack(void);
static void unable_dma_fe(void);
static void init_dma_fe(void);
static void print_bd_data(UINT32 bd_data, UINT16 bd_index);
static void init_bd_rx(void);
static void init_bd_tx(void);
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
    bd_data=read_bd_rx(bd_index);

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
*  write_bd_rx
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
static void write_bd_rx(UINT32 data,UINT16 bd_index)
{
    volatile gmac_dmal_reg *p_gel_dma = (volatile gmac_dmal_reg *)OP_IPMUX_GEL_DMA_ADDR;

    /* add the end flag to data */
    if (bd_index==DMABUF_NUM_GEL_RX-1) {
        data=data | BD_END_BIT;
    }

    #ifdef DEBUGDMA_DETAIL_BD_RX
        printf("Write shadow 0x%x to 0x%x, ctrl 0x%x to 0x%x when write rx bd\n", &p_gel_dma->SHADOW_SET_RX,data \
               ,&p_gel_dma->MEM_INDIR_ACC_CTRL_RX, IND_ACC_BUSY | IND_ACC_TYPE_WRITE | RX_BDTABLE_ID | bd_index);
    #endif

    p_gel_dma->SHADOW_SET_RX = data ;
    p_gel_dma->MEM_INDIR_ACC_CTRL_RX =\
                                  IND_ACC_BUSY | IND_ACC_TYPE_WRITE | RX_BDTABLE_ID | bd_index;

    opipmux_sync();
    while (p_gel_dma->MEM_INDIR_ACC_CTRL_RX & IND_ACC_BUSY) {
#ifdef  DEBUGDMA
        printf("Waiting write RX bd table !\n");
        udelay(1000);
#endif
    }
}

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
static UINT32 bd_one_rx_valid(UINT16 bd_index)
{
    UINT32 bd_data;

    bd_data=read_bd_rx(bd_index);

    return(bd_data & BD_VALID_BIT);
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

static UINT16 find_valid_rx(void)
{

    UINT32 i ;
    UINT16 old_id_rx;
    old_id_rx=bd_id_rx;
    for (i=bd_id_rx;i<(bd_id_rx+DMABUF_NUM_GEL_RX);i++) {
        if (bd_one_rx_valid((UINT16)(i%DMABUF_NUM_GEL_RX))) {
            bd_id_rx=(UINT16)(i%DMABUF_NUM_GEL_RX);
#ifdef DEBUGMODE
            if (old_id_rx!=bd_id_rx) {
                printf("!!!!!!!!bd table wrong,old bd_id_r=%x  new bd_id_r=%x\n",old_id_rx,bd_id_rx);
            }
#endif
            return(1); /*find valid bd*/
        }
    }

    return(0); /*no valid bd*/
}

/**************************************************************************
*
*  read_bd_rx
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
static UINT32 read_bd_rx(UINT16 bd_index)
{
    volatile gmac_dmal_reg *p_gel_dma = (volatile gmac_dmal_reg *)OP_IPMUX_GEL_DMA_ADDR;
    ulong i=0;

    #ifdef DEBUGDMA_DETAIL_BD_RX
        printf("Write ctrl 0x%x to 0x%x.\n", \
               &p_gel_dma->MEM_INDIR_ACC_CTRL_RX, IND_ACC_BUSY | IND_ACC_TYPE_READ | RX_BDTABLE_ID | bd_index);
    #endif
    p_gel_dma->MEM_INDIR_ACC_CTRL_RX =\
                                  IND_ACC_BUSY | IND_ACC_TYPE_READ | RX_BDTABLE_ID | bd_index;

    opipmux_sync();

    i=get_timer(0);
    while (p_gel_dma->MEM_INDIR_ACC_CTRL_RX & IND_ACC_BUSY) {
#ifdef  DEBUGDMA_DETAIL
        printf("waiting in read_bd_rx, bd_index=%x !\n",bd_index);
        udelay(1000);
#endif
        if (get_timer_usec(i)>MAX_WAIT_USEC) {
            printf("Read RX bd table timeout\n");
            break;
        }
        udelay(1);
        i++;
    }

    return(p_gel_dma->SHADOW_SET_RX);

}

#ifdef DEBUGDMA
void print_bd_current_rx(void)
{
    print_bd_one_rx(bd_id_rx,0);
}
#endif
void print_bd_one_rx(UINT16 bd_index,int check_valid)
{
    UINT32 bd_data;

    bd_data=read_bd_rx(bd_index);

    if ( ((bd_data & BD_VALID_BIT)!=0 && check_valid) || (!check_valid) ) {
        print_bd_data(bd_data, bd_index);
    }
}


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
static void write_bd_tx(UINT32 data,UINT16 bd_index)
{
    volatile gmac_dmal_reg *p_gel_dma = (volatile gmac_dmal_reg *)OP_IPMUX_GEL_DMA_ADDR;
    UINT32 i=0;

    /* add the end flag to data */
    if (bd_index==DMABUF_NUM_GEL_TX-1) {
        data=data | BD_END_BIT;
    }
    #ifdef DEBUGDMA_DETAIL_BD_TX
        printf("Write shadow 0x%x to 0x%x, ctrl 0x%x to 0x%x when write tx bd.\n", &p_gel_dma->SHADOW_SET_TX,data \
               ,&(p_gel_dma->MEM_INDIR_ACC_CTRL_TX), IND_ACC_BUSY | IND_ACC_TYPE_WRITE | TX_BDTABLE_ID | bd_index);
    #endif
    p_gel_dma->SHADOW_SET_TX = data ;
    p_gel_dma->MEM_INDIR_ACC_CTRL_TX =\
                                  IND_ACC_BUSY | IND_ACC_TYPE_WRITE | TX_BDTABLE_ID | bd_index;

    opipmux_sync();

    i=get_timer(0);
    while (p_gel_dma->MEM_INDIR_ACC_CTRL_TX & IND_ACC_BUSY) {

#ifdef  DEBUGDMA
        printf("waiting in write_bd_tx !\n");
#endif

        if (get_timer_usec(i)>MAX_WAIT_USEC) {
            printf("!!!Write TX bd table timeout, CTRL_TX 0x%x = 0x%x. \n", &(p_gel_dma->MEM_INDIR_ACC_CTRL_TX) ,p_gel_dma->MEM_INDIR_ACC_CTRL_TX);
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
static UINT32 read_bd_tx(UINT16 bd_index)
{
    volatile gmac_dmal_reg *p_gel_dma = (volatile gmac_dmal_reg *)OP_IPMUX_GEL_DMA_ADDR;

    #ifdef DEBUGDMA_DETAIL_BD_TX
        printf("Write ctrl 0x%x to 0x%x when read tx bd.\n", \
               &p_gel_dma->MEM_INDIR_ACC_CTRL_TX, IND_ACC_BUSY | IND_ACC_TYPE_READ | TX_BDTABLE_ID | bd_index);
    #endif
    p_gel_dma->MEM_INDIR_ACC_CTRL_TX =\
                                  IND_ACC_BUSY | IND_ACC_TYPE_READ | TX_BDTABLE_ID | bd_index;

    opipmux_sync();

    while (p_gel_dma->MEM_INDIR_ACC_CTRL_TX & IND_ACC_BUSY) {

#ifdef  DEBUGDMA
        printf("Waiting Read TX bd table !\n");
        udelay(1000);
#endif
    }

    return(p_gel_dma->SHADOW_SET_TX);
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
    UINT32 bd_data;

    bd_data=read_bd_tx(bd_index);
    if ( ((bd_data & BD_VALID_BIT)!=0 && check_valid) || (!check_valid) ) {
        print_bd_data(bd_data,bd_index);
    }

}

void print_bd_rx(int printAll)
{
    UINT32 i ;
    if (0==printAll)
        printf("   valid rx bds\n");
    else
        printf("   all rx bds\n");

    for (i=0;i<DMABUF_NUM_GEL_RX;i++) {
        print_bd_one_rx(i,1-printAll);
    }
}

void print_bd_tx(int printAll)
{
    UINT32 i ;
    if (0==printAll)
        printf("   valid tx bds\n");
    else
        printf("   all tx bds\n");

    for (i=0;i<DMABUF_NUM_GEL_TX;i++) {
        print_bd_one_tx(i,1-printAll);
    }

}

#ifdef DEBUGDMA
void testBD(void)
{
    UINT32 i,bd_data;

    printf("    test gel Rx bd :  write all bd i*10 -> read back ******\n");
    for (i=0;i<DMABUF_NUM_GEL_RX;i++) {
        write_bd_rx(10*i,i);

        bd_data=read_bd_rx(i);

        if ((bd_data & BD_LENGTH_BIT)!=10*i) {
            printf("    !!!err:bd %x should be  %x, now data =%x, F=%x, Length=%x\n",i, 10*i,bd_data,\
                   (bd_data & BD_VALID_BIT)>0x11,bd_data & BD_LENGTH_BIT) ;
        }

    }

    for (i=0;i<DMABUF_NUM_GEL_RX;i++) {
        bd_data=read_bd_rx(i);
        if ((bd_data & BD_LENGTH_BIT)!=10*i) {
            printf("    !!!err:bd %x data =%x, F=%x, Length=%x\n",i,bd_data,\
                   (bd_data & BD_VALID_BIT)>0x11,bd_data & BD_LENGTH_BIT) ;
        }
    }

    printf("    test gel rx bd over ******\n");



    printf("    test gel tx bd :  write all bd i*10+5 -> read back ******\n");
    //print_bd_tx();
    for (i=0;i<DMABUF_NUM_GEL_TX;i++) {
        write_bd_tx(10*i+5,i);
    }

    for (i=0;i<DMABUF_NUM_GEL_TX;i++) {
        bd_data=read_bd_tx(i);
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

static void init_bd_rx(void)
{

    UINT16 i;
    UINT32 bd_data;
    for (i=0;i<DMABUF_NUM_GEL_RX;i++) {
        bd_data = RX_BUF_BLOCKID(i);    /* indicate the block id. NOTE : the end flag is set in write_bd_rx() */
        write_bd_rx(bd_data,i);
    }
}

static void init_bd_tx(void)
{
    UINT16 i;
    UINT32 bd_data;
    for (i=0;i<DMABUF_NUM_GEL_TX;i++) {
        /* must init tx lenth to 1 */
        bd_data = 1<<BD_LENGTH_SHIFT | TX_BUF_BLOCKID(i);   /* indicate the block id. NOTE : the end flag is set in write_bd_rx() */
        write_bd_tx(bd_data,i);
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
int opipmux_gel_send(struct eth_device* dev, volatile void *packet, int length)
{
    int i;
	int head1_ofs = 0, head2_ofs = 0;
    int j=0; //wei debug
    UINT32 bd_data;
    UINT8 *pucPkt = NULL;
	int wait = 1;

    /* copy data to buffer  */
#ifdef  DEBUGDMA
    printf("--------TX : bd=%d, packaddr=0x%x, data len=%d, bufaddr=%x\n", \
           bd_id_tx,packet,length,(char *)(TX_BUF_ADDR(bd_id_tx)));
#endif

#ifdef  DEBUGDMA_DETAIL_TX
    printf("    copy pack from %x to %x, length = %d\n", packet, \
           TX_BUF_ADDR(bd_id_tx),  length);
#endif

    /* begin added by jiangmingli for atheros switch */
    #ifdef CONFIG_IPMUX_ATHEROS_PKT_HEADTAG
    pucPkt = (char *)TX_BUF_ADDR(bd_id_tx);
    /* dmac + smac */
    memcpy(pucPkt, (char *)packet, 12);
    /* atheros header */
    if (0 == memcmp(aucBroadcastMac, (char *)packet, 6))
    {
    	#ifdef  DEBUGDMA_DETAIL_TX
    	printf("set broadcast\n");
	#endif
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
	head1_ofs = 0;
	head2_ofs = 2;
    #else    //#ifdef CONFIG_IPMUX_ATHEROS_PKT_HEADTAG
    #ifdef CONFIG_IPMUX_ONU_A_F13_BC
    pucPkt = (char *)TX_BUF_ADDR(bd_id_tx);
    pucPkt[0] = 0;
    memcpy(pucPkt + 1, (char *)packet, length);
	head1_ofs = 1;
	head2_ofs = 0;
    #else  
    memcpy((char *)(TX_BUF_ADDR(bd_id_tx)),(char *)packet,length);
	head1_ofs = 0;
	head2_ofs = 0;
    #endif
    #endif
    /* end added by jiangmingli for atheros switch */

	/* make sure the all bytes of packet are copied to DMA buffer */
	wait = 1;
	while(wait)
	{
		wait = (*((char *)(TX_BUF_ADDR(bd_id_tx)) + length + head1_ofs - 8) != 
			*((char *)packet + length - head2_ofs -8));
		wait += (*((char *)(TX_BUF_ADDR(bd_id_tx)) + length + head1_ofs - 7) != 
			*((char *)packet + length - head2_ofs -7));
		wait += (*((char *)(TX_BUF_ADDR(bd_id_tx)) + length + head1_ofs - 6) != 
			*((char *)packet + length - head2_ofs -6));
		wait += (*((char *)(TX_BUF_ADDR(bd_id_tx)) + length + head1_ofs - 5) != 
			*((char *)packet + length - head2_ofs -5));
		wait += (*((char *)(TX_BUF_ADDR(bd_id_tx)) + length + head1_ofs - 4) != 
			*((char *)packet + length - head2_ofs -4));
		wait += (*((char *)(TX_BUF_ADDR(bd_id_tx)) + length + head1_ofs - 3) != 
			*((char *)packet + length - head2_ofs -3));
		wait += (*((char *)(TX_BUF_ADDR(bd_id_tx)) + length + head1_ofs - 2) != 
			*((char *)packet + length - head2_ofs -2));
		wait += (*((char *)(TX_BUF_ADDR(bd_id_tx)) + length + head1_ofs - 1) != 
			*((char *)packet + length - head2_ofs -1));
	}

#ifdef  DEBUGDMA_DETAIL_TX
    printf("    copy pack ok.\n");
#endif
    opipmux_sync();

    if (debug_tx>0) 
    {
        debug_pck_send((char *)TX_BUF_ADDR(bd_id_tx), length, bd_id_tx);
    }
    /*xwang : endian */
    ntoh_range((UINT32 *)(TX_BUF_ADDR(bd_id_tx)),length);

#ifdef  DEBUGDMA_DETAIL_TX
    printf("    send:net byte order\n");
    print_mem((TX_BUF_ADDR(bd_id_tx)),length);
#endif

#ifdef  DEBUGDMA_DETAIL_TX
    printf("    send:wait for dma readey\n");
#endif

    /* wait for dma ready for send  */
    i=get_timer(0);
    while (read_bd_tx(bd_id_tx) & BD_VALID_BIT) {
        if (get_timer_usec(i)>MAX_WAIT_USEC) {
            printf("!!!TX wait timeout\n");
            break;
        }
        udelay(1);
        i++;
    }

#ifdef  DEBUGDMA_DETAIL_TX
    printf("    send:send data\n");
#endif
 
    /* set dma status   */
    length = (length << BD_LENGTH_SHIFT) & PCK_MAXLEN_MASK;

    /* max len = 1536   */
    bd_data = length | BD_VALID_BIT | TX_BUF_BLOCKID(bd_id_tx);  /* valid bit        */

#ifdef  DEBUGDMA_DETAIL_TX
    printf("    write bd bd_id_tx %d to  0x%x\n", bd_id_tx, bd_data);
    print_bd_data(bd_data,bd_id_tx);
#endif

    write_bd_tx(bd_data,bd_id_tx);

    /* dma will transmit the packet */
    opipmux_sync();


#ifdef  DEBUGDMA_DETAIL_TX
    printf("    send:wait for send complete\n");
#endif

    /* wait for send complete   */
    i=get_timer(0);
    while ((read_bd_tx(bd_id_tx) & BD_VALID_BIT)) {
        if (get_timer_usec(i)>MAX_WAIT_USEC) {
            //printf("TX commit timeout\n");
            inc_bdid_tx();
            return(0);
            break;
        }
        udelay(1);
        i++;
    }

#ifdef  DEBUGDMA_DETAIL_TX
    printf("    ^^^^^^^^^^^^^^^ send:OK ^^^^^^^^^^^^^^\n");
#endif
    inc_bdid_tx();

    return(1);
}

static void inc_bdid_tx(void)
{
    /* transmit ok  */
    bd_id_tx++;
    if (bd_id_tx>=DMABUF_NUM_GEL_TX) {    /* tx number could't beyond NO_OF_FIFOS */
        bd_id_tx=0;
    }
}

static void inc_bdid_rx(void)
{
    /* receive ok   */
    bd_id_rx++;
    if (bd_id_rx>=DMABUF_NUM_GEL_RX) {    /* tx number could't beyond NO_OF_FIFOS */
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
    int num_int=0;
    int length;
    UINT32 bd_data;
    printf("/****** fe init ok ******/");
    opipmux_gel_init();

    for (;;) {

        opipmux_sync();

        if (ctrlc()) {
            printf("user interrupted\n");
            num_int += spaToBdZero();
            return(num_int);
        }
        if (0==find_valid_rx()) continue;
        bd_data=read_bd_rx(bd_id_rx);

        if (!(bd_data & BD_VALID_BIT)) {
            /* Nothing has been received */
            continue;
        }

        /* read packet from dma buffer  */
        length = (bd_data & PCK_MAXLEN_MASK) >> BD_LENGTH_SHIFT;
        num_int++;
        if (debug_rx>0) debug_pck_Rcv((char *)RX_BUF_ADDR(bd_id_rx),length - 4,bd_id_rx);
        print_mem((char *)RX_BUF_ADDR(bd_id_rx), length - 4);
        ntoh_range((UINT32 *)(RX_BUF_ADDR(bd_id_rx)),length);

        /* send packet back */
        opipmux_gel_send(NULL,(char *)(RX_BUF_ADDR(bd_id_rx)),length-4);
        bd_data = bd_data & (~BD_VALID_BIT);

        /* clear the RD */
#ifdef BD_WRITE_UNTIL_OK
        write_bd_rx_check(bd_data, bd_id_rx);
#else
        write_bd_rx(bd_data, bd_id_rx);
#endif

        inc_bdid_rx();

        if (ctrlc()) {
                printf("user interrupted\n");
                num_int += spaToBdZero();
                return num_int;
        }
    }

    num_int += spaToBdZero();
#ifdef DEBUGDMA_DETAIL
    printf("^^^^^^^^^^^^^^^^rx over^^^^^^^^^^^^^^^^^^");
#endif
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
static int opipmux_gel_recv(struct eth_device* dev){

    int length;

    UINT32 bd_data;
    UINT8 *pucPkt = NULL;
    #ifdef CONFIG_IPMUX_ATHEROS_PKT_HEADTAG
    PKT_HEADER_ATHEROS_s *pstAtherosHeader = NULL;
    #endif
    
    if (0==find_valid_rx()) return(-1);

    for (;;) {

        opipmux_sync();

        bd_data=read_bd_rx(bd_id_rx);

        if (!(bd_data & BD_VALID_BIT)) {
            /* Nothing has been received */
            return(-1);
        }

        /* read packet from dma buffer  */
        length = (bd_data & PCK_MAXLEN_MASK) >> BD_LENGTH_SHIFT;

#ifdef  DEBUGDMA
        /*
        printf("\nRX %d--------------------------------------------\n",bd_id_rx) ;
        */
        printf("--------------------------RX : bd=%d, bd_data=0x%x, F=%d, Data Len=%d, bufaddr=%x\n",\
               bd_id_rx,bd_data,(bd_data & BD_VALID_BIT)>0x11,length,\
               (UINT32 *)(RX_BUF_ADDR(bd_id_rx))) ;

        print_bd_data(bd_data,bd_id_rx);
#endif

        ntoh_range((UINT32 *)(RX_BUF_ADDR(bd_id_rx)),length);

#ifdef DEBUGDMA_DETAIL_RX

        print_mem((char *)(RX_BUF_ADDR(bd_id_rx)),length);
        //printf("length = %d ",length);
        if ((64==length) && (*(char *)(RX_BUF_ADDR(bd_id_rx))!=0xffffffff)) {
            printf("length = %d , char = %x\n ",length,(*(char *)(RX_BUF_ADDR(bd_id_rx))));
            print_mem((char *)(RX_BUF_ADDR(bd_id_rx)),length);
        }

#endif

        /*
        proc_arp((char *)(RX_BUF_ADDR(bd_id_rx)),length);
        */

        /*Pass packet up to the protocol layers. This route is defined in file net.c*/
        /* discard the crc32 tail */

#ifdef DEBUGDMA_DETAIL
        if (length>=0x45) {
            printf("length  = %x, so call test_gel_debug \n",length);

            test_gel_debug();
        } else {
            printf("up to protocal\n");
        }
#endif
        debug_pck_Rcv((char *)RX_BUF_ADDR(bd_id_rx),length - 4,bd_id_rx);

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
        #ifdef CONFIG_IPMUX_ONU_A_F13_BC
        pucPkt = (char *)RX_BUF_ADDR(bd_id_rx);
        memcpy(RxBufTmp, pucPkt + 4, length - 4);
        NetReceive(RxBufTmp, length - 4);
        #else
        NetReceive((char *)RX_BUF_ADDR(bd_id_rx), length - 4);
        #endif
        #endif
        /* end added by jiangmingli for atheros switch */

#ifdef DEBUGDMA_DETAIL
        printf("rx netrecv ok\n");
#endif

        bd_data = bd_data & (~BD_VALID_BIT);

        /* clear the RX RD */
#ifdef BD_WRITE_UNTIL_OK
        write_bd_rx_check(bd_data, bd_id_rx);
#else
        write_bd_rx(bd_data, bd_id_rx);
#endif

#ifdef DEBUGDMA_DETAIL
        printf("rx bd %d ok\n",bd_id_rx);
#endif

        inc_bdid_rx();
    }

#ifdef DEBUGDMA_DETAIL
    printf("^^^^^^^^^^^^^^^^rx over^^^^^^^^^^^^^^^^^^");
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

    /*wangxing:disable dma */
    unable_dma_fe();

}

#if 0
void opipmux_gel_reset(void)
{
    volatile gmac_ctrl_reg *greg = (volatile gmac_ctrl_reg *)OP_IPMUX_GEL_ADDR;
    UINT32 val;

    /* program mac config1 register leve tx and rx disabled */
    val = GMAC_CONFIG_1(    1,  /*soft_reset*/
                            1,  /*sim_reset*/
                            1,  /*reset_rx_cntl*/
                            1,  /*reset_tx_cntl*/
                            1,  /*reset_rx_func*/
                            1,  /*reset_tx_func*/
                            0,  /*loopback*/
                            1,  /*rx_flow_cntl*/
                            1,  /*tx_flow_cntl*/
                            0,  /*rx_enable*/
                            0   /*tx_enable*/
                       ) ;
    greg->mac_config1=val;

    /*wangxing:disable dma */
    unable_dma_fe();

}
#endif

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
    printf("        TxBaddr kseg1=%x\n", TX_BUF_ADDR(0));

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
static void init_dma_fe(void)
{
    volatile gmac_dmal_reg *p_gel_dma = (volatile gmac_dmal_reg *)OP_IPMUX_GEL_DMA_ADDR;

#ifdef OPULAN_INTERRUPT_MODE

    /* clear source */
    //p_gel_dma->DMAEN=0x0;
    p_gel_dma->DMAEN=0x3;
#endif

    //p_gel_dma->BDIE=0x0;
    //p_gel_dma->BDIE=0x7;
    p_gel_dma->DMAEN=0x3;       /* wwei mark */

    /* for test */
    p_gel_dma->BDIE=0x3;

#if defined(CONFIG_ONU_2)
	*(unsigned*)OP_IPMUX_GEL_MAC_ADDR=0x3;/*kxu*/
#endif
#if defined(CONFIG_IPMUX_ONU)
    *(unsigned*)OP_IPMUX_FEMAC_ADDR=0x3;/*kxu 20110226*/
#endif
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
static void unable_dma_fe(void)
{
    volatile gmac_dmal_reg *p_gel_dma = (volatile gmac_dmal_reg *)OP_IPMUX_GEL_DMA_ADDR;

    p_gel_dma->DMAEN=0x0;
    p_gel_dma->BDIE=0x0;

    printf("Disable FEMAC\n");

#if defined(CONFIG_ONU_2)
	*(unsigned*)OP_IPMUX_GEL_MAC_ADDR=0x0;/*kxu 20110407*/
#endif

#if defined(CONFIG_IPMUX_ONU)
    *(UINT32*)OP_IPMUX_FEMAC_ADDR = 0; /*kxu 20110307*/
#endif
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

    volatile gmac_dmal_reg *p_gel_dma = (volatile gmac_dmal_reg *)OP_IPMUX_GEL_DMA_ADDR;
    UINT32 i;
    /******** init dma ********/
#ifdef BUF_TO_ZERO
    Buffers=(UINT8**)0x0;
    TxBuf=(UINT8**)0x0;
#endif

    for (i = 0; i < DMABUF_NUM_GEL_RX+DMABUF_NUM_GEL_TX; i++) {
        memset((char *)RX_BUF_ADDR(i),i+1,DMABUF_LEN_GEL);
    }
    //memset((char *)Buffers,0, (DMABUF_NUM_GEL_RX+DMABUF_NUM_GEL_TX)*DMABUF_LEN_GEL);

    p_gel_dma->BASEADDR=(UINT32)Buffers;

    bd_id_tx=0;
    bd_id_rx=0;

    /******** init gel ********/
    opipmux_gel_reset();

 

    /* init bd tables */
    init_bd_tx();
    init_bd_rx();

    /*enable dma*/
    init_dma_fe();


    bd_id_rx=0;
    bd_id_tx=0;


#ifdef DEBUGDMA_DETAIL
    print_config();
#endif

    /* begin added by jiangmingli for atheros switch */
    #ifdef CONFIG_IPMUX_ATHEROS_PKT_HEADTAG
    #if 0
    g_stAtherosHeader.usPortNum = 0;
    g_stAtherosHeader.usVersionLo = 2;
    g_stAtherosHeader.usFromCpu = 1;
    g_stAtherosHeader.usBroadcast = 0;
    g_stAtherosHeader.usPackType = 0;
    g_stAtherosHeader.usPriority = 3;
    g_stAtherosHeader.usVersionHi = 2;
    #endif
    /* begin modified by jiangmingli for new atheros packet head struct */
    g_stAtherosHeader.usVersionHi = 2;
    g_stAtherosHeader.usPriority = 3;
    g_stAtherosHeader.usPackType = 0;
    g_stAtherosHeader.usFromCpu = 1;
    g_stAtherosHeader.usPortNum = 0x1e;    
    /* end modified by jiangmingli for new atheros packet head struct */
    #endif
    /* end added by jiangmingli for atheros switch */


    //print_config();
    return(1);

}
#if 0
int opipmux_gel_init(void)
{
    volatile gmac_ctrl_reg *greg = (volatile gmac_ctrl_reg *)OP_IPMUX_GEL_ADDR;
    volatile gmac_dmal_reg *p_gel_dma = (volatile gmac_dmal_reg *)OP_IPMUX_GEL_DMA_ADDR;
    UINT32 val;
    UINT32 i;
    /******** init dma ********/
#ifdef BUF_TO_ZERO
    Buffers=(UINT8**)0x0;
    TxBuf=(UINT8**)0x0;
#endif

    for (i = 0; i < DMABUF_NUM_GEL_RX+DMABUF_NUM_GEL_TX; i++) {
        memset((char *)RX_BUF_ADDR(i),i+1,DMABUF_LEN_GEL);
    }
    //memset((char *)Buffers,0, (DMABUF_NUM_GEL_RX+DMABUF_NUM_GEL_TX)*DMABUF_LEN_GEL);

    p_gel_dma->BASEADDR=(UINT32)Buffers;

    bd_id_tx=0;
    bd_id_rx=0;

    /******** init gel ********/
    opipmux_gel_reset();

    /* program mac config1 register leve tx and rx disabled */
    val = GMAC_CONFIG_1(    0,  /*soft_reset*/
                            0,  /*sim_reset*/
                            0,  /*reset_rx_cntl*/
                            0,  /*reset_tx_cntl*/
                            0,  /*reset_rx_func*/
                            0,  /*reset_tx_func*/
                            0,  /*loopback*/
                            1,  /*rx_flow_cntl*/
                            1,  /*tx_flow_cntl*/
                            0,  /*rx_enable*/
                            0   /*tx_enable*/
                       ) ;
    greg->mac_config1=val;

    val = GMAC_CONFIG_2(    1,  /*full_duplex : 0=half,1=full*/
                            1,  /*crc_en*/
                            1,  /*pad_and_crc_en*/
                            0,  /*len_field_check_en*/
                            1,  /*huge_frame_en*/
                            1,  /*interface_mode : 1=10M/100M , 2=1000M*/
                            0x7 /*preamble_length*/
                       ) ;
    greg->mac_config2=val;

    /* init bd tables */
    init_bd_tx();
    init_bd_rx();

    /*enable dma*/
    init_dma_fe();

    /* After all setup, finally enable tx and rx module */
    val = greg->mac_config1;

    bd_id_rx=0;
    bd_id_tx=0;

    val |= GMAC_RX_ENABLE;
    val |= GMAC_TX_ENABLE;
    greg->mac_config1=val;

#ifdef DEBUGDMA_DETAIL
    print_config();
#endif

    //print_config();
    return(1);

}
#endif

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

#ifdef CONFIG_IPMUX_ONU_A_F13_BC
int opipmux_bc_init(bd_t *bd)
{
	UINT32 regdata1, regdata2;
	printf("opipmux_bc_init\n");
	*(volatile u32*)0xbf00c0c0 = 0xFFFFFFFF;
	*(volatile u32*)0xbf00c0c4 = 0x0000FFFF;
	regdata2 = bd->bi_enetaddr[0]*0x100 + bd->bi_enetaddr[1];
	regdata1 = bd->bi_enetaddr[2]*0x1000000 + bd->bi_enetaddr[3]*0x10000 + bd->bi_enetaddr[4]*0x100 + bd->bi_enetaddr[5];
	*(volatile u32*)0xbf00c0c8 = regdata1;
	*(volatile u32*)0xbf00c0cc = regdata2;
	*(volatile u32*)0xbf00c0f4 = 0x00000808;
	
	*(volatile u32*)0xbf008000 = 0x00000003;
	*(volatile u32*)OP_IPMUX_GMAC_ADDR = 0x00000003;//Gmac Tx Rx en  /*kxu 20110303*/

#if defined(CONFIG_ONU_1_1_RF400)
      // *(volatile u32*)(0xbf000000+0x909*4) = 0x1;
       //*(volatile u32*)(0xbf000000+0x906*4) = 12;
       //*(volatile u32*)(0xbf000000+0x907*4) = 7;
#else
    /* configure GMAC speed */
    
    if (g_phy_speed == 2)
    {
	    *(volatile u32*)(0xbf000000+0x909*4) = 0x1;
        *(volatile u32*)(0xbf000000+0x906*4) = 12;
        *(volatile u32*)(0xbf000000+0x907*4) = 7;
    }
    else
    {
        *(volatile u32*)(0xbf000000+0x909*4) = 0x0;
        *(volatile u32*)(0xbf000000+0x906*4) = 22;
        *(volatile u32*)(0xbf000000+0x907*4) = 15;
    }
    
    #ifdef CONFIG_ONU_1_1_C13_SPI_DDR_SIZE_512M_BC
    
    if (g_phy_speed == 2) /* 1000M */
    {
        regdata1 = *(volatile u32*)(0xbf000000+0x991*4);
	    *(volatile u32*)(0xbf000000+0x991*4) = 0x2 | (regdata1 & 0xFFFFFFFC);
        *(volatile u32*)(0xbf000000+0x907*4) = 0x7;
    }
    else if (g_phy_speed == 1) /* 100M */
    {
        regdata1 = *(volatile u32*)(0xbf000000+0x991*4);
	    *(volatile u32*)(0xbf000000+0x991*4) = 0x1 | (regdata1 & 0xFFFFFFFC);
        *(volatile u32*)(0xbf000000+0x907*4) = 0x7B;
    }
    else /* 10M */
    {
        regdata1 = *(volatile u32*)(0xbf000000+0x991*4);
	    *(volatile u32*)(0xbf000000+0x991*4) = regdata1 & 0xFFFFFFFC;
        *(volatile u32*)(0xbf000000+0x907*4) = 0x7B;
    }
    #endif

#endif

}
#endif

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
    
    #ifdef CONFIG_IPMUX_ONU_A_F13_BC
    opipmux_bc_init(bis);
    #endif

    return(1);
}



#endif 

