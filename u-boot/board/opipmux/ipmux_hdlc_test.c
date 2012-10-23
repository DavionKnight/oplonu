#include <common.h>
#ifdef INCLUDE_PERIPHA_TEST
#include "opl_driver.h"
#include "ipmux_hdlc.h"

extern IPMUX_HDLC_STATUS_T HDLCMODE[];
/*******************************************************************************
*
* ipmuxHdlcTest
*
* DESCRIPTION:
* 	a simple test example.
*
* INPUTS:
*	N/A.
*
* OUTPUT:
*	the data from the hdlc.
*
* RETURNS:
*	N/A.
*
* SEE ALSO:
*
*/
void test_ipmux_hdlc()
{
	char s[] = "HDLC test";
	int i;
	char c;
	char result;
	ipmuxHdlcInit();
	printf("HDLC intial ok\n");
	for(i = 0; i < 8; i++)
	{
		ipmuxHdlcByteSend(s[i],0);
		result=ipmuxHdlcByteRecv(&c);
		switch(result) {
		case 0x0:
			printf("pck start:  %c\n",c);
			break;
		case 0x01:
			printf("pck mid:%c\n",c);
			break;
		case 0x10:
			printf("pck end:%c\n",c);
			break;
		case 0x11:
			printf("pck bad FCS:%c\n",c);
			break;
		default:
			break;
		}
	}
	
	ipmuxHdlcByteSend('E',1);
	result=ipmuxHdlcByteRecv(&c);
	switch(result) {
	case 0x0:
		printf("pck start:  %c\n",c);
		break;
	case 0x01:
		printf("pck mid:%c\n",c);
		break;
	case 0x10:
		printf("pck end:%c\n",c);
		break;
	case 0x11:
		printf("pck bad FCS:%c\n",c);
		break;
	default:
		break;
	}
}





/*******************************************************************************
*
* test_ipmux_hdlc_loopback
*
* DESCRIPTION:
* 	a simple test example of hdlc worked in software loop back mode.
*
* INPUTS:
*	N/A.
*
* OUTPUT:
*	the data from the hdlc.
*
* RETURNS:
*	N/A.
*
* SEE ALSO:
*
*/
void test_ipmux_hdlc_loopback()
{
	char s[] = "HDLC test";
	int i;
	int ilast;
	char c;
	
	char result;
	printf("------test hdlc in software loop back mode!------\n");
	ipmuxHdlcAddrSet(0x55);
	ipmuxHdlcInit();
	
	printf("HDLC intial ok\n");

	while (1)
	{
		if (ctrlc())
		{
			printf("\ninterrupted!\n");
			return ;
		}
		if (ipmuxHdlcRxReady())
		{
			result=ipmuxHdlcByteRecv(&c);
			ilast=0;
			switch(result) {
				case 0:
					printf("\nstart:<%c>",c);
					break;
				case 1:
					printf("<%c>",c);
					break;
				case 2:
					printf("<%c>\n",c);
					ilast=1;
					break;
				case 3:
					printf("<%c>",c);
					printf(" pck bad FCS\n");
					break;
				default:
					printf("result=<%d>",result);
					break;
				}
			ipmuxHdlcByteSend(c,ilast);
		}
	}
}

/*******************************************************************************
*
* test_ipmux_hdlc_recv
*
* DESCRIPTION:
* 	a simple test example of hdlc recv.
*
* INPUTS:
*	N/A.
*
* OUTPUT:
*	the data from the hdlc.
*
* RETURNS:
*	N/A.
*
* SEE ALSO:
*
*/
void test_ipmux_hdlc_recv()
{
	char s[] = "HDLC test";
	int i;
	char c='A';
	char result;
	printf("------test hdlc in recv mode!------\n");	
	ipmuxHdlcAddrSet(0x55);
	ipmuxHdlcInit();
	
	printf("HDLC intial ok\n");

	while (1)
	{
		if (ctrlc())
		{
			printf("interrupted!\n");
			return ;
		}

		if (ipmuxHdlcRxReady())
		{
			result=ipmuxHdlcByteRecv(&c);

			switch(result) {
			case 0:
				printf("\nstart:<%c>",c);
				break;
			case 1:
				printf("<%c>",c);
				break;
			case 2:
				printf("<%c>\n",c);
				break;
			case 3:
				printf("<%c>",c);
				printf(" pck bad FCS\n");
				break;
			default:
				printf("result=<%d>",result);
				break;
			}
		}
		
	}
}


void ipmuxHdlcStatusPrint()
{
	UINT32 status=0;
	status=ipmuxHdlcIntGet();
	printf("int : 0x%08x\n",status);
}



/*******************************************************************************
*
* test_ipmux_hdlc_tx_entx
*
* DESCRIPTION:
* 	a simple test example of hdlc recv.
*
* INPUTS:
*	N/A.
*
* OUTPUT:
*	the data from the hdlc.
*
* RETURNS:
*	N/A.
*
* SEE ALSO:
*
*/
void test_ipmux_hdlc_tx_entx()
{
	volatile IPMUX_HDLC_REG_T *p=(volatile IPMUX_HDLC_REG_T *)0xbf02C000;
	char s[] = "f01234567mn0abcdefghijk112233445566778899aabbccddeeffgghh";
	int i;
	char c='A';
	char result;
	printf("------test hdlc in tx_en mode!------\n");	
	ipmuxHdlcAddrSet(0x55);
	ipmuxHdlcInit();


	printf("HDLC intial ok\n");

	for (i=0;i<strlen(s)-1;i++)
	{
		ipmuxHdlcByteSend(s[i],0);
	}
	
	ipmuxHdlcByteSend(s[strlen(s)-1],1);

}


/*******************************************************************************
*
* test_ipmux_hdlc_tx_disabletx
*
* DESCRIPTION:
* 	a simple test example of hdlc recv.
*
* INPUTS:
*	N/A.
*
* OUTPUT:
*	the data from the hdlc.
*
* RETURNS:
*	N/A.
*
* SEE ALSO:
*
*/
void test_ipmux_hdlc_tx_disabletx()
{
	volatile IPMUX_HDLC_REG_T *p=(volatile IPMUX_HDLC_REG_T *)0xbf02C000;
	char s[] = "f01234567mn0abcdefghijk112233445566778899aabbccddeeffgghh";
	int i;
	char result;
	printf("------test hdlc in tx_en mode!------\n");	
	ipmuxHdlcAddrSet(0x55);
	ipmuxHdlcInit();


	/*gWrite32BitMask(&(p->CFG),HDLCMODE[HDLC_MASK_ID_TXEN].MASK & HDLCMODE[HDLC_MASK_ID_RXEN].MASK,HDLCMODE[HDLC_MASK_ID_TXEN].MASK & HDLCMODE[HDLC_MASK_ID_RXEN].MASK);*/
	p->CFG=HDLCMODE[HDLC_MASK_ID_RXEN].MASK;

	printf("HDLC intial ok\n");

	//ipmuxHdlcStrSend(s);

	for (i=0;i<strlen(s)-1;i++)
	{
		ipmuxHdlcByteSend(s[i],0);
	}
	
	ipmuxHdlcByteSend(s[strlen(s)-1],1);
	
	p->CFG=HDLCMODE[HDLC_MASK_ID_TXEN].MASK | HDLCMODE[HDLC_MASK_ID_RXEN].MASK;
}



/*******************************************************************************
*
* ipmuxHdlcCounterPrint
*
* DESCRIPTION:
* 	This function Read the counter values from the IPMux HDLC and print them.
*
* INPUTS:
*	N/A.
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	the counter values.
* SEE ALSO:
*	
*/
UINT32 ipmuxHdlcCounterPrint()
{
	UINT32 c=0;
	
	c=ipmuxHdlcRxCounterGet ();
	printf("%15.15s 0x%x=%d\n","rx",  c, c);

	c=ipmuxHdlcTxCounterGet ();
	printf("%15.15s 0x%x=%d\n","Tx",  c, c);
	
/*	c=ipmuxHdlcDiscCounterGet();
	printf("%15.15s 0x%x=%d\n","abort fm",  c, c);
*/
	c=ipmuxHdlcCrcErrCounterGet();
	printf("%15.15s 0x%x=%d\n","crc err ",  c, c);

	c=ipmuxHdlcAbortCounterGet ();
	printf("%15.15s 0x%x=%d\n","abort fm",  c, c);


	c=ipmuxHdlcFrmDropCounterGet();
	printf("%15.15s 0x%x=%d\n","frm drop",  c, c);
}



/*******************************************************************************
*
* ipmuxHdlcStrSend
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
void ipmuxHdlcStrSend(char *str)
{
	volatile IPMUX_HDLC_REG_T *p=(volatile struct IPMUX_HDLC_REG_T *)OP_IPMUX_HDLC_ADDR;
	int i=0;
	int ilen=strlen(str);
	
	ipmuxHdlcAddrSet(0x55);
	ipmuxHdlcInit();
	printf("Send str %s len %d...\n",str,ilen);
	p->TXD=0x66;		
	for(i=0;i<ilen-1;i++)
	{
		while  (p->INT & 0x10000)
		{
		}
		p->TXD=*str | 0 << 8;	
		str++;
	}

	/* set the last byte*/
	while  (p->INT & 0x10000)
	{
	}	
	p->TXD=*str | 1 << 8;	
}


/*******************************************************************************
*
* ipmuxHdlcMemSend
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
void ipmuxHdlcMemSend(char *addr,UINT32 len)
{

	volatile IPMUX_HDLC_REG_T *p=(volatile struct IPMUX_HDLC_REG_T *)OP_IPMUX_HDLC_ADDR;

	UINT32 lenRest =len;
	UINT32 lenOne=50;
	int i;
	printf("Send mem at %08x len %d...\n",addr,len);	
	p->TXD=0x66;
		
	while(lenRest >0)
	{
	
		for(i=0;i<(lenRest>=lenOne?lenOne:lenRest)-1;i++)
		{
			while  (p->INT & 0x10000)
			{
			}
			p->TXD=*addr;
			addr++;
		}
		
		p->TXD=*addr | (1 << 8);	/*because of fifo limit, seperate the mem to frames of size 50*/
		lenRest=lenRest-lenOne;

	}

}


#endif
