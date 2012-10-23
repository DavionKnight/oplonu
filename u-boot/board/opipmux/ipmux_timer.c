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
#include <common.h>
//#include <configs/opipmux.h>
#include "ipmux_timer.h"
#include "ipmux_int.h"

/* forward declarations */
static void regWrite32BitMask (UINT32 * addr, UINT32 data, UINT32 MASK);
static void regWrite32Bit(UINT32 *addr, UINT32 data);
static UINT32 regRead32BitMask (UINT32 * addr, UINT32 MASK);
static UINT32 regRead32Bit (UINT32 * addr);
extern __inline__ mips_cp0_get(u32 reg_num);
/*******************************************************************************
*
* opl_timer_enable  
*
* DESCRIPTION:
* 	This function enable the timer .
*
* INPUTS:
*	timerIndex:
*		0=timer1
*		1=timer2
*		2=timer3
*		3=timer4
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	n/a.
*
* SEE ALSO: 
*/
void opl_timer_enable(UINT8 timerIndex)
{
	volatile  TIMER_STRUCT_REGISTER* p=(volatile TIMER_STRUCT_REGISTER*)TIMER_BASE_ADRS;
	regWrite32BitMask((UINT32*)(&(p->CON[timerIndex])),0xFFFFFFFF,0x00000001);
}


/*******************************************************************************
*
* opl_timer_disable  
*
* DESCRIPTION:
* 	This function disable the timer .
*
* INPUTS:
*	timerIndex:
*		0=timer1
*		1=timer2
*		2=timer3
*		3=timer4
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	
*
* SEE ALSO: 
*/
void opl_timer_disable(UINT8 timerIndex)
{
	volatile  TIMER_STRUCT_REGISTER* p = (volatile TIMER_STRUCT_REGISTER*)TIMER_BASE_ADRS;

	//printf("----------------opl_timer_disable 1 p->CON[i]=%x\n",p->CON[timerIndex]);

	regWrite32BitMask((UINT32*)(&(p->CON[timerIndex])),0x0,0x00000001);
	//printf("----------------opl_timer_disable 2 p->CON[i]=%x\n",p->CON[timerIndex]);
}


/*******************************************************************************
*
* opl_timer_mode_set  
*
* DESCRIPTION:
* 	This function set the timer mode.
*
* INPUTS:
*	timerIndex:
*		0=timer1
*		1=timer2
*		2=timer3
*		3=timer4
*
*	When timer is enable
*		MOD =0 when matched, timer running on
*		MOD =1 when matched, stop timer
*		MOD =2 when matched reset timer
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	
*
* SEE ALSO: 
*/
void opl_timer_mode_set(UINT8 timerIndex ,UINT8 mode)
{
	volatile TIMER_STRUCT_REGISTER  *p=(volatile TIMER_STRUCT_REGISTER  *)TIMER_BASE_ADRS;
	//printf("opl_timer_mode_set mode=%x",mode);
	regWrite32BitMask((UINT32*)(&(p->CON[timerIndex])),mode<<1,0x00000006);
}


/*******************************************************************************
*
* opl_timer_reset  
*
* DESCRIPTION:
* 	This function reset the timer.
*
* INPUTS:
*	timerIndex:
*		0=timer1
*		1=timer2
*		2=timer3
*		3=timer4
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	
*
* SEE ALSO: 
*/
void opl_timer_reset(UINT8 timerIndex)
{
	 volatile TIMER_STRUCT_REGISTER  *p=(volatile TIMER_STRUCT_REGISTER  *)TIMER_BASE_ADRS;	
	regWrite32BitMask((UINT32*)(&(p->CON[timerIndex])),0xFFFFFFFF,0x00000008);
}

/*******************************************************************************
*
* opl_timer_int_enable  
*
* DESCRIPTION:
* 	This function enable the timer interrupt.
*
* INPUTS:
*	timerIndex:
*		0=timer1
*		1=timer2
*		2=timer3
*		3=timer4
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	
*
* SEE ALSO: 
*/
void opl_timer_int_enable(UINT8 timerIndex)
{
	 volatile TIMER_STRUCT_REGISTER  *p=(volatile TIMER_STRUCT_REGISTER  *)TIMER_BASE_ADRS;
	
	regWrite32BitMask((UINT32*)(&(p->CON[timerIndex])),0xFFFFFFFF,0x00000080);
}

/*******************************************************************************
*
* opl_timer_int_disable  
*
* DESCRIPTION:
* 	This function disable the timer interrupt  .
*
* INPUTS:
*	timerIndex:
*		0=timer1
*		1=timer2
*		2=timer3
*		3=timer4
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	
*
* SEE ALSO: 
*/
void opl_timer_int_disable(UINT8 timerIndex)
{
	 volatile TIMER_STRUCT_REGISTER  *p=(volatile TIMER_STRUCT_REGISTER  *)TIMER_BASE_ADRS;
	
	regWrite32BitMask((UINT32*)(&(p->CON[timerIndex])),0x0000000,0x00000080);
}

/*******************************************************************************
*
* opl_timer_matchvalue_set  
*
* DESCRIPTION:
* 	This function set the timer matched count value.
*
* INPUTS:
*	timerIndex:
*		0=timer1
*		1=timer2
*		2=timer3
*		3=timer4
*	matchValue	-	the timer matched count value.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	
*
* SEE ALSO: 
*/
void opl_timer_matchvalue_set(UINT8 timerIndex,UINT32 matchValue)
{
	 volatile TIMER_STRUCT_REGISTER  *p=(volatile TIMER_STRUCT_REGISTER  *)TIMER_BASE_ADRS;

	regWrite32Bit((UINT32*)(&(p->MR[timerIndex])),matchValue);
}


/*******************************************************************************
*
* opl_timer_matchvalue_get  
*
* DESCRIPTION:
* 	This function get the timer matched count value.
*
* INPUTS:
*	timerIndex:
*		0=timer1
*		1=timer2
*		2=timer3
*		3=timer4
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	The timer matched count value.
*
* SEE ALSO: 
*/
UINT32 opl_timer_matchvalue_get(UINT8 timerIndex)
{
	volatile TIMER_STRUCT_REGISTER  *p=(volatile TIMER_STRUCT_REGISTER  *)TIMER_BASE_ADRS;

	return regRead32Bit((UINT32*)(&(p->MR[timerIndex])));
}


/*******************************************************************************
*
* opl_timer_currentvalue_get  
*
* DESCRIPTION:
* 	This function get current count value of the timer.
*
* INPUTS:
*	timerIndex:
*		0=timer1
*		1=timer2
*		2=timer3
*		3=timer4
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	The current count value of the timer.
*
* SEE ALSO: 
*/
UINT32 opl_timer_currentvalue_get(UINT8 timerIndex)
{
	 volatile TIMER_STRUCT_REGISTER  *p=(volatile TIMER_STRUCT_REGISTER  *)TIMER_BASE_ADRS;
	
	return regRead32Bit((UINT32*)(&(p->CV[timerIndex])));
}

/*******************************************************************************
*
* opl_timer_prescaler_set  
*
* DESCRIPTION:
* 	  This function reset the timer Prescaler value. The prescaler is programmed 
*   to divide the clock input by a value between 1 and 256. 
*   A 0x00 value divides the clock by 1; 0xFF divides it by 256.
* INPUTS:
*	timerIndex:
*		0=timer1
*		1=timer2
*		2=timer3
*		3=timer4
*
*	prescaler:
*		the timer Prescaler value.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	
*
* SEE ALSO: 
*/
void opl_timer_prescaler_set(UINT8 timerIndex,UINT8 prescaler)
{
	 volatile TIMER_STRUCT_REGISTER  *p=(volatile TIMER_STRUCT_REGISTER  *)TIMER_BASE_ADRS;
	
	regWrite32BitMask((UINT32*)(&(p->CON[timerIndex])),prescaler<<8,0x0000FF00);
}

/*******************************************************************************
*
* opl_timer_prescaler_get  
*
* DESCRIPTION:
* 	  This function read the timer Prescaler value. 
* INPUTS:
*	timerIndex:
*		0=timer1
*		1=timer2
*		2=timer3
*		3=timer4
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	
*
* SEE ALSO: 
*/
UINT32 opl_timer_prescaler_get(UINT8 timerIndex)
{
	volatile TIMER_STRUCT_REGISTER  *p=(volatile TIMER_STRUCT_REGISTER  *)TIMER_BASE_ADRS;
	return regRead32BitMask((UINT32*)(&(p->CON[timerIndex])),0xFF00);
}

static void regWrite32BitMask (UINT32 * addr, UINT32 data, UINT32 MASK)
{
	UINT32 oldValue;
	
	oldValue=regRead32BitMask(addr,~MASK);
	oldValue |=(data & MASK);

	//printf("	>>> write addr %x val %x (data=%x MASK=%x data & MASK=%x)\n",addr,oldValue,data,MASK, data & MASK);
	*((volatile UINT32 *)addr) = oldValue;   
}

static UINT32 regRead32BitMask (UINT32 * addr, UINT32 MASK)
{
	return (*((volatile UINT32 *)addr) & MASK);
}


static void regWrite32Bit(UINT32 *addr, UINT32 data)
{
	*((volatile UINT32 *)addr) = data;   
}

static UINT32 regRead32Bit (UINT32 * addr)
{
	return (*((volatile UINT32 *)addr));
}

void init_ipmux_timer_int_ctrl(void)
{
	volatile INT_CTRL *p_int=(volatile INT_CTRL *)INT_CTRL_ADDR;

	/* enable all interrupt */
	p_int->ENABLE1=0x1E000000; /* timer */

	/* IPMux Interrupt generator enable */
	p_int->CFG=0x1;
	printf(" p_int->ENABLE1=%x\n",p_int->ENABLE1);

}

void ipmux_timer_int_ctrl_disable(void)
{
	volatile INT_CTRL *p_int=(volatile INT_CTRL *)INT_CTRL_ADDR;

	/* enable all interrupt */
	p_int->ENABLE1=p_int->ENABLE1 & (~0x1E000000); /* timer */
	p_int->CFG=0x1;
	printf(" p_int->ENABLE1=%x\n",p_int->ENABLE1);
}

int get_ipmux_hw0_int(void)
{
	return	mips_cp0_get(CP0_ID_CAUSE)>>10 & 0x1;
}

#ifdef INCLUDE_PERIPHA_TEST	
	
int test_ipmux_timer_int_disable (void)
{
	volatile INT_CTRL *p_int=(volatile INT_CTRL *)INT_CTRL_ADDR;

	int i=0;
	 volatile TIMER_STRUCT_REGISTER  *p=(volatile TIMER_STRUCT_REGISTER  *)TIMER_BASE_ADRS;	
	record_time_def;
	print_cp0reg_int();

	mips_cp0_set(CP0_ID_COMPARE,0x0); 
	print_cp0reg_int();
	mips_cp0_set(CP0_ID_STATUS,0x40fb05);   //ie = bit0	

	ipmux_timer_int_ctrl_disable();		//disalbe int in it.
	
	for(i=0;i<4;i++)
	{
		printf(" ****** test ipmux timer  int/EN %d****** \n",i);
		
		print_cp0reg_int();
		
		opl_timer_disable(i);	

		opl_timer_int_enable(i);

		opl_timer_mode_set(i,0);
		opl_timer_prescaler_set(i,24);	
			
		printf("	set match time value = %d \n",10000000 );
		opl_timer_matchvalue_set(i,10000000);  

		printf("	MR=0x%x\n",p->MR[i]);
		opl_timer_enable(i);

		record_time;
		
		printf("	CON=0x%x\n",p->CON[i]);
		while(1)
		{
			udelay(2000000);
			print_cp0reg_int();
			//printf("	timer current v = 0x%x\n",p->CV[i]);

			if(get_ipmux_hw0_int() )
			{	
				printf("     --------result--------\n");
				printf("	timer current v = %d\n",p->CV[i]);
				print_record_time;
				printf("	PASSED: interrupt occured, Please check the time passed is equal to real time\n");
				
				print_cp0reg_int();
				//clear the int
				printf("	clear the int\n");
				printf("		CON=0x%x p->CON[i]&0x40=%x\n",p->CON[i],p->CON[i]&0x40);
				print_cp0reg_int();
				break;
			}
			
		}
				
		printf(" ------DONE %d ------\n\n",i);
	}
}
	
int test_ipmux_timer_int_Prescaler (void)
{
	volatile INT_CTRL *p_int=(volatile INT_CTRL *)INT_CTRL_ADDR;

	int i=0;
	 volatile TIMER_STRUCT_REGISTER  *p=(volatile TIMER_STRUCT_REGISTER  *)TIMER_BASE_ADRS;	
	record_time_def;
	print_cp0reg_int();

	mips_cp0_set(CP0_ID_COMPARE,0x0); 
	print_cp0reg_int();
	mips_cp0_set(CP0_ID_STATUS,0x40fb05);   //ie = bit0	

	init_ipmux_timer_int_ctrl();		//disalbe int in it.
	
	for(i=0;i<4;i++)
	{
		printf(" ****** test ipmux timer  int/EN %d****** \n",i);
		
		print_cp0reg_int();
		
		opl_timer_disable(i);	

		opl_timer_int_enable(i);

		opl_timer_mode_set(i,0);
		opl_timer_prescaler_set(i,24);	
			
		printf("	set match time value = %d \n",10000000 );
		opl_timer_matchvalue_set(i,10000000);  

		printf("	MR=0x%x\n",p->MR[i]);
		opl_timer_enable(i);

		record_time;
		
		printf("	CON=0x%x\n",p->CON[i]);
		while(1)
		{
			udelay(2000000);
			print_cp0reg_int();
			//printf("	timer current v = 0x%x\n",p->CV[i]);

			if(get_ipmux_hw0_int() )
			{	
				printf("     --------result--------\n");
				printf("	timer current v = %d\n",p->CV[i]);
				print_record_time;
				printf("	PASSED: interrupt occured, Please check the time passed is equal to real time\n");
				
				print_cp0reg_int();
				//clear the int
				printf("	clear the int\n");
				printf("		CON=0x%x p->CON[i]&0x40=%x\n",p->CON[i],p->CON[i]&0x40);
				print_cp0reg_int();
				break;
			}
	
			
			/* if(p->CV[0]>=0x3005)
			if(p->CV[i]>=10000000+1)
			{
				opl_timer_reset(i);
				printf("	reset timer,0x%x\n",p->CON[i]);
			}
			*/
			
		}
				
		printf(" ------DONE %d ------\n\n",i);
	}
}



int test_ipmux_timer_reset(void)
{
	int i=0;
	 volatile TIMER_STRUCT_REGISTER  *p=(volatile TIMER_STRUCT_REGISTER  *)TIMER_BASE_ADRS;	
	record_time_def;
	
	for(i=0;i<4;i++)
	{
		printf(" ****** test ipmux timer  reset %d****** \n",i);
		opl_timer_disable(i);	

		opl_timer_int_enable(i);
		
		opl_timer_mode_set(i,0x2);
		
		opl_timer_prescaler_set(i,24);	
			
		printf("	set match time value = %d \n",10000000 ); //30s
		opl_timer_matchvalue_set(i,10000000);  

		printf("	MR=0x%x\n",p->MR[i]);
		opl_timer_enable(i);

		record_time;
		printf("	CON=0x%x\n",p->CON[i]);
		while(1)
		{
			//printf("    cv=0x%x\n",p->CV[i]);	
			if(p->CON[i]&0x40)
			{	
				printf("     --------result--------\n");	
				printf("	timer current v = %d\n",p->CV[i]);
				print_record_time;
				printf("	interrupt occured: cv arrive 10000000\n");
				printf("	CON=0x%x\n",p->CON[i]);

				
				if(p->CV[i]!=0x0)
				{
					printf("	***warning, because of reset, the value should be zero\n");
				}
				print_cp0reg_int();
				break;
			}
			
		
		}

		printf(" ------DONE %d ------\n\n",i);
	}
}



int test_ipmux_timer_continue(void)
{
	int i=0;
	 volatile TIMER_STRUCT_REGISTER  *p=(volatile TIMER_STRUCT_REGISTER  *)TIMER_BASE_ADRS;	
	record_time_def;
	
	for(i=0;i<4;i++)
	{
		printf(" ****** test ipmux timer  continue %d****** \n",i);
		opl_timer_disable(i);	

		opl_timer_int_enable(i);
		
		opl_timer_mode_set(i,0x0);
		
		opl_timer_prescaler_set(i,24);	
			
		printf("	set match time value = %d \n",10000000 ); //30s
		opl_timer_matchvalue_set(i,10000000);  

		printf("	MR=0x%x\n",p->MR[i]);
		opl_timer_enable(i);

		record_time;
		printf("	CON=0x%x\n",p->CON[i]);
		while(1)
		{
			//udelay(400000);
			//printf("	timer current v = %d\n",p->CV[i]);
			if(p->CV[i]> 10000000+1)
			{
				printf("     --------result--------\n");
				printf("	timer current v = %d\n",p->CV[i]);
				printf("	PASSED \n");
				
				break;
			}	
		
		}

		printf(" ------DONE %d ------\n\n",i);
	}
}


int test_ipmux_timer_stop(void)
{
	int i=0;
	 volatile TIMER_STRUCT_REGISTER  *p=(volatile TIMER_STRUCT_REGISTER  *)TIMER_BASE_ADRS;	
	record_time_def;
	
	for(i=0;i<4;i++)
	{
		printf(" ****** test ipmux timer  stop %d****** \n",i);
		opl_timer_disable(i);	

		opl_timer_int_enable(i);
		
		opl_timer_mode_set(i,0x1);
		
		opl_timer_prescaler_set(i,24);	
			
		printf("	set match time value = %d \n",10000000 ); //30s
		opl_timer_matchvalue_set(i,10000000);  

		printf("	MR=0x%x\n",p->MR[i]);
		opl_timer_enable(i);

		record_time;
		printf("	CON=0x%x\n",p->CON[i]);
		while(1)
		{
			//printf("    cv=0x%x\n",p->CV[i]);	
			if(p->CON[i]&0x40)
			{	

				printf("     --------result--------\n");
				print_record_time;
				
				printf("	interrupt occured: cv arrive 10000000\n");
				printf("	CON=0x%x\n",p->CON[i]);
				printf("	timer current v=%d\n",p->CV[i]);
				
				if(p->CV[i]!=0x0)
				{
					printf("	***warning, because of reset, the value should be zero\n");
				}
				print_cp0reg_int();
				break;
			}
			
		
		}

		printf(" ------DONE %d ------\n\n",i);
	}
}

void test_ipmux_timer(void)
{
	test_ipmux_timer_int_disable();
	return;
	test_ipmux_timer_int_Prescaler ();

	test_ipmux_timer_continue();
	test_ipmux_timer_reset();
	test_ipmux_timer_stop();

}
#endif









