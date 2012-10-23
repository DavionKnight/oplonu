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
	a_uint32_t rate_flag;
	fal_port_policer_t policer;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	memset(&policer, 0, sizeof(fal_port_policer_t));
	isis_rate_port_policer_get(0,atherosPort,&policer);

	rate_flag = 0;
	if(stormLimitType==0){
		rate_flag = FAL_INGRESS_POLICING_UNK_UNI;
	}
	else if(stormLimitType==1){
		rate_flag = FAL_INGRESS_POLICING_UNK_MUL;
	}
	else if(stormLimitType==2){
		rate_flag = FAL_INGRESS_POLICING_BROAD;
	}
	else {
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if (limitEnable==0){
		policer.e_rate_flag &= ~rate_flag; 
		if (policer.e_rate_flag&(FAL_INGRESS_POLICING_STORM)==0){
			policer.e_enable = A_FALSE;
			policer.e_rate_flag = 0x0;
			opl_port_flowctrl_set(0, atherosPort, 1);           //flowctrl is enable when storm is disable, due to s17 chip limitation
		}
	}
	else {
		policer.e_enable = A_TRUE;
		policer.e_rate_flag |= rate_flag;
		opl_port_flowctrl_set(0, atherosPort, 0);           //flowctrl is disable when storm is enable, due to s17 chip limitation
	}
	//printf("%s e_enable %d e_rate_flag %x eir %x.\n", __FUNCTION__, policer.e_enable, policer.e_rate_flag, policer.eir);
	return isis_rate_port_policer_set(0,atherosPort,&policer);
}

OPL_STATUS dalStormCtrlFrameGet(UINT32 portId,UINT32 stormLimitType,UINT32 *limitEnable)
{
	a_uint32_t rate_flag;
    fal_port_policer_t policer;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	memset(&policer, 0, sizeof(fal_port_policer_t));
	isis_rate_port_policer_get(0,atherosPort,&policer);

	if(stormLimitType==0){
		rate_flag = FAL_INGRESS_POLICING_UNK_UNI;
	}
	else if(stormLimitType==1){
		rate_flag = FAL_INGRESS_POLICING_UNK_MUL;
	}
	else if(stormLimitType==0){
		rate_flag = FAL_INGRESS_POLICING_BROAD;
	}
	else {
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if (policer.e_enable)
		*limitEnable = (rate_flag&policer.e_rate_flag);
	else 
		*limitEnable = 0;
	
//	return isis_storm_ctrl_frame_get(0, atherosPort, stormLimitType, limitEnable);	
    return OPL_OK;
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
    fal_port_policer_t policer;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	memset(&policer, 0, sizeof(fal_port_policer_t));
	isis_rate_port_policer_get(0,atherosPort,&policer);

	policer.eir = rateLimit;
	policer.meter_unit = FAL_BYTE_BASED;
	policer.e_enable = 1;
	policer.ebs = rateLimit;

	policer.c_enable = 0;

	//printf("%s e_enable %d e_rate_flag %x eir %x.\n", __FUNCTION__, policer.e_enable, policer.e_rate_flag, policer.eir);

	return isis_rate_port_policer_set(0,atherosPort,&policer);
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
    fal_port_policer_t policer;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	memset(&policer, 0, sizeof(fal_port_policer_t));
	isis_rate_port_policer_get(0,atherosPort,&policer);

	*rateLimit = policer.eir;
    return OPL_OK;
}



