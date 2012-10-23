
#include "opl_driver.h"
#ifndef INCLUDE_HDLC_H
#define INCLUDE_HDLC_H

#define HDLC_MODE_ID_LOOP 0
#define HDLC_MASK_ID_RXEN 1
#define HDLC_MASK_ID_TXEN 2

#define OP_IPMUX_HDLC_ADDR  0xbf002C00

#define IPMUX_HDLC_INT_MASK_RXRY 0x00000001
#define IPMUX_HDLC_INT_MASK_RFF	 0x00000002
#define IPMUX_HDLC_INT_MASK_RFE	 0x00000004
#define IPMUX_HDLC_INT_MASK_FCSE 0x00000008
#define IPMUX_HDLC_INT_MASK_ABR  0x00000010
#define IPMUX_HDLC_INT_MASK_MF	 0x00000040
#define IPMUX_HDLC_INT_MASK_CDL	 0x00000080
#define IPMUX_HDLC_INT_MASK_RLT	 0x00000100
#define IPMUX_HDLC_INT_MASK_TFF	 0x00010000
#define IPMUX_HDLC_INT_MASK_TFE	 0x00020000
#define IPMUX_HDLC_INT_MASK_CTSL 0x00080000
#define IPMUX_HDLC_INT_MASK_TLT	 0x00100000

typedef struct IPMUX_HDLC_REGISTERS_s
{
	volatile UINT32 CFG ;	 /* 0x2C00 */
	volatile UINT32 MAXFL  ; /* 0x2C04 */	
	volatile UINT32 ADDR  ;  /* 0x2C08 */	
	volatile UINT32 TXD    ; /* 0x2C0C */	
	volatile UINT32 RXD    ; /* 0x2C10 */
	volatile UINT32 FSTA   ; /* 0x2C14 */	
	volatile UINT32 INT    ; /* 0x2C18 */	
	volatile UINT32 INTMSK ; /* 0x2C1C */	
	
	volatile UINT32 TxCNT  ; /* 0x2C20 */	
	volatile UINT32 RxCNT  ; /* 0x2C24 */	
	
	volatile UINT32 DISCNT_REMOVED ; /* 0x2C28 removed */	
	
	volatile UINT32 ABCNT  ; /* 0x2C2C : rx sequence 1 max then 7  */	
	volatile UINT32 CRCECNT; /* 0x2C30 */	
	
	volatile UINT32 BUFABORTCNT_REMOVED ; /* 0x2C34 removed */	
	
	volatile UINT32 RESERVED ; /* 0x2C3C */	
	
	volatile UINT32 FRMDROPCNT ; /* 0x2C3C:Rx hdlc frame drop counter(fifo full) , read clear. */	

} IPMUX_HDLC_REG_T;


enum IPMUX_HDLC_BYTE_STATUS_e
{
	IPMUX_HDLC_BYTE_STATUS_FIRST,
	IPMUX_HDLC_BYTE_STATUS_MIDDLE,
	IPMUX_HDLC_BYTE_STATUS_LAST,
	IPMUX_HDLC_BYTE_STATUS_BADFCS
};

typedef struct IPMUX_HDLC_STATUS_s
{
	UINT32 MASK;
	char comment[30];
} IPMUX_HDLC_STATUS_T;


#endif

