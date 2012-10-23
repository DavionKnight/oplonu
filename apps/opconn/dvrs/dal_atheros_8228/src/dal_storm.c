/*
=============================================================================
     file Name: dal_storm.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                		Date              	Description of Changes
----------------   ------------  ----------------------------------------------
 zran 				2009/05/20		Initial Version	
----------------   ------------  ----------------------------------------------
*/

#include <dal_lib.h>
#include <sw_error.h>
#include <fal_rate.h>



OPL_STATUS dalStormCtrlFrameSet(UINT32 portId,UINT32 stormLimitType,UINT32 limitEnable)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
	return shiva_storm_ctrl_frame_set(0, atherosPort, stormLimitType, limitEnable);	
}

OPL_STATUS dalStormCtrlFrameGet(UINT32 portId,UINT32 stormLimitType,UINT32 *limitEnable)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
	return shiva_storm_ctrl_frame_get(0, atherosPort, stormLimitType, limitEnable);	
}



/*******************************************************************************
* dalStormCtrlRateSet
*
* DESCRIPTION:
*  this API is used to set storm control rate for ports. The Api dalStormCtrlFrameSet should be called 
*	before calling this Api.
*
* INPUTS:
*	portId	: 	port ID
*	rateLimit   	:	  	0: disable storm control
*						1: 1k Frame per second
*						2: 2k Frame per second
*						3: 4k Frame per second
*						.................................
*						B: 1024k Frame per second
*	    
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalStormCtrlRateSet(UINT32 portId,UINT32 rateLimit)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
	return shiva_storm_ctrl_rate_set(0, atherosPort, &rateLimit);	
}



/*******************************************************************************
* dalStormCtrlRateGet
*
* DESCRIPTION:
*  this API is used to get storm control rate for ports. The Api dalStormCtrlFrameSet should be called 
*	before calling this Api.
*
* INPUTS:
*	portId	: 	port ID
*
* OUTPUTS:
*	rateLimit   	:	  	0: disable storm control
*						1: 1k Frame per second
*						2: 2k Frame per second
*						3: 4k Frame per second
*						.................................
*						B: 1024k Frame per second
*	    

*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalStormCtrlRateGet(UINT32 portId,UINT32 *rateLimit)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
    sw_error_t ret = shiva_storm_ctrl_rate_get(0, atherosPort, rateLimit);	
	
    return ret;
}



