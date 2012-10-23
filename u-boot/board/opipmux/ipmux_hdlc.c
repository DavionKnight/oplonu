/*
=============================================================================
Header Name: Ipmux_hdlc.c

  General Description:
  
	this file includes the routines that read and accumulate,clear,show register counter values.
	===============================================================================
	Opulan Confidential Proprietary                     
	ID and version: xxxxxxxxxxxxxx  Version 1.00
	(c) Copyright Opulan XXXX - XXXX, All Rights Reserved
	
	  
		Revision History:
		Modification
		Author			Date				Description of Changes
		---------------	---------------	----------------------------------------------
		xwang   		04/17/2006		Initial Version
		---------------	---------------	----------------------------------------------
*/

#include "ipmux_hdlc.h"

IPMUX_HDLC_STATUS_T HDLCMODE[]={
	{0x00020000,"External loopback"},
	{0x00001000,"Receive enable"},
	{0x00000800,"Transmission enable"}
};

static void hdlcWrite32BitMask (UINT32 * addr, UINT32 data, UINT32 MASK);
static UINT32 hdlcRead32BitMask (UINT32 * addr, UINT32 MASK);
void ipmuxHdlcInit(void);
void ipmuxHdlcLoopSet(int loop);
void ipmuxHdlcMaxFrameSet(UINT16 len);
void ipmuxHdlcIntDisable(UINT32 mask);
void ipmuxHdlcIntEnable(UINT32 mask);
UINT32 ipmuxHdlcIntGet(void);
UINT32 ipmuxHdlcRxReady(void);
void ipmuxHdlcAddrSet(UINT16 addr);
void ipmuxHdlcByteSend(char ValByte, int last);
int ipmuxHdlcByteRecv(char *c);
UINT32 ipmuxHdlcTxCounterGet(void);
UINT32 ipmuxHdlcBufAbortCounterGet(void);
UINT32 ipmuxHdlcRxCounterGet(void);
UINT32 ipmuxHdlcAbortCounterGet(void);
UINT32 ipmuxHdlcCrcErrCounterGet(void);
UINT32 ipmuxHdlcFrmDropCounterGet(void);


static void hdlcWrite32BitMask (UINT32 * addr, UINT32 data, UINT32 MASK)
{
	UINT32 oldValue;
	oldValue=hdlcRead32BitMask(addr,~MASK);
	oldValue |=(data & MASK);
	*((volatile UINT32 *)addr) = oldValue;
}

static UINT32 hdlcRead32BitMask (UINT32 * addr, UINT32 MASK)
{
	return (*((volatile UINT32 *)addr) & MASK);
}

/*******************************************************************************
*
* ipmuxHdlcInit
*
* DESCRIPTION:
* 	This function init the IPMux HDLC.
*
* INPUTS:
*	N/A.
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	N/A.
*
* SEE ALSO:
*
*/
void ipmuxHdlcInit(void)
{
	volatile IPMUX_HDLC_REG_T *p=(volatile IPMUX_HDLC_REG_T *)OP_IPMUX_HDLC_ADDR;
	/*enable tx and rx*/
	p->CFG=HDLCMODE[HDLC_MASK_ID_TXEN].MASK | HDLCMODE[HDLC_MASK_ID_RXEN].MASK;
}


/*******************************************************************************
*
* ipmuxHdlcLoopSet
*
* DESCRIPTION:
* 	This function set the the IPMux HDLC External loopback .
*
* INPUTS:
*	loop	-  1 to loop mode; 0 to normal mode.
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	N/A.
*
* SEE ALSO:
*
*/
void ipmuxHdlcLoopSet(int loop)
{
	volatile IPMUX_HDLC_REG_T *p=(volatile IPMUX_HDLC_REG_T *)OP_IPMUX_HDLC_ADDR;
	if (loop)
	{
		hdlcWrite32BitMask((UINT32 *)&(p->CFG), HDLCMODE[HDLC_MODE_ID_LOOP].MASK ,HDLCMODE[HDLC_MODE_ID_LOOP].MASK);
	}
	else
	{
		hdlcWrite32BitMask((UINT32 *)&(p->CFG),0x00000000,0x00000000);
	}
}

/*******************************************************************************
*
* ipmuxHdlcConfigPrint
*
* DESCRIPTION:
* 	This function print HDLC curent configurations.
*
* INPUTS:
*	N/A.
*
* OUTPUT:
*	HDLC configurations.
*
* RETURNS:
*	N/A.
*
* SEE ALSO:
*

static void ipmuxHdlcConfigPrint(void)
{
	u32 mask;
	volatile IPMUX_HDLC_REG_T *p=(volatile IPMUX_HDLC_REG_T *)OP_IPMUX_HDLC_ADDR;
	int i;
	for(i=0; i< sizeof(HDLCMODE); i++)
	{
		if (HDLCMODE[i].MASK & p->CFG)
		{
			printf("%x %s\n",HDLCMODE[i].MASK & p->CFG,HDLCMODE[i].comment);
		}
	}
	if (p->CFG & 0x4)
	{
		mask=0xff;
	}
	else
	{
		mask=0xf;
	}
	printf("address=%x\n", (p->ADDR) & mask);
}
*/

/*******************************************************************************
*
* ipmuxHdlcMaxFrameSet
*
* DESCRIPTION:
* 	This function set the Max frame length of the incoming HDLC frame.
*
* INPUTS:
*	len	- the Max frame length of the incoming HDLC frame.
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	N/A.
*
* SEE ALSO:
*
*/
void ipmuxHdlcMaxFrameSet(UINT16 len)
{
	volatile IPMUX_HDLC_REG_T *p=(volatile IPMUX_HDLC_REG_T *)OP_IPMUX_HDLC_ADDR;
	p->MAXFL=(len);
}


/*******************************************************************************
*
* ipmuxHdlcIntDisable
*
* DESCRIPTION:
* 	This function disable the HDLC interrupt.
*
* INPUTS:
*	mask	- the HDLC int mask.
*        IPMUX_HDLC_INT_MASK_RXRY : Receive ready
*        IPMUX_HDLC_INT_MASK_RFF  : Receive fifo full
*        IPMUX_HDLC_INT_MASK_RFE  : Receive fifo empty
*        IPMUX_HDLC_INT_MASK_FCSE : Receive frame FCS error
*        IPMUX_HDLC_INT_MASK_ABR  : Receive abort frame
*        IPMUX_HDLC_INT_MASK_MF	  : Long beyond the MFL frame received 
*        IPMUX_HDLC_INT_MASK_CDL  : CD lost    
*        IPMUX_HDLC_INT_MASK_RLT  : Greater than Receive trigger level 
*        IPMUX_HDLC_INT_MASK_TFF  : Transmit fifo full 
*        IPMUX_HDLC_INT_MASK_TFE  : Transmit fifo empty 
*        IPMUX_HDLC_INT_MASK_CTSL : CTS lost                                       
*        IPMUX_HDLC_INT_MASK_TLT  : Less than Transmit trigger level 
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	N/A.
*
* SEE ALSO:
*
*/
void ipmuxHdlcIntDisable(UINT32 mask)
{
	volatile IPMUX_HDLC_REG_T *p=(volatile IPMUX_HDLC_REG_T *)OP_IPMUX_HDLC_ADDR;
	p->INTMSK=p->INTMSK & ~mask;
}


/*******************************************************************************
*
* ipmuxHdlcIntEnable
*
* DESCRIPTION:
* 	This function enable the IPMux HDLC interrupt.
*
* INPUTS:
*	mask	- the HDLC int mask.
*        IPMUX_HDLC_INT_MASK_RXRY : Receive ready
*        IPMUX_HDLC_INT_MASK_RFF  : Receive fifo full
*        IPMUX_HDLC_INT_MASK_RFE  : Receive fifo empty
*        IPMUX_HDLC_INT_MASK_FCSE : Receive frame FCS error
*        IPMUX_HDLC_INT_MASK_ABR  : Receive abort frame
*        IPMUX_HDLC_INT_MASK_MF	  : Long beyond the MFL frame received 
*        IPMUX_HDLC_INT_MASK_CDL  : CD lost    
*        IPMUX_HDLC_INT_MASK_RLT  : Greater than Receive trigger level 
*        IPMUX_HDLC_INT_MASK_TFF  : Transmit fifo full 
*        IPMUX_HDLC_INT_MASK_TFE  : Transmit fifo empty 
*        IPMUX_HDLC_INT_MASK_CTSL : CTS lost                                       
*        IPMUX_HDLC_INT_MASK_TLT  : Less than Transmit trigger level 
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	N/A.
*
* SEE ALSO:
*
*/
void ipmuxHdlcIntEnable(UINT32 mask)
{
	volatile IPMUX_HDLC_REG_T *p=(volatile IPMUX_HDLC_REG_T *)OP_IPMUX_HDLC_ADDR;
	p->INTMSK=p->INTMSK | mask;
}


/*******************************************************************************
*
* ipmuxHdlcIntGet
*
* DESCRIPTION:
* 	This function get the IPMux HDLC interrupt.
*
* INPUTS:
*	mask	- the HDLC int mask.
*        IPMUX_HDLC_INT_MASK_RXRY : Receive ready
*        IPMUX_HDLC_INT_MASK_RFF  : Receive fifo full
*        IPMUX_HDLC_INT_MASK_RFE  : Receive fifo empty
*        IPMUX_HDLC_INT_MASK_FCSE : Receive frame FCS error
*        IPMUX_HDLC_INT_MASK_ABR  : Receive abort frame
*        IPMUX_HDLC_INT_MASK_MF	  : Long beyond the MFL frame received 
*        IPMUX_HDLC_INT_MASK_CDL  : CD lost    
*        IPMUX_HDLC_INT_MASK_RLT  : Greater than Receive trigger level 
*        IPMUX_HDLC_INT_MASK_TFF  : Transmit fifo full 
*        IPMUX_HDLC_INT_MASK_TFE  : Transmit fifo empty 
*        IPMUX_HDLC_INT_MASK_CTSL : CTS lost                                       
*        IPMUX_HDLC_INT_MASK_TLT  : Less than Transmit trigger level 
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	N/A.
*
* SEE ALSO:
*
*/
UINT32 ipmuxHdlcIntGet(void)
{
	volatile IPMUX_HDLC_REG_T *p=(volatile IPMUX_HDLC_REG_T *)OP_IPMUX_HDLC_ADDR;
	return p->INT;
}


/*******************************************************************************
*
* ipmuxHdlcRxReady
*
* DESCRIPTION:
* 	This function get the IPMux HDLC interrupt.
*
* INPUTS:
*	mask	- the HDLC int mask.
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	N/A.
*
* SEE ALSO:
*
*/
UINT32 ipmuxHdlcRxReady(void)
{
	return IPMUX_HDLC_INT_MASK_RXRY & ipmuxHdlcIntGet();
}


/*******************************************************************************
*
* ipmuxHdlcAddrSet
*
* DESCRIPTION:
* 	This function set the recognition address of the incoming HDLC.
*
* INPUTS:
*	addr	- the recognition address of the incoming HDLC .
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	N/A.
*
* SEE ALSO:
*
*/
void ipmuxHdlcAddrSet(UINT16 addr)
{
	volatile IPMUX_HDLC_REG_T *p=(volatile IPMUX_HDLC_REG_T *)OP_IPMUX_HDLC_ADDR;
	p->ADDR=addr;
}



/*******************************************************************************
*
* ipmuxHdlcByteSend
*
* DESCRIPTION:
* 	This function invoke the IPMux HDLC sending the byte to interface.
*
* INPUTS:
*	ValByte	- the value to be sent.
*	last	- if the last byte of the packet
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	N/A.
*
* SEE ALSO:
*
*/
void ipmuxHdlcByteSend(char ValByte, int last)
{
	volatile IPMUX_HDLC_REG_T *p=(volatile IPMUX_HDLC_REG_T *)OP_IPMUX_HDLC_ADDR;
	while  (p->INT & 0x10000)
	{
	}	
	p->TXD=(0xFF & ValByte) | last << 8;
	
	//printf("<%c>",(0xFF & ValByte) );
}

/*******************************************************************************
*
* ipmuxHdlcByteRecv
*
* DESCRIPTION:
* 	This function Read the data from the IPMux HDLC interface.
*
* INPUTS:
*	N/A.
*
* OUTPUT:
*	the data from the hdlc interface.
*
* RETURNS:
*	the received data type.
*        IPMUX_HDLC_BYTE_STATUS_FIRST  : the first byte of the packet                    
*        IPMUX_HDLC_BYTE_STATUS_MIDDLE : the middle bype of the packet                   
*        IPMUX_HDLC_BYTE_STATUS_LAST   : the last byte of the packet with the correct FCS
*        IPMUX_HDLC_BYTE_STATUS_BADFCS : the last byte of the packet with the bad FCS    
*
* SEE ALSO:
*
*/
int ipmuxHdlcByteRecv(char *c)
{
	volatile IPMUX_HDLC_REG_T *p=(volatile IPMUX_HDLC_REG_T *)OP_IPMUX_HDLC_ADDR;
	u32 v;
	v=p->RXD;
	*c=v  & 0xFF;
	return (v & 0x300) >> 8;
}


/*******************************************************************************
*
* ipmuxHdlcTxCounterGet
*
* DESCRIPTION:
* 	This function Read the counter value of transmit frame from the IPMux HDLC.
*
* INPUTS:
*	N/A.
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	
* SEE ALSO:
*	The counter value of transmit frame.
*/
UINT32 ipmuxHdlcTxCounterGet(void)
{
	volatile IPMUX_HDLC_REG_T *p=(volatile IPMUX_HDLC_REG_T *)OP_IPMUX_HDLC_ADDR;
	
	return p->TxCNT;

}


/*******************************************************************************
*
* ipmuxHdlcBufAbortCounterGet
*
* DESCRIPTION:
*	This function Read the counter value of buffer abort frame from the IPMux HDLC.
*
* INPUTS:
*	N/A.
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	
* SEE ALSO:
*	The counter value of buffer abort frame.

UINT32 ipmuxHdlcBufAbortCounterGet(void)
{
	volatile IPMUX_HDLC_REG_T *p=(volatile IPMUX_HDLC_REG_T *)OP_IPMUX_HDLC_ADDR;
	
	return p->BUFABORTCNT;

}
*/

/*******************************************************************************
*
* ipmuxHdlcRxCounterGet
*
* DESCRIPTION:
* 	This function Read the counter value of transmit frame from the IPMux HDLC.
*
* INPUTS:
*	N/A.
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	The counter value of receive frame.
* SEE ALSO:
*	
*/
UINT32 ipmuxHdlcRxCounterGet(void)
{
	volatile IPMUX_HDLC_REG_T *p=(volatile IPMUX_HDLC_REG_T *)OP_IPMUX_HDLC_ADDR;
	
	return p->RxCNT;
	
}


/*******************************************************************************
*
* ipmuxHdlcAbortCounterGet
*
* DESCRIPTION:
* 	This function Read the counter value of sequence number of 1 wrong frames from the IPMux HDLC.
*
* INPUTS:
*	N/A.
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	The counter value of receive frame.
* SEE ALSO:
*	
*/
UINT32 ipmuxHdlcAbortCounterGet(void)
{
	volatile IPMUX_HDLC_REG_T *p=(volatile IPMUX_HDLC_REG_T *)OP_IPMUX_HDLC_ADDR;
	
	return p->ABCNT;
}



/*******************************************************************************
*
* ipmuxHdlcCrcErrCounterGet
*
* DESCRIPTION:
* 	This function Read the counter value of CRC error frame from the IPMux HDLC.
*
* INPUTS:
*	N/A.
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	
* SEE ALSO:
*	The counter value of CRC error frame.
*/
UINT32 ipmuxHdlcCrcErrCounterGet(void)
{
	volatile IPMUX_HDLC_REG_T *p=(volatile IPMUX_HDLC_REG_T *)OP_IPMUX_HDLC_ADDR;
	
	return p->CRCECNT;
}

/*******************************************************************************
*
* ipmuxHdlcDiscCounterGet
*
* DESCRIPTION:
* 	This function Read the counter value of abort frame from the IPMux HDLC.
*
* INPUTS:
*	N/A.
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	The counter value of abort frame.
* SEE ALSO:
*	
*/
UINT32 ipmuxHdlcFrmDropCounterGet(void)
{
	volatile IPMUX_HDLC_REG_T *p=(volatile IPMUX_HDLC_REG_T *)OP_IPMUX_HDLC_ADDR;
	
	return p->FRMDROPCNT;
}


	
