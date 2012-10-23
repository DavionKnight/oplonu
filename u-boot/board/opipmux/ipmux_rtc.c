/************************************************************************************/
/**  COPYRIGHT (C) 2005- Opulan, INC. ALL RIGHTS RESERVED.                         **/
/**--------------------------------------------------------------------------------**/
/** This software embodies materials and concepts which are proprietary and        **/
/** confidential to Opulan, Inc.                                                   **/
/**--------------------------------------------------------------------------------**/
/**                                                                                **/
/**    	MODULE  : rtc source file                                      **/
/**    	FILE        : ipmux_rtc.c                                                         **/
/**                                                                                **/
/**     DESCRIPTION:                                                               **/
/**                                                                                **/
/**       This provides a library of routines to set and retrieve the date         **/
/**       from the real time clock on the IPMUX Rtc chip .                         **/
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
#ifdef LINUX
#include <ipmuxRtc.h>
#include <common.h>
#endif
#include <common.h>
#include "opl_driver.h"

#include "ipmux_rtc.h"

//#define DEBUG_RTC 1
/* statics */
/*
static char *monthTable[12] ={
    "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
		"JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};
*/
static unsigned char daysInMo[] ={0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
/*
static char *dayTable [7] =
{
    "Sunday", "Monday", "Tuesday",
		"Wednesday", "Thursday", "Friday", "Saturday"
};
*/

/* forward declarations */
void ipmuxRtcGet (struct rtc_time *tmp);
void ipmuxRtcSet (struct rtc_time *tmp);
/*static int dayOfYear( int month, int day, int isLeapYear );*/
void ipmuxRtcEnable(OPL_BOOL enable);
void ipmuxRtcEnableWrite(OPL_BOOL enable);
OPL_STATUS ipmuxRtcDateSet(int year,	int month,int mDay, int wDay,int yDay);
void ipmuxRtcAlarmGet(struct rtc_time* tmp);
void ipmuxRtcAlarmSet(struct rtc_time* tmp);
void ipmuxRtcTimeSet( int hour, int minute, int SEC );
void ipmuxRtcAIntEnable(UINT8 mask);
void ipmuxRtcAIntDisable(UINT8 mask);
void ipmuxRtcIncIntEnable(UINT32 mask);
void ipmuxRtcIncIntDisable(UINT32 mask);
int ipmuxRtcIntHandle(void);
void ipmuxRtcIntClear(void);
static void rtcRegWrite32BitMask (UINT32 addr, UINT32 data, UINT32 MASK);
int rtcRegRead32BitMask (UINT32 addr, UINT32 MASK);
int getMaskShift(UINT32 MASK);

/*******************************************************************************
*
* ipmuxRtcDateTimeGet  
*
* DESCRIPTION:
* 	This function Reset the IPMux RTC. We setting the date back to 1970-01-01.
*
* INPUTS:
*	n/a.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	
*
* SEE ALSO: 
*/
void ipmuxRtcReset (void)
{
	/* enable write to register */ 
	ipmuxRtcEnableWrite(OPL_TRUE);
	
	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_CTCRST,0x1,	IPMUX_RTC_OFFSET_CTCRST_MASK);
	//ipmuxRtcDateSet(1970,1,1,1);
	//ipmuxRtcTimeSet(0,0,0);	

	ipmuxRtcEnableWrite(OPL_FALSE);	
}


/*******************************************************************************
*
* ipmuxRtcDateTimeGet  
*
* DESCRIPTION:
* 	This function Get the current time from the IPMux RTC.
*
* INPUTS:
*	tmp	: time struct.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	
*
* SEE ALSO: 
* 
*/
UINT32 ipmuxRtcCounterGet (void){
	return rtcRegRead32BitMask((UINT32) IPMUX_RTC_OFFSET_CLOCK_TICK_COUNTER, IPMUX_RTC_OFFSET_CLOCK_TICK_COUNTER_MASK); 
}



/*******************************************************************************
*
* ipmuxRtcDateTimeGet  
*
* DESCRIPTION:
* 	This function Get the current time from the IPMux RTC.
*
* INPUTS:
*	tmp	: time struct.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	
*
* SEE ALSO: 
* 
*/
void ipmuxRtcGet (struct rtc_time *tmp){
	tmp->tm_sec  = rtcRegRead32BitMask((UINT32) IPMUX_RTC_OFFSET_SEC, IPMUX_RTC_OFFSET_SEC_MASK); 
	tmp->tm_min  = rtcRegRead32BitMask((UINT32) IPMUX_RTC_OFFSET_MINUTE, IPMUX_RTC_OFFSET_MINUTE_MASK); 
	tmp->tm_hour = rtcRegRead32BitMask((UINT32) IPMUX_RTC_OFFSET_HOUR,   IPMUX_RTC_OFFSET_HOUR_MASK); 
	tmp->tm_wday = rtcRegRead32BitMask((UINT32) IPMUX_RTC_OFFSET_DATE_WEEK,   IPMUX_RTC_OFFSET_DATE_WEEK_MASK); 
	tmp->tm_mday = rtcRegRead32BitMask((UINT32) IPMUX_RTC_OFFSET_DATE,   IPMUX_RTC_OFFSET_DATE_MASK); 
	tmp->tm_mon  = rtcRegRead32BitMask((UINT32) IPMUX_RTC_OFFSET_MON,  IPMUX_RTC_OFFSET_MON_MASK); 
	tmp->tm_year = rtcRegRead32BitMask((UINT32) IPMUX_RTC_OFFSET_YEAR,   IPMUX_RTC_OFFSET_YEAR_MASK); 
	tmp->tm_yday = rtcRegRead32BitMask((UINT32) IPMUX_RTC_OFFSET_DATE_YEAR,   IPMUX_RTC_OFFSET_DATE_YEAR_MASK); 

	tmp->tm_isdst= 0;

	
#ifdef DEBUG_RTC
	printf("	Get ALARM DATE: %04d-%02d-%02d (wday=%d,yday=%d)  TIME: %2d:%02d:%02d\n",\
		tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,tmp->tm_yday,\
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
#endif
	
}


/*******************************************************************************
*
* ipmuxRtcDateTimeGet  
*
* DESCRIPTION:
* 	This function Set the IPMux RTC time.
*
* INPUTS:
*	tmp	: the time struct store the time to be set to IPMux RTC.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	
*
* SEE ALSO: 
*/
void ipmuxRtcSet (struct rtc_time *tmp)
{
#ifdef DEBUG_RTC
	printf("	Set DATE: %4d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d\n",
		tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
#endif
	ipmuxRtcDateSet(tmp->tm_year, tmp->tm_mon,tmp->tm_mday,tmp->tm_wday,tmp->tm_yday);
	ipmuxRtcTimeSet(tmp->tm_hour, tmp->tm_min,tmp->tm_sec);
}

/*******************************************************************************
*
* dayOfYear 
* DESCRIPTION:
* 	This function evaluate day of the year.
*
* INPUTS:
*	 month :month of year
*	 day   :date of month
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	day of the year.
*
* SEE ALSO: 


static int dayOfYear( int month, int day, int isLeapYear )
{
    int currentDay = 0;
	
    switch (month)
	{
	case 12:
		currentDay += 30;
	case 11:
		currentDay += 31;
	case 10:
		currentDay += 30;
	case 9:
		currentDay += 31;
	case 8:
		currentDay += 31;
	case 7:
		currentDay += 30;
	case 6:
		currentDay += 31;
	case 5:
		currentDay += 30;
	case 4:
		currentDay += 31;
	case 3:
		if (isLeapYear == 0)
			currentDay += 29;
		else
			currentDay += 28;
	case 2:
		currentDay += 31;
	case 1:
		currentDay += day;
	}
	
    return (currentDay);
}*/

/******************************************************************************
*
* ipmuxRtcEnable 
*
* DESCRIPTION:
* 	This function enable IPMux RTC clock.
*
* INPUTS:
*	n/a.
*
* OUTPUT:
*	enable: 0 disable, 1:enable.
*
* RETURNS:
* 	void
*
* SEE ALSO: 
*/
void ipmuxRtcEnable(OPL_BOOL enable)
{
	if (enable) 
	{
		rtcRegWrite32BitMask((UINT32) IPMUX_RTC_OFFSET_CLKEN,1,IPMUX_RTC_OFFSET_CLKEN_MASK); 
	}
	else
	{
		rtcRegWrite32BitMask((UINT32) IPMUX_RTC_OFFSET_CLKEN,0,IPMUX_RTC_OFFSET_CLKEN_MASK); 
	}
	
}

/******************************************************************************
*
* ipmuxRtcEnableWrite
*
* DESCRIPTION:
* 	Write protect bit. Before any write operation to the Clock, this bit must be 0. 
*	It has no affection to itself.
*
* INPUTS:
*	enable: 0 disable, 1:enable.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	void
*
* SEE ALSO: 
*/

void ipmuxRtcEnableWrite(OPL_BOOL enable)
{
	if (OPL_TRUE==enable) 
	{
		rtcRegWrite32BitMask((UINT32) IPMUX_RTC_OFFSET_WP,0,IPMUX_RTC_OFFSET_WP_MASK); 
	}
	else
	{
		rtcRegWrite32BitMask((UINT32) IPMUX_RTC_OFFSET_WP,1,IPMUX_RTC_OFFSET_WP_MASK); 
	}
}

/*************************************************************************** 
* ipmuxRtcSetDate  
*  
* DESCRIPTION:set the date
*
* INPUTS:
*	year: The value is from 0-4095
*
*	Month: the value is from 1-12
*  
*	mDay: the value is from 1-28,29,30,31 (Depend whether the month is lunar year)
*	
*	wDay: The value is from 0-6
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	void
*
* SEE ALSO: 
*/
OPL_STATUS ipmuxRtcDateSet(int year,	int month,int mDay, int wDay,int yDay)
{ 
	//int yDay;
	/* disable IPMux RTC  */ 	
	ipmuxRtcEnable(OPL_FALSE);
	
	/* enable write to register */ 
	ipmuxRtcEnableWrite(OPL_TRUE);
	
	//yDay = dayOfYear (month, mDay, ((!(year % 4) && (year % 100)) || !(year % 400)));
	
	/* data range check  */ 
	if (year > 4095)
	{
		printErr ("Invalid day of year: %d. Range is 0 - 4095. \n", year);
		return OPL_ERROR;
	}
	
	if (wDay < 0 || wDay > 6)
	{
		printErr ("Invalid day of week: %d\n", wDay);
		return OPL_ERROR;
	}
	if (month < 1 || month > 12)
	{
		printErr ("Invalid month: %d\n", month);
		return OPL_ERROR;
	}
	
	if (mDay > (daysInMo[month] + ((month == 2) && ((!(year % 4) && (year % 100)) || !(year % 400)))))
	{
		printErr ("Invalid day of month: %d\n", mDay);
		return OPL_ERROR;
	}
	
	/* write to register  */ 
	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_YEAR,year,IPMUX_RTC_OFFSET_YEAR_MASK);
	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_MON,month,IPMUX_RTC_OFFSET_MON_MASK);
	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_DATE,mDay,IPMUX_RTC_OFFSET_DATE_MASK);
	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_DATE_WEEK,wDay,IPMUX_RTC_OFFSET_DATE_WEEK_MASK);
	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_DATE_YEAR ,yDay,IPMUX_RTC_OFFSET_DATE_YEAR_MASK);

	/* disable write to register */ 
	ipmuxRtcEnableWrite(OPL_FALSE);
	
	/* enable IPMux RTC  */ 
	ipmuxRtcEnable(OPL_TRUE);
	return OPL_OK;
} 

/*************************************************************************** 
* ipmuxRtcAlarmGet 
*
* DESCRIPTION:This function get the alarm date time.
*
* INPUTS:
*	tmp	: time struct.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	void
*
* SEE ALSO: 
*/
void ipmuxRtcAlarmGet(struct rtc_time* tmp)
{ 
	tmp->tm_sec  = rtcRegRead32BitMask((UINT32) IPMUX_RTC_OFFSET_ASEC,      IPMUX_RTC_OFFSET_ASEC_MASK); 
	tmp->tm_min  = rtcRegRead32BitMask((UINT32) IPMUX_RTC_OFFSET_AMIN,      IPMUX_RTC_OFFSET_AMIN_MASK); 
	tmp->tm_hour = rtcRegRead32BitMask((UINT32) IPMUX_RTC_OFFSET_AHOUR,        IPMUX_RTC_OFFSET_AHOUR_MASK); 
	tmp->tm_wday = rtcRegRead32BitMask((UINT32) IPMUX_RTC_OFFSET_ADOW,        IPMUX_RTC_OFFSET_ADOW_MASK); 
	tmp->tm_mday = rtcRegRead32BitMask((UINT32) IPMUX_RTC_OFFSET_ADOM,        IPMUX_RTC_OFFSET_ADOM_MASK); 
	tmp->tm_yday = rtcRegRead32BitMask((UINT32) IPMUX_RTC_OFFSET_ADOY,   IPMUX_RTC_OFFSET_ADOY_MASK); 
	tmp->tm_mon  = rtcRegRead32BitMask((UINT32) IPMUX_RTC_OFFSET_AMON,         IPMUX_RTC_OFFSET_AMON_MASK); 
	tmp->tm_year = rtcRegRead32BitMask((UINT32) IPMUX_RTC_OFFSET_AYEAR,        IPMUX_RTC_OFFSET_AYEAR_MASK); 


	tmp->tm_isdst= 0;

#ifdef DEBUG_RTC
	printf("	Get DATE: %04d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d\n",\
		tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,\
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
#endif
}

/*************************************************************************** 
* ipmuxRtcAlarmDateSet 
*
* DESCRIPTION:This function set the alarm date.
*
* INPUTS:
*	year: The value is from 0-4095
*
*	Month: the value is from 1-12
*  
*	mDay: the value is from 1-28,29,30,31 (Depend whether the month is lunar year)
*	
*	wDay: The value is from 0-6
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	void
*
* SEE ALSO: 
*/
void ipmuxRtcAlarmSet(struct rtc_time* tmp)
{ 

	/* enable write to register */ 
	ipmuxRtcEnableWrite(OPL_TRUE);
	//tmp->tm_yday = dayOfYear (tmp->tm_mon, tmp->tm_mday,((!(tmp->tm_year % 4) && (tmp->tm_year % 100)) || !(tmp->tm_year % 400)));
		
	/* write to register  */ 
	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_AYEAR,tmp->tm_year, IPMUX_RTC_OFFSET_AYEAR_MASK);
	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_AMON, tmp->tm_mon, IPMUX_RTC_OFFSET_AMON_MASK);
	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_ADOM, tmp->tm_mday,IPMUX_RTC_OFFSET_ADOM_MASK);
	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_ADOW, tmp->tm_wday,IPMUX_RTC_OFFSET_ADOW_MASK);
	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_ADOY ,tmp->tm_yday, IPMUX_RTC_OFFSET_ADOY_MASK);
	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_AHOUR,tmp->tm_hour, IPMUX_RTC_OFFSET_AHOUR_MASK);
	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_AMIN, tmp->tm_min,   IPMUX_RTC_OFFSET_AMIN_MASK);
	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_ASEC, tmp->tm_sec,  IPMUX_RTC_OFFSET_ASEC_MASK);	
	
	/* disable write to register */ 
	ipmuxRtcEnableWrite(OPL_FALSE);
	
	/* enable IPMux RTC  */ 
	ipmuxRtcEnable(OPL_TRUE);
	
} 

/*******************************************************************************
*
* ipmuxRtcTimeSet - set the time
*
* DESCRIPTION:set the time
*
* INPUTS:
*	Hour: the value is from 0-23
*  
*	Minutes: the value is from 0-59
*	
*	SEC: the value is from 0-59
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	void
*
* SEE ALSO: 
*/
void ipmuxRtcTimeSet( int hour, int minute, int SEC )
{
	/* disable IPMux RTC  */ 	
	ipmuxRtcEnable(OPL_FALSE);
	
	/* enable write to register */ 
	ipmuxRtcEnableWrite(OPL_TRUE);
	
	
	if ((hour >= 24) )
	{
		printErr ("Invalid hour: %d\n", hour);
		/*return OPL_ERROR;*/
	}
	
	if ((minute >= 60) )
	{
		printErr ("Invalid Minute: %d\n", minute);
		/*return OPL_ERROR;*/
	}
	
	if ((SEC >= 60) )
	{
		printErr ("Invalid SEC: %d\n", SEC);
		/*return OPL_ERROR;*/
	}
	
	/* write to register  */ 
	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_HOUR,hour,IPMUX_RTC_OFFSET_HOUR_MASK);
	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_MINUTE,minute,IPMUX_RTC_OFFSET_MINUTE_MASK);
	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_SEC,SEC,IPMUX_RTC_OFFSET_SEC_MASK);
	
	/* disable write to register */ 
	ipmuxRtcEnableWrite(OPL_FALSE);
	
	/* enable IPMux RTC  */ 
	ipmuxRtcEnable(OPL_TRUE);
	
	
}

/*************************************************************************** 
* ipmuxRtcAIntEnable
*      
* DESCRIPTION:
*	This function enable IPMux RTC alarm int.
*
* INPUTS:
*
*	mask: alarm int mask, there are 8 masks correspond to every alarm int.
*			IPMUX_RTC_OFFSET_CMYEAR_MASK: year compare
*			IPMUX_RTC_OFFSET_CMMON_MASK : month compare
*			IPMUX_RTC_OFFSET_CMDOY_MASK : day of year compare
*			IPMUX_RTC_OFFSET_CMDOM_MASK: day of month compare
*			IPMUX_RTC_OFFSET_CMDOW_MASK: day of week compare
*			IPMUX_RTC_OFFSET_CMHOUR_MASK: hour compare
*			IPMUX_RTC_OFFSET_CMMIN_MASK: minute compare
*			IPMUX_RTC_OFFSET_CMSEC_MASK: second compare
*
*
* EXAMPLE :
*
*	enable cyear and cminute: ipmuxRtcAIntEnable(IPMUX_RTC_OFFSET_CMYEAR_MASK | IPMUX_RTC_OFFSET_CMMIN_MASK)
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	void
*
* SEE ALSO: 
*/
void ipmuxRtcAIntEnable(UINT8 mask)  
{ 
	ipmuxRtcEnableWrite(OPL_TRUE);

	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_CMYEAR,0xFFFFFFFF,mask);
	
	ipmuxRtcEnableWrite(OPL_FALSE);
	
} 

/*************************************************************************** 
* ipmuxRtcAIntDisable
*      
* DESCRIPTION:
*	This function disable IPMux RTC alarm int.
*
* INPUTS:
*	mask: alarm int mask, there are 8 masks correspond to every alarm int.
*			IPMUX_RTC_OFFSET_CMYEAR_MASK: year compare
*			IPMUX_RTC_OFFSET_CMMON_MASK : month compare
*			IPMUX_RTC_OFFSET_CMDOY_MASK : day of year compare
*			IPMUX_RTC_OFFSET_CMDOM_MASK: day of month compare
*			IPMUX_RTC_OFFSET_CMDOW_MASK: day of week compare
*			IPMUX_RTC_OFFSET_CMHOUR_MASK: hour compare
*			IPMUX_RTC_OFFSET_CMMIN_MASK: minute compare
*			IPMUX_RTC_OFFSET_CMSEC_MASK: second compare
*
* EXAMPLE :
*
*	unalbe cyear and cminute: ipmuxRtcAIntEnable(IPMUX_RTC_OFFSET_CMYEAR_MASK | IPMUX_RTC_OFFSET_CMMIN_MASK)
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	void
*
* SEE ALSO: 
*/
void ipmuxRtcAIntDisable(UINT8 mask)  
{ 
	ipmuxRtcEnableWrite(OPL_TRUE);
	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_CMYEAR,0,mask);
	ipmuxRtcEnableWrite(OPL_FALSE);
} 


/*************************************************************************** 
* ipmuxRtcIncIntEnable
*
* DESCRIPTION:
*	This function enable IPMux RTC increment interrupts of the IPMux RTC.
* 
* INPUTS:
*	mask: increment int mask, there are 8 masks correspond to every alarm int.
*			IPMUX_RTC_OFFSET_IYEAR_MASK: int per year 
*			IPMUX_RTC_OFFSET_IMON_MASK : int per month 
*			IPMUX_RTC_OFFSET_IDOY_MASK : int per day of year 
*			IPMUX_RTC_OFFSET_IDOM_MASK : int per day of month
*			IPMUX_RTC_OFFSET_IDOW_MASK : int per day of week 
*			IPMUX_RTC_OFFSET_IHOUR_MASK: int per hour 
*			IPMUX_RTC_OFFSET_IMIN_MASK : int per minute 
*			IPMUX_RTC_OFFSET_ISEC_MASK : int per second                                                  
*
* EXAMPLE :
*	enable  interrupt per year increment and interrupt per minute increment : ipmuxRtcIncIntEnable(IPMUX_RTC_OFFSET_IMYEAR_MASK | IPMUX_RTC_OFFSET_IMMIN_MASK)
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	value of SEC. 
*
* SEE ALSO: 
*/
void ipmuxRtcIncIntEnable(UINT32 mask)  
{ 
	ipmuxRtcEnableWrite(OPL_TRUE);
	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_IMYEAR,0xFFFFFFFF,mask);
	ipmuxRtcEnableWrite(OPL_FALSE);
} 

/*************************************************************************** 
* ipmuxRtcIncIntDisable
*
* DESCRIPTION:
*	This function disable IPMux RTC increment interrupts of the IPMux RTC.
* 
* INPUTS:
*	mask: increment int mask, there are 8 masks correspond to every alarm int.
*			IPMUX_RTC_OFFSET_IYEAR_MASK: int per year 
*			IPMUX_RTC_OFFSET_IMON_MASK : int per month 
*			IPMUX_RTC_OFFSET_IDOY_MASK : int per day of year 
*			IPMUX_RTC_OFFSET_IDOM_MASK : int per day of month
*			IPMUX_RTC_OFFSET_IDOW_MASK : int per day of week 
*			IPMUX_RTC_OFFSET_IHOUR_MASK: int per hour 
*			IPMUX_RTC_OFFSET_IMIN_MASK : int per minute 
*			IPMUX_RTC_OFFSET_ISEC_MASK : int per second 
*
* EXAMPLE :
*	unalbe cyear and cminute: ipmuxRtcIncIntDisable(IPMUX_RTC_OFFSET_CMYEAR_MASK | IPMUX_RTC_OFFSET_CMMIN_MASK)
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	value of SEC. 
*
* SEE ALSO: 
*/
void ipmuxRtcIncIntDisable(UINT32 mask)  
{ 
	ipmuxRtcEnableWrite(OPL_TRUE);

	rtcRegWrite32BitMask((UINT32)IPMUX_RTC_OFFSET_CMYEAR,0,mask);
	
	ipmuxRtcEnableWrite(OPL_FALSE);

} 

/******************************************************************************* 
* ipmuxRtcIntHandle 
* 
* DESCRIPTION:
*	This function handle the increment and alarm interrupt of the IPMux RTC 
* 
* INPUTS:
*	n/a.
*
* OUTPUT:
*	n/a.
*
* RETURNS: 
*	value of the interrupt type.
*
* SEE ALSO: 
*/ 
int ipmuxRtcIntHandle(void)
{
	UINT32 indications=0;
	UINT32 enable=0;
	
	int intOccured=0;
	indications=rtcRegRead32BitMask((UINT32)IPMUX_RTC_OFFSET_IYEAR,0xFFFFFFFF);
	enable=rtcRegRead32BitMask((UINT32)IPMUX_RTC_OFFSET_IMYEAR,0x00FFFFFF);
	indications=indications & enable;
	
	if (IPMUX_RTC_OFFSET_IYEAR_MASK & indications) 
	{
		/*  interrupt per year, todo : add your code */ 
		printf("IYEAR occured.\n");
		intOccured=1;
	}
	
	if (IPMUX_RTC_OFFSET_IMON_MASK & indications) 
	{
		/*  interrupt per month, todo : add your code */ 
		printf("IMON occured.\n");
				intOccured=1;
	}
	
	if (IPMUX_RTC_OFFSET_IDOY_MASK & indications) 
	{
		/*  interrupt per day of year, todo : add your code */ 
		printf("IDOY occured.\n");
				intOccured=1;
	}
	
	if (IPMUX_RTC_OFFSET_IDOW_MASK & indications) 
	{
		/*  interrupt per day of week, todo : add your code */ 
		printf("IDOW occured.\n");
		intOccured=1;		
	}
	
	if (IPMUX_RTC_OFFSET_IDOM_MASK & indications) 
	{
		/*  interrupt per day of month , todo : add your code */ 
		printf("IDOM occured.\n");
		intOccured=1;		
	}
	
	if (IPMUX_RTC_OFFSET_IHOUR_MASK & indications) 
	{
		/*  interrupt per hour , todo : add your code */ 
		printf("IHOUR occured.\n");
		intOccured=1;		
	}
	
	if (IPMUX_RTC_OFFSET_IMIN_MASK & indications) 
	{
		/*  interrupt per minute , todo : add your code */ 
		printf("IMIN occured.\n");
		intOccured=1;		
	}
	
	if (IPMUX_RTC_OFFSET_ISEC_MASK & indications) 
	{
		/*  interrupt per SEC , todo : add your code */ 
		printf("ISEC occured.\n");
		intOccured=1;		
	}

	if (IPMUX_RTC_OFFSET_ICMYEAR_MASK & indications) 
	{
		/*  interrupt per year, todo : add your code */ 
		printf("ICMYEAR occured.\n");
		intOccured=1;
	}
	
	if (IPMUX_RTC_OFFSET_ICMMON_MASK & indications) 
	{
		/*  interrupt per month, todo : add your code */ 
		printf("ICMMON occured.\n");
				intOccured=1;
	}
	
	if (IPMUX_RTC_OFFSET_ICMDOY_MASK & indications) 
	{
		/*  interrupt per day of year, todo : add your code */ 
		printf("	ICMDOY occured.\n");
				intOccured=1;
	}
	
	if (IPMUX_RTC_OFFSET_ICMDOW_MASK & indications) 
	{
		/*  interrupt per day of week, todo : add your code */ 
		printf("	ICMDOW occured.\n");
		intOccured=1;		
	}
	
	if (IPMUX_RTC_OFFSET_ICMDOM_MASK & indications) 
	{
		/*  interrupt per day of month , todo : add your code */ 
		printf("	ICMDOM occured.\n");
		intOccured=1;		
	}
	
	if (IPMUX_RTC_OFFSET_ICMHOUR_MASK & indications) 
	{
		/*  interrupt per hour , todo : add your code */ 
		printf("	ICMHOUR occured.\n");
		intOccured=1;		
	}
	
	if (IPMUX_RTC_OFFSET_ICMMIN_MASK & indications) 
	{
		/*  interrupt per minute , todo : add your code */ 
		printf("	ICMMIN occured.\n");
		intOccured=1;		
	}
	
	if (IPMUX_RTC_OFFSET_ICMSEC_MASK & indications) 
	{
		/*  interrupt per SEC , todo : add your code */ 
		printf("	ICMSEC occured.\n");
		intOccured=1;		
	}
	
	if (0==intOccured)
	{
		printf("	NO INT (indications=0x08%x) !\n",indications);
	}
	else
	{

		printf("	indications=0x08%x\n",indications);
	}
	
	return indications;
}


/******************************************************************************* 
* ipmuxRtcIntClear 
* 
* DESCRIPTION:
*	This function clear increment and alarm interrupt of the IPMux RTC 
* 
* INPUTS:
*	n/a.
*
* OUTPUT:
*	n/a.
*
* RETURNS: 
*	n/a.
*
* SEE ALSO: 
*/ 
void ipmuxRtcIntClear(void)
{
	UINT32 indications=0;

	indications=rtcRegRead32BitMask((UINT32)IPMUX_RTC_OFFSET_IYEAR,0xFFFFFFFF);
	printf("	clear rtc int\n");	
}

/******************************************************************************* 
* ipmuxRtcTimePrint
* 
* DESCRIPTION:
*	This prisnts current time to stdout.  Used to manually test if date was 
*	set properly 
*
* INPUTS:
*	n/a.
*
* OUTPUT:
*	n/a.
*
* RETURNS: 
*	n/a.
*
* SEE ALSO: 
*/ 
void ipmuxRtcTimePrint (void) 
{ 
	struct rtc_time tm;
	struct rtc_time *tmp;

	tmp=&tm;
	ipmuxRtcGet(tmp);
	printf("\t");			
	/*
	switch (tm.tm_wday) { 
	case 0: 
		printf("Date: Sunday "); 
		break; 
	case 1: 
		printf("Date: Monday "); 
		break; 
	case 2: 
		printf("Date: Tuesday "); 
		break; 
	case 3: 
		printf("Date: Wednesday "); 
		break; 
	case 4: 
		printf("Date: Thursday "); 
		break; 
	case 5: 
		printf("Date: Friday "); 
		break; 
	case 6: 
		printf("Date: Saturday "); 
		break; 
	default:
		printf("Date: INVALID WEEKDAY!!! \n"); 
		break; 			
	} 
	*/
	printf("RTC  TIME:%04d-%02d-%02d (wday=%d,yday=%02d)  %2d:%02d:%02d\n",\
		tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,tmp->tm_yday,\
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
	
} 


/******************************************************************************* 
* ipmuxRtcATimePrint
* 
* DESCRIPTION:
*	This prints current time to stdout.  Used to manually test if date was 
*	set properly 
*
* INPUTS:
*	n/a.
*
* OUTPUT:
*	n/a.
*
* RETURNS: 
*	n/a.
*
* SEE ALSO: 
*/ 
void ipmuxRtcATimePrint (void) 
{ 
	struct rtc_time tm;
	struct rtc_time *tmp;

	tmp=&tm;
	ipmuxRtcAlarmGet(tmp);
	printf("\t");	
/*
	switch (tm.tm_wday) { 
		
	case 0: 
		printf("Date: Sunday "); 
		break; 
	case 1: 
		printf("Date: Monday "); 
		break; 
	case 2: 
		printf("Date: Tuesday "); 
		break; 
	case 3: 
		printf("Date: Wednesday "); 
		break; 
	case 4: 
		printf("Date: Thursday "); 
		break; 
	case 5: 
		printf("Date: Friday "); 
		break; 
	case 6: 
		printf("Date: Saturday "); 
		break; 
	default:
		printf("Date: INVALID WEEKDAY!!! \n"); 
		break; 			
	} 

*/
	printf("RTC ALARM:%04d-%02d-%02d (wday=%d,yday=%02d)  %2d:%02d:%02d\n",\
		tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,tmp->tm_yday,\
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
	
} 

static void rtcRegWrite32BitMask (UINT32 addr, UINT32 data, UINT32 MASK)
{
	UINT32 oldValue;
	
	oldValue=(*((volatile UINT32 *)(IPMUX_RTC_BASE_ADDR+addr)) & ~MASK);
	oldValue |=((data<<getMaskShift(MASK)) & MASK);

	//printf("	wr addr=%x, mask=%x,val=%x\n",IPMUX_RTC_BASE_ADDR+addr,MASK,oldValue);
		
	*((volatile UINT32 *)(addr+IPMUX_RTC_BASE_ADDR)) = oldValue;   
}

int rtcRegRead32BitMask (UINT32 addr, UINT32 MASK)
{
	//printf("	read addr=%x(addr=%x,IPMUX_RTC_BASE_ADDR=%x), mask=%x\n",IPMUX_RTC_BASE_ADDR+addr,MASK,addr,IPMUX_RTC_BASE_ADDR);
	/*
	if (IPMUX_RTC_OFFSET_MON_MASK==MASK)
	{
		printf("	read addr=%x(addr=%x,IPMUX_RTC_BASE_ADDR=%x), mask=%x\n",IPMUX_RTC_BASE_ADDR+addr,MASK,addr,IPMUX_RTC_BASE_ADDR);
		printf("	val=%x\n",(*((volatile UINT32 *)(IPMUX_RTC_BASE_ADDR+addr)) ));
		printf("	val=%x\n",(*((volatile UINT32 *)(IPMUX_RTC_BASE_ADDR+addr)) & MASK));
	}
	*/
	return (*((volatile UINT32 *)(IPMUX_RTC_BASE_ADDR+addr)) & MASK)>>getMaskShift(MASK) ;
}




int getMaskShift(UINT32 MASK)
{
	int result=0;
	UINT32 tmp=MASK;
	while (tmp) {
		
		if ((tmp & 0x1)==1) 
		{
			//printf(" MASK=%x\n",MASK>>result);
			return result;
		}
		result++;
		tmp=tmp>>1;	

	}

	return 0;
}










