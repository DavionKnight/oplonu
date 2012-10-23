/******************************************************************************/
/**  COPYRIGHT (C) 2003- Opulan, INC. ALL RIGHTS RESERVED.                   **/
/**--------------------------------------------------------------------------**/
/** This software embodies materials and concepts which are proprietary and  **/
/** confidential to Opulan, Inc.                                             **/
/**--------------------------------------------------------------------------**/
/**                                                                          **/
/******************************************************************************/

#ifndef __OP_UART_H__
#define __OP_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_IPMUX_2

typedef struct uart
{
	union{
		volatile unsigned int uisr;
		volatile unsigned int ufiocr;
		volatile unsigned int enfr;
		volatile unsigned int res0[2];
	} w0;

	union{
		volatile unsigned int uier;
		volatile unsigned int dir1;
		volatile unsigned int res1[4];
	} w1;

	union{
		volatile unsigned int urhr;
		volatile unsigned int uthr;
		volatile unsigned int dir0;
		volatile unsigned int res2[3];
	} w2;

	volatile unsigned int lcr;

	volatile unsigned int mcr;

	volatile unsigned int lsr;

	volatile unsigned int msr;

	volatile unsigned int scr;

   // volatile unsigned int efr;
} IPMUX_2_REG_UART_t;

/* Line Control Register */

#define CHAR_LEN_5	0x00
#define CHAR_LEN_6	0x01
#define CHAR_LEN_7	0x02
#define CHAR_LEN_8	0x03
#define LCR_STB		0x04
#define ONE_STOP	0x00
#define TWO_STOP	0x04
#define ONE_HALF_STOP   0x40
#define LCR_PEN		0x08
#define PARITY_NONE	0x00
#define PARITY_ODD	0x08  
#define PARITY_EVEN	0x18 
#define FORCE_PARITY_1 0x28
#define FORCE_PARITY_0 0x38

#define LCR_EPS		0x10
#define LCR_SP		0x20
#define LCR_SBRK	0x40
#define LCR_DLAB	0x80
#define DLAB		LCR_DLAB


/* Line Status Register */

#define LSR_DR		0x01
#define RxCHAR_AVAIL	LSR_DR
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_FERR	0x80

/* Interrupt Status Register */

#define ISR_IP		0x01
#define ISR_ID		0x0e
#define ISR_RLS		0x06
#define Rx_INT		ISR_RLS
#define ISR_RDA		0x04
#define RxFIFO_INT	ISR_RDA
#define ISR_THRE	0x02
#define TxFIFO_INT	ISR_THRE
#define ISR_MSTAT	0x00
#define ISR_TIMEOUT	0x0c

/* Interrupt Enable Register */

#define IER_ERDAI	0x01
#define RxFIFO_BIT	IER_ERDAI
#define IER_ETHREI	0x02
#define TxFIFO_BIT	IER_ETHREI
#define IER_ELSI	0x04
#define Rx_BIT		IER_ELSI
#define IER_RTS		0x40
#define IER_CTS		0x80

/* Modem Control Register */

#define MCR_DTR		0x01
#define DTR		MCR_DTR
#define MCR_RTS		0x02
#define MCR_OUT1	0x04
#define MCR_OUT2	0x08
#define MCR_LOOP	0x10

/* Register offsets from base address */

#define RHR	0x00
#define THR	0x00
#define DLL	0x00
#define IER	0x01
#define DLM	0x01
#define IIR	0x02
#define FCR	0x02
#define LCR	0x03
#define MCR	0x04
#define LSR	0x05
#define MSR	0x06
#define SCR	0x07

/* Modem Status Register */

#define MSR_DCTS	0x01
#define MSR_DDSR	0x02
#define MSR_TERI	0x04
#define MSR_DDCD	0x08
#define MSR_CTS		0x10
#define MSR_DSR		0x20
#define MSR_RI		0x40
#define MSR_DCD		0x80

/* FIFO Control Register */

#define FCR_EN		0x01
#define FIFO_ENABLE	FCR_EN
#define FCR_RXCLR	0x02
#define RxCLEAR		FCR_RXCLR
#define FCR_TXCLR	0x04
#define TxCLEAR		FCR_TXCLR
#define FCR_DMA		0x08
#define FCR_RXTRIG_L	0x40
#define FCR_RXTRIG_H	0x80
#define RX_TRIG_CLR	0x3f
#define RX_TRIG_8	0x00
#define RX_TRIG_16	0x40
#define RX_TRIG_32	0x80
#define RX_TRIG_48	0xc0
#define TX_TRIG_CLR	0xcf
#define TX_TRIG_8	0x10
#define TX_TRIG_16	0x00
#define TX_TRIG_32	0x20
#define TX_TRIG_48	0x30

typedef struct CommPara
{
	char datalen;
	char stopbit;
	char parity;
} IPMUX_UART_PARA_t;

enum COMMAND_REQ{
	BAUD_RATE=0,
	COMM_PARA,
	LOOPBACK_ENABLE,
	LOOPBACK_DISABLE,
	BLOCK_ENABLE,
	BLOCK_DISABLE,
	DMA_ENABLE,
	DMA_DISABLE,
	RX_TRIG_SET,
	TX_TRIG_SET
};

#endif

#ifdef __cplusplus
	}
#endif

#endif // __OP_UART_H__

