/************************************************************************************/
/**  COPYRIGHT (C) 2005- Opulan, INC. ALL RIGHTS RESERVED.                         **/
/**--------------------------------------------------------------------------------**/
/** This software embodies materials and concepts which are proprietary and        **/
/** confidential to Opulan, Inc.                                                   **/
/**--------------------------------------------------------------------------------**/
/**                                                                                **/
/**    	MODULE     : synchro file source file                                      **/
/**    	FILE       : 	ipmuxRtc.c                                                         **/
/**                                                                                **/
/**     DESCRIPTION:                                                               **/
/**                                                                                **/
/**       This provides a library of routines to set and retrieve the date         **/
/**       from the real time clock on the IPMUX ipmuxRtc chip .                         **/
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
#include "rtc_drv.h"

/* #defines  */ 
#define ipmuxRtcYear            tmp->tm_year
//#define ipmuxRtcLeapYearDist    tmp->tm_LeapYearDist
#define ipmuxRtcMonth           tmp->tm_mon
#define ipmuxRtcDayOfYear       tmp->tm_yday
#define ipmuxRtcmDay      tmp->tm_mday
#define ipmuxRtcwDay       tmp->tm_wday
#define ipmuxRtcHour            tmp->tm_hour
#define ipmuxRtcMinute          tmp->tm_min
#define ipmuxRtcSEC          tmp->tm_sec

/* locals */
LOCAL char *monthTable[12] ={
    "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
		"JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};

LOCAL unsigned char daysInMo[] =
{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

LOCAL char *dayTable [7] =
{
    "Sunday", "Monday", "Tuesday",
		"Wednesday", "Thursday", "Friday", "Saturday"
};

LOCAL int ipmuxRtcDrvNum;        /* driver number assigned to this driver */

LOCAL int mode;

LOCAL Rtc_DATE_TIME ipmuxRtcDateTime;

/* forward declarations */
LOCAL int dayOfYear( int month, int day, int isLeapYear );

LOCAL void rtcRegWrite32BitMask(UINT32 *addr, UINT32 data, UINT32 MASK);
UINT32 rtcRegRead32BitMask(UINT32 *addr, UINT32 MASK);
void ipmuxRtcEnableWrite(BOOL enable);

/*******************************************************************************
*
* ipmuxRtcDateTimeGet  
*
* DESCRIPTION:
* 	this function Get the current time from the ipmuxRtc.
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
void ipmuxRtcGet (struct rtc_time *tmp)
{
	tmp->tm_sec  = rtcRegRead32BitMask((UINT32 *) IPMUX_RTC_OFFSET_SEC, IPMUX_RTC_OFFSET_SEC_MASK); 
	tmp->tm_min  = rtcRegRead32BitMask((UINT32 *) IPMUX_RTC_OFFSET_MINUTE, IPMUX_RTC_OFFSET_MINUTE_MASK); 
	tmp->tm_hour = rtcRegRead32BitMask((UINT32 *) IPMUX_RTC_OFFSET_HOUR,   IPMUX_RTC_OFFSET_HOUR_MASK); 
	tmp->tm_wday = rtcRegRead32BitMask((UINT32 *) IPMUX_RTC_OFFSET_DATE_WEEK,   IPMUX_RTC_OFFSET_DATE_WEEK_MASK); 
	tmp->tm_mday = rtcRegRead32BitMask((UINT32 *) IPMUX_RTC_OFFSET_DATE,   IPMUX_RTC_OFFSET_DATE_MASK); 
	tmp->tm_mon  = rtcRegRead32BitMask((UINT32 *) IPMUX_RTC_OFFSET_MON,  IPMUX_RTC_OFFSET_MON_MASK); 
	tmp->tm_year = rtcRegRead32BitMask((UINT32 *) IPMUX_RTC_OFFSET_YEAR,   IPMUX_RTC_OFFSET_YEAR_MASK); 
	tmp->tm_yday = rtcRegRead32BitMask((UINT32 *) IPMUX_RTC_OFFSET_DATE_YEAR,   IPMUX_RTC_OFFSET_DATE_YEAR_MASK); 

	tmp->tm_isdst= 0;

	
#ifdef DEBUG_RTC
	printf("Get ALARM DATE: %4d-%02d-%02d (wday=%d,yday=%d)  TIME: %2d:%02d:%02d\n",\
		tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,tmp->tm_yday,\
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
#endif
	
}


/*******************************************************************************
*
* ipmuxRtcDateTimeGet  
*
* DESCRIPTION:
* 	this function Set the ipmuxRtc time.
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
void ipmuxRtcSet (struct rtc_time *tmp)
{
#ifdef DEBUG_RTC
	printf("Set DATE: %4d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d\n",
		tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
#endif
	
	ipmuxRtcDateSet(tmp->tm_year, tmp->tm_mon,tmp->tm_mday,tmp->tm_wday,tmp->tm_yday);
	ipmuxRtcTimeSet(tmp->tm_hour, tmp->tm_min,tmp->tm_sec);
	
}

/*******************************************************************************
*
* ipmuxRtcDateTimeGet  
*
* DESCRIPTION:
* 	this function Reset the ipmuxRtc. We setting the date back to 1970-01-01.
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
	/* unable ipmuxRtc  */ 	
	ipmuxRtcEnable(FALSE);
	
	/* enable write to register */ 
	ipmuxRtcEnableWrite(TRUE);
	
	ipmuxRtcDateSet(1970,1,1,1,0);
	ipmuxRtcTimeSet(0,0,0);	
}

/*******************************************************************************
*
* dayOfYear 
* DESCRIPTION:
* 	this function evaluate day of the year.
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
*/

LOCAL int dayOfYear( int month, int day, int isLeapYear )
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
}

/******************************************************************************
*
* ipmuxRtcEnable 
*
* DESCRIPTION:
* 	change status of clock enable.
*
* INPUTS:
*	n/a.
*
* OUTPUT:
*	enalbe: 0 unable, 1:enable.
*
* RETURNS:
* 	void
*
* SEE ALSO: 
*/
void ipmuxRtcEnable(BOOL enalbe)
{
	if (enalbe) 
	{
		rtcRegWrite32BitMask((UINT32 *) IPMUX_RTC_OFFSET_CLKEN,1,IPMUX_RTC_OFFSET_CLKEN_MASK); 
	}
	else
	{
		rtcRegWrite32BitMask((UINT32 *) IPMUX_RTC_OFFSET_CLKEN,0,IPMUX_RTC_OFFSET_CLKEN_MASK); 
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
*	enalbe: 0 unable, 1:enable.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	void
*
* SEE ALSO: 
*/

void ipmuxRtcEnableWrite(BOOL enable)
{
	if (TRUE==enable) 
	{
		rtcRegWrite32BitMask((UINT8 *) IPMUX_RTC_OFFSET_WP,0,IPMUX_RTC_OFFSET_WP_MASK); 
	}
	else
	{
		rtcRegWrite32BitMask((UINT8 *) IPMUX_RTC_OFFSET_WP,1,IPMUX_RTC_OFFSET_WP_MASK); 
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
void ipmuxRtcDateSet(int year,	int month,int mDay, int wDay,int yDay)
{ 
	/* unable ipmuxRtc  */ 	
	ipmuxRtcEnable(FALSE);
	
	/* enable write to register */ 
	ipmuxRtcEnableWrite(TRUE);
	
	ipmuxRtcYear = year;
	ipmuxRtcMonth = month;
	ipmuxRtcmDay = mDay;
	ipmuxRtcwDay = wDay;
	ipmuxRtcDayOfYear = dayOfYear (month, mDay, ((!(year % 4) && (year % 100)) || !(year % 400)));
	
	/* data range check  */ 
	if (ipmuxRtcYear > 4095)
	{
		printErr ("Invalid day of year: %d. Range is 0 - 4095. \n", ipmuxRtcYear);
		/*return OPL_ERROR;*/
	}
	
	if (ipmuxRtcwDay < 0 || ipmuxRtcwDay > 6)
	{
		printErr ("Invalid day of week: %d\n", ipmuxRtcwDay);
		/*return OPL_ERROR;*/
	}
	if (ipmuxRtcMonth < 1 || ipmuxRtcMonth > 12)
	{
		printErr ("Invalid month: %d\n", ipmuxRtcMonth);
		/*return OPL_ERROR;*/
	}
	
	if (ipmuxRtcmDay > (daysInMo[ipmuxRtcMonth] + ((ipmuxRtcMonth == 2) && ipmuxRtcLeapYearDist)))
	{
		printErr ("Invalid day of month: %d\n", ipmuxRtcmDay);
		/*return OPL_ERROR;*/
	}
	
	/* write to register  */ 
	rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_YEAR,year,IPMUX_RTC_OFFSET_YEAR_MASK);
	rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_MON,month,IPMUX_RTC_OFFSET_MON_MASK);
	rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_DATE,mDay,IPMUX_RTC_OFFSET_DATE_MASK);
	rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_DATE_WEEK,wDay,IPMUX_RTC_OFFSET_DATE_WEEK_MASK);
	rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_DATE_YEAR ,yDay,IPMUX_RTC_OFFSET_DATE_YEAR_MASK);
	
	/* unable write to register */ 
	ipmuxRtcEnableWrite(FALSE);
	
	/* enable ipmuxRtc  */ 
	ipmuxRtcEnable(TRUE);
	
} 

/*************************************************************************** 
* ipmuxRtcAlarmGet 
*
* DESCRIPTION:set the alarm date time.
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
	tmp->tm_sec  = rtcRegRead32BitMask((UINT32 *) IPMUX_RTC_OFFSET_ASEC,      IPMUX_RTC_OFFSET_ASEC_MASK); 
	tmp->tm_min  = rtcRegRead32BitMask((UINT32 *) IPMUX_RTC_OFFSET_AMIN,      IPMUX_RTC_OFFSET_AMIN_MASK); 
	tmp->tm_hour = rtcRegRead32BitMask((UINT32 *) IPMUX_RTC_OFFSET_AHOUR,        IPMUX_RTC_OFFSET_AHOUR_MASK); 
	tmp->tm_wday = rtcRegRead32BitMask((UINT32 *) IPMUX_RTC_OFFSET_ADOW,        IPMUX_RTC_OFFSET_ADOW_MASK); 
	tmp->tm_mday = rtcRegRead32BitMask((UINT32 *) IPMUX_RTC_OFFSET_ADOM,        IPMUX_RTC_OFFSET_ADOM_MASK); 
	tmp->tm_yday = rtcRegRead32BitMask((UINT32 *) IPMUX_RTC_OFFSET_ADOY,   IPMUX_RTC_OFFSET_ADOY_MASK); 
	tmp->tm_mon  = rtcRegRead32BitMask((UINT32 *) IPMUX_RTC_OFFSET_AMON,         IPMUX_RTC_OFFSET_AMON_MASK); 
	tmp->tm_year = rtcRegRead32BitMask((UINT32 *) IPMUX_RTC_OFFSET_AYEAR,        IPMUX_RTC_OFFSET_AYEAR_MASK); 


	tmp->tm_isdst= 0;

#ifdef DEBUG_RTC
	printf("Get DATE: %4d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d\n",\
		tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,\
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
#endif
}

/*************************************************************************** 
* ipmuxRtcAlarmDateSet 
*
* DESCRIPTION:set the alarm date
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
	ipmuxRtcEnableWrite(TRUE);
	ipmuxRtcDayOfYear = dayOfYear (month, tmp->mday,((!(tmp->year % 4) && (tmp->year % 100)) || !(tmp->year % 400)));
		
	/* write to register  */ 
	rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_AYEAR,ipmuxRtcYear,        IPMUX_RTC_OFFSET_AYEAR_MASK);
	rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_AMON, ipmuxRtcMonth,       IPMUX_RTC_OFFSET_AMON_MASK);
	rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_ADOM, ipmuxRtcmDay,  IPMUX_RTC_OFFSET_ADOM_MASK);
	rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_ADOY ,ipmuxRtcDayOfYear,   IPMUX_RTC_OFFSET_ADOY_MASK);
	rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_AHOUR,ipmuxRtcYear,        IPMUX_RTC_OFFSET_AHOUR_MASK);
	rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_AMIN, ipmuxRtcMonth,       IPMUX_RTC_OFFSET_AMIN_MASK);
	rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_ASEC, ipmuxRtcmDay,  IPMUX_RTC_OFFSET_ASEC_MASK);	
	
	/* unable write to register */ 
	ipmuxRtcEnableWrite(FALSE);
	
	/* enable ipmuxRtc  */ 
	ipmuxRtcEnable(TRUE);
	
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
	/* unable ipmuxRtc  */ 	
	ipmuxRtcEnable(FALSE);
	
	/* enable write to register */ 
	ipmuxRtcEnableWrite(TRUE);
	
    ipmuxRtcHour = hour;
    ipmuxRtcMinute = minute;
    ipmuxRtcSEC = SEC;
	
	if ((ipmuxRtcHour >= 24) )
	{
		printErr ("Invalid hour: %d\n", ipmuxRtcHour);
		/*return OPL_ERROR;*/
	}
	
	if ((ipmuxRtcMinute >= 60) )
	{
		printErr ("Invalid Minute: %d\n", ipmuxRtcMinute);
		/*return OPL_ERROR;*/
	}
	
	if ((ipmuxRtcSEC >= 60) )
	{
		printErr ("Invalid SEC: %d\n", ipmuxRtcSEC);
		/*return OPL_ERROR;*/
	}
	
	/* write to register  */ 
	rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_HOUR,ipmuxRtcYear,IPMUX_RTC_OFFSET_HOUR_MASK);
	rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_MINUTE,ipmuxRtcMonth,IPMUX_RTC_OFFSET_MINUTE_MASK);
	rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_SEC,ipmuxRtcmDay,IPMUX_RTC_OFFSET_SEC_MASK);
	
	/* unable write to register */ 
	ipmuxRtcEnableWrite(FALSE);
	
	/* enable ipmuxRtc  */ 
	ipmuxRtcEnable(TRUE);
	
	
}

/*************************************************************************** 
* ipmuxRtcAIntEnable
*      
* DESCRIPTION:
*	enable alarm 
*
* INPUTS:
*
*	mask: which enalbe
*
* EXAMPLE :
*
*	enalbe cyear and cminute: ipmuxRtcAIntEnable(IPMUX_RTC_OFFSET_CMYEAR_MASK | IPMUX_RTC_OFFSET_CMMIN_MASK)
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
	
	void result;
	UINT8 ipmuxRtcDigit; 
	UINT8 typeCur=0;
	unsigned int i;
	result=OPL_OK;
	
	ipmuxRtcEnableWrite(TRUE);
	
	for(i=0; i< 8 ; i++)
	{
		
		if (i & mask) 
		{
			rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_CMYEAR,0,i);
		}
	}
	
	ipmuxRtcEnableWrite(FALSE);
	
} 

/*************************************************************************** 
* ipmuxRtcAIntEnable
*      
* DESCRIPTION:
*	enable alarm 
*
* INPUTS:
*
*	mask: which enalbe
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
	void result;
	char data[2]={0,0};
	UINT8 ipmuxRtcDigit; 
	UINT8 typeCur=0;
	unsigned int i;
	result=OPL_OK;
	ipmuxRtcEnableWrite(TRUE);
	for(i=IPMUX_RTC_OFFSET_CMSEC_MASK; i<= IPMUX_RTC_OFFSET_CMYEAR ; i++)
	{
		
		if (i & mask) 
		{
			if (OPL_ERROR==	rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_CMYEAR,i,i))
			{
				result=OPL_ERROR;
			}
		}
	}
	ipmuxRtcEnableWrite(FALSE);
	//return (result); 
} 


/*************************************************************************** 
* ipmuxRtcIncIntEnable
*
* DESCRIPTION:
*	This function enalbe ipmuxRtc increment interrupt of the ipmuxRtc 
* 
* INPUTS:
*	mask:which enalbe
*
* EXAMPLE :
*	enalbe  interrupt per year increment and interrupt per minute increment : ipmuxRtcIncIntEnable(IPMUX_RTC_OFFSET_IMYEAR_MASK | IPMUX_RTC_OFFSET_IMMIN_MASK)
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
	void result;
	UINT8 ipmuxRtcDigit; 
	UINT8 typeCur=0;
	UINT32  i;
	result=OPL_OK;
	
	ipmuxRtcEnableWrite(TRUE);
	
	for(i = IPMUX_RTC_OFFSET_IMSEC_MASK; i <= IPMUX_RTC_OFFSET_IMYEAR_MASK ; i++)  
	{
		
		if (i & mask) 
		{
			if (OPL_ERROR == rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_IMYEAR,0,i))
			{
				result = OPL_ERROR;
			}
		}
	}
	
	ipmuxRtcEnableWrite(FALSE);
	//return (result); 
} 

/*************************************************************************** 
* ipmuxRtcIncIntUnable
*
* DESCRIPTION:
*	This function unable ipmuxRtc increment interrupt of the ipmuxRtc 
* 
* INPUTS:
*	mask:which enalbe
*
* EXAMPLE :
*	unalbe cyear and cminute: ipmuxRtcIncIntUnable(IPMUX_RTC_OFFSET_CMYEAR_MASK | IPMUX_RTC_OFFSET_CMMIN_MASK)
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	value of SEC. 
*
* SEE ALSO: 
*/
void ipmuxRtcIncIntUnable(UINT32 mask)  
{ 
	void result;
	UINT8 ipmuxRtcDigit; 
	UINT8 typeCur=0;
	UINT32 i;
	result=OPL_OK;
	ipmuxRtcEnableWrite(TRUE);
	for(i=0; i< 8 ; i++)
	{
		if (i & mask) 
		{
			if (OPL_ERROR==	rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_CMYEAR,i,i))
			{
				result=OPL_ERROR;
			}
		}
	}
	ipmuxRtcEnableWrite(FALSE);
	//return (result); 
} 

/******************************************************************************* 
* ipmuxRtcIntHandle 
* 
* DESCRIPTION:
*	This function handle interrupt per year/mon/date/minute/SEC indication from the ipmuxRtc 
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
int ipmuxRtcIntHandle()
{
	UINT32 indications=0;
	indications=rtcRegRead32BitMask(IPMUX_RTC_OFFSET_IYEAR,0xFF000000);
	if (IPMUX_RTC_OFFSET_IYEAR_MASK & indications) 
	{
		/*  interrupt per year, todo : add your code */ 
		printf("	IYEAR int occured.\n");
	}
	
	if (IPMUX_RTC_OFFSET_IMON_MASK & indications) 
	{
		/*  interrupt per month, todo : add your code */ 
		printf("	IMON int occured.\n");
	}
	
	if (IPMUX_RTC_OFFSET_IDOY_MASK & indications) 
	{
		/*  interrupt per day of year, todo : add your code */ 
		printf("	IDOY int occured.\n");
	}
	
	if (IPMUX_RTC_OFFSET_IDOW_MASK & indications) 
	{
		/*  interrupt per day of week, todo : add your code */ 
		printf("	IDOW int occured.\n");
	}
	
	if (IPMUX_RTC_OFFSET_IDOM_MASK & indications) 
	{
		/*  interrupt per day of month , todo : add your code */ 
		printf("	IDOM int occured.\n");
	}
	
	if (IPMUX_RTC_OFFSET_IHOUR_MASK & indications) 
	{
		/*  interrupt per hour , todo : add your code */ 
		printf("	IHOUR int occured.\n");
	}
	
	if (IPMUX_RTC_OFFSET_IMIN_MASK & indications) 
	{
		/*  interrupt per minute , todo : add your code */ 
		printf("	IMIN int occured.\n");
	}
	
	if (IPMUX_RTC_OFFSET_ISEC_MASK & indications) 
	{
		/*  interrupt per SEC , todo : add your code */ 
		printf("	ISEC int occured.\n");
	}
	
	return indications;
}

/******************************************************************************* 
* ipmuxRtcBaseClockSet 
* 
* DESCRIPTION:
*	This function set the Base clock of frequency divider
*		The formula for PREINT is: PREINT = int(sysclk/32768) - 1
*		The formula for PREINT is: PREFRC =sysclk - (PREINT+1)*32768
* 
* INPUTS:
*	preInteger : The integer part of Base clock divider
*	preFraction: The fraction part of Base clock divider
*
* OUTPUT:
*	n/a.
*
* RETURNS: 
* 	void
*
* SEE ALSO: 
*/ 
void ipmuxRtcBaseClockSet(UINT16 preInteger,UINT16 preFraction)
{
	UINT32 data;
	ipmuxRtcEnableWrite(TRUE);
	
	data=(preInteger<<16 & IPMUX_RTC_OFFSET_PREINT_MASK) |  (preFraction & IPMUX_RTC_OFFSET_PREFRC_MASK);
	rtcRegWrite32BitMask(IPMUX_RTC_OFFSET_PREINT,data,IPMUX_RTC_OFFSET_PREINT_MASK | IPMUX_RTC_OFFSET_PREFRC_MASK);
	
	ipmuxRtcEnableWrite(FALSE);
}


/******************************************************************************* 
* ipmuxRtcPrintTime
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
void ipmuxRtcPrintTime (void) 
{ 
	struct rtc_time tm;
	struct rtc_time *tmp;

	tmp=&tm;

	
	switch (tm.tm_wday) { 
		
	case 1: 
		printf("Date: Sunday "); 
		break; 
	case 2: 
		printf("Date: Monday "); 
		break; 
	case 3: 
		printf("Date: Tuesday "); 
		break; 
	case 4: 
		printf("Date: Wednesday "); 
		break; 
	case 5: 
		printf("Date: Thursday "); 
		break; 
	case 6: 
		printf("Date: Friday "); 
		break; 
	case 7: 
		printf("Date: Saturday "); 
		break; 
	} 

	printf("Get ALARM DATE: %4d-%02d-%02d (wday=%d,yday=%d)  TIME: %2d:%02d:%02d\n",\
		tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,tmp->tm_yday,\
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
	
} 


LOCAL void rtcRegWrite32BitMask (UINT32 * addr, UINT32 data, UINT32 MASK)
{
	UINT32 oldValue;
	
	oldValue=rtcRegRead32BitMask(addr,~MASK);
	oldValue |=(data & MASK);
	
	*((volatile UINT32 *)(addr+IPMUX_RTC_BASE_ADDR) = oldValue;   
}

UINT32 rtcRegRead32BitMask (UINT32 * addr, UINT32 MASK)
{
	return (*((volatile UINT32 *)(addr+IPMUX_RTC_BASE_ADDR)) & MASK);
}

















