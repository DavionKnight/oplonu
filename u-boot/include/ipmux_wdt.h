/******************************************************************************/
/**  COPYRIGHT (C) 2003- Opulan, INC. ALL RIGHTS RESERVED.                   **/
/**--------------------------------------------------------------------------**/
/** This software embodies materials and concepts which are proprietary and  **/
/** confidential to Opulan, Inc.                                      		 **/
/**--------------------------------------------------------------------------**/
/**                                                                          **/
/**                             Revision History                             **/
/**--------------------------------------------------------------------------**/
/**	Rev #       Date		         Author		Description                          **/
/**	-----       ------		-------		-----------                          **/
/**	1.0	    2005-8-5 14:43	xwang		Initial version.                     **/
/******************************************************************************/

#ifndef _OPIPMUX_H_
#include <config.h>
#endif

/* #define DEBUG_RTC 0  */ 
//#define LINUX  
#define WDT_UBOOT 

/*
#ifdef WDT_LINUX
	#include <rtc.h>
	#include <common.h>
#endif*/

/* =============  for u-boot ============= */ 
#ifdef WDT_LINUX


#endif

//#define printErr(a,b) printf(a,b);
#define printErr(a,b)  ;
/*<<<<===============from pub.h===============*/


/* macro to check that <v> is between <l> and <h> inclusive range */

//e00*4
#ifdef FPGA_ONU_16B
#ifndef	WDT_BASE_ADRS
#define	WDT_BASE_ADRS	0xbf002c00     /* wwei mark */   
#endif
#else
#ifndef	WDT_BASE_ADRS
#define	WDT_BASE_ADRS	0xbf002800   
#endif
#endif /*FPGA_ONU_16B*/

typedef struct _wdt_struct_register{
	volatile unsigned int WDMOD ;	
	volatile unsigned int WDFEED ;
	volatile unsigned int WDConst  ;
	volatile unsigned int WDVAL ;
	
} wdt_struct_register;


/*---------------------------- End of WDT_drv.h -------------------------------*/

