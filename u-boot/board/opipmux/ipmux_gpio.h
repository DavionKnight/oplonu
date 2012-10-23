/******************************************************************************/
/**  COPYRIGHT (C) 2005- Opulan, INC. ALL RIGHTS RESERVED.                   **/
/**--------------------------------------------------------------------------**/
/** This software embodies materials and concepts which are proprietary and  **/
/** confidential to Opulan, Inc.                                             **/
/**--------------------------------------------------------------------------**/
/**                                                                          **/
/******************************************************************************/

#ifndef __IPMUX_GPIO_H__
#define __IPMUX_GPIO_H__

#ifndef _OPIPMUX_H_
#include <config.h>
#endif

#include "opl_driver.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef FPGA_ONU_16B
#define IPMUX_GPIO_BASEADDR 0xbf002c80    /* wwei mark */
#else
#define IPMUX_GPIO_BASEADDR 0xbf002880
#endif
	
typedef struct IPMUX_GPIO_REGISTERS_s
{
	volatile UINT32 val;
	volatile UINT32 dir;
	volatile UINT32 set;
	volatile UINT32 clr;
	volatile UINT32 intMode;
	volatile UINT32 intLevel;
	volatile UINT32 intEdge;
} IPMUX_GPIO_REGISTERS_S;

#ifdef __cplusplus
	}
#endif

#endif 

