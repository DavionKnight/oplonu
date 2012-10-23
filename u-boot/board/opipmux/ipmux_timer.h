/******************************************************************************/
/**  COPYRIGHT (C) 2003- Opulan, INC. ALL RIGHTS RESERVED.                   **/
/**--------------------------------------------------------------------------**/
/** This software embodies materials and concepts which are proprietary and  **/
/** confidential to Opulan, Inc.                                      		 **/
/**--------------------------------------------------------------------------**/
/**                                                                          **/
/**                             Revision History                             **/
/**--------------------------------------------------------------------------**/
/**	Rev #       Date		Author		Description                          **/
/**	-----       ------		-------		-----------                          **/
/**	1.0	    2005-8-5 14:43	xwang		Initial version.                     **/
/******************************************************************************/
#include "opl_driver.h"
#define TIMER_UBOOT 

#ifdef FPGA_ONU_16B
#define	TIMER_BASE_ADRS	0xbf002d00     /* wwei mark */
#else
#define	TIMER_BASE_ADRS	0xbf002900
#endif

typedef struct _TIMER_STRUCT_REGISTER{
	volatile unsigned int CON[4];		// Timer1,2,3,4 control register
	volatile unsigned int MR[4]  ;		// Timer1,2,3,4 match register
	volatile unsigned int CV[4] ;		// Timer1,2,3,4 Current count value
} TIMER_STRUCT_REGISTER;


/*---------------------------- End of TIMER_drv.h -------------------------------*/

























