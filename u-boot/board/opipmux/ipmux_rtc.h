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
/*#include "commonforAPI.h"*/
#ifndef _IPMUX_RTC_H_
#define _IPMUX_RTC_H_ 1
#include "opl_driver.h"

#ifndef _OPIPMUX_H_
#include <config.h>
#endif

#define printErr(a,b) printf(a,b);
/*<<<<===============from pub.h===============*/

/* macro to check that <v> is between <l> and <h> inclusive range */
#define	BOOL	int
#define	CHECK_RANGE(v,l,h)	(((v)>=(l))&&((v)<=(h)))
#ifdef FPGA_ONU_16B
#define	IPMUX_RTC_BASE_ADDR 0xbf002d80    /*wwei mark*/
#else
#define	IPMUX_RTC_BASE_ADDR 0xbf002980
#endif

/* defines */
#define RTC_YEAR           1
#define RTC_MONTH		   2  
#define RTC_DAY_OF_MONTH   3 
#define RTC_DAY_OF_WEEK    4 
#define RTC_HOUR		   5 
#define RTC_MINUTE         6 
#define RTC_SEC         7

#define RTC_YEAR_RANGE_MAX  4095
#define RTC_YEAR_RANGE_MIN  0
/* register define  */ 
#define IPMUX_RTC_OFFSET_CLOCK_TICK_COUNTER 0x0
#define IPMUX_RTC_OFFSET_CLOCK_TICK_COUNTER_MASK 0x0FFFFFFF

#define IPMUX_RTC_OFFSET_CLKEN        0x4
#define IPMUX_RTC_OFFSET_CLKEN_MASK   0x80000000

#define IPMUX_RTC_OFFSET_WP           0x4
#define IPMUX_RTC_OFFSET_WP_MASK      0x40000000

#define IPMUX_RTC_OFFSET_IMYEAR       0x4
#define IPMUX_RTC_OFFSET_IMYEAR_MASK  0x00800000

#define IPMUX_RTC_OFFSET_IMMON        0x4
#define IPMUX_RTC_OFFSET_IMMON_MASK   0x00400000

#define IPMUX_RTC_OFFSET_IMDOY        0x4
#define IPMUX_RTC_OFFSET_IMDOY_MASK   0x00200000

#define IPMUX_RTC_OFFSET_IMDOW        0x4
#define IPMUX_RTC_OFFSET_IMDOW_MASK   0x00100000

#define IPMUX_RTC_OFFSET_IMDOM        0x4
#define IPMUX_RTC_OFFSET_IMDOM_MASK   0x00080000

#define IPMUX_RTC_OFFSET_IMHOUR       0x4
#define IPMUX_RTC_OFFSET_IMHOUR_MASK  0x00040000

#define IPMUX_RTC_OFFSET_IMMIN        0x4
#define IPMUX_RTC_OFFSET_IMMIN_MASK   0x00020000

#define IPMUX_RTC_OFFSET_IMSEC        0x4
#define IPMUX_RTC_OFFSET_IMSEC_MASK   0x00010000

#define IPMUX_RTC_OFFSET_CTCRST       0x4
#define IPMUX_RTC_OFFSET_CTCRST_MASK  0x00008000

#define IPMUX_RTC_OFFSET_CMYEAR       0x4
#define IPMUX_RTC_OFFSET_CMYEAR_MASK  0x00000080

#define IPMUX_RTC_OFFSET_CMMON        0x4
#define IPMUX_RTC_OFFSET_CMMON_MASK   0x00000040

#define IPMUX_RTC_OFFSET_CMDOY        0x4
#define IPMUX_RTC_OFFSET_CMDOY_MASK   0x00000020

#define IPMUX_RTC_OFFSET_CMDOW        0x4
#define IPMUX_RTC_OFFSET_CMDOW_MASK   0x00000010

#define IPMUX_RTC_OFFSET_CMDOM        0x4
#define IPMUX_RTC_OFFSET_CMDOM_MASK   0x00000008

#define IPMUX_RTC_OFFSET_CMHOUR       0x4
#define IPMUX_RTC_OFFSET_CMHOUR_MASK  0x00000004

#define IPMUX_RTC_OFFSET_CMMIN        0x4
#define IPMUX_RTC_OFFSET_CMMIN_MASK   0x00000002

#define IPMUX_RTC_OFFSET_CMSEC        0x4
#define IPMUX_RTC_OFFSET_CMSEC_MASK   0x00000001

#define IPMUX_RTC_OFFSET_IYEAR        0x8
#define IPMUX_RTC_OFFSET_IYEAR_MASK   0x00800000

#define IPMUX_RTC_OFFSET_IMON         0x8
#define IPMUX_RTC_OFFSET_IMON_MASK    0x00400000

#define IPMUX_RTC_OFFSET_IDOY         0x8
#define IPMUX_RTC_OFFSET_IDOY_MASK    0x00200000

#define IPMUX_RTC_OFFSET_IDOW         0x8
#define IPMUX_RTC_OFFSET_IDOW_MASK    0x00100000

#define IPMUX_RTC_OFFSET_IDOM         0x8
#define IPMUX_RTC_OFFSET_IDOM_MASK    0x00080000

#define IPMUX_RTC_OFFSET_IHOUR        0x8
#define IPMUX_RTC_OFFSET_IHOUR_MASK   0x00040000

#define IPMUX_RTC_OFFSET_IMIN         0x8
#define IPMUX_RTC_OFFSET_IMIN_MASK    0x00020000

#define IPMUX_RTC_OFFSET_ISEC         0x8
#define IPMUX_RTC_OFFSET_ISEC_MASK    0x00010000

#define IPMUX_RTC_OFFSET_ICMYEAR      0x8
#define IPMUX_RTC_OFFSET_ICMYEAR_MASK 0x00000080

#define IPMUX_RTC_OFFSET_ICMMON       0x8
#define IPMUX_RTC_OFFSET_ICMMON_MASK  0x00000040

#define IPMUX_RTC_OFFSET_ICMDOY       0x8
#define IPMUX_RTC_OFFSET_ICMDOY_MASK  0x00000020

#define IPMUX_RTC_OFFSET_ICMDOW       0x8
#define IPMUX_RTC_OFFSET_ICMDOW_MASK  0x00000010

#define IPMUX_RTC_OFFSET_ICMDOM       0x8
#define IPMUX_RTC_OFFSET_ICMDOM_MASK  0x00000008

#define IPMUX_RTC_OFFSET_ICMHOUR      0x8
#define IPMUX_RTC_OFFSET_ICMHOUR_MASK 0x00000004

#define IPMUX_RTC_OFFSET_ICMMIN       0x8
#define IPMUX_RTC_OFFSET_ICMMIN_MASK  0x00000002

#define IPMUX_RTC_OFFSET_ICMSEC       0x8
#define IPMUX_RTC_OFFSET_ICMSEC_MASK  0x00000001

#define IPMUX_RTC_OFFSET_DATE_WEEK         0xc
#define IPMUX_RTC_OFFSET_DATE_WEEK_MASK    0x07000000

#define IPMUX_RTC_OFFSET_HOUR         0xc
#define IPMUX_RTC_OFFSET_HOUR_MASK    0x001f0000

#define IPMUX_RTC_OFFSET_MINUTE       0xc
#define IPMUX_RTC_OFFSET_MINUTE_MASK  0x00003f00

#define IPMUX_RTC_OFFSET_SEC       0xc
#define IPMUX_RTC_OFFSET_SEC_MASK  0x0000003f

#define IPMUX_RTC_OFFSET_YEAR         0x10
#define IPMUX_RTC_OFFSET_YEAR_MASK    0x0fff0000

#define IPMUX_RTC_OFFSET_MON        0x10
#define IPMUX_RTC_OFFSET_MON_MASK   0x00000F00

#define IPMUX_RTC_OFFSET_DATE         0x10
#define IPMUX_RTC_OFFSET_DATE_MASK    0x0000001f

#define IPMUX_RTC_OFFSET_DATE_YEAR    0x14
#define IPMUX_RTC_OFFSET_DATE_YEAR_MASK 0x000001ff

#define IPMUX_RTC_OFFSET_ADOM         0x18
#define IPMUX_RTC_OFFSET_ADOM_MASK    0x1f000000

#define IPMUX_RTC_OFFSET_AHOUR        0x18
#define IPMUX_RTC_OFFSET_AHOUR_MASK   0x001f0000

#define IPMUX_RTC_OFFSET_AMIN         0x18
#define IPMUX_RTC_OFFSET_AMIN_MASK    0x00003f00

#define IPMUX_RTC_OFFSET_ASEC         0x18
#define IPMUX_RTC_OFFSET_ASEC_MASK    0x0000003f

#define IPMUX_RTC_OFFSET_AYEAR        0x1c
#define IPMUX_RTC_OFFSET_AYEAR_MASK   0xfff00000

#define IPMUX_RTC_OFFSET_AMON       0x1c
#define IPMUX_RTC_OFFSET_AMON_MASK  0x000f0000

#define IPMUX_RTC_OFFSET_ADOY         0x1c
#define IPMUX_RTC_OFFSET_ADOY_MASK    0x00001ff8

#define IPMUX_RTC_OFFSET_ADOW         0x1c
#define IPMUX_RTC_OFFSET_ADOW_MASK    0x00000007

#define IPMUX_RTC_OFFSET_PREINT       0x20
#define IPMUX_RTC_OFFSET_PREINT_MASK  0x1fff0000

#define IPMUX_RTC_OFFSET_PREFRC       0x20
#define IPMUX_RTC_OFFSET_PREFRC_MASK  0x00007fff

/* rtc_drv.c */
void rtcDateTimeGet(struct rtc_time *dateTime);
void rtcInit(void);
void rtcEnable(BOOL enalbe);
void rtcEnableWrite(BOOL enable);
void rtcDateSet(int year, int month, int dayOfMonth, int dayOfWeek);
void rtcAlarmDateSet(int year, int month, int dayOfMonth, int dayOfWeek);
void rtcTimeSet(int hour, int minute, int SEC);
void rtcAlarmTimeSet(int hour, int minute, int SEC);
void rtcAlarmEnable(UINT8 mask);
void rtcAlarmDisable(UINT8 mask);
void rtcIncrementIntEnable(UINT32 mask);
void rtcIncrementIntDisable(UINT32 mask);
void rtcIntHandle(void);
void rtcBaseClockSet(UINT16 preInteger, UINT16 preFraction);

int rtcGetDayOfYear(void);
int rtcGetYear(void);
int rtcGetMonth(void);
int rtcGetDay(void);
int rtcGetDayOfWeek(void);
int rtcGetHour(void);
int rtcGetMin(void);
int rtcGetSec(void);

void rtcPrintTime(void);


//#define LINUX  1

#endif
/*---------------------------- End of rtc_drv.h -------------------------------*/




