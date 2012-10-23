

        
/*---------------------------------------------------------*/

/*--------------------------------------------------------------------------------*/
/*Uart register*/
#define UART0_SA_BASE  0x600*4
#define UART1_SA_BASE  0x700*4




/*------------------------------------------------------------------------------------*/
/*Interrupt Register*/
#define ONU_INT_BASE 0xb80*4

#define MOD_INT_STATUS_0  0xb80*4 /*module level interrupt status bit register 0*/
#define MOD_INT_STATUS_1  0xb81*4 
#define MOD_INT_ENABLE_0 0xb82*4
#define MOD_INT_ENABLE_1 0xb83*4

#define MOD_INT_CONFIG 0xb86*4
#define IRQ0_MODE (1<<15)  /*1: IRQ0 is NMI interrupt  0: IRQ0 is normal interrup*/
#define TOP_INT_EN  (1<<0)


/*interrupt bits*/
#define IRQ_BIT_PMAC_IRQ (1<<0)
#define IRQ_BIT_GPIO4 (1<<4)
#define IRQ_BIT_GPIO5 (1<<5)
#define IRQ_BIT_GPIO10 (1<<10)
#define IRQ_BIT_UPTM (1<<14)
#define IRQ_BIT_PCM  (1<<19)
#define IRQ_BIT_DMA0  (1<<20)
#define IRQ_BIT_DMA1 (1<<21)
#define IRQ_BIT_UART0 (1<<30)
#define IRQ_BIT_UART1 (1<<31)
#define IRQ_BIT_TIMER0 (1<<25)
#define IRQ_BIT_TIMER1 (1<<26)
/*-------add GPIO interrupt bit for 485/232 card---------------*/
#define IRQ_BIT_GPIO11 (1<<11)
#define IRQ_485_STATUS 0xBF00000B
#define IRQ_485_MASK1  0x8
#define IRQ_485_MASK2  0x10
#define IRQ_485_MASK3  0x20
#define IRQ_485_MASK4  0x40

/*-------------------------------------------------------*/

/*------------------------------------------------------------------------------------*/
/*FEDMA register*/
#define  FEDMA_BASE_ADDR  0x7000*4
#define  CPDMA_BASE_ADDR  0x6000*4

#define FE_RXBD_SHADOW  0x0*4
#define RXBD_SHADOW 0x3ffffff

#define FE_RXBD_CTL  0x1*4
#define RXBD_OP_BUSY  (1<<31)
#define RXBD_OP_WRITE (1<<30)  /*1: write  0:read*/
#define RXBD_OP_READ (0<<30)  /*1: write  0:read*/
#define RXBD_REC_INDEX  0x1f

#define FE_TXBD_SHADOW  0x2*4
#define TXBD_SHADOW 0x3ffffff

#define FE_TXBD_CTL  0x3*4
#define TXBD_OP_BUSY  (1<<31)
#define TXBD_OP_WRITE (1<<30)  /*1: write  0:read*/
#define TXBD_OP_READ (0<<30)  /*1: write  0:read*/
#define TXBD_REC_INDEX  0x1f

#define FE_DMA_BASE_ADDR  0x4*4 /*dma base addr*/

#define FE_CONTRL   0x5*4
#define FEDMA_EN   (1<<1)
#define CPTM_RD_EN   (1<<0)


#define FE_INT_STATUS 0x6*4   /*FEDMA interrupt status register*/
#define FE_INT_ENABLE 0x7*4   /*FEDMA interrupt enable register*/
#define LL_FULL_INT   (1<<6)
#define QUEUE_FULL_INT (1<<5)
#define TX_LONG_PKT_INT (1<<4)
#define CPTM_RD_ERR_INT (1<<3)
#define RXBD_FULL_INT (1<<2)
#define FEDMA_TX_INT  (1<<1)
#define FEDMA_RX_INT  (1<<0)

#define FEDMA_TXBD_POINTER  0x31*4
#define CPDMA_TXBD_POINTER  0x81*4
/*BD Table entry*/
#define BD_VALID_BIT (1<<29)
#define BD_END_BIT  (1<<28)
#define BD_INDEX  (0x7ff)
#define BD_LENGTH_SHIFT 17
#define BD_LENGTH_MASK  0x7ff
#define BD_TABLE_ID  (0<<6)



