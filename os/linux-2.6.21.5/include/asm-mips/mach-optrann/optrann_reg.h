

        
/*---------------------------------------------------------*/
#define PPH_BASE  0x800*4

#define PPH_EIC_INT_STATUS_0  0x0
#define PPH_DMAC_INT_STATUS   (1<<25)
#define PPH_UART3_INT_STATUS   (1<<23)
#define PPH_UART2_INT_STATUS   (1<<22)
#define PPH_UART1_INT_STATUS   (1<<21)
#define PPH_UART0_INT_STATUS   (1<<20)

#define PPH_EIC_INT_STATUS_1  (0x1*4)


#define PPH_EIC_INT_ENABLE_0  (0x4*4)
#define PPH_DMAC_INT_ENABLE    (1<<25)
#define PPH_UART3_INT_ENABLE   (1<<23)
#define PPH_UART2_INT_ENABLE   (1<<22)
#define PPH_UART1_INT_ENABLE   (1<<21)
#define PPH_UART0_INT_ENABLE   (1<<20)

#define PPH_EIC_INT_ENABLE_1  (0x5*4)
#define PPH_EIC_INT_MASK_0  (0x8 *4)
#define PPH_EIC_INT_MASK_1  (0x9*4)

#define PPH_EIC_CFG  (0x20*4)
#define IRQ1_EN (1<<16)    /*1: Independent interrupt  0:normal interrupt*/
#define IRQ0_EN (1<<8)     /*1: NMI interrupt  0: normal interrrupt*/
#define TOP_PIN_EN  (1<<0)  /*1: enable  */

/*--------------------------------------------------------------------------------*/
/*Uart register*/
#define UART0_SA_BASE  0x1000*4
#define UART1_SA_BASE  0x1800*4
#define UART2_SA_BASE  0x2000*4
#define UART3_SA_BASE  0x2800*4

#if 0
#define UART_INT_STATUS  0x0*4
#define UART_FIFO_EN  (0x3<<6)
#define UART_CTS_RTS_INT  (1<<5)
#define UART_INT_PENDING   (1<<0) /*0: have interrupt pending*/


#define UART_FIFO_CTRL    0x0*4
#define UART_RX_THRESH  (0x3 <<6)
#define UART_TX_THRESH  (0x3 << 4)
#define UART_TX_RST  (1<<2)
#define UART_RX_RST (1<<1)
#define UART_MODE   (1<<0)

#define ENH_FEA                 0x0*4
#define UART_CTS_EN  (1<<7)
#define UART_RTS_EN (1<<6)

#define UART_INT_ENABLE 0x2*4
#define UART_CTS_INT_ENABLE (1<<7)
#define UART_RTS_INT_ENABLE (1<<6)
#define UART_RLS_INT_ENABLE (1<<2)
#define UART_TX_EMPTY_INT     (1<<1)
#define UART_RX_AVAIL_INT      (1<<0)

#define DIV_REG_1              0x2*4   /*System clock divider : bit [0~7]*/
#define DIV_REG_0             0x6*4   /*system clock divider : bit [8~15]*/

#define UART_RX_HOLD_REG 0x6*4
#define UART_RX_DATA 0xff

#define UART_TX_HLOD_REG 0x6*4
#define UART_TX_DATA  0xff

#define UART_LINE_CTRL      0x9*4  /* Line Control Register */
#define UART_DIV_EN   (1<<7)
#define UART_BREAK_EN  (1<<6)
#define UART_FORCE_PARITY  (1<<5)
#define UART_EVEN_PARITY    (1<<4)
#define UART_PARITY_EN        (1<<3)
#define UART_STOP  (1<<2)
#define UART_WORD_LEN  (0x3<<0)


#define UART_MODEM_CTRL  0xa*4

#define UART_LINE_STA       0xb*4   /*Line Status Register */
#define UART_ERR_IND   (1<<7)
#define UART_TX_EMPTY (1<<6)
#define UART_RX_THR_EMPTY (1<<5)
#define UART_BREAK_ERR    (1<<4)
#define UART_FRAME_ERR (1<<3)
#define UART_PARITY_ERR  (1<<2)
#define UART_OF_ERR (1<<1)
#define UART_RX_READY (1<<0)

#define MODEM_STATUS       0xc*4
#define SCR_PAD_REG         0xd*4
#endif

/*------------------------------------------------------------------------------------*/
/*Optrann DMAC register base addr*/
#define IPMUX_DMAC_REG   0x5000*4     /*0x5000 *4 */	

/*Optrann DMAC registers*/
#define DMAC_INT_STATUS             0x0*4

#define PPE_RX_FULL_INT_ENABLE (1<<25)
#define PPE_RX_INT_ENABLE (1<<24)
#define TF_TX_INT_STATUS  (1<<18)
#define TF_RX_FULL_INT_STATUS (1<<17)
#define TF_RX_INT_STATUS (1<<16)
#define MSG_RX_DRP_STATUS (1<<3)
#define MSG_TX_INT_STATUS (1<<2)
#define MSG_RX_FULL_INT_STATUS (1<<1)
#define MSG_RX_INT_STATUS  (1<<0)

#define DMAC_INT_ENABLE             0x4*4
#define TF_TX_INT_ENABLE (1<<18)
#define TF_RX_FULL_INT_ENABLE (1<<17)
#define TF_RX_INT_ENABLE  (1<<16)
#define MSG_RX_DRP_ENABLE (1<<3)
#define MSG_TX_INT_ENABLE (1<<2)
#define MSG_RX_FULL_INT_ENABLE (1<<1)
#define MSG_RX_INT_ENABLE (1<<0)

#define DMAC_INT_MASK                0x8*4
#define MSG_RX_DRP_MASK  (1<<3)
#define MSG_TX_INT_MASK  (1<<2)
#define MSG_RX_FULL_INT_MASK (1<<1)
#define MSG_RX_INT_MASK (1<<0)

#define DMAC_TBL_ACC_CTL            0xc*4
#define TBL_BUSY   ( 1 <<31)               /*read/write BD Table should be write "1".  when it done, it be '0'*/
#define TBL_ACC_MOD_WRITE  (1 << 30)
#define TBL_ACC_MODE_READ (0<<30)
#define TBL_ID     (0<<16)   /*0: only support one table*/
#define TBL_ID_SHIFT  16
#define TBL_ACC_ADDR  (0xff)   

#define DMAC_TBL_ACC_DATA_0     0xe*4
#define DMAC_TBL_ACC_DATA_1      0xf*4

#define DMAC_INIT                           0x1f*4 /*initial BD Table*/
#define DMAC_INIT_BUSY  (1<<16)                   /*when read 0, finished*/
#define DMAC_INIT_EN      (1<<0)   /*write 1 to initial BD Table*/

#define DMAC_BA                              0x20*4  /*DMA buffer base addr*/
#define DMAC_MSG_CFG                    0x21*4
#define DMAC_PCIE_CFG                   0x22*4
#define DMAC_TF_CFG                      0x23*4
#define DMAC_MSG_POINT               0x24*4
#define DMAC_PCIE_POINT               0x25*4
#define DMAC_TF_POINT                  0x26*4
#define DMAC_MSG_MTU                  0x30*4
#define DMAC_MSG_RD_THR            0x31*4
#define DMAC_PCIE_RD_THR           0x32*4
#define DMAC_TF_RD_THR              0x33*4
#define DMAC_SCH_MODE               0x40*4
#define DMAC_MSG_PS_CFG           0x50*4
#define DMAC_MSG_RX_PKT_CNT    0x60*4
#define DMAC_MSG_RX_DRP_PKT   0x61*4
#define DMAC_MSG_TX_PKT_CNT   0x62*4
#define DMAC_PCIE_RX_PKT_CNT  0x63*4
#define DMAC_PCIE_TX_PKT_CNT   0x64*4
#define DMAC_TF_RX_PKT_CNT      0x65*4
#define DMAC_TF_TX_PKT_CNT       0x66*4


/*BD Table Desc bit*/
#define MSG_BD_STA  (1<<12)  /*1: valid  0: unvalid*/
#define MSG_BD_LAST (1<<11) /*last BD*/
#define MSG_BD_LEN   (0x7FF)  /*lenght*/



/*MSG register*/
#define MSG_SA_BASE   0xa000*4  

#define MSG_INT_STATUS  0x0*4
#define MII_ERR_DROP_INT_STATUS (1<<7)
#define ALIGNMENT_ERR_INT_STATUS (1<<6)
#define CRC_ERR_INT_STATUS (1<<5)
#define OVER_SIZE_INT_STATUS (1<<4)
#define UNDER_SIZE_INT_STATUS (1<<3)
#define LEN_MISMATCH_INT_STATUS (1<<2)
#define IFG_ERR_INT_STATUS (1<<1)
#define FALSE_CARRIER_INT_STATUS (1<<0)


#define MSG_MAC_ADDR_H 0x24*4
#define MSG_MAC_ADDR_L 0x25*4
#define MSG_CFG        0x26*4
