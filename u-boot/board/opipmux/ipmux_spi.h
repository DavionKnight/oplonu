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

#define SPI_CTL 0x24
#define SPI_SS 0x01
#define SPI_START 0x01
#define SPI_RESPIN_START 0x02
#define SPI_RESPIN_BUF0_READY 0x01
#define SPI_RESPIN_BUF1_READY 0x02
#define SPI_RESPIN_BUF_SIZE 0x04 /* 4 bytes */

#define SPI_AGENT_READ_OFS ((((*(volatile u32 *)(0xBF000000))&0xF)==1)?0:0x10)

#define SPI_ADDR	0xBF000c00
#define IPMUX_REG_BASE 0xbf000000

typedef struct ipmux_spi {
	volatile unsigned int tr0; /* transmit register */
	volatile unsigned int tr1; /* transmit register */
	volatile unsigned int tr2; /* transmit register */
	volatile unsigned int tr3; /* transmit register */
	
	volatile unsigned int rr0; /* receive register */
	volatile unsigned int rr1; /* receive register */
	volatile unsigned int rr2; /* receive register */
	volatile unsigned int rr3; /* receive register */
	
	volatile unsigned int div; /* divide frequency */

	volatile unsigned int len; /* length of data to transmit*/

	volatile unsigned int ctl; /* control register */

	volatile unsigned int ss;  /* slave select */

	volatile unsigned int str; /* start a transmit */
		

}IPMUX_SPI;


typedef struct onu_respin_spi {
	volatile unsigned int tr0; /* transmit register */
	volatile unsigned int tr1; /* transmit register */
	volatile unsigned int tr2; /* transmit register */
	volatile unsigned int tr3; /* transmit register */
	volatile unsigned int tr4; /* transmit register */
	volatile unsigned int tr5; /* transmit register */
	volatile unsigned int tr6; /* transmit register */
	volatile unsigned int tr7; /* transmit register */
	
	volatile unsigned int rr0; /* receive register */
	volatile unsigned int rr1; /* receive register */
	volatile unsigned int rr2; /* receive register */
	volatile unsigned int rr3; /* receive register */
	volatile unsigned int rr4; /* receive register */
	volatile unsigned int rr5; /* receive register */
	volatile unsigned int rr6; /* receive register */
	volatile unsigned int rr7; /* receive register */

	volatile unsigned int div; /* divide frequency */

	volatile unsigned int len; /* length of data to transmit*/

	volatile unsigned int ctl; /* control register */

	volatile unsigned int ss;  /* slave select */

	volatile unsigned int str; /* start a transmit */

	volatile unsigned int intr; /* interrupt register */

	volatile unsigned int csdly;  /* CS delay */

	volatile unsigned int ready; /* buffer ready */

}ONU_RESPIN_SPI;



#define  	FLASH_UNKNOWN  	0xFFFF
#define 	FLASH_MAN_ATMEL	0x1f
#define 	FLASH_AT26_ATMEL	0x47

#define  FLASH_MAN_MX 0xc2
#define  FLASH_MX_L320 0x20
#define  FLASH_MAN_WB 0xef
#define  FLASH_WB_25X16 0x30
#define  FLASH_MAN_SST 0xbf
#define  FLASH_MAN_ST 0x20
#define  FLASH_SST_25VF 0x25

#define SECTOR_SIZE 0x10000
#define BOOTLOADER_ADDR ((((*(volatile u32 *)(0xBF000000))&0xF)==1)?(0xb0000000):(0xbfc00000))

#define BOOTLOADER_BEGIN 0
#define BOOTLOADER_SECTOR 4

#define ENV_BEGIN 4
#define ENV_SECTOR 1
#endif /*__OP_UART_H__*/

