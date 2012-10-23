/*
=============================================================================
     Header Name: dal_port.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                		Date              	Description of Changes
----------------   ------------  ----------------------------------------------
 zran 				 2009/05/20		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#include <dal_lib.h>
#include <odm_port.h>

#include <fal_port_ctrl.h>
#include <fal_portvlan.h>
#include <fal_qos.h>
#include <fal_vlan.h>
#include <fal_stp.h>
#include <fal_misc.h>
#include <fal_rate.h>
#include <aos_types.h>
#include "defs.h"
#include <dal_acl.h>

extern VLAN_CONFI_INFO_t dal_vtt_info[];
extern VTT_VLAN_8228_t	dal_vlan8228[];
extern UINT16	dal_vlan_trunk_num[];
extern UINT8	dal_vlan_transl_num[];
/*******************************************************************************
* dalPortStateGet
*
* DESCRIPTION:
*  		this function is used to get the ethernet port status down or up.
*
*	INPUTS:
*			portId:0x0
*
*	OUTPUTS:
*			status:0x01:up,0x00:down		     
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortStateGet(UINT8 portId,UINT8 *status)
{
    sw_error_t retVal;
    a_bool_t up;
    UINT32  atherosPort = dalToAtherosPortMap[portId];    

#if 0
    get_port_link_state(portId, &status);
#else
    retVal = isis_port_link_status_get(0, atherosPort, &up);
    *status = (UINT8)up;
#endif
        
	return retVal;
}



/*******************************************************************************
* dalPortSpeedSet
*
* DESCRIPTION:
*  		this function is used to set port speed 
*
*	INPUTS:
*			portId:0x01-0x04
*			speed:10 100 1000.
*	OUTPUTS:
*			 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortSpeedSet(UINT8 portId,UINT32 portSpeed)
{
	OPL_STATUS iStatus = OPL_OK;
    unsigned int    speed = 0;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    
	if(PORT_IS_OUT_OF_RANTE(atherosPort))
	{
		OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

    switch(portSpeed)
    {
        case ODM_PORT_SPEED_10M:
            speed = FAL_SPEED_10;
            break;
        case ODM_PORT_SPEED_100M:
            speed = FAL_SPEED_100;
            break;
        case ODM_PORT_SPEED_1000M:
            speed = FAL_SPEED_1000;
            break;
        default:
            OPL_LOG_TRACE();
		    return OPL_ERR_INVALID_PARAMETERS;
            break;
    }
	
	return isis_port_speed_set(0, atherosPort, speed);
}



/*******************************************************************************
* dalPortSpeedGet
* DESCRIPTION:
*  		this function is used to get the port speed
*
*	INPUTS:
*			portId:0x01-0x04
*			speed:10 100 1000.
*	OUTPUTS:
*			 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortSpeedGet(UINT8 portId,UINT32 *portSpeed)
{
	fal_port_speed_t tmpSpeed;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
	if(PORT_IS_OUT_OF_RANTE(atherosPort))
	{
		OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

    if(OPL_NULL == portSpeed)
    {
        return OPL_ERR_NULL_POINTER;
    }

	isis_port_speed_get(0, atherosPort, &tmpSpeed);

	switch(tmpSpeed)
	{
		case FAL_SPEED_10:
			*portSpeed = ODM_PORT_SPEED_10M;
			break;
		case FAL_SPEED_100:
			*portSpeed = ODM_PORT_SPEED_100M;
			break;
		case FAL_SPEED_1000:
			*portSpeed = ODM_PORT_SPEED_1000M;
			break;
		default:
			OPL_LOG_TRACE();
			return OPL_ERR_INVALID_PARAMETERS;
	}

	return OPL_OK;	
}

/* begin: add by xukang for bug 3265 */
/*******************************************************************************
* dalPortSpecificSpeedRead
* DESCRIPTION:
*  		this function is used to get the port speed
*
*	INPUTS:
*			portId:0x01-0x04
*			speed:10 100 1000.
*	OUTPUTS:
*			 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortSpecificSpeedRead(UINT8 portId,UINT32 *portSpeed)
{
	fal_port_speed_t tmpSpeed;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
	if(PORT_IS_OUT_OF_RANTE(atherosPort))
	{
		OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

    if(OPL_NULL == portSpeed)
    {
        return OPL_ERR_NULL_POINTER;
    }

	isis_port_speed_get(0, atherosPort, &tmpSpeed);

	switch(tmpSpeed)
	{
		case FAL_SPEED_10:
			*portSpeed = ODM_PORT_SPEED_10M;
			break;
		case FAL_SPEED_100:
			*portSpeed = ODM_PORT_SPEED_100M;
			break;
		case FAL_SPEED_1000:
			*portSpeed = ODM_PORT_SPEED_1000M;
			break;
		default:
			OPL_LOG_TRACE();
			return OPL_ERR_INVALID_PARAMETERS;
	}

	return OPL_OK;	
}
/* end: add by xukang for bug 3265 */
/*******************************************************************************
* dalPortDuplexSet
*
* DESCRIPTION:
*  		this function is used to set port speed 
*
*	INPUTS:
*			portId:0x01-0x04
*			duplex: 1: half;	2: full;
*	OUTPUTS:
*			 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDuplexSet(UINT8 portId,UINT32 duplex)
{
	unsigned int	duplexMode = 0;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
	if(PORT_IS_OUT_OF_RANTE(atherosPort))
	{
		OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

	switch(duplex)
	{
		case ODM_PORT_DUPLEX_HALF:
			duplexMode = FAL_HALF_DUPLEX;
			break;
		case ODM_PORT_DUPLEX_FULL:
			duplexMode = FAL_FULL_DUPLEX;
			break;
		default:
			OPL_LOG_TRACE();
			return OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	
	return isis_port_duplex_set(0, atherosPort, duplexMode);
}



/*******************************************************************************
* dalPortDuplexGet
*
* DESCRIPTION:
*  		this function is used to get the port duplex
*
*	INPUTS:
*			portId:0x01-0x04
*			duplex: Full-duplex or half-duplex.
*	OUTPUTS:
*			 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDuplexGet(UINT8 portId,UINT32 *duplex)
{
	fal_port_duplex_t	duplexMode;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
	if(PORT_IS_OUT_OF_RANTE(atherosPort))
	{
		OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

    if(OPL_NULL == duplex)
    {
        return OPL_ERR_NULL_POINTER;
    }

	isis_port_duplex_get(0, atherosPort, &duplexMode);

	switch(duplexMode)
	{
		case FAL_HALF_DUPLEX:
			*duplex = ODM_PORT_DUPLEX_HALF;
			break;
		case FAL_FULL_DUPLEX:
			*duplex = ODM_PORT_DUPLEX_FULL;
			break;
		default:
			OPL_LOG_TRACE();
			return OPL_ERR_INVALID_PARAMETERS;
	}

	return OPL_OK;	
}
/* begin: add by xukang for bug 3265 */
/*******************************************************************************
* dalPortSpecificDuplexRead
*
* DESCRIPTION:
*  		this function is used to get the port specific duplex
*
*	INPUTS:
*			portId:0x01-0x04
*			duplex: Full-duplex or half-duplex.
*	OUTPUTS:
*			 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortSpecificDuplexRead(UINT8 portId,UINT32 *duplex)
{
	fal_port_duplex_t	duplexMode;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
	if(PORT_IS_OUT_OF_RANTE(atherosPort))
	{
		OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

    if(OPL_NULL == duplex)
    {
        return OPL_ERR_NULL_POINTER;
    }

	isis_port_duplex_get(0, atherosPort, &duplexMode);

	switch(duplexMode)
	{
		case FAL_HALF_DUPLEX:
			*duplex = ODM_PORT_DUPLEX_HALF;
			break;
		case FAL_FULL_DUPLEX:
			*duplex = ODM_PORT_DUPLEX_FULL;
			break;
		default:
			OPL_LOG_TRACE();
			return OPL_ERR_INVALID_PARAMETERS;
	}

	return OPL_OK;	
}
/* end: add by xukang for bug 3265 */

OPL_STATUS dalPortPvidSet(UINT8 portId,UINT16 pvid)
{	
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if(PORT_IS_OUT_OF_RANTE(atherosPort))
	{
		OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

    return isis_port_default_cvid_set(0, atherosPort, pvid);
}



OPL_STATUS dalPortPvidGet(UINT8 portId,UINT16 *pvid)
{
	a_uint32_t vid;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
	if(PORT_IS_OUT_OF_RANTE(atherosPort))
	{
		OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

    if(OPL_NULL == pvid)
    {
        return OPL_ERR_NULL_POINTER;
    }

    isis_port_default_cvid_get(0, atherosPort, &vid);

	*pvid = vid;

	return OPL_OK;
}



OPL_STATUS dalPortDefaultPriSet(UINT8 portId,UINT8 defaultPri)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
//    return isis_qos_port_default_up_set(0,atherosPort, defaultPri);
isis_qos_port_default_cpri_set(0,atherosPort, defaultPri);
    return OPL_OK;
}



OPL_STATUS dalPortDefaultPriGet(UINT8 portId,UINT8 *defaultPri)
{
	a_uint32_t up;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if (NULL == defaultPri)
	{
		return OPL_ERROR;
	}
	
 //   isis_qos_port_default_up_get(0, atherosPort, &up);
 isis_qos_port_default_cpri_get(0,atherosPort, &defaultPri);

	*defaultPri = up;

	return OPL_OK;
}



/*******************************************************************************
* dalPortFlowcontrolSet
*
* DESCRIPTION:
*  		this function is used to dsiable or enable the flow control
*
*	INPUTS:
*			portId:0x0
*			enable:0x00:disable,0x01:enable.
*	OUTPUTS:
*			 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortFlowcontrolSet(UINT8 portId,UINT8 enable)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    fal_port_policer_t policer;

    dalPortFlowctrlAclSet(0, atherosPort, enable);

    memset(&policer, 0, sizeof(fal_port_policer_t));
    isis_rate_port_policer_get(0, atherosPort, &policer);
    policer.deficit_en = enable;
    if(1 == enable)
    {
        policer.cbs =  2 * 1024 * 1024;
        policer.ebs =  2 * 1024 * 1024;
    }
    else
    {
        policer.cbs =  ODM_US_CBS;
        policer.ebs =  ODM_US_EBS;        
    }
    isis_rate_port_policer_set(0, atherosPort, &policer);

    return opl_port_flowctrl_set(0, atherosPort, enable);

}



/*******************************************************************************
* dalPortFlowcontrolGet
*
* DESCRIPTION:
*  			this function is used to get flowcontrol config info.
*
*	INPUTS:
*			portId:0x0
*	OUTPUTS:
*			enable:0x00:disable,0x01:enable. 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortFlowcontrolGet(UINT8 portId,UINT8 *enable)
{
	a_bool_t en;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if (NULL == enable)
	{
		return OPL_ERROR;
	}
	
	isis_port_flowctrl_get(0, atherosPort, &en);

	*enable = en;

	return OPL_OK;
}



/*******************************************************************************
* dalPortLoopbackSet
*
* DESCRIPTION:
*  		this function is used to dsiable or enable the externel loop back
*
*	INPUTS:
*			portId:0x0
*			enable:0x00:disable,0x01:enable.
*	OUTPUTS:
*			 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortLoopbackSet(UINT8 portId,UINT8 enable)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	return isis_port_lpbk_set(0, atherosPort, enable);

}



/*******************************************************************************
* dalPortUsPolicingEnableSet
*
* DESCRIPTION:
*  			this function is used to set the upstream policing parameter.
*
*	INPUTS:
*			portId:0x0
*			enable:0x00:disable,0x01:enable
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingEnableSet(UINT8 portId,UINT8 enable)
{
	fal_port_policer_t policer;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	memset(&policer, 0, sizeof(fal_port_policer_t));
	isis_rate_port_policer_get(0, atherosPort, &policer);

/*
	if (policer.c_enable)
	{	
		if (0x7fff == speed)
		{
			speed = 0x7fff - 1;
		}
	}*/

	policer.c_enable = enable;
	policer.c_rate_flag = 0xFE;

	//printf("dalPortUsPolicingEnableSet c_enale %d %d.\n", policer.c_enable, policer.cir);
	
	return isis_rate_port_policer_set(0, atherosPort, &policer);

}



/*******************************************************************************
* dalPortUsPolicingEnableGet
*
* DESCRIPTION:
*  			this function is used to set the upstream policing parameter.
*
*	INPUTS:
*			portId:0x0
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingEnableGet(UINT8 portId,UINT8 *enable)
{
	fal_port_policer_t policer;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
	if (NULL == enable)
	{
		return OPL_ERROR;
	}

	memset(&policer, 0, sizeof(fal_port_policer_t));
	isis_rate_port_policer_get(0, atherosPort, &policer);

	*enable = policer.c_enable;

	return OPL_OK;
}



/*******************************************************************************
* dalPortUsPolicingCirSet
*
* DESCRIPTION:
*  			this function is used to set the upstream policing parameter.
*
*	INPUTS:
*			portId:0x0
*			cir:
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingCirSet(UINT8 portId,UINT32 cir)
{
	fal_port_policer_t policer;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	memset(&policer, 0, sizeof(fal_port_policer_t));
	isis_rate_port_policer_get(0, atherosPort, &policer);

	policer.cir = cir;

	return isis_rate_port_policer_set(0, atherosPort, &policer);
}



/*******************************************************************************
* dalPortUsPolicingCirGet
*
* DESCRIPTION:
*  			this function is used to set the upstream policing parameter.
*
*	INPUTS:
*			portId:0x0
*			cir:
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingCirGet(UINT8 portId,UINT32 *cir)
{
	fal_port_policer_t policer;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    OPL_STATUS ret;

	if (NULL == cir)
	{
		return OPL_ERROR;
	}

	memset(&policer, 0, sizeof(fal_port_policer_t));
	isis_rate_port_policer_get(0, atherosPort, &policer);

	*cir = policer.cir;

    return OPL_OK;
}



/*******************************************************************************
* dalPortUsPolicingCbsSet
*
* DESCRIPTION:
*  			this function is used to set the upstream policing parameter.
*
*	INPUTS:
*			portId:0x0
*			ebs:
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingCbsSet(UINT8 portId,UINT32 cbs)
{
	fal_port_policer_t policer;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	memset(&policer, 0, sizeof(fal_port_policer_t));
	isis_rate_port_policer_get(0, atherosPort, &policer);

	policer.cbs = cbs;

	return isis_rate_port_policer_set(0, atherosPort, &policer);
}



/*******************************************************************************
* dalPortUsPolicingCbsGet
*
* DESCRIPTION:
*  			this function is used to set the upstream policing parameter.
*
*	INPUTS:
*			portId:0x0
*			cbs:
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingCbsGet(UINT8 portId,UINT32 *cbs)
{
	fal_port_policer_t policer;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if (NULL == cbs)
	{
		return OPL_ERROR;
	}

	memset(&policer, 0, sizeof(fal_port_policer_t));
	isis_rate_port_policer_get(0, atherosPort, &policer);

	*cbs = policer.cbs;

    return OPL_OK;
}



/*******************************************************************************
* dalPortUsPolicingEbsSet
*
* DESCRIPTION:
*  			this function is used to set the upstream policing parameter.
*
*	INPUTS:
*			portId:0x0
*			ebs:
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingEbsSet(UINT8 portId,UINT32 ebs)
{
	fal_port_policer_t policer;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	memset(&policer, 0, sizeof(fal_port_policer_t));
	isis_rate_port_policer_get(0, atherosPort, &policer);

	policer.ebs = ebs;

	return isis_rate_port_policer_set(0, atherosPort, &policer);
}



/*******************************************************************************
* dalPortUsPolicingEbsGet
*
* DESCRIPTION:
*  			this function is used to set the upstream policing parameter.
*
*	INPUTS:
*			portId:0x0
*			ebs:
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingEbsGet(UINT8 portId,UINT32 *ebs)
{
	fal_port_policer_t policer;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if (NULL == ebs)
	{
		return OPL_ERROR;
	}

	memset(&policer, 0, sizeof(fal_port_policer_t));
	isis_rate_port_policer_get(0, atherosPort, &policer);

	*ebs = policer.ebs;

    return OPL_OK;
}


/*******************************************************************************
* dalPortUsPolicingSet
*
* DESCRIPTION:
*  			this function is used to set the upstream policing parameter.
*
*	INPUTS:
*			portId:0x0
*			enable:0x00:disable,0x01:enable
*			cir:  0x000000-0xffffff
*			cbs:
*			ebs:
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingSet(UINT8 portId,UINT8 enable,UINT32 cir,UINT32 cbs,UINT32 ebs)
{
	fal_port_policer_t policer;
	fal_port_speed_t speed;
	a_uint32_t atheroscbs =  128*1024;
       UINT32  atherosPort = dalToAtherosPortMap[portId];
	UINT8  flowctrlEnable;

	if (enable){
		isis_port_speed_get(0, atherosPort, &speed);

		switch(speed)
		{
			case FAL_SPEED_10:
				atheroscbs = 4*1024;
				break;
			case FAL_SPEED_100:
				atheroscbs = 32*1024;
				break;
			case FAL_SPEED_1000:
				atheroscbs = 128*1024;
				break;
		}

		if (cbs < atheroscbs){
			cbs = atheroscbs;
		}
		if (ebs < atheroscbs){
			ebs = atheroscbs;
		}
	}
	else
	{
	    cir = 0;
	}
	
       
       dalAclIngressRatelimitSet(portId, DAL_ACL_INGRESS_RATELIMIT_PORT1_POLICY_ID+atherosPort-1, cir);  //for s17 flow control

	dalPortFlowcontrolGet(portId, &flowctrlEnable);
	memset(&policer, 0, sizeof(fal_port_policer_t));
	isis_rate_port_policer_get(0, atherosPort, &policer);

	policer.c_enable = enable;
	policer.c_rate_flag = 0xFE;
	policer.cir = cir;
	if(flowctrlEnable == 1)
	{
	    policer.cbs = 2 * 1024 * 1024;
	    policer.ebs = 2 * 1024 * 1024;
	}
	else
	{
	    policer.cbs = cbs;
	    policer.ebs = ebs;
	}

	//printf("dalPortUsPolicingSet c_enale %d %d.\n", enable, cir);
       isis_rate_port_policer_set(0, atherosPort, &policer);
	
	return  OPL_OK;
}



/*******************************************************************************
* dalPortUsPolicingGet
*
* DESCRIPTION:
*  			this function is used to get the shaper parameter
*
*	INPUTS:
*			portId:0x0
*			
*	OUTPUTS:
*			enable:0x00:disable,0x01:enable
*			cir:
*			cbs:
*			ebs:	
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingGet(UINT8 portId,UINT8 *enable,UINT32 *cir,UINT32 *cbs,UINT32 *ebs)
{
	fal_port_policer_t policer;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	memset(&policer, 0, sizeof(fal_port_policer_t));
	isis_rate_port_policer_get(0, atherosPort, &policer);

	*enable = policer.c_enable;
	*cir = policer.cir;
	*cbs = policer.cbs;
	*ebs = policer.ebs;
	
	return OPL_OK;
}




/*******************************************************************************
* dalPortDsPolicingEnableSet
*
* DESCRIPTION:
*  			this function is used to set the shaper parameter
*
*	INPUTS:
*			portId:0x0
*			enable:0x00:disable,0x01:enable
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDsPolicingEnableSet(UINT8 portId,UINT8 enable)
{
	a_bool_t en;
	fal_egress_shaper_t shaper;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	memset(&shaper, 0, sizeof(fal_egress_shaper_t));
	
	isis_rate_port_shaper_get(0, atherosPort, &en, &shaper);

	en = enable;

	//printf("dalPortDsPolicingEnableSet c_enale %d %d.\n", enable, shaper.cir);

	
	return isis_rate_port_shaper_set(0, atherosPort,en, &shaper);
}



/*******************************************************************************
* dalPortDsPolicingEnableGet
*
* DESCRIPTION:
*  			this function is used to get the shaper parameter
*
*	INPUTS:
*			portId:0x0
*			
*	OUTPUTS:
*			enable:0x00:disable,0x01:enable
*			cir:
*			cbs:
*			ebs:	
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDsPolicingEnableGet(UINT8 portId,UINT8 *enable)
{
	a_bool_t en;
	fal_egress_shaper_t shaper;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	memset(&shaper, 0, sizeof(fal_egress_shaper_t));
	
	isis_rate_port_shaper_get(0, atherosPort, &en, &shaper);

	*enable = en;
    return OPL_OK;
}



/*******************************************************************************
* dalPortDsPolicingCirSet
*
* DESCRIPTION:
*  			this function is used to set the shaper parameter
*
*	INPUTS:
*			portId:0x0
*           cir:
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDsPolicingCirSet(UINT8 portId,UINT32 cir)
{
	a_bool_t en;
	fal_egress_shaper_t shaper;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	memset(&shaper, 0, sizeof(fal_egress_shaper_t));
	
	isis_rate_port_shaper_get(0, atherosPort, &en, &shaper);

	shaper.cir = cir;
	
	return isis_rate_port_shaper_set(0, atherosPort,en, &shaper);
}



/*******************************************************************************
* dalPortDsPolicingCirGet
*
* DESCRIPTION:
*  			this function is used to get the shaper parameter
*
*	INPUTS:
*			portId:0x0
*			cir
*	OUTPUTS:
*			enable:0x00:disable,0x01:enable
*			cir:
*			cbs:
*			ebs:	
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDsPolicingCirGet(UINT8 portId,UINT32 *cir)
{
	a_bool_t en;
	fal_egress_shaper_t shaper;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	memset(&shaper, 0, sizeof(fal_egress_shaper_t));
	
	isis_rate_port_shaper_get(0, atherosPort, &en, &shaper);

	*cir = shaper.cir;
	
	return OPL_OK;
}



/*******************************************************************************
* dalPortDsPolicingPirSet
*
* DESCRIPTION:
*  			this function is used to set the shaper parameter
*
*	INPUTS:
*			portId:0x0
*			enable:0x00:disable,0x01:enable
*           pir
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDsPolicingPirSet(UINT8 portId,UINT32 cbs)
{
	a_bool_t en;
	fal_egress_shaper_t shaper;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	memset(&shaper, 0, sizeof(fal_egress_shaper_t));
	
	isis_rate_port_shaper_get(0, atherosPort, &en, &shaper);

	shaper.cbs = cbs;
	
	return isis_rate_port_shaper_set(0, atherosPort,en, &shaper);
}



/*******************************************************************************
* dalPortDsPolicingPirGet
*
* DESCRIPTION:
*  			this function is used to set the shaper parameter
*
*	INPUTS:
*			portId:0x0
*			enable:0x00:disable,0x01:enable
*           pir
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDsPolicingPirGet(UINT8 portId,UINT32 *cbs)
{
	a_bool_t en;
	fal_egress_shaper_t shaper;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	memset(&shaper, 0, sizeof(fal_egress_shaper_t));
	
	isis_rate_port_shaper_get(0, atherosPort, &en, &shaper);

	*cbs = shaper.cbs;
	
	return OPL_OK;
}



/*******************************************************************************
* dalPortDsPolicingSet
*
* DESCRIPTION:
*  			this function is used to set the shaper parameter
*
*	INPUTS:
*			portId:0x0
*			enable:0x00:disable,0x01:enable
*			cir:
*			cbs:
*			ebs:
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDsPolicingSet(UINT8 portId,UINT8 enable,UINT32 cir,UINT32 cbs)
{
	a_bool_t en;
	fal_egress_shaper_t shaper;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	memset(&shaper, 0, sizeof(fal_egress_shaper_t));
	
	isis_rate_port_shaper_get(0, atherosPort, &en, &shaper);

	shaper.cir = cir;
	shaper.cbs = cbs;
	en = enable;
	shaper.meter_unit = FAL_BYTE_BASED;

	//printf("dalPortDsPolicingSet en %d %d.\n", en, shaper.cir);
	
	return isis_rate_port_shaper_set(0, atherosPort,en, &shaper);
}



/*******************************************************************************
* dalPortDsPolicingGet
*
* DESCRIPTION:
*  			this function is used to get the shaper parameter
*
*	INPUTS:
*			portId:0x0
*			
*	OUTPUTS:
*			enable:0x00:disable,0x01:enable
*			cir:
*			cbs:
*			ebs:	
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDsPolicingGet(UINT8 portId,UINT8 *enable,UINT32 *cir,UINT32 *cbs)
{
	a_bool_t en;
	fal_egress_shaper_t shaper;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	memset(&shaper, 0, sizeof(fal_egress_shaper_t));
	
	isis_rate_port_shaper_get(0, atherosPort, &en, &shaper);

	*enable = en;
	*cir = shaper.cir;
	*cbs = shaper.cbs;
	
	return OPL_OK;
}



/*******************************************************************************
* dalLoopDetectionStatusSet
*
* DESCRIPTION:
*  		this function is used to set looping detection function.
*
*	INPUTS:
*		time: 		loop check timer
*		enable:		enable:1;	disable:0
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalLoopDetectionStatusSet(UINT8 time, a_bool_t enable)
{
	fal_loop_check_time_t tmpTime = time;
	
	if (FAL_LOOP_CHECK_500MS < time)
	{
		tmpTime = FAL_LOOP_CHECK_500MS;
	}
	
//	return isis_loop_check_status_set(0,(fal_loop_check_time_t)tmpTime, 
//		(a_bool_t)enable);
    return OPL_OK;
}



/*******************************************************************************
* dalLoopDetectionStatusGet
*
* DESCRIPTION:
*  		this function is used to get looping detection status.
*
*	INPUTS:
*
*	OUTPUTS:
*		time: 		loop check timer
					1: 1ms; 2: 10ms; 3: 100ms; 4:500ms; 
*		enable:		enable:1;	disable:0
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalLoopDetectionStatusGet(fal_loop_check_time_t * time, UINT8 * enable)
{
	if ((NULL == time) || (NULL == enable))
	{
		return OPL_ERROR;
	}
	
//	return isis_loop_check_status_get(0,time, (a_bool_t *)enable);
    return OPL_OK;
}



/*******************************************************************************
* dalLoopDetectionInfoGet
*
* DESCRIPTION:
*  		this function is used to get looping old and new ports.
*
*	INPUTS:
*
*	OUTPUTS:
*		oldPortId: 		looping old port
*		newPortId: 		looping new port
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalLoopDetectionInfoGet(UINT32 * oldPortId, UINT32 * newPortId)
{
	if ((NULL == oldPortId) || (NULL == newPortId))
	{
		return OPL_ERROR;
	}
	
//	isis_loop_check_info_get(0, oldPortId, newPortId);
	*oldPortId = atherosToDalPortMap[*oldPortId];
	*newPortId = atherosToDalPortMap[*newPortId];

	return OPL_OK;
}



/*******************************************************************************
* dalLoopDetectionIntrGet
*
* DESCRIPTION:
*  		this function is used to get looping interrupt status.
*
*	INPUTS:
*
*	OUTPUTS:
*		intr: 		looping detection interrupt status (0: off; 1: on)
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalLoopDetectionIntrGet(a_bool_t * intr)
{
	if (NULL == intr)
	{
		return OPL_ERROR;
	}
	
//	return isis_loop_check_intr_get(0, intr);
    return OPL_OK;
}



/*******************************************************************************
* dalLoopDetectionIntrClear
*
* DESCRIPTION:
*  		this function is used to clear looping interrupt status.
*
*	INPUTS:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalLoopDetectionIntrClear(void)
{
//	return isis_loop_check_intr_clear(0);
    return OPL_OK;
}



/*******************************************************************************
* dalPhyAdminStateGet
*
* DESCRIPTION:
*  		this function is used to get the port's phy admin status.0x01 for disable,0x02 for enable.
*
*	INPUTS:
*		portId: 		portId
*		state:		enable:0x02,disable:0x01
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPhyAdminStateGet(UINT8 portId,UINT32 *state)
{
    sw_error_t retVal;
    a_bool_t up = A_FALSE;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

    retVal = isis_port_admin_status_get(0, atherosPort, &up);
    *state = (UINT32)up;
	
    return OPL_OK;
}



/*******************************************************************************
* dalPhyAdminControlSet
*
* DESCRIPTION:
*  		this function is used to set the port phy admin status,0x0 for disable,0x01 for enable
*
*	INPUTS:
*		portId: 		portId
*		state:		disable:0x00, enable:0x01
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPhyAdminControlSet(UINT8 portId,UINT32 state)
{
    UINT32  phyPort = logicToPhyPortMap[portId];
    UINT32  atherosPort = dalToAtherosPortMap[portId];
   sw_error_t rv;
    a_uint32_t val;

	if(PORT_IS_OUT_OF_RANTE(portId))
	{
		OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(PHY_PORT_ADMIN_ENABLE == state)
	{
		isis_stp_port_state_set(0, 0, atherosPort, FAL_STP_FARWARDING);

              isis_port_shutdown_set_only(0, atherosPort, 0);
              //isis_set_port_enable(0, atherosPort, PHY_PORT_ADMIN_ENABLE);     
              return OPL_OK;
	}
	else if(PHY_PORT_ADMIN_DISABLE == state)
	{
		/*fixed bug 3272,clear fdb table,the same as operation fuction*/
		dalArlFlushDynamic();
		isis_stp_port_state_set(0, 0, atherosPort, FAL_STP_DISABLED);

              isis_port_shutdown_set_only(0, atherosPort, 1);
              //isis_set_port_enable(0, atherosPort, PHY_PORT_ADMIN_DISABLE);    
              return OPL_OK;
	}
	else
	{
		OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}
}




/*******************************************************************************
* dalPhyAutonegAdminStateGet
*
* DESCRIPTION:
*  		this function is used to get autonegocitation config info for the port.0x01 for disable,0x02 for enable
*
*	INPUTS:
*		portId: 		portId
*		
*	OUTPUTS:
*		state:		disable:0x01,enable:0x02
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPhyAutonegAdminStateGet(UINT8 portId,UINT32 *state)
{
	a_bool_t status;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
	isis_port_autoneg_status_get(0, atherosPort, &status);

	if (status)
	{
		*state = 0x2;
	}
	else
	{
		*state = 0x1;
	}

	return OPL_OK;
}
/* begin: add by xukang for bug 3265 */
/*******************************************************************************
* dalPhySpecificAutonegRead
*
* DESCRIPTION:
*  		this function is used to get autonegocitation status info for the port.0x01 for completed,0x02 for failed
*
*	INPUTS:
*		portId: 		portId
*		
*	OUTPUTS:
*		state:		completed:0x01,failed:0x02
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPhySpecificAutonegRead(UINT8 portId, UINT32  *state)
{
	a_bool_t  status;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
		
    isis_port_autoneg_done_read(0, atherosPort, &status);
    if (status==A_FALSE)
	{
		*state = 0x0;
	}
	else
	{
		*state = 0x1;
	}
	
	return OPL_OK;	
}
/* end: add by xukang for bug 3265 */

/*******************************************************************************
* dalPhyAutonegEnableSet
*
* DESCRIPTION:
*  		this function is used to set autonegocitation enable. 0x01 for dsiable,0x02 for enable
*
*	INPUTS:
*		portId: 		portId
*		enable:		disable:0x01;eanble:0x02
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPhyAutonegEnableSet(UINT8 portId,UINT32 enable)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if(PORT_IS_OUT_OF_RANTE(atherosPort))
	{
		OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	if(enable == DAL_PORT_ADMIN_ENABLE)
	{
		return isis_port_autoneg_enable(0, atherosPort);
	}
	else if(enable == DAL_PORT_ADMIN_DISABLE)
	{
		return isis_port_autoneg_disable(0, atherosPort);
	}
	else 
	{
		OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}
}



/*******************************************************************************
* dalPhyAutonegRestart
*
* DESCRIPTION:
*  		this function is used to force the port restart autonegocitation.
*
*	INPUTS:
*		portId: 		portId
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPhyAutonegRestart(UINT8 portId)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	return isis_port_autoneg_restart(0, atherosPort);
}

OPL_STATUS dalVlan8021QDelCheck(UINT32 vlanId)
{
    int rv;
	
    if(((dal_vlan8228[vlanId].member >> 1) & 0xF) == 0)
   {
       rv = isis_vlan_delete(0, vlanId);
      if (SW_OK == rv)
      {
        memset(&dal_vlan8228[vlanId], 0 , sizeof(VTT_VLAN_8228_t));
        dal_vlan8228[vlanId].type = DAL_VLAN_UNUSED;
        dal_vlan8228[vlanId].member = 0;
      }
    }

    return  OPL_OK;
}
/*******************************************************************************
* dalToTransparentModeHandle
*
* DESCRIPTION:
*  		this function is used to set default vlan of all ports when the mode is changed to transparent
*
*	INPUTS:
*		portId: 		portId
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalToTransparentModeHandle(UINT8 portId)
{
	OPL_STATUS iStatus = OPL_OK;
	a_uint32_t vid;
	fal_vlan_t p_vlan;
	fal_pbmp_t member;
	fal_pbmp_t u_member = 0;	
	int rv;
       UINT32  atherosPort = dalToAtherosPortMap[portId];
	UINT32 entryIndex;
	UINT32 i, j, cVlan, sVlan;
	UINT32 ar8228Port;
	a_uint32_t tagVid;
	fal_vlan_t  tag_p_vlan;
	fal_pbmp_t tag_member;
	OPL_BOOL  tagFlag = OPL_FALSE;

#if 0
       //judge if there are vlans configured on the other ports 
	for (i = DAL_MIN_PORT_NUM; i <= DAL_MAX_PORT_NUM; i++)
	{
                    ar8228Port = dalToAtherosPortMap[i];
		if (ar8228Port == atherosPort)
	       {
	              continue;
	       }
		if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TAG) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRUNK)|
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION))
		{     
		       isis_port_default_cvid_get(0, ar8228Port, &vid);
		       tagVid = dal_vtt_info[atherosPort].defaultVlanTag;

			if (vid == tagVid)
			{
			        tagFlag = OPL_TRUE;
				 break;
			}

			if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRUNK)
			{
			    for(j = 1; j <= 4094; j++)
			    {
				    if (OPL_OK == dalTrunkFind(ar8228Port, j))
				    {
				        if(j == tagVid)
				        {
				            tagFlag = OPL_TRUE;
					     break;
				        }
				    }
			    }
			}

			if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION|
				dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION)
			{
			      for(j = 0; j < ODM_NUM_OF_VTT; j++)
			      {
			          if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))    
			          {
			               if(sVlan == tagVid)
				        {
				            tagFlag = OPL_TRUE;
					     break;
				        }
				    }
			    }
			}
		}
	}

	if (tagFlag == OPL_FALSE)
       {
	       for (i = DAL_MIN_PORT_NUM; i <= DAL_MAX_PORT_NUM; i++)
		{
                        ar8228Port = dalToAtherosPortMap[i];
			if (ar8228Port == atherosPort)
	              {
	                   continue;
	              }
			if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSPARENT) & 
				((dal_vtt_info[atherosPort].vlanMode == VLAN_TAG) |
				(dal_vtt_info[atherosPort].vlanMode == VLAN_TRANSLATION) |
				(dal_vtt_info[atherosPort].vlanMode == VLAN_TRUNK)|
				(dal_vtt_info[atherosPort].vlanMode == VLAN_AGGREGATION)))
			{  
                          vid = dal_vtt_info[atherosPort].defaultVlanTag;
			     rv = isis_vlan_find(0, vid, &p_vlan);
			     if (SW_OK == rv)
		              {     
			             p_vlan.mem_ports  &= ~(1 << ar8228Port);
                         p_vlan.mem_ports &= ~(1 << 6);
                      //   isis_vlan_member_add(0, vid, p_vlan.mem_ports, 0);
                      isis_vlan_member_del(0, vid, ar8228Port);
					  isis_vlan_member_del(0, vid, 6);
				      dal_vlan8228[vid].portId[ar8228Port] = 0;
                      dal_vlan8228[vid].portId[6] = 0;
				      dal_vlan8228[vid].member = p_vlan.mem_ports;
		              }
			      if((dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSPARENT) &
				  	(dal_vtt_info[atherosPort].vlanMode == VLAN_TRUNK))
			      	{
			      	      for(j = 1; j <= 4094; j++)
			             {
				           if ((OPL_OK != dalTrunkFind(atherosPort, j)) | (j == vid))
		                         {
			                       continue;
		                         }
				           else
				           {
				                rv = isis_vlan_find(0, j, &p_vlan);
				                if (SW_OK == rv)
		                              {     
			                           p_vlan.mem_ports  &= ~(1 << ar8228Port);
                                                p_vlan.mem_ports &= ~(1 << 6);
                                        //      isis_vlan_member_add(0, j, p_vlan.mem_ports, 0);
                                       isis_vlan_member_del(0, j, ar8228Port);
					  					isis_vlan_member_del(0, j, 6);
				                    dal_vlan8228[j].portId[ar8228Port] = 0;
                                                dal_vlan8228[j].portId[6] = 0;
				                    dal_vlan8228[j].member = p_vlan.mem_ports;
		                              }
				           }
			             }
		              }

				if((dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSPARENT) &
				  	(dal_vtt_info[atherosPort].vlanMode == VLAN_TRANSLATION|
				  	dal_vtt_info[atherosPort].vlanMode == VLAN_AGGREGATION))
			      {
			           for(j = 0; j < ODM_NUM_OF_VTT; j++)
			           {
			                if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))
				         {
				              rv = isis_vlan_find(0, sVlan, &p_vlan);
					       if (SW_OK == rv)
	                                   {     
		                                 p_vlan.mem_ports &=  (~(1 << ar8228Port));
					           // isis_vlan_member_add(0, sVlan, p_vlan.mem_ports, 0);
								isis_vlan_member_del(0, sVlan, ar8228Port);
					            dal_vlan8228[sVlan].portId[ar8228Port] = 0;
	                                        dal_vlan8228[sVlan].member = p_vlan.mem_ports;
					            dalVlan8021QDelCheck(sVlan);
	                                    }
				           }						
		                   }
			      }
			}
	       }
       }
#endif
	/* delete existing vlan or records */
	dalTagDelvlan(portId);
	dalVttDelVlan(portId);
	dalVttDelRec(portId);
	dalTrunkDelvlan(portId);

	for (vid = 0; vid <= DAL_VLAN_VID_MAX; vid++)
	{
		if (DAL_VLAN_UNUSED == dal_vlan8228[vid].type)
			continue;
		
		if (DAL_VLAN_MULTICAST != dal_vlan8228[vid].type && DAL_VLAN_MC_VTT != dal_vlan8228[vid].type	
			&& dal_vlan8228[vid].portId[atherosPort])
		{
			if (dal_vlan8228[vid].member == (1 << portId)){
				printf("%s delete vid %d only for this port %d.", __FUNCTION__, vid, portId);
				rv = isis_vlan_delete(0, vid);
				if (SW_OK == rv)
				{
					memset(&dal_vlan8228[vid], 0 , sizeof(VTT_VLAN_8228_t));
					dal_vlan8228[vid].type = DAL_VLAN_UNUSED;
					dal_vlan8228[vid].member = 0;
				}
			}
		}
	}


#if 0
	for (i = DAL_MIN_PORT_NUM; i <= DAL_MAX_PORT_NUM; i++)
	{
                    ar8228Port = dalToAtherosPortMap[i];
		if (ar8228Port == atherosPort)
	       {
	              continue;
	       }
		if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TAG) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRUNK)|
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION))
		{     
		       isis_port_default_cvid_get(0, ar8228Port, &vid);
			rv = isis_vlan_find(0, vid, &p_vlan);
			if (SW_OK == rv)
	              {     
		             p_vlan.mem_ports |= (1 << atherosPort);
                     isis_vlan_member_add(0, vid, atherosPort, 0);	
			      dal_vlan8228[vid].portId[atherosPort] = 1;
			      dal_vlan8228[vid].member = p_vlan.mem_ports;
	              }

			if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRUNK)
			{
			    for(j = 1; j <= 4094; j++)
			    {
				    if ((OPL_OK != dalTrunkFind(ar8228Port, j)) | (j == vid))
		                  {
			               continue;
		                  }
				    else
				    {
				        rv = isis_vlan_find(0, j, &p_vlan);
					 if (SW_OK == rv)
	                             {     
		                          p_vlan.mem_ports |= (1 << atherosPort);
								isis_vlan_member_add(0, j, atherosPort, 0);	
			                   dal_vlan8228[j].portId[atherosPort] = 1;
			                   dal_vlan8228[j].member = p_vlan.mem_ports;
	                             }
				    }
			    }
			}

			if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION|
				dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION)
			{
			    for(j = 0; j < ODM_NUM_OF_VTT; j++)
			    {
			        if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))
				 {
				      rv = isis_vlan_find(0, sVlan, &p_vlan);
				      if (SW_OK == rv)
	                           {     
		                          p_vlan.mem_ports |= (1 << atherosPort);
                                  isis_vlan_member_add(0, sVlan, atherosPort, 0);	
			                   dal_vlan8228[sVlan].portId[atherosPort] = 1;
			                   dal_vlan8228[sVlan].member = p_vlan.mem_ports;
	                           }
				  }		
			    }
			}
		}
	}
#endif

	return OPL_OK;
}


/*******************************************************************************
* dalToTagModeHandle
*
* DESCRIPTION:
*  		this function is used to set default vlan of all ports when the mode is changed to tag
*
*	INPUTS:
*		portId: 		portId
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalToTagModeHandle(UINT8 portId)
{
	OPL_STATUS iStatus = OPL_OK;
	a_uint32_t vid;
	fal_vlan_t p_vlan;
	fal_pbmp_t member;
	fal_pbmp_t u_member = 0;	
	int rv;
       UINT32  atherosPort = dalToAtherosPortMap[portId];
	UINT32 entryIndex;
	UINT32 i, j, cVlan, sVlan;
	UINT32 ar8228Port;
	a_uint32_t tagVid;
	fal_vlan_t  tag_p_vlan;
	fal_pbmp_t tag_member;

       /* add vlan, its vid = this port pcvid. for odm does not create such vlan */
	isis_port_default_cvid_get(0, atherosPort, &vid);

	//printf("dalToTagModeHandle atherosport %d cvid %d.\n", atherosPort, vid);

	if ((DAL_VLAN_UNUSED != dal_vlan8228[vid].type)
		&& (DAL_VLAN_VTT != dal_vlan8228[vid].type) 
		&& (DAL_VLAN_MULTICAST != dal_vlan8228[vid].type) 
		&& (DAL_VLAN_MC_VTT != dal_vlan8228[vid].type)
		&& (DAL_VLAN_TRUNK != dal_vlan8228[vid].type)
		&& (DAL_VLAN_AGGRESSTION != dal_vlan8228[vid].type))
	{
		return OPL_ERROR;
	}


	rv = isis_vlan_find(0, vid, &p_vlan);
	if (SW_OK == rv)
	{
		member = p_vlan.mem_ports | (1 << atherosPort);
	}
	else
	{
		rv = isis_vlan_create(0, vid);
		if (SW_OK != rv)
		{
			return OPL_ERROR;
		}
		member = 0x1 + (1 << atherosPort);
		isis_vlan_member_add(0, vid, 0, FAL_EG_TAGGED);
	}

	
	rv = isis_vlan_member_add(0, vid, atherosPort, FAL_EG_UNTAGGED);
	//printf("return %d vid %d-0x%x.\n", rv, vid, member);
	#if 0
             /* added by cylan for bug3027*/
	for(i = DAL_MIN_PORT_NUM; i <= DAL_MAX_PORT_NUM; i++)
	{
        ar8228Port = dalToAtherosPortMap[i];
		if (ar8228Port == atherosPort)
	    {
	        continue;
	    }

		if (dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSPARENT)
		{
			member |=  (1 << ar8228Port);
			isis_vlan_member_add(0, vid, ar8228Port, 0);
		}
		else if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TAG) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRUNK)|
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION))
		{
		        if (dal_vtt_info[atherosPort].vlanMode == VLAN_TRANSPARENT)
		        {
		              isis_port_default_cvid_get(0, ar8228Port, &tagVid);
					  rv = isis_vlan_find(0, tagVid, &tag_p_vlan);
                     if (SW_OK == rv)
                     {     
	                     tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
						//isis_vlan_member_add(0, tagVid, tag_p_vlan.mem_ports, 0);
						isis_vlan_member_del(0, tagVid, atherosPort);
						dal_vlan8228[tagVid].portId[atherosPort] = 0;
                        dal_vlan8228[tagVid].member = tag_p_vlan.mem_ports;
                      }

				if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRUNK)
				{
				     for(j = 1; j <= 4094; j++)
			            {
				         if ((OPL_OK != dalTrunkFind(ar8228Port, j)) | (j == tagVid))
		                       {
			                    continue;
		                       }
				         else
				        {
				             rv = isis_vlan_find(0, j, &tag_p_vlan);
					      	 if (SW_OK == rv)
                              {     
                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
								//isis_vlan_member_add(0, j, tag_p_vlan.mem_ports, 0);
								isis_vlan_member_del(0, j, atherosPort);
			          			dal_vlan8228[j].portId[atherosPort] = 0;
                                  dal_vlan8228[j].member = tag_p_vlan.mem_ports;
                              }
				        }
			           }
				}

				if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION|
					dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION)
				{
				    for(j = 0; j < ODM_NUM_OF_VTT; j++)
				    {
				        if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))
				        {
				            rv = isis_vlan_find(0, sVlan, &tag_p_vlan);
					      if (SW_OK == rv)
	                                  {     
		                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
										//isis_vlan_member_add(0, sVlan, tag_p_vlan.mem_ports, 0);
										isis_vlan_member_del(0, sVlan, atherosPort);
					          dal_vlan8228[sVlan].portId[atherosPort] = 0;
	                                      dal_vlan8228[sVlan].member = tag_p_vlan.mem_ports;
	                                  }
				        }
			           }
				}
		        }
		}

		if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSPARENT) & 
			((dal_vtt_info[atherosPort].vlanMode == VLAN_TRANSLATION) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_TRUNK) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_TAG)|
			(dal_vtt_info[atherosPort].vlanMode == VLAN_AGGREGATION)))
		{   
                   tagVid = dal_vtt_info[atherosPort].defaultVlanTag;
		     rv = isis_vlan_find(0, tagVid, &tag_p_vlan);
		     if (SW_OK == rv)
	              {  
		             tag_p_vlan.mem_ports  &= ~(1 << ar8228Port);
					//isis_vlan_member_add(0, tagVid, tag_p_vlan.mem_ports, 0);
					isis_vlan_member_del(0, tagVid, ar8228Port);
			      dal_vlan8228[tagVid].portId[ar8228Port] = 0;
			      dal_vlan8228[tagVid].member = tag_p_vlan.mem_ports;
			      dalVlan8021QDelCheck(tagVid);
	              }

		      if(dal_vtt_info[atherosPort].vlanMode == VLAN_TRUNK)
		      {
		          for(j = 1; j <= 4094; j++)
  	                 {
  		          if ((OPL_OK != dalTrunkFind(atherosPort, j)) | (j == tagVid))
                        {
  	                    continue;
                        }
  		          else
  		          {
  		               rv = isis_vlan_find(0, j, &tag_p_vlan);
  			        if (SW_OK == rv)
                             {     
                                  tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
							//	isis_vlan_member_add(0, j, tag_p_vlan.mem_ports, 0);
							isis_vlan_member_del(0, j, ar8228Port);
  			             dal_vlan8228[j].portId[ar8228Port] = 0;
                                  dal_vlan8228[j].member = tag_p_vlan.mem_ports;
				      dalVlan8021QDelCheck(j);
                             }
  		          }
  	                 }       
	              }

			if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION|
				dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION)
		       {
		           for(j = 0; j < ODM_NUM_OF_VTT; j++)
			    {
			        if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))
				 {
				       rv = isis_vlan_find(0, sVlan, &tag_p_vlan);
					if (SW_OK == rv)
	                            {     
		                           tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
								//isis_vlan_member_add(0, sVlan, tag_p_vlan.mem_ports, 0);
								isis_vlan_member_del(0, sVlan, ar8228Port);
					      dal_vlan8228[sVlan].portId[ar8228Port] = 0;
	                                  dal_vlan8228[sVlan].member = tag_p_vlan.mem_ports;
					      dalVlan8021QDelCheck(sVlan);
	                             }
				  }						
		            }
		         }
		}
					
	}
	#endif
	//rv = isis_vlan_member_add(0, vid, member, 0);
	//printf("2return %d vid %d-0x%x.\n", rv, vid, member);

#if 0
	if (DAL_VLAN_UNUSED != dal_vlan8228[vid].type)
	{
		dal_vlan8228[vid].type = DAL_VLAN_VTT;
	}
	else  if (DAL_VLAN_MULTICAST != dal_vlan8228[vid].type)
	{
		dal_vlan8228[vid].type = DAL_VLAN_MC_VTT;
	}
#else
	dal_vlan8228[vid].type = DAL_VLAN_TAG;
#endif

	dal_vlan8228[vid].portId[atherosPort] = 1;
	dal_vlan8228[vid].member = member;

	dal_vlan_transl_num[atherosPort] = 0;

       dal_vtt_info[atherosPort].vlanMode = VLAN_TAG;
	dal_vtt_info[atherosPort].defaultVlanTag = vid;

	isis_port_default_svid_set(0, atherosPort, vid);

	//iStatus = dalVttTagEntryAdd(0, 0, 0, vid, portId, &entryIndex, DAL_VTT_UC);

	return OPL_OK;
}


/*******************************************************************************
* dalToTranslationModeHandle
*
* DESCRIPTION:
*  		this function is used to set default vlan of all ports when the mode is changed to translation
*
*	INPUTS:
*		portId: 		portId
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalToTranslationModeHandle(UINT8 portId)
{
	OPL_STATUS iStatus = OPL_OK;
	a_uint32_t vid;
	fal_vlan_t p_vlan;
	fal_pbmp_t member;
	fal_pbmp_t u_member = 0;	
	int rv;
       UINT32  atherosPort = dalToAtherosPortMap[portId];
	UINT32 entryIndex;
	UINT32 i,j,cVlan,sVlan;
	UINT32 ar8228Port;
	a_uint32_t tagVid;
	fal_vlan_t  tag_p_vlan;
	fal_pbmp_t tag_member;
	/* add vlan, its vid = this port pcvid. for odm does not create such vlan */
	isis_port_default_cvid_get(0, atherosPort, &vid);

	if ((DAL_VLAN_UNUSED != dal_vlan8228[vid].type)
		&& (DAL_VLAN_VTT != dal_vlan8228[vid].type) 
		&& (DAL_VLAN_MULTICAST != dal_vlan8228[vid].type) 
		&& (DAL_VLAN_MC_VTT != dal_vlan8228[vid].type)
		&& (DAL_VLAN_TRUNK != dal_vlan8228[vid].type)
		&& (DAL_VLAN_AGGRESSTION != dal_vlan8228[vid].type))
	{
		return OPL_ERROR;
	}

	rv = isis_vlan_find(0, vid, &p_vlan);
	if (SW_OK == rv)
	{
		member = p_vlan.mem_ports | (1 << atherosPort);
	}
	else
	{
		rv = isis_vlan_create(0, vid);
		if (SW_OK != rv)
		{
			return OPL_ERROR;
		}
		member = 0x1 + (1 << atherosPort);
		isis_vlan_member_add(0, vid, 0, FAL_EG_TAGGED);
	}

	rv = isis_vlan_member_add(0, vid, atherosPort, FAL_EG_UNTAGGED);

#if 0
	/* added by cylan for bug3027*/
	for(i = DAL_MIN_PORT_NUM; i <= DAL_MAX_PORT_NUM; i++)
	{
                    ar8228Port = dalToAtherosPortMap[i];
		if (ar8228Port == atherosPort)
	       {
	              continue;
	       }

		if (dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSPARENT)
		{
			member |=  (1 << ar8228Port);
			isis_vlan_member_add(0, vid, ar8228Port, FAL_EG_UNTAGGED);
		}
		else if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TAG) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRUNK)|
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION))
		{
		        if (dal_vtt_info[atherosPort].vlanMode == VLAN_TRANSPARENT)
		        {
		              isis_port_default_cvid_get(0, ar8228Port, &tagVid);
				rv = isis_vlan_find(0, tagVid, &tag_p_vlan);
	                     if (SW_OK == rv)
	                     {     
		                     tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
						//isis_vlan_member_add(0, tagVid, tag_p_vlan.mem_ports, 0);
						isis_vlan_member_del(0, tagVid, atherosPort);
					dal_vlan8228[tagVid].portId[atherosPort] = 0;
	                            dal_vlan8228[tagVid].member = tag_p_vlan.mem_ports;
	                      }

				if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRUNK)
				{
				     for(j = 1; j <= 4094; j++)
			            {
				         if ((OPL_OK != dalTrunkFind(ar8228Port, j)) | (j == tagVid))
		                       {
			                    continue;
		                       }
				         else
				        {
				             rv = isis_vlan_find(0, j, &tag_p_vlan);
					      if (SW_OK == rv)
	                                  {     
		                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
										//isis_vlan_member_add(0, j, tag_p_vlan.mem_ports, 0);
										isis_vlan_member_del(0, j, atherosPort);
					          dal_vlan8228[j].portId[atherosPort] = 0;
	                                      dal_vlan8228[j].member = tag_p_vlan.mem_ports;
	                                  }
				        }
			           }
				}

				if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION|
					dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION)
				{
				    for(j = 0; j < ODM_NUM_OF_VTT; j++)
				    {
				        if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))
				        {
				            rv = isis_vlan_find(0, sVlan, &tag_p_vlan);
					      if (SW_OK == rv)
	                                  {     
		                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
										//isis_vlan_member_add(0, sVlan, tag_p_vlan.mem_ports, 0);
										isis_vlan_member_del(0, sVlan, atherosPort);
					          dal_vlan8228[sVlan].portId[atherosPort] = 0;
	                                      dal_vlan8228[sVlan].member = tag_p_vlan.mem_ports;
	                                  }
				        }
			           }
				}
		        }
		}

		if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSPARENT) & 
			((dal_vtt_info[atherosPort].vlanMode == VLAN_TAG) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_TRUNK) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_TRANSLATION)|
			(dal_vtt_info[atherosPort].vlanMode == VLAN_AGGREGATION)))
		{   
                         tagVid = dal_vtt_info[atherosPort].defaultVlanTag;
		     rv = isis_vlan_find(0, tagVid, &tag_p_vlan);
		     if (SW_OK == rv)
	              {     
		             tag_p_vlan.mem_ports  &= ~(1 << ar8228Port);
					//isis_vlan_member_add(0, tagVid, tag_p_vlan.mem_ports, 0);
					isis_vlan_member_del(0, tagVid, ar8228Port);
			      dal_vlan8228[tagVid].portId[ar8228Port] = 0;
			      dal_vlan8228[tagVid].member = tag_p_vlan.mem_ports;
			      dalVlan8021QDelCheck(tagVid);
	              }

			if(dal_vtt_info[atherosPort].vlanMode == VLAN_TRUNK)
		      {
		          for(j = 1; j <= 4094; j++)
  	                 {
  		          if ((OPL_OK != dalTrunkFind(atherosPort, j)) | (j == tagVid))
                        {
  	                    continue;
                        }
  		          else
  		          {
  		               rv = isis_vlan_find(0, j, &tag_p_vlan);
  			        if (SW_OK == rv)
                             {     
                                  tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
								//isis_vlan_member_add(0, j, tag_p_vlan.mem_ports, 0);
								isis_vlan_member_del(0, j, ar8228Port);
  			             dal_vlan8228[j].portId[ar8228Port] = 0;
                                  dal_vlan8228[j].member = tag_p_vlan.mem_ports;
				      dalVlan8021QDelCheck(j);
                             }
  		          }
  	                 }       
	              }

			if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION|
				dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION)
		       {
		           for(j = 0; j < ODM_NUM_OF_VTT; j++)
			    {
			        if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))
				 {
				       rv = isis_vlan_find(0, sVlan, &tag_p_vlan);
					if (SW_OK == rv)
	                            {     
		                           tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
								//isis_vlan_member_add(0, sVlan, tag_p_vlan.mem_ports, 0);
								isis_vlan_member_del(0, sVlan, ar8228Port);
					      dal_vlan8228[sVlan].portId[ar8228Port] = 0;
	                                  dal_vlan8228[sVlan].member = tag_p_vlan.mem_ports;
					      dalVlan8021QDelCheck(sVlan);
                             }
  		          }
  	                 }       
	              }
		}
					
	}

#endif

	//isis_vlan_member_add(0, vid, member, 0);

	if (DAL_VLAN_MC_VTT != dal_vlan8228[vid].type)
	{
		dal_vlan8228[vid].type = DAL_VLAN_VTT;
	}

	dal_vlan8228[vid].portId[atherosPort] = 1;
	dal_vlan8228[vid].member = member;

	dal_vlan_transl_num[atherosPort] = 0;

       dal_vtt_info[atherosPort].vlanMode = VLAN_TRANSLATION;
	dal_vtt_info[atherosPort].defaultVlanTag = vid;

	isis_port_default_svid_set(0, atherosPort, vid);

	return  OPL_OK;
}


/*******************************************************************************
* dalToTrunkModeHandle
*
* DESCRIPTION:
*  		this function is used to set default vlan of all ports when the mode is changed to translation
*
*	INPUTS:
*		portId: 		portId
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalToTrunkModeHandle(UINT8 portId)
{
	OPL_STATUS iStatus = OPL_OK;
	a_uint32_t vid;
	fal_vlan_t p_vlan;
	fal_pbmp_t member;
	fal_pbmp_t u_member = 0;	
	int rv;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	UINT32 entryIndex;
	UINT32 i, j, cVlan, sVlan;
	UINT32 ar8228Port;
	a_uint32_t tagVid;
	fal_vlan_t  tag_p_vlan;
	fal_pbmp_t tag_member;

	/* add vlan, its vid = this port pcvid. for odm does not create such vlan */
	isis_port_default_cvid_get(0, atherosPort, &vid);

	if ((DAL_VLAN_UNUSED != dal_vlan8228[vid].type)
		&& (DAL_VLAN_VTT != dal_vlan8228[vid].type) 
		&& (DAL_VLAN_MULTICAST != dal_vlan8228[vid].type) 
		&& (DAL_VLAN_MC_VTT != dal_vlan8228[vid].type)
		&& (DAL_VLAN_TRUNK != dal_vlan8228[vid].type)
		&& (VLAN_AGGREGATION != dal_vlan8228[vid].type))
	{
		return OPL_ERROR;
	}

	rv = isis_vlan_find(0, vid, &p_vlan);
	if (SW_OK == rv)
	{
		member = p_vlan.mem_ports | (1 << atherosPort);
	}
	else
	{
		rv = isis_vlan_create(0, vid);
		if (SW_OK != rv)
		{
			return OPL_ERROR;
		}
		member = 0x1 + (1 << atherosPort);
		isis_vlan_member_add(0, vid, 0, FAL_EG_TAGGED);
	}
	
	rv = isis_vlan_member_add(0, vid, atherosPort, FAL_EG_UNTAGGED);

#if 0
	/* added by cylan for bug3027*/
	for(i = DAL_MIN_PORT_NUM; i <= DAL_MAX_PORT_NUM; i++)
	{
                    ar8228Port = dalToAtherosPortMap[i];
		if (ar8228Port == atherosPort)
	       {
	              continue;
	       }

		if (dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSPARENT)
		{
			member |=  (1 << ar8228Port);
			isis_vlan_member_add(0, vid, ar8228Port, FAL_EG_UNTAGGED);
		}
		else if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TAG) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRUNK)|
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION))
		{
		        if (dal_vtt_info[atherosPort].vlanMode == VLAN_TRANSPARENT)
		        {
		              isis_port_default_cvid_get(0, ar8228Port, &tagVid);
				rv = isis_vlan_find(0, tagVid, &tag_p_vlan);
	                     if (SW_OK == rv)
	                     {    
		                     tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
						//isis_vlan_member_add(0, tagVid, tag_p_vlan.mem_ports, 0);
						isis_vlan_member_del(0, tagVid, atherosPort);
					dal_vlan8228[tagVid].portId[atherosPort] = 0;
	                            dal_vlan8228[tagVid].member = tag_p_vlan.mem_ports;
	                      }

			        if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRUNK)
				{
				     for(j = 1; j <= 4094; j++)
			            {
				         if ((OPL_OK != dalTrunkFind(ar8228Port, j)) | (j == tagVid))
		                       {
			                    continue;
		                       }
				         else
				        {
				             rv = isis_vlan_find(0, j, &tag_p_vlan);
					      if (SW_OK == rv)
	                                  {     
		                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
								//isis_vlan_member_add(0, j, tag_p_vlan.mem_ports, 0);
								isis_vlan_member_del(0, j, atherosPort);
					          dal_vlan8228[j].portId[atherosPort] = 0;
	                                      dal_vlan8228[j].member = tag_p_vlan.mem_ports;
	                                  }
				        }
			           }
				}

				if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION|
					dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION)
				{
				    for(j = 0; j < ODM_NUM_OF_VTT; j++)
				    {
				        if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))
				        {
				            rv = isis_vlan_find(0, sVlan, &tag_p_vlan);
					      if (SW_OK == rv)
	                                  {     
		                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
										//isis_vlan_member_add(0, sVlan, tag_p_vlan.mem_ports, 0);
										isis_vlan_member_del(0, sVlan, atherosPort);
					          dal_vlan8228[sVlan].portId[atherosPort] = 0;
	                                      dal_vlan8228[sVlan].member = tag_p_vlan.mem_ports;
	                                  }
				        }
			           }
				}
		        }
		}

		if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSPARENT) & 
			((dal_vtt_info[atherosPort].vlanMode == VLAN_TAG) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_TRANSLATION) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_TRUNK)|
			(dal_vtt_info[atherosPort].vlanMode == VLAN_AGGREGATION)))
		{   
                         tagVid = dal_vtt_info[atherosPort].defaultVlanTag;
		     rv = isis_vlan_find(0, tagVid, &tag_p_vlan);
		     if (SW_OK == rv)
	              {     
		             tag_p_vlan.mem_ports  &= ~(1 << ar8228Port);
				//isis_vlan_member_add(0, tagVid, tag_p_vlan.mem_ports, 0);
				isis_vlan_member_del(0, tagVid, ar8228Port);
			      dal_vlan8228[tagVid].portId[ar8228Port] = 0;
			      dal_vlan8228[tagVid].member = tag_p_vlan.mem_ports;
			      dalVlan8021QDelCheck(tagVid);
	              }

		      if(dal_vtt_info[atherosPort].vlanMode == VLAN_TRUNK)
		      {
		          for(j = 1; j <= 4094; j++)
  	                 {
  		          if ((OPL_OK != dalTrunkFind(atherosPort, j)) | (j == tagVid))
                        {
  	                    continue;
                        }
  		          else
  		          {
  		               rv = isis_vlan_find(0, j, &tag_p_vlan);
  			        if (SW_OK == rv)
                             {     
                                  tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
								//isis_vlan_member_add(0, j, tag_p_vlan.mem_ports, 0);
								isis_vlan_member_del(0, j, ar8228Port);
  			             dal_vlan8228[j].portId[ar8228Port] = 0;
                                  dal_vlan8228[j].member = tag_p_vlan.mem_ports;
				      dalVlan8021QDelCheck(j);
                             }
  		          }
  	                 }       
	              }

			if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION |
				dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION)
		       {
		           for(j = 0; j < ODM_NUM_OF_VTT; j++)
			    {
			        if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))
				 {
				       rv = isis_vlan_find(0, sVlan, &tag_p_vlan);
					if (SW_OK == rv)
	                            {     
		                           tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
									//isis_vlan_member_add(0, sVlan, tag_p_vlan.mem_ports, 0);
									isis_vlan_member_del(0, sVlan, ar8228Port);
					      dal_vlan8228[sVlan].portId[ar8228Port] = 0;
	                                  dal_vlan8228[sVlan].member = tag_p_vlan.mem_ports;
					      dalVlan8021QDelCheck(sVlan);
                             }
  		          }
  	                 }       
	              }
		}
					
	}

	#endif
	//isis_vlan_member_add(0, vid, member, 0);
	dal_vlan8228[vid].type = DAL_VLAN_TRUNK;
	dal_vlan8228[vid].portId[atherosPort] = 1;
	dal_vlan8228[vid].member = member;

	dal_vlan_trunk_num[atherosPort] = 0;

	dal_vtt_info[atherosPort].vlanMode = VLAN_TRUNK;
	dal_vtt_info[atherosPort].defaultVlanTag = vid;

	isis_port_default_svid_set(0, atherosPort, vid);

	return  OPL_OK;
}


OPL_STATUS dalToAggregationHandle(UINT8 portId)
{
	OPL_STATUS iStatus = OPL_OK;
	a_uint32_t vid;
	fal_vlan_t p_vlan;
	fal_pbmp_t member;
	fal_pbmp_t u_member = 0;	
	int rv;
       UINT32  atherosPort = dalToAtherosPortMap[portId];
	UINT32 entryIndex;
	UINT32 i,j,cVlan,sVlan;
	UINT32 ar8228Port;
	a_uint32_t tagVid;
	fal_vlan_t  tag_p_vlan;
	fal_pbmp_t tag_member;
	/* add vlan, its vid = this port pcvid. for odm does not create such vlan */
	isis_port_default_cvid_get(0, atherosPort, &vid);

	if ((DAL_VLAN_UNUSED != dal_vlan8228[vid].type)
		&& (DAL_VLAN_VTT != dal_vlan8228[vid].type) 
		&& (DAL_VLAN_MULTICAST != dal_vlan8228[vid].type) 
		&& (DAL_VLAN_MC_VTT != dal_vlan8228[vid].type)
		&& (DAL_VLAN_TRUNK != dal_vlan8228[vid].type)
		&& (DAL_VLAN_AGGRESSTION != dal_vlan8228[vid].type))
	{
		return OPL_ERROR;
	}

	rv = isis_vlan_find(0, vid, &p_vlan);
	if (SW_OK == rv)
	{
		member = p_vlan.mem_ports | (1 << atherosPort);
	}
	else
	{
		rv = isis_vlan_create(0, vid);
		if (SW_OK != rv)
		{
			return OPL_ERROR;
		}
		member = 0x1 + (1 << atherosPort);
		isis_vlan_member_add(0, vid, 0, FAL_EG_TAGGED); /*6 to 0*/
	}
	
	rv = isis_vlan_member_add(0, vid, atherosPort, FAL_EG_UNTAGGED);
#if 0
	/* added by cylan for bug3027*/
	for(i = DAL_MIN_PORT_NUM; i <= DAL_MAX_PORT_NUM; i++)
	{
                    ar8228Port = dalToAtherosPortMap[i];
		if (ar8228Port == atherosPort)
	       {
	              continue;
	       }

		if (dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSPARENT)
		{
			member |=  (1 << ar8228Port);
			isis_vlan_member_add(0, vid, ar8228Port, FAL_EG_UNTAGGED);
		}
		else if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TAG) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRUNK) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION))
		{
		        if (dal_vtt_info[atherosPort].vlanMode == VLAN_TRANSPARENT)
		        {
		              isis_port_default_cvid_get(0, ar8228Port, &tagVid);
				rv = isis_vlan_find(0, tagVid, &tag_p_vlan);
	                     if (SW_OK == rv)
	                     {     
		                     tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
						//isis_vlan_member_add(0, tagVid, tag_p_vlan.mem_ports, 0);
						isis_vlan_member_del(0, tagVid, atherosPort);
					dal_vlan8228[tagVid].portId[atherosPort] = 0;
	                            dal_vlan8228[tagVid].member = tag_p_vlan.mem_ports;
	                      }

				if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRUNK)
				{
				     for(j = 1; j <= 4094; j++)
			            {
				         if ((OPL_OK != dalTrunkFind(ar8228Port, j)) | (j == tagVid))
		                       {
			                    continue;
		                       }
				         else
				        {
				             rv = isis_vlan_find(0, j, &tag_p_vlan);
					      if (SW_OK == rv)
	                                  {     
		                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
										//isis_vlan_member_add(0, j, tag_p_vlan.mem_ports, 0);
										isis_vlan_member_del(0, j, atherosPort);
					          dal_vlan8228[j].portId[atherosPort] = 0;
	                                      dal_vlan8228[j].member = tag_p_vlan.mem_ports;
	                                  }
				        }
			           }
				}

				if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION |
					dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION)
				{
				    for(j = 0; j < ODM_NUM_OF_VTT; j++)
				    {
				        if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))
				        {
				            rv = isis_vlan_find(0, sVlan, &tag_p_vlan);
					      if (SW_OK == rv)
	                                  {     
		                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
										//isis_vlan_member_add(0, sVlan, tag_p_vlan.mem_ports, 0);
										isis_vlan_member_del(0, sVlan, atherosPort);
					          dal_vlan8228[sVlan].portId[atherosPort] = 0;
	                                      dal_vlan8228[sVlan].member = tag_p_vlan.mem_ports;
	                                  }
				        }
			           }
				}
		        }
		}

		if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSPARENT) & 
			((dal_vtt_info[atherosPort].vlanMode == VLAN_TAG) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_TRUNK) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_TRANSLATION) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_AGGREGATION)))
		{   
                         tagVid = dal_vtt_info[atherosPort].defaultVlanTag;
		     rv = isis_vlan_find(0, tagVid, &tag_p_vlan);
		     if (SW_OK == rv)
	              {     
		             tag_p_vlan.mem_ports  &= ~(1 << ar8228Port);
					//isis_vlan_member_add(0, tagVid, tag_p_vlan.mem_ports, 0);
					isis_vlan_member_del(0, tagVid, ar8228Port);
			      dal_vlan8228[tagVid].portId[ar8228Port] = 0;
			      dal_vlan8228[tagVid].member = tag_p_vlan.mem_ports;
			      dalVlan8021QDelCheck(tagVid);
	              }

			if(dal_vtt_info[atherosPort].vlanMode == VLAN_TRUNK)
		      {
		          for(j = 1; j <= 4094; j++)
  	                 {
  		          if ((OPL_OK != dalTrunkFind(atherosPort, j)) | (j == tagVid))
                        {
  	                    continue;
                        }
  		          else
  		          {
  		               rv = isis_vlan_find(0, j, &tag_p_vlan);
  			        if (SW_OK == rv)
                             {     
                                  tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
								//isis_vlan_member_add(0, j, tag_p_vlan.mem_ports, 0);
								isis_vlan_member_del(0, j, ar8228Port);
  			             dal_vlan8228[j].portId[ar8228Port] = 0;
                                  dal_vlan8228[j].member = tag_p_vlan.mem_ports;
				      dalVlan8021QDelCheck(j);
                             }
  		          }
  	                 }       
	              }

			if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION |
				dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION)
		       {
		           for(j = 0; j < ODM_NUM_OF_VTT; j++)
			    {
			        if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))
				 {
				       rv = isis_vlan_find(0, sVlan, &tag_p_vlan);
					if (SW_OK == rv)
	                            {     
		                           tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
								//isis_vlan_member_add(0, sVlan, tag_p_vlan.mem_ports, 0);
								isis_vlan_member_del(0, sVlan, ar8228Port);
					      dal_vlan8228[sVlan].portId[ar8228Port] = 0;
	                                  dal_vlan8228[sVlan].member = tag_p_vlan.mem_ports;
					      dalVlan8021QDelCheck(sVlan);
                             }
  		          }
  	                 }       
	              }
		}
					
	}
#endif
	//isis_vlan_member_add(0, vid, member, 0);

	if (DAL_VLAN_MC_VTT != dal_vlan8228[vid].type)
	{
		dal_vlan8228[vid].type = DAL_VLAN_VTT;
	}

	dal_vlan8228[vid].portId[atherosPort] = 1;
	dal_vlan8228[vid].member = member;

	dal_vlan_transl_num[atherosPort] = 0;

    dal_vtt_info[atherosPort].vlanMode = VLAN_AGGREGATION;
	dal_vtt_info[atherosPort].defaultVlanTag = vid;

	isis_port_default_svid_set(0, atherosPort, vid);

	return  OPL_OK;
}




/*******************************************************************************
* dalPortVlanModeSet
*
* DESCRIPTION:
*  		this function is used to set port vlan for the specifies port
*
*	INPUTS:
*		portId: 		portId
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortVlanModeSet(UINT8 portId,UINT32 mode)
{
	OPL_STATUS iStatus = OPL_OK;
	a_uint32_t vid;
	fal_vlan_t p_vlan;
	fal_pbmp_t member;
	fal_pbmp_t u_member = 0;	
	int rv;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	UINT32 entryIndex;
	UINT32 i;

	if(PORT_IS_OUT_OF_RANTE(atherosPort))
	{
		OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

	isis_port_eg_mac_base_vlan_set(0, atherosPort, 0);

	/* enable port 0 802.1qmode.*/
	iStatus = isis_port_1qmode_set(0,0,FAL_1Q_FALLBACK);
	if(OPL_OK != iStatus)
	{
		return iStatus;
	}

		/*enable tls on port 0.*/
	iStatus = isis_port_tls_set(0, 0, 1);
	if(OPL_OK != iStatus)
	{
		return iStatus;
	}


	switch(mode)
	{
		case VLAN_TRANSPARENT:
			iStatus = isis_port_1qmode_set(0, atherosPort, FAL_1Q_FALLBACK);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_egvlanmode_set(0, atherosPort, FAL_EG_UNMODIFIED);//fixed bug 3483 
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_invlan_mode_set(0, atherosPort, FAL_INVLAN_ADMIT_ALL);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_force_default_vid_set(0, atherosPort, 0);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_tls_set(0, atherosPort, 1);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_arp_leaky_set(0, atherosPort, 1);//fixed bug 3483 
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			/* clone and prop */
			isis_port_vlan_propagation_set(0, atherosPort, FAL_VLAN_PROPAGATION_CLONE);

			/* added by cylan for bug3027*/
			dalToTransparentModeHandle(portId);

			/* get GE's pcvid and set it to portId's pcvid */
			isis_port_default_cvid_get(0, 0, &vid);
			isis_port_default_cvid_set(0, atherosPort, vid);

			isis_vlan_find(0, 4095, &p_vlan);
			p_vlan.mem_ports |=  (1 << atherosPort);
			//isis_vlan_member_add(0, 4095, p_vlan.mem_ports, 0);
			isis_vlan_member_add(0, 4095, atherosPort);
			dal_vlan8228[4095].member = p_vlan.mem_ports;
			dal_vlan8228[4095].portId[atherosPort] = 1;

			/* add this port to GE port based vlan member */	
			isis_portvlan_member_add(0, 0, atherosPort);
			isis_portvlan_member_add(0, atherosPort, 0);
			
            dal_vtt_info[atherosPort].vlanMode = VLAN_TRANSPARENT;
            dal_vtt_info[atherosPort].defaultVlanTag = 0xFFF;

			break;
		case VLAN_TAG:
			iStatus = isis_port_1qmode_set(0,atherosPort,FAL_1Q_SECURE);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_invlan_mode_set(0, atherosPort, FAL_INVLAN_ADMIT_UNTAGGED);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_egvlanmode_set(0, atherosPort, FAL_EG_UNTAGGED);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_force_default_vid_set(0, atherosPort, 0);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_tls_set(0, atherosPort, 0);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_arp_leaky_set(0, atherosPort, 1);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			/* clone and prop */
			isis_port_vlan_propagation_set(0, atherosPort, FAL_VLAN_PROPAGATION_REPLACE);

			/* delete existing vlan or records */
			dalTagDelvlan(portId);
			dalVttDelVlan(portId);
			dalVttDelRec(portId);
			dalTrunkDelvlan(portId);

			isis_vlan_find(0, 4095, &p_vlan);
			p_vlan.mem_ports &=  ~(1 << atherosPort);
			//isis_vlan_member_add(0, 4095, p_vlan.mem_ports, 0);
			isis_vlan_member_del(0, 4095, atherosPort);
			dal_vlan8228[4095].member = p_vlan.mem_ports;
			dal_vlan8228[4095].portId[atherosPort] = 0;

			/* del this port from GE port based vlan member */	
			isis_portvlan_member_del(0, 0, atherosPort);

			/* add vlan, its vid = this port pcvid. for odm does not create such vlan */
			dalToTagModeHandle(portId);

			break;
		case VLAN_TRANSLATION:
			iStatus = isis_port_1qmode_set(0,atherosPort,FAL_1Q_SECURE);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_invlan_mode_set(0, atherosPort, FAL_INVLAN_ADMIT_ALL);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_egvlanmode_set(0, atherosPort, FAL_EG_UNMODIFIED);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_force_default_vid_set(0, atherosPort, 0);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_tls_set(0, atherosPort, 0);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_arp_leaky_set(0, atherosPort, 1);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			/* clone and prop */
			isis_port_vlan_propagation_set(0, atherosPort, FAL_VLAN_PROPAGATION_DISABLE);

			/* delete existing vlan */
			dalTagDelvlan(portId);
			dalVttDelVlan(portId);
			dalVttDelRec(portId);
			dalTrunkDelvlan(portId);

			isis_vlan_find(0, 4095, &p_vlan);
			p_vlan.mem_ports &=  ~(1 << atherosPort);
//			isis_vlan_member_add(0, 4095, p_vlan.mem_ports, 0);
			isis_vlan_member_del(0, 4095, atherosPort);
			dal_vlan8228[4095].member = p_vlan.mem_ports;
			dal_vlan8228[4095].portId[atherosPort] = 0;

			/* del this port from GE port based vlan member */	
			isis_portvlan_member_del(0, 0, atherosPort);

			/* added by cylan for bug3027*/
			dalToTranslationModeHandle(portId);
			
			break;
		case VLAN_TRUNK:

			iStatus = isis_port_1qmode_set(0,atherosPort,FAL_1Q_SECURE);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}
		
			iStatus = isis_port_invlan_mode_set(0, atherosPort, FAL_INVLAN_ADMIT_ALL);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}
		
			iStatus = isis_port_egvlanmode_set(0, atherosPort, FAL_EG_UNMODIFIED);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}
		
			iStatus = isis_port_force_default_vid_set(0, atherosPort, 0);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}
		
			iStatus = isis_port_tls_set(0, atherosPort, 0);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_arp_leaky_set(0, atherosPort, 1);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}
		
			/* clone and prop */
			isis_port_vlan_propagation_set(0, atherosPort, FAL_VLAN_PROPAGATION_REPLACE);

			/* delete existing vlan or records */
			dalTagDelvlan(portId);
			dalVttDelVlan(portId);
			dalVttDelRec(portId);
			dalTrunkDelvlan(portId);

			if (SW_OK == isis_vlan_find(0, 4095, &p_vlan))
			{
				p_vlan.mem_ports &=  ~(1 << atherosPort);
#if 1
				//if (SW_OK == isis_vlan_member_add(0, 4095, p_vlan.mem_ports, 0))
				if (SW_OK == isis_vlan_member_del(0, 4095, atherosPort))
				{
					dal_vlan8228[4095].member = p_vlan.mem_ports;
					dal_vlan8228[4095].portId[atherosPort] = 0;
				}
				else
				{
					return OPL_ERROR;
				}
#endif               
			}
			else
			{
				return OPL_ERROR;
			}
	
			/* delete this port from GE port based vlan member */	
			isis_portvlan_member_del(0, 0, atherosPort);

			/* added by cylan for bug3027*/
			dalToTrunkModeHandle(portId);
			
			break;
		case VLAN_AGGREGATION:
			iStatus = isis_port_1qmode_set(0,atherosPort,FAL_1Q_SECURE);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_invlan_mode_set(0, atherosPort, FAL_INVLAN_ADMIT_ALL);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_egvlanmode_set(0, atherosPort, FAL_EG_UNMODIFIED);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_force_default_vid_set(0, atherosPort, 0);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_tls_set(0, atherosPort, 0);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = isis_port_arp_leaky_set(0, atherosPort, 1);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			/* clone and prop */
			isis_port_vlan_propagation_set(0, atherosPort, FAL_VLAN_PROPAGATION_REPLACE);

			/* delete existing vlan */
			dalTagDelvlan(portId);
			dalVttDelVlan(portId);
			dalVttDelRec(portId);
			dalTrunkDelvlan(portId);

			isis_vlan_find(0, 4095, &p_vlan);
			p_vlan.mem_ports &=  ~(1 << atherosPort);
			//isis_vlan_member_add(0, 4095, p_vlan.mem_ports, 0);
			isis_vlan_member_del(0, 4095, atherosPort);
			dal_vlan8228[4095].member = p_vlan.mem_ports;
			dal_vlan8228[4095].portId[atherosPort] = 0;

			/* del this port from GE port based vlan member */	
			isis_portvlan_member_del(0, 0, atherosPort);

			isis_port_eg_mac_base_vlan_set(0, atherosPort, 1);
			
			dalToAggregationHandle(portId);
			break;
		default:
			break;
	}
	
	return iStatus;
}



/*******************************************************************************
* dalPortVlanModeGet
*
* DESCRIPTION:
*  		this function is used to set port vlan for the specifies port
*
*	INPUTS:
*		portId: 		portId
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortVlanModeGet(UINT8 portId,UINT32 *vlanMode)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    *vlanMode = dal_vtt_info[atherosPort].vlanMode;
    return OPL_OK;
}


/*******************************************************************************
* dalPortLearnEnableSet
*
* DESCRIPTION:
*  		this function is used to set per port learn enable
*
*	INPUTS:
*		portId: 		portId
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortLearnEnableSet(UINT8 portId,UINT32 enable)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	return isis_fdb_port_learn_set(0, atherosPort, enable);
}


/*******************************************************************************
* dalPortInit
*
* DESCRIPTION:
*  		this function is used to init the port parameter.
*
*	INPUTS:
*		
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortInit(void)
{
	OPL_STATUS retVal = OPL_OK;
	BRG_PORT_TAB_GE_t gePortEntry;
	BRG_PORT_TAB_PON_t ponPortEntry;
	
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	gePortEntry.rcv_en = OPL_ENABLE;
	gePortEntry.lern = OPL_DISABLE;
	gePortEntry.lp = OPL_DISABLE;	
	gePortEntry.rsv_en = OPL_ENABLE;
	gePortEntry.cos = 0;
	gePortEntry.pvid = 1;
	gePortEntry.i_action = OPL_DISABLE;
	gePortEntry.i_cos = 0;
	gePortEntry.untag = OPL_ENABLE;
	gePortEntry.tag = OPL_ENABLE;
	gePortEntry.trsmt_en = OPL_ENABLE;

	retVal = brgPortEntryHwWrite(BRG_PORT_GE,(UINT32 *)&gePortEntry);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	ponPortEntry.rcv_en = OPL_ENABLE;
	ponPortEntry.lern = OPL_DISABLE;
	ponPortEntry.lp = OPL_DISABLE;	
	ponPortEntry.rsv_en = OPL_ENABLE;
	ponPortEntry.cos = 0;
	ponPortEntry.pvid = 1;
	ponPortEntry.i_action = OPL_DISABLE;
	ponPortEntry.i_cos = 0;
	ponPortEntry.untag = OPL_ENABLE;
	ponPortEntry.tag = OPL_ENABLE;
	ponPortEntry.trsmt_en = OPL_ENABLE;
	retVal = brgPortEntryHwWrite(BRG_PORT_PON,(UINT32 *)&ponPortEntry);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	retVal = oplRegWrite(REG_GMAC_CFG_LENMENA,0);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	retVal = oplRegWrite(REG_PMAC_CFG_LENMENA,0);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	retVal = brgVttLookupFailActionHwWrite(OPL_TRUE);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	/* to accept untag frame */
	retVal = brgGePortLinkControlSet(GE_PORT_PVID, 0xFFF);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	
	return OPL_OK;
}


