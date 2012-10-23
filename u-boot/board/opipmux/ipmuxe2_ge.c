/** THE FORMAT is FOR DOXYGEN to Generate the document automatically

  *  @defgroup 
  *  @copyright 
  *  @author 
  *  @version 
  *  @date 
  *  @histroy 
  *  author      date        description
  *  -------------------------------------------------------------------

  *  @{
  */
#include "ipmuxe2_ge.h"

#include <config.h>
#include <common.h>
#include <malloc.h>
#include <net.h>
#include <configs/opipmux.h>

#if defined(IPMUX_E2_ETH_GE)
/** @name Macro definition
 *  @{
 */
/*--------------------------Macro definition------------------------- */
#define IPMUX_DMA_NUMS 9
#define DMABUF_NUM_GEL_RX  64
#define DMABUF_NUM_GEL_TX  64

#define DMABUF_TX_OFFSET   (DMABUF_NUM_GEL_RX*DMABUF_LEN_GEL)

/* bd table format */
#define BD_VALID_BIT   0x08000000   /* valid bit mask in BD */
#define BD_VALID_SHIFT 27           /* valid bit shift in BD */
#define BD_END_BIT     0x04000000   /* end flag mask in BD */
#define BD_END_SHIFT   26           /* end flag shift in BD */
#define BD_BLOCKID_BIT 0x000000FF   /* buffer address(block id) mask in BD */

#define BD_LENGTH_BIT  0x03FF8000   /* data length mask in BD */
#define BD_LENGTH_SHIFT 15

#define CFG_HZ 50000000

#define IPMUX_DMA_DEBUG_RX            0x0001
#define IPMUX_DMA_DEBUG_TX            0x0002
#define IPMUX_DMA_DEBUG_LOAD          0x0004
#define IPMUX_DMA_DEBUG_OWN_MAC       0x0008

#define IPMUX_BD_NUM_DMA0_RX    4
#define IPMUX_BD_NUM_DMA0_TX    16
#define DMABUF_NUM_HOST_RX      IPMUX_BD_NUM_DMA0_RX
#define DMABUF_NUM_HOST_TX      IPMUX_BD_NUM_DMA0_TX
#define DMABUF_LEN_GEL          IPMUX_DMA_BUF_LEN

#define OP_IPMUX_DMA0_ADDR      0xBF002B40
#define OP_IPMUX_DMA1_ADDR      0xBF002BC0
#define OP_IPMUX_GEL_DMA_ADDR   OP_IPMUX_DMA1_ADDR

/* BD table record format */
/*  -------------------------------------------------------------------------
 *  |BD_VALID   |ENDFLAG    |   Packet Length   |StartByte  |BufferAdd      |
 *  -------------------------------------------------------------------------
 *  b27         b26         b25-b15             b14-b8      b7-b0
 */
#define PCK_MAXLEN_MASK         0x03FF8000
#define BD_VALID_BIT            0x08000000    /* valid bit mask in BD */
#define BD_LENGTH               0x03FF8000
#define BD_LEN_OFF              15


#define MAX_WAIT_USEC           0x500

/* BD table Indirectly Access Control, 0x2B44 and 0x2BC0 */
#define BDTABLE_ID_SHIFT        16
#define IND_ACC_TYPE_WRITE      0x1<<30
#define IND_ACC_TYPE_READ       0<<30
#define IND_ACC_BUSY            0x1<<31
#define TX_BDTABLE_ID           1<<BDTABLE_ID_SHIFT
#define RX_BDTABLE_ID           0<<BDTABLE_ID_SHIFT

#define OP_IPMUX_INTCTRL_DMA    0x1

#define KSEG0_TO_KSEG1(x) ((unsigned long)(x)|0xa0000000)

#define RX_BUF_ADDR(dmas, bdIdx) (KSEG0_TO_KSEG1((unsigned long)Buffers[dmas] + (bdIdx)*DMABUF_LEN_GEL))
#define TX_BUF_ADDR(dmas, bdIdx) (KSEG0_TO_KSEG1((unsigned long)Buffers[dmas] + (bdIdx)*DMABUF_LEN_GEL + DMABUF_TX_OFFSET))
#define RX_BUF_BLOCKID(bdIdx) (bdIdx)
#define TX_BUF_BLOCKID(bdIdx) (bdIdx+DMABUF_NUM_GEL_RX)

#define TX_SHADDOW_ADDR(p) (unsigned long)&p->SHADOW_SET
#define TX_CTRL_ADDR(p) (unsigned long)&p->MEM_INDIR_ACC_CTRL
#define RX_SHADDOW_ADDR(p) ((unsigned long)&p->SHADOW_SET_RX)
#define RX_CTRL_ADDR(p) ((unsigned long)&p->MEM_INDIR_ACC_CTRL_RX)

#define REG_DMA_BUFFER_POINTER(dmas, base) (base + (dmas == IPMUX_DMA1 ? 0x24 : 0x1C))
#define DMA_BUFFER_POINTER_MASK 0x3F
#define DMA_BUFFER_POINTER_SHIFT_RX 8
#define DMA_BUFFER_POINTER_SHIFT_TX 0

#ifdef ETH_GE_DEBUG
#undef ETH_GE_DEBUG
#endif

#ifdef ETH_GE_DEBUG
#define dprintf printf
#else
#define dprintf(fmt...) 
#endif
/** @}*/

/** @name type definition
 *  @{
 */
/*--------------------------type definition------------------------- */
typedef struct GMAC_DMAL_REGISTERS_s
{
  volatile unsigned long  SHADOW_SET;            /*E2 : indirected access data for TX; E : indirected access data for RX/TX*/
  volatile unsigned long  MEM_INDIR_ACC_CTRL;    /*E2 : indirected access control for TX; E: indirected access control for RX/TX*/

  volatile unsigned long  BASEADDR;              /*buffer base address*/
  volatile unsigned long  REMOVED_TxBADDR;       /*Transmit Buffer base address*/
  volatile unsigned long  DMAEN;                 /*DMA enable*/
  volatile unsigned long  BDINT;                 /*Buffer DMA interrupt*/
  volatile unsigned long  BDIE;                  /*Buffer DMA interrupt enable*/

  volatile unsigned long  RESERVED0;                     
  volatile unsigned long  RESERVED1;                     
  volatile unsigned long  RESERVED2;  

  /*E2 only*/
  volatile unsigned long  SHADOW_SET_RX;         /*indirected access data for RX*/
  volatile unsigned long  MEM_INDIR_ACC_CTRL_RX; /*indirected access control for RX*/
} GMAC_DMAL_REG_t;




/** @}*/

/** @name global variable and function imported
 *  @{
 */
/*-----------------global varible/function declaration--------------- */

static char *g_mac_addr = NULL;
static int g_current_is_ge0 = 0;



/** @}*/
   
/** @name local function declaration
 *  @{
 */
/*-------------------local  function declaration---------------------- */
static unsigned long readBdRx(int dmas,unsigned short bdIndex);
static unsigned long readBdTx(int dmas, unsigned short bdIndex);
static unsigned long  getIpmuxDmaRegbase(int dmas);
static void incBdidRx(int dmas);
static void incBdidTx(int dmas);
static void initBdRx(int dmas);
static void initBdTx(int dmas);
static void writeBdRx(int dmas, unsigned long data,unsigned short bdIndex);
static unsigned long mipsCountGet(void);
static void writeBdTx(int dmas, unsigned long  data,unsigned short bdIndex);





/** @}*/
 

/** @name local variable declaration and definition
 *  @{
 */
/*--------------local  variable declaration and definition------------- */

static unsigned short bdIdRx[IPMUX_DMA_NUMS];
static unsigned short bdIdTx[IPMUX_DMA_NUMS];
static char * Buffers[IPMUX_DMA_NUMS] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};




/** @}*/
 

/** @name global variable and function exported
 *  @{
 */
/*------------------global variable and function exported-------------- */





/** @}*/


/** @defgroup

 *  @{
 */
/*----------------------local  function definition--------------------- */

/* ********************for DMA operations****************************** */
void parse_mac(char *mac,int *val)
{
  int i=0;
  char value;
  char base = 0;
  while(*mac != '\0'){
    if(*mac != ':'){
      if(*mac>='0'&& *mac<='9')
    base = 0x30;
      else if(*mac>= 'a' && *mac <= 'z')
    base = 'a'-10;
      else if(*mac>='A' && *mac<= 'Z')
    base = 'A'-10;
      value = (*mac) - base;
      mac++;
    }else{
      i++;
      mac++;
      continue;
    }
    val[i] = ((val[i])<<4)|value;
  }
}

static unsigned long mipsCountGet(void)
{
  static int iCount = 0;
  iCount ++;
  return iCount;
}
static unsigned long timerUsecGet(int base)
{
  return (mipsCountGet() - base);
}


static unsigned long getIpmuxDmaRegbase(int dmas)
{
  unsigned long offset;

  offset = 0;
  switch (dmas) {
  case IPMUX_DMA0: 
    return 0x2B40 + offset;
  case IPMUX_DMA1: 
    return 0x2BC0 + offset;
  case IPMUX_DMA2: 
    return 0x2AC0 + offset;
  case IPMUX_DMA3: 
    return 0x2B00 + offset;
  default:
    printf("invalid dmas %d passed.\n", dmas);
    return 0xFFFFFFFF;
  }
}

void ipmuxDmaEnable(int dmas)
{
  volatile GMAC_DMAL_REG_t *pGelDma = (volatile GMAC_DMAL_REG_t *)getIpmuxDmaRegbase(dmas);
  ipMuxRegWrite((unsigned long)&pGelDma->DMAEN, 0x1);
}

static void initBdTx(int dmas)
{
  short i;
  unsigned long bdData;
  
  for (i=0;i<DMABUF_NUM_GEL_TX;i++) {
    bdData = 1<<BD_LENGTH_SHIFT | TX_BUF_BLOCKID(i);
    writeBdTx(dmas, bdData,i);
  }
}

static void initBdRx(int dmas)
{
  short i;
  int bdData;
  for (i=0; i<DMABUF_NUM_GEL_RX; i++) {
    bdData = RX_BUF_BLOCKID(i);
    writeBdRx(dmas, bdData, i);
  }
}

static void incBdidTx(int dmas)
{
  volatile GMAC_DMAL_REG_t *pGelDma = (volatile GMAC_DMAL_REG_t *)getIpmuxDmaRegbase(dmas);
  volatile unsigned long regVal;

  ipMuxRegRead(REG_DMA_BUFFER_POINTER(dmas, (unsigned long)pGelDma), &regVal);
  bdIdTx[dmas] = (regVal >> DMA_BUFFER_POINTER_SHIFT_TX) & DMA_BUFFER_POINTER_MASK ;
  //dprintf("%s:bdidTx[%d]=%x\n",__FUNCTION__,dmas,bdIdTx[dmas]);
}

static void incBdidRx(int dmas)
{
  volatile GMAC_DMAL_REG_t *pGelDma = (volatile GMAC_DMAL_REG_t *)getIpmuxDmaRegbase(dmas);
  volatile unsigned long regVal;
  
  ipMuxRegRead(REG_DMA_BUFFER_POINTER(dmas, (unsigned long)pGelDma), &regVal);
  bdIdRx[dmas] = (regVal >> DMA_BUFFER_POINTER_SHIFT_RX) & DMA_BUFFER_POINTER_MASK ;
}
static unsigned long  readBdTx(int dmas, unsigned short bdIndex)
{
  volatile GMAC_DMAL_REG_t *pGelDma = (volatile GMAC_DMAL_REG_t *)getIpmuxDmaRegbase(dmas);
  volatile unsigned long regVal;
  
  ipMuxRegWrite(TX_CTRL_ADDR(pGelDma), \
                IND_ACC_BUSY | IND_ACC_TYPE_READ | TX_BDTABLE_ID | bdIndex);
  
    while(ipMuxRegBitTest(TX_CTRL_ADDR(pGelDma), IND_ACC_BUSY)){
      dprintf("Warning : waiting write bd table of dma %d !\n", dmas);
    }

  ipMuxRegRead(TX_SHADDOW_ADDR(pGelDma), &regVal);
  return regVal;
}

static void writeBdTx(int dmas, unsigned long data,unsigned short bdIndex)
{
  volatile GMAC_DMAL_REG_t *pGelDma = (volatile GMAC_DMAL_REG_t *)getIpmuxDmaRegbase(dmas);

  unsigned long i=0;
  
  /* add the end flag to data */
  if (bdIndex==DMABUF_NUM_GEL_TX-1){
    data = (data | BD_END_BIT);
  }
  ipMuxRegWrite(TX_SHADDOW_ADDR(pGelDma), data);
  ipMuxRegWrite(TX_CTRL_ADDR(pGelDma), \
        IND_ACC_BUSY | IND_ACC_TYPE_WRITE | TX_BDTABLE_ID | bdIndex);
  i = mipsCountGet();
  while (ipMuxRegBitTest(TX_CTRL_ADDR(pGelDma) , IND_ACC_BUSY)) {
    if (timerUsecGet(i)>MAX_WAIT_USEC) {
      dprintf("Warning : Write TX bd table of dma %d  timeout\n", dmas);
      break;
    }
  }
}

static unsigned long  readBdRx(int dmas,unsigned short bdIndex)
{
  volatile GMAC_DMAL_REG_t *pGelDma = (volatile GMAC_DMAL_REG_t *)getIpmuxDmaRegbase(dmas);
  int i=0;
  volatile unsigned long regVal;
  
  ipMuxRegWrite(RX_CTRL_ADDR(pGelDma), \
                IND_ACC_BUSY | IND_ACC_TYPE_READ | RX_BDTABLE_ID | bdIndex);
  
  i = mipsCountGet();
  while(ipMuxRegBitTest(RX_CTRL_ADDR(pGelDma), IND_ACC_BUSY)){
    if(timerUsecGet(i)>MAX_WAIT_USEC){
      dprintf("read RX bd table timeout\n");
      break;
    }
  }
  ipMuxRegRead(RX_SHADDOW_ADDR(pGelDma), &regVal);
  return regVal;
}

static void writeBdRx(int dmas, unsigned long data,unsigned short bdIndex)
{
  int i;
  volatile GMAC_DMAL_REG_t *pGelDma = (volatile GMAC_DMAL_REG_t *)getIpmuxDmaRegbase(dmas);

  /* add the end flag to data */
  if (bdIndex==DMABUF_NUM_GEL_RX-1) {
    data=data | BD_END_BIT;
  }
  
  ipMuxRegWrite(RX_SHADDOW_ADDR(pGelDma), data);
  ipMuxRegWrite(RX_CTRL_ADDR(pGelDma), \
        IND_ACC_BUSY | IND_ACC_TYPE_WRITE | RX_BDTABLE_ID | bdIndex);
  
  i = mipsCountGet();
  while (ipMuxRegBitTest(RX_CTRL_ADDR(pGelDma), IND_ACC_BUSY)) {
    if (timerUsecGet(i)>MAX_WAIT_USEC) {
      dprintf("Warning : Write RX bd table of dma %d timeout\n", dmas);
      break;
    }
  }
}

int ipmuxDmaInit(int dmas)
{
  int size;
  volatile GMAC_DMAL_REG_t *pGelDma;
  /*if(IS_IPMUX_E){
    printf("the GE replace FE doesnt support for IPMUX-E board\n");
    return -1;
    }*/

  
  if((dmas == IPMUX_DMA_FE) || (dmas == IPMUX_DMA_HOST)){
    size = (DMABUF_NUM_GEL_RX)*(DMABUF_LEN_GEL);
    size = (DMABUF_NUM_GEL_RX)*(DMABUF_LEN_GEL);
  }else if((dmas == IPMUX_DMA_GE0) || (dmas == IPMUX_DMA_GE1)){
    size = (DMABUF_NUM_GEL_TX+DMABUF_NUM_GEL_RX)*(DMABUF_LEN_GEL);
  }else{
    printf("the DMA number is error\n");
    return -1;
  }
  
  pGelDma = (volatile GMAC_DMAL_REG_t *)getIpmuxDmaRegbase(dmas);
  if(!Buffers[dmas]){
    Buffers[dmas] = (char *)malloc(size);
    if(!Buffers[dmas]){
      printf("malloc memory for DMA buffers[%d] failed\n",dmas);
      return -1;
    }
    dprintf("malloc addr 0x%x for dma %d 0x%x Bytes memory\n",
        (unsigned long)(Buffers[dmas]),dmas,size);
  }
  memset((char *)KSEG0_TO_KSEG1(Buffers[dmas]),0,size);
  ipMuxRegWrite((unsigned long)&pGelDma->BASEADDR,(unsigned long)Buffers[dmas]);
  bdIdRx[dmas] = 0;
  bdIdTx[dmas] = 0;
  if(dmas <= IPMUX_DMA_FE){
    initBdRx(dmas);
  }else{
    initBdTx(dmas);
  }
  ipmuxDmaEnable(dmas);
  return 0;
}

void ipmuxDmaDeinit(int dmas)
{
  if(Buffers[dmas]){
    free(Buffers[dmas]);
    Buffers[dmas] = NULL;
  }
}
int ipmuxDmaRx(int dmas, volatile char **pkt,int copied)
{
  int bdData;
  int len;
  /*if(IS_IPMUX_E()){
    printf("the GE replace FE doesnt support for IPMUX-E board\n");
    return -1;
    } */ 
  /* have copied */
  
  if(copied){
    bdData = RX_BUF_BLOCKID(bdIdRx[dmas]);
    writeBdRx(dmas,bdData,bdIdRx[dmas]);
    incBdidRx(dmas);
    return 0;
  }
  
  bdData = readBdRx(dmas,bdIdRx[dmas]);
  if(!(bdData & BD_VALID_BIT)){
    return -1;
  }
  
  len = (bdData & PCK_MAXLEN_MASK) >> BD_LENGTH_SHIFT;
  *pkt = (char *)RX_BUF_ADDR(dmas,bdIdRx[dmas]);
  return len;
}

int ipmuxDmaTx(int dmas, void *pkt,int length)
{
  int i;
  int bdData;

  if(!pkt){
    printf("error: pkt is null\n");
    return -1;
  }
  /* if(IS_IPMUX_E()){
    printf("the GE replace FE doesnt support for IPMUX-E board\n");
    return -1;
  }
  */
  //incBdidTx(dmas);
  memcpy((char *)(TX_BUF_ADDR(dmas,bdIdTx[dmas])),(char *)pkt,length);
  //dprintf("bdIdTx[%d] = %d\n",dmas,bdIdTx[dmas]);
#ifdef ETH_GE_DEBUG
  for(i = 1;i<length+1;i++){
    printf("%02x\t",*(unsigned char *)(TX_BUF_ADDR(dmas,bdIdTx[dmas])+i-1));
    if(i%16==0)
      printf("\n");
  }
  printf("\n");
#endif
  i = mipsCountGet();
  while (readBdTx(dmas, bdIdTx[dmas]) & BD_VALID_BIT) {
    if (timerUsecGet(i)>MAX_WAIT_USEC) {
      incBdidTx(dmas);
      return -1;
    }
  }
  length = (length << BD_LENGTH_SHIFT) & PCK_MAXLEN_MASK;
  bdData = length | BD_VALID_BIT | TX_BUF_BLOCKID(bdIdTx[dmas]);

  writeBdTx(dmas, bdData,bdIdTx[dmas]);
  
  i = mipsCountGet();
  unsigned long value = readBdTx(dmas,bdIdTx[dmas]);
  while (value & BD_VALID_BIT) {
    if (timerUsecGet(i)>MAX_WAIT_USEC) {
      incBdidTx(dmas);
      printf("warning:time out........\n");
      return -1;
    }
    value = readBdTx(dmas,bdIdTx[dmas]);
  }
  incBdidTx(dmas);
  return 0;
}
static unsigned long bdRxValid(int dmas, unsigned short bdIndex)
{
    unsigned long bdData;

    bdData=readBdRx(dmas, bdIndex);

    return   bdData & BD_VALID_BIT;
}
static int findValidBdRx(int dmas)
{
    int i ;
    unsigned short oldIdRx;
    oldIdRx = bdIdRx[dmas];

    for (i=bdIdRx[dmas];i<(bdIdRx[dmas]+DMABUF_NUM_GEL_RX);i++) {
        if (bdRxValid(dmas,(unsigned short)(i%DMABUF_NUM_GEL_RX))) {
            bdIdRx[dmas] = (unsigned short)(i%DMABUF_NUM_GEL_RX);
        
            if (oldIdRx!=bdIdRx[dmas]) {
          //dprintf("RX Bd table of dma %d wrong, old bdId=%x  new bdId=%x\n", dmas, oldIdRx,bdIdRx[dmas]);
            }
        /*find valid bd*/
            return(1);
        }
    }
    /*no valid bd*/
    return 0;
}
/* ********************for eth interface******************************* */
static int ipmux_ge_init(struct eth_device *dev,bd_t *bis)
{
  char *ge = NULL;
  char *addr = NULL;
  int mac_addr[6] = {0};
  ge = getenv("GE_PORT");
  if(ge){
    if(!strcmp("GE0",ge) && !g_current_is_ge0){
      ipmuxDmaDeinit(8);
      ipmuxDmaInit(4);
      g_current_is_ge0 = 1;
    }else if(!strcmp("GE1",ge) && g_current_is_ge0){
      ipmuxDmaDeinit(4);
      ipmuxDmaInit(8);
      g_current_is_ge0 = 0;
    }
  }

  addr = getenv("ethaddr");
  if(addr){
    if(!strcmp(addr,g_mac_addr))
      return 0;
    else{
      parse_mac(addr,mac_addr);
      ipMuxRegWrite(0x2005c,(mac_addr[0]<<8)|(mac_addr[1]));
      ipMuxRegWrite(0x20060,(mac_addr[2]<<24|mac_addr[3]<<16|mac_addr[4]<<8|mac_addr[5]));
      free(g_mac_addr);
      g_mac_addr = strdup(addr);
    }
  }
  return 0;
}

static void ipmux_ge_halt(struct eth_device *dev)
{
  return ;
}

static int ipmux_ge_tx(struct eth_device *dev,volatile void *packet,int length)
{
  int i;
  int dmas;
  /* the host to generate the packet to the server */
  if(g_current_is_ge0){
    dmas = 4;
  }else{
    dmas = 8;
  }
#ifdef ETH_GE_DEBUG
  dprintf("begin Tx:\n");
  for(i = 1;i<length+1;i++){
    dprintf("%02x\t",((unsigned char *)packet)[i-1]);
    if(i%16==0)
      dprintf("\n");
  }
  dprintf("\n");
#endif
  if(!ipmuxDmaTx(dmas,(char *)packet,length))
    return 0;
  else 
    return -1;
}

static int ipmux_ge_rx(struct eth_device *dev)
{
  unsigned char *pkt;
  int i,flag=0;
  int rsvd = 0;
  int len = 0;
  /* receive the packet by host read. */
  if(findValidBdRx(1) == 0)
    return -1;
  //for(;;){
    len = ipmuxDmaRx(1,(volatile unsigned char *)&pkt,0);
    if(len == -1){
      return -1;
    }
    if((pkt[4] == 0x01 && pkt[5] == 0x80 && pkt[6] == 0xc2 && pkt[7] == 0x00 && pkt[8] == 0x00) ||
       (pkt[4] == 0x00 && pkt[5] == 0x00 && pkt[6] == 0x00 && pkt[7] == 0x00 && pkt[8] == 0x00 && pkt[9] ==0x00)){
      flag = 1;
      rsvd = 1;
    }
    
    if(pkt[4] == 0xff && pkt[5] == 0xff && pkt[6] == 0xff && pkt[7] == 0xff && pkt[8] == 0xff && pkt[9] ==0xff){
      flag = 1;
    }
#ifdef ETH_GE_DEBUG    
    if(!flag) {
      dprintf("begin Rx:\n");
      for(i = 1;i+4<len+1;i++){
    dprintf("%02x\t",pkt[i+3]);
    if(i%16 == 0)
      dprintf("\n");
      }
      dprintf("\n");
    }
#endif
    if(!rsvd)
      NetReceive(pkt+4,len-8);
    ipmuxDmaRx(1,NULL,1);
  //}
  return 0;
}
int ipmux_eth_initialize(bd_t *bis)
{
  int i;
  struct eth_device *dev;
  char *mac = NULL;
  char *ge = NULL;
  int mac_addr[6]={0};
  printf("ge initialized\n");
  dev = (struct eth_device*)malloc(sizeof(struct eth_device));
  if(!dev){
    printf("%s:malloc memory for eth_device failed\n");
    return (-1);
  }
  memset(dev,0,sizeof(struct eth_device));
  sprintf(dev->name,"OPULAN IPMUX GE");
  setenv("GE_PORT","GE0");
  /* do the basic configuration for chip. */
  ipMuxRegWrite(0x00008, 0x4);
  ipMuxRegWrite(0x00010, 0xffff);     
  ipMuxRegWrite(0x13C00, 0x800a0000);          
  ipMuxRegWrite(0x1040, 0xFc00000);
  
  ipMuxRegWrite(0x0000c, 0x3);
  ipMuxRegWrite(0x14130, 0xd0080000);
  ipMuxRegWrite(0x14e48, 0xa0100000);
  ipMuxRegWrite(0x02b50, 1);                                 
  ipMuxRegWrite(0x14e04, 0x0);
  ipMuxRegWrite(0x14204, 0x0);
  ipMuxRegWrite(0x14200, 0x0);
  
  ipMuxRegWrite(0x2a08, 0x40300000);                      
  ipMuxRegWrite(0x200a8, 0x7f);                      
  ipMuxRegWrite(0x8200,0x0);
  
  ipMuxRegWrite(0x8204, 0x7127);
  ipMuxRegWrite(0x8008, 0x0);

  ipMuxRegWrite(0xA204, 0x7127);
  ipMuxRegWrite(0xa008, 0x0);
  mac = getenv("ethaddr");
  if(mac){
    g_mac_addr = strdup(mac);
    printf("initial mac addr is %s\n",g_mac_addr);
    parse_mac(mac,mac_addr);
  }
  for(i = 0;i<6;i++){
    printf("%02x\t",mac_addr[i]);
  }
  printf("\n");
  ipMuxRegWrite(0x2005c,(mac_addr[0]<<8)|(mac_addr[1]));
  ipMuxRegWrite(0x20060,(mac_addr[2]<<24)|(mac_addr[3]<<16)|(mac_addr[4]<<8)|(mac_addr[5]));

  ipMuxRegWrite(0x20064,0xffff);
  ipMuxRegWrite(0x20068,0xffffffff);
  
  ipmuxDmaInit(1);
  ge = getenv("GE_PORT");
  if(ge && !strcmp("GE0",ge)){
    ipmuxDmaInit(4);
    g_current_is_ge0 = 1;
  }else{
    ipmuxDmaInit(8);
    g_current_is_ge0 = 0;
  }
  ipMuxRegWrite(0x8200, 0x3f);
  ipMuxRegWrite(0xA200, 0x3f);
  


  dev->init = ipmux_ge_init;
  dev->halt = ipmux_ge_halt;
  dev->send = ipmux_ge_tx;
  dev->recv = ipmux_ge_rx;

  eth_register(dev);
  return (1);
}
#endif /* if defined(IPMUX_E2_ETH_GE) */
/** @}*/
/** @}*/
