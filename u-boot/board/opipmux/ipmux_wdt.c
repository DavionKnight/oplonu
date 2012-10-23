/************************************************************************************/
/**  COPYRIGHT (C) 2005- Opulan, INC. ALL RIGHTS RESERVED.                         **/
/**--------------------------------------------------------------------------------**/
/** This software embodies materials and concepts which are proprietary and        **/
/** confidential to Opulan, Inc.                                                   **/
/**--------------------------------------------------------------------------------**/
/**                                                                                **/
/**    	MODULE     : synchro file source file                                      **/
/**    	FILE       : rtc.c                                                         **/
/**                                                                                **/
/**     DESCRIPTION:                                                               **/
/**                                                                                **/
/**       This provides a library of routines to set and retrieve the date         **/
/**       from the real time clock on the IPMUX rtc chip .                         **/
/**                                                                                **/
/**       If just used for display, displaying the value returned as               **/
/**       a hex value will give the desired results.  If the user wants            **/
/**       to do any mathematical operations with the data - such as subtraction,   **/
/**       the user is responsible for converting and/or taking into account        **/
/**       that the data is not stored in standard decimal format.                  **/ 
/**                                                                                **/
/**     modification history                                                       **/
/**     	--------------------                                                   **/
/**	        2005-8-1 11:23 xwang  written                                          **/
/**                                                                                **/
/************************************************************************************/

/*includes*/ 
#include "ipmux_wdt.h"
#include <common.h>

/* forward declarations */
void opl_wdt_init(int div_val, int timeout_value,int resetEnable);
void opl_wdt_enable(void);
void opl_wdt_disable(void);
void opl_wdt_feed(void);
void opl_wdt_set_timeoutvalue(int timeout_value);
unsigned int opl_wdt_getcurtime(void);

static void regWrite32BitMask(unsigned int *addr, unsigned int data, unsigned int MASK);
static unsigned int regRead32BitMask(unsigned int *addr, unsigned int MASK);
static unsigned int regRead32Bit (unsigned int * addr);
/*******************************************************************************
*
* opl_wdt_getcurtime
*
* DESCRIPTION:
* 	This function get the current value of Watchdog timer value.
*
*
* INPUTS:
*	N/A.
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	The current value of Watchdog timer value.
*
* SEE ALSO: 
*/
unsigned int opl_wdt_getcurtime()
{
	volatile wdt_struct_register *p=(volatile wdt_struct_register *)WDT_BASE_ADRS;
	return regRead32Bit((unsigned int *)&(p->WDVAL));	
}


/*******************************************************************************
*
* opl_wdt_enable
*
* DESCRIPTION:
* 	This function enable watch dog timer.
*
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
*/
void opl_wdt_enable(void)
{
	volatile wdt_struct_register *p=(volatile wdt_struct_register *)WDT_BASE_ADRS;

	regWrite32BitMask((unsigned int *)&(p->WDMOD),0x1,0x00000001);

}


/*******************************************************************************
*
* opl_wdt_resetenable
*
* DESCRIPTION:
* 	This function enable reset whole system when time out.
*
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
*/
void opl_wdt_resetenable(void)
{
	volatile wdt_struct_register *p=(volatile wdt_struct_register *)WDT_BASE_ADRS;
	regWrite32BitMask((unsigned int *)&(p->WDMOD),0x2,0x00000002);
	
}



/*******************************************************************************
*
* opl_wdt_resetenable
*
* DESCRIPTION:
* 	This function enable reset whole system when time out.
*
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
*/
void opl_wdt_resetedisable(void)
{
	volatile wdt_struct_register *p=(volatile wdt_struct_register *)WDT_BASE_ADRS;
	regWrite32BitMask((unsigned int *)&(p->WDMOD),0x0,0x00000002);
	
}

/*******************************************************************************
*
* opl_wdt_timeout
*
* DESCRIPTION:
* 	
*	This function check if the watch dog timer timeout.
*
* INPUTS:
*	N/A.
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	1,                   the watch dog timer timeout.
*	0,                   the watch dog timer doesn't timeout.
*
* SEE ALSO: 
*/
unsigned int opl_wdt_timeout(void)
{
	volatile wdt_struct_register *p=(volatile wdt_struct_register *)WDT_BASE_ADRS;
	if(p->WDMOD&0x04)
		return 1;
	else
		return 0;
//	return (regRead32BitMask(&(p->WDMOD)&0x04))>>2;
	
}

/*******************************************************************************
*
* opl_wdt_disable 
*
* DESCRIPTION:
* 	This function disable watch dog timer.
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
*/
void opl_wdt_disable(void)
{
	volatile wdt_struct_register *p=(volatile wdt_struct_register *)WDT_BASE_ADRS;
	regWrite32BitMask((unsigned int *)&(p->WDMOD),0x0,0x00000001);
}


/*******************************************************************************
*
* opl_wdt_setdiv
*
* DESCRIPTION:
* 	This function set the system clock divide scaler of watch dog timer. 
*
* INPUTS:
*	div_val	-	the system clock divide scaler of watch dog timer. 
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	N/A.
*
* SEE ALSO: 
*/
void opl_wdt_setdiv(unsigned int div_val)
{
	volatile wdt_struct_register *p=(volatile wdt_struct_register *)WDT_BASE_ADRS;
	regWrite32BitMask((unsigned int *)&(p->WDMOD),div_val<<16,0xFFFF0000);		
}


/*******************************************************************************
*
* opl_wdt_init
*
* DESCRIPTION:
* 	This function config the watch dog timer and start it.
*
* INPUTS:
*	div_val		-	the system clock divide scaler of watch dog timer. 
*	timeout_value	-	the watch dog timer time out value.
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	N/A.
*
* SEE ALSO: 
*/
void opl_wdt_init(int div_val, int timeout_value,int resetEnable)
{


	opl_wdt_disable();

	opl_wdt_setdiv(div_val); //24

	opl_wdt_set_timeoutvalue(timeout_value);

	if (resetEnable)
	   opl_wdt_resetenable();
	else
	   opl_wdt_resetedisable();	
   

	opl_wdt_enable();

	/* test for timeout  */ 
	opl_wdt_feed();
}


/*******************************************************************************
*
* opl_wdt_feed
*
* DESCRIPTION:
* 	This function generate the service sequence to reload the timer.
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
*/
void opl_wdt_feed()
{
	volatile wdt_struct_register *p=(volatile wdt_struct_register *)WDT_BASE_ADRS;
	regWrite32BitMask((unsigned int *)&(p->WDFEED),0xaa,0x000000FF);
	regWrite32BitMask((unsigned int *)&(p->WDFEED),0x55,0x000000FF);

}


/*******************************************************************************
*
* opl_wdt_set_timeoutvalue
*
* DESCRIPTION:
* 	This function set the timerout value for watch dog timer.
*
* INPUTS:
*	timeout_value	-	the watch dog timer time out value.
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	N/A.
*
* SEE ALSO: 
*/
void opl_wdt_set_timeoutvalue(int timeout_value )
{
	volatile wdt_struct_register *p=(volatile wdt_struct_register *)WDT_BASE_ADRS;
	regWrite32BitMask((unsigned int *)&(p->WDConst),timeout_value,0xFFFFFFFF);
}



static void regWrite32BitMask (unsigned int * addr, unsigned int data, unsigned int MASK)
{
	unsigned int oldValue;
	
	oldValue=regRead32BitMask(addr,~MASK);
	oldValue |=(data & MASK);
	
	*((volatile unsigned int *)addr) = oldValue;   
}

static unsigned int regRead32BitMask (unsigned int * addr, unsigned int MASK)
{
	return (*((volatile unsigned int *)addr) & MASK);
}

static unsigned int regRead32Bit (unsigned int * addr)
{
	return (*((volatile unsigned int *)addr));
}

#ifdef INCLUDE_PERIPHA_TEST	
static void print_config()
{	
	volatile wdt_struct_register *p=(volatile wdt_struct_register *)WDT_BASE_ADRS;	
	printf("	[current config:]\n");
	printf("		time out value = 0x%x.\n",p->WDConst);
	printf("		div value = 0x%x.\n",(p->WDMOD & 0xFFFF0000)>>16);
	printf("		mode= 0x%x.\n",p->WDMOD );
	printf("		reset enable= 0x%x.\n",(p->WDMOD & 0x2) >> 1);
}

int test_ipmux_wdt_timeout_int()
{

	record_time_def;

	int i=0;

	printf("******test edt  timeout_int******\n");
	opl_wdt_init(24,10000000,0);		//test scaler , reset time 10s
	//opl_wdt_init(12,192307692,0);	//test scaler, reset time 100s

	record_time;

	print_config();

	while (1)
	{
		i++;
		i=i%10000000;
		if (opl_wdt_getcurtime()<0x4000)  
		{
			print_record_time;
			printf("	cur 1 WDVAL= 0x%x,i=%d\n",opl_wdt_getcurtime(),i);
			print_config();

		}
/*
		if ((p->WDMOD)&0x04) 
		{
			print_record_time;
			printf("	cur WDVAL= 0x%x\n",opl_wdt_getcurtime());
			printf("	WDMOD=0x%x,time out !\n",p->WDMOD);
			break;
		}
		*/
	}
	
	printf(" 	------DONE------ \n\n");
	return 0;
}

int test_ipmux_wdt_reset()
{

	int i=0;
	record_time_def;
	volatile wdt_struct_register *p=(volatile wdt_struct_register *)WDT_BASE_ADRS;	

	printf("******test edt  reset:reset board 10 seconds later ******\n");
	opl_wdt_init(24,10000000,1);

	
	//opl_wdt_resetenable();
	print_config();
	record_time;	
	
	//printf ("new %x/%x/%x (%x) \nTime: %x:%x:%x\n",tmp.tm_mon,tmp.tm_mday,tmp.tm_year,tmp.tm_yday,tmp.tm_hour,tmp.tm_min,tmp.tm_sec); 
	
	/* test for timeout  */ 
	opl_wdt_feed();
	printf("	cur = 0x%x\n",opl_wdt_getcurtime());
	
	while (1)
	{
		i++;
		i=i%10000000;
		if (i%3000000==0)  
		{
			printf("	cur 1= 0x%x,i=%d\n",opl_wdt_getcurtime(),i);
			print_record_time;
		}
			
		//if(opl_wdt_getcurtime()==0) printf("	WDMOD=0x%x\n",p->WDMOD);
		//opipmux_sync();
		//printf("	cur = 0x%x\n",opl_wdt_getcurtime());
		if ((p->WDMOD)&0x04) 
		{
			print_record_time;
			printf("	cur 2= 0x%x\n",opl_wdt_getcurtime());
			printf("	WDMOD=0x%x,time out !\n",p->WDMOD);
			break;
		}
	}
	
	printf(" 	------DONE------ \n\n");
	return 0;
}

int test_ipmux_wdt()
{
	test_ipmux_wdt_timeout_int();
	test_ipmux_wdt_reset();
	return 0;
}

#endif







