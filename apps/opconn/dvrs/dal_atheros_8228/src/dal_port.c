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
#include <aos_types.h>
#include "defs.h"

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

    retVal = shiva_port_link_status_get(0, atherosPort, &up);
    *status = (UINT8)up;
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
	
	return shiva_port_speed_set(0, atherosPort, speed);
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

	shiva_port_speed_get(0, atherosPort, &tmpSpeed);

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

	shiva_port_speed_get(0, atherosPort, &tmpSpeed);

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
	
	return shiva_port_duplex_set(0, atherosPort, duplexMode);
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

	shiva_port_duplex_get(0, atherosPort, &duplexMode);

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

	shiva_port_duplex_get(0, atherosPort, &duplexMode);

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

    return shiva_port_default_cvid_set(0, atherosPort, pvid);
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

    shiva_port_default_cvid_get(0, atherosPort, &vid);

	*pvid = vid;

	return OPL_OK;
}



OPL_STATUS dalPortDefaultPriSet(UINT8 portId,UINT8 defaultPri)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    return shiva_qos_port_default_up_set(0,atherosPort, defaultPri);
}



OPL_STATUS dalPortDefaultPriGet(UINT8 portId,UINT8 *defaultPri)
{
	a_uint32_t up;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if (NULL == defaultPri)
	{
		return OPL_ERROR;
	}
	
    shiva_qos_port_default_up_get(0, atherosPort, &up);

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
	
	shiva_port_flowctrl_get(0, atherosPort, &en);

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
	return shiva_port_lpbk_set(0, atherosPort, enable);
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
	a_uint32_t speed;
	a_bool_t tmpenable;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	shiva_rate_port_inrl_get(0, atherosPort, &speed, &tmpenable);

	if (enable)
	{	
		if (0x7fff == speed)
		{
			speed = 0x7fff - 1;
		}
	}

	tmpenable = enable;
	
	return shiva_rate_port_inrl_set(0, atherosPort, &speed, enable);
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
	a_uint32_t speed;
	a_bool_t tmpEn;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
	if (NULL == enable)
	{
		return OPL_ERROR;
	}
	
	shiva_rate_port_inrl_get(0, atherosPort, &speed, &tmpEn);

	*enable = tmpEn;

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
	a_uint32_t speed;
	a_bool_t tmpenable;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	shiva_rate_port_inrl_get(0, atherosPort, &speed, &tmpenable);

	return shiva_rate_port_inrl_set(0, atherosPort, &cir, tmpenable);
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
	a_bool_t tmpenable;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    OPL_STATUS ret;

	if (NULL == cir)
	{
		return OPL_ERROR;
	}

	return shiva_rate_port_inrl_get(0, atherosPort, cir, &tmpenable);
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
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    return OPL_OK;
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
    UINT32  atherosPort = dalToAtherosPortMap[portId];
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
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    return OPL_OK;
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
    UINT32  atherosPort = dalToAtherosPortMap[portId];
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
	a_uint32_t speed, chipCir;
	a_bool_t en;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	shiva_rate_port_inrl_get(0, atherosPort, &speed, &en);

	if (0 == enable)
	{
		shiva_rate_port_inrl_set(0, atherosPort, &speed, 0);
	}
	else
	{
		shiva_rate_port_inrl_set(0, atherosPort, &cir, 1);		
	}
	
	return OPL_OK;
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
	a_uint32_t speed;
	a_bool_t en;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if ((NULL == enable) || (NULL == cir))
	{
		return OPL_ERROR;
	}
	
	shiva_rate_port_inrl_get(0, atherosPort, &speed, &en);

	*enable = en;
	*cir = speed;
	
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
	a_uint32_t speed;
	a_bool_t tmpenable;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	shiva_rate_port_egrl_get(0, atherosPort, &speed, &tmpenable);
	
    return shiva_rate_port_egrl_set(0, atherosPort, &speed, enable);
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
	a_uint32_t speed;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
    return shiva_rate_port_egrl_get(0, atherosPort, &speed, enable);
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
	a_uint32_t speed;
	a_bool_t tmpenable;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	shiva_rate_port_egrl_get(0, atherosPort, &speed, &tmpenable);
	
    return shiva_rate_port_egrl_set(0, atherosPort, &cir, tmpenable);
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
	a_bool_t tmpenable;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	return shiva_rate_port_egrl_get(0, atherosPort, cir, &tmpenable);
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
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    return OPL_OK;
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
    UINT32  atherosPort = dalToAtherosPortMap[portId];
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
OPL_STATUS dalPortDsPolicingSet(UINT8 portId,UINT8 enable,UINT32 cir,UINT32 pir)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    return shiva_rate_port_egrl_set(0, atherosPort, &cir, enable);
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
OPL_STATUS dalPortDsPolicingGet(UINT8 portId,UINT8 *enable,UINT32 *cir,UINT32 *pir)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    return shiva_rate_port_egrl_get(0, atherosPort, cir, enable);
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
	
	return shiva_loop_check_status_set(0,(fal_loop_check_time_t)tmpTime, 
		(a_bool_t)enable);
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
	
	return shiva_loop_check_status_get(0,time, (a_bool_t *)enable);
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
	
	shiva_loop_check_info_get(0, oldPortId, newPortId);
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
	
	return shiva_loop_check_intr_get(0, intr);
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
	return shiva_loop_check_intr_clear(0);
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
    a_bool_t up;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

    retVal = shiva_port_admin_status_get(0, atherosPort, &up);
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
		shiva_stp_port_state_set(0, 0, atherosPort, FAL_STP_FARWARDING);
		return shiva_port_shutdown_set(0, atherosPort, 0);
	}
	else if(PHY_PORT_ADMIN_DISABLE == state)
	{
		/*fixed bug 3272,clear fdb table,the same as operation fuction*/
		dalArlFlushDynamic();
		shiva_stp_port_state_set(0, 0, atherosPort, FAL_STP_DISABLED);
		return shiva_port_shutdown_set(0, atherosPort, 1);
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
	
	shiva_port_autoneg_status_get(0, atherosPort, &status);

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
		
    shiva_phy_autoneg_done_read(0, atherosPort, &status);
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
		return shiva_port_autoneg_enable(0, atherosPort);
	}
	else if(enable == DAL_PORT_ADMIN_DISABLE)
	{
		return shiva_port_autoneg_disable(0, atherosPort);
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
	return shiva_port_autoneg_restart(0, atherosPort);
}

OPL_STATUS dalVlan8021QDelCheck(UINT32 vlanId)
{
    int rv;
	
    if(((dal_vlan8228[vlanId].member >> 1) & 0xF) == 0)
   {
       rv = shiva_vlan_delete(0, vlanId);
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
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRUNK))
		{     
		       shiva_port_default_cvid_get(0, ar8228Port, &vid);
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

		    else if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION)
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

			else if(dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION)
			{
				if (OPL_OK == dalVttVlanAggFind(ar8228Port, tagVid))
			    {
				     tagFlag = OPL_TRUE;
				     break;
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
				(dal_vtt_info[atherosPort].vlanMode == VLAN_AGGREGATION) |
				(dal_vtt_info[atherosPort].vlanMode == VLAN_TRUNK)))
			{  
                          vid = dal_vtt_info[atherosPort].defaultVlanTag;
			     rv = shiva_vlan_find(0, vid, &p_vlan);
			     if (SW_OK == rv)
		              {     
			             p_vlan.mem_ports  &= ~(1 << ar8228Port);
                         p_vlan.mem_ports &= ~(1 << 6);
                                  shiva_vlan_member_update(0, vid, p_vlan.mem_ports, 0);
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
				                rv = shiva_vlan_find(0, j, &p_vlan);
				                if (SW_OK == rv)
		                              {     
			                           p_vlan.mem_ports  &= ~(1 << ar8228Port);
                                                p_vlan.mem_ports &= ~(1 << 6);
                                                shiva_vlan_member_update(0, j, p_vlan.mem_ports, 0);
				                    dal_vlan8228[j].portId[ar8228Port] = 0;
                                                dal_vlan8228[j].portId[6] = 0;
				                    dal_vlan8228[j].member = p_vlan.mem_ports;
		                              }
				           }
			             }
		              }

				if((dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSPARENT) &
				  	(dal_vtt_info[atherosPort].vlanMode == VLAN_TRANSLATION))
			      {
			           for(j = 0; j < ODM_NUM_OF_VTT; j++)
			           {
			                if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))
				         {
				              rv = shiva_vlan_find(0, sVlan, &p_vlan);
					       if (SW_OK == rv)
	                                   {     
		                                 p_vlan.mem_ports &=  (~(1 << ar8228Port));
					            shiva_vlan_member_update(0, sVlan, p_vlan.mem_ports, 0);
					            dal_vlan8228[sVlan].portId[ar8228Port] = 0;
	                                        dal_vlan8228[sVlan].member = p_vlan.mem_ports;
					            dalVlan8021QDelCheck(sVlan);
	                                    }
				           }						
		                   }
			      }

				if((dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSPARENT) &
				  	(dal_vtt_info[atherosPort].vlanMode == VLAN_AGGREGATION))
				{
			      	      for(j = 1; j <= 4094; j++)
			             {
				           if ((OPL_OK != dalVttVlanAggFind(atherosPort, j)) | (j == vid))
		                         {
			                       continue;
		                         }
				           else
				           {
				                rv = shiva_vlan_find(0, j, &p_vlan);
				                if (SW_OK == rv)
		                              {     
			                           p_vlan.mem_ports  &= ~(1 << ar8228Port);
                                                p_vlan.mem_ports &= ~(1 << 6);
                                                shiva_vlan_member_update(0, j, p_vlan.mem_ports, 0);
				                    dal_vlan8228[j].portId[ar8228Port] = 0;
                                                dal_vlan8228[j].portId[6] = 0;
				                    dal_vlan8228[j].member = p_vlan.mem_ports;
		                              }
				           }
			             }
		              }
			}
	       }
       }

	/* delete existing vlan or records */
	dalTagDelvlan(portId);
	dalVttDelVlan(portId);
	dalVttDelRec(portId);
	dalTrunkDelvlan(portId);
	dalVttVlanAggDelvlan(portId);

	for (i = DAL_MIN_PORT_NUM; i <= DAL_MAX_PORT_NUM; i++)
	{
                    ar8228Port = dalToAtherosPortMap[i];
		if (ar8228Port == atherosPort)
	       {
	              continue;
	       }
		if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TAG) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRUNK))
		{     
		       shiva_port_default_cvid_get(0, ar8228Port, &vid);
			rv = shiva_vlan_find(0, vid, &p_vlan);
			if (SW_OK == rv)
	              {     
		             p_vlan.mem_ports |= (1 << atherosPort);
                           shiva_vlan_member_update(0, vid, p_vlan.mem_ports, 0);	
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
				        rv = shiva_vlan_find(0, j, &p_vlan);
					 if (SW_OK == rv)
	                             {     
		                          p_vlan.mem_ports |= (1 << atherosPort);
                                        shiva_vlan_member_update(0, j, p_vlan.mem_ports, 0);	
			                   dal_vlan8228[j].portId[atherosPort] = 1;
			                   dal_vlan8228[j].member = p_vlan.mem_ports;
	                             }
				    }
			    }
			}

			if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION)
			{
			    for(j = 0; j < ODM_NUM_OF_VTT; j++)
			    {
			        if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))
				 {
				      rv = shiva_vlan_find(0, sVlan, &p_vlan);
				      if (SW_OK == rv)
	                           {     
		                          p_vlan.mem_ports |= (1 << atherosPort);
                                        shiva_vlan_member_update(0, sVlan, p_vlan.mem_ports, 0);	
			                   dal_vlan8228[sVlan].portId[atherosPort] = 1;
			                   dal_vlan8228[sVlan].member = p_vlan.mem_ports;
	                           }
				  }		
			    }
			}

			if(dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION)
			{
			    for(j = 1; j <= 4094; j++)
			    {
				    if ((OPL_OK != dalVttVlanAggFind(ar8228Port, j)) | (j == vid))
		                  {
			               continue;
		                  }
				    else
				    {
				        rv = shiva_vlan_find(0, j, &p_vlan);
					 if (SW_OK == rv)
	                             {     
		                          p_vlan.mem_ports |= (1 << atherosPort);
                                        shiva_vlan_member_update(0, j, p_vlan.mem_ports, 0);	
			                   dal_vlan8228[j].portId[atherosPort] = 1;
			                   dal_vlan8228[j].member = p_vlan.mem_ports;
	                             }
				    }
			    }
			}
		}
	}

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
	shiva_port_default_cvid_get(0, atherosPort, &vid);

	if ((DAL_VLAN_UNUSED != dal_vlan8228[vid].type)
		&& (DAL_VLAN_VTT != dal_vlan8228[vid].type) 
		&& (DAL_VLAN_MULTICAST != dal_vlan8228[vid].type) 
		&& (DAL_VLAN_MC_VTT != dal_vlan8228[vid].type)
		&& (DAL_VLAN_AGGREGATION != dal_vlan8228[vid].type)
		&& (DAL_VLAN_TRUNK != dal_vlan8228[vid].type))
	{
		return OPL_ERROR;
	}


	rv = shiva_vlan_find(0, vid, &p_vlan);
	if (SW_OK == rv)
	{
		member = p_vlan.mem_ports | (1 << atherosPort);
	}
	else
	{
		rv = shiva_vlan_create(0, vid);
		if (SW_OK != rv)
		{
			return OPL_ERROR;
		}
		member = 0x40 + (1 << atherosPort);
	}
	shiva_vlan_member_update(0, vid, member, 0);
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
		}
		else if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TAG) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRUNK))
		{
		        if (dal_vtt_info[atherosPort].vlanMode == VLAN_TRANSPARENT)
		        {
		              shiva_port_default_cvid_get(0, ar8228Port, &tagVid);
				rv = shiva_vlan_find(0, tagVid, &tag_p_vlan);
	                     if (SW_OK == rv)
	                     {     
		                     tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
					shiva_vlan_member_update(0, tagVid, tag_p_vlan.mem_ports, 0);
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
				             rv = shiva_vlan_find(0, j, &tag_p_vlan);
					      if (SW_OK == rv)
	                                  {     
		                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
					          shiva_vlan_member_update(0, j, tag_p_vlan.mem_ports, 0);
					          dal_vlan8228[j].portId[atherosPort] = 0;
	                                      dal_vlan8228[j].member = tag_p_vlan.mem_ports;
	                                  }
				        }
			           }
				}

				if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION)
				{
				    for(j = 0; j < ODM_NUM_OF_VTT; j++)
				    {
				        if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))
				        {
				            rv = shiva_vlan_find(0, sVlan, &tag_p_vlan);
					      if (SW_OK == rv)
	                                  {     
		                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
					          shiva_vlan_member_update(0, sVlan, tag_p_vlan.mem_ports, 0);
					          dal_vlan8228[sVlan].portId[atherosPort] = 0;
	                                      dal_vlan8228[sVlan].member = tag_p_vlan.mem_ports;
	                                  }
				        }
			           }
				}
				if(dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION)
				{
				     for(j = 1; j <= 4094; j++)
			            {
				         if ((OPL_OK != dalVttVlanAggFind(ar8228Port, j)) | (j == tagVid))
		                       {
			                    continue;
		                       }
				         else
				        {
				             rv = shiva_vlan_find(0, j, &tag_p_vlan);
					      if (SW_OK == rv)
	                                  {     
		                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
					          shiva_vlan_member_update(0, j, tag_p_vlan.mem_ports, 0);
					          dal_vlan8228[j].portId[atherosPort] = 0;
	                                      dal_vlan8228[j].member = tag_p_vlan.mem_ports;
	                                  }
				        }
			           }
				}
				
		        }
		}

		if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSPARENT) & 
			((dal_vtt_info[atherosPort].vlanMode == VLAN_TRANSLATION) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_TRUNK) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_AGGREGATION) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_TAG)))
		{   
                   tagVid = dal_vtt_info[atherosPort].defaultVlanTag;
		     rv = shiva_vlan_find(0, tagVid, &tag_p_vlan);
		     if (SW_OK == rv)
	              {  
		             tag_p_vlan.mem_ports  &= ~(1 << ar8228Port);
                           shiva_vlan_member_update(0, tagVid, tag_p_vlan.mem_ports, 0);
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
  		               rv = shiva_vlan_find(0, j, &tag_p_vlan);
  			        if (SW_OK == rv)
                             {     
                                  tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
  			             shiva_vlan_member_update(0, j, tag_p_vlan.mem_ports, 0);
  			             dal_vlan8228[j].portId[ar8228Port] = 0;
                                  dal_vlan8228[j].member = tag_p_vlan.mem_ports;
				      dalVlan8021QDelCheck(j);
                             }
  		          }
  	                 }       
	              }

			if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION)
		       {
		           for(j = 0; j < ODM_NUM_OF_VTT; j++)
			    {
			        if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))
				 {
				       rv = shiva_vlan_find(0, sVlan, &tag_p_vlan);
					if (SW_OK == rv)
	                            {     
		                           tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
					      shiva_vlan_member_update(0, sVlan, tag_p_vlan.mem_ports, 0);
					      dal_vlan8228[sVlan].portId[ar8228Port] = 0;
	                                  dal_vlan8228[sVlan].member = tag_p_vlan.mem_ports;
					      dalVlan8021QDelCheck(sVlan);
	                             }
				  }						
		            }
		         }
			if(dal_vtt_info[atherosPort].vlanMode == VLAN_AGGREGATION)
		      {
		          for(j = 1; j <= 4094; j++)
  	                 {
  		          if ((OPL_OK != dalVttVlanAggFind(atherosPort, j)) | (j == tagVid))
                        {
  	                    continue;
                        }
  		          else
  		          {
  		               rv = shiva_vlan_find(0, j, &tag_p_vlan);
  			        if (SW_OK == rv)
                             {     
                                  tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
  			             shiva_vlan_member_update(0, j, tag_p_vlan.mem_ports, 0);
  			             dal_vlan8228[j].portId[ar8228Port] = 0;
                                  dal_vlan8228[j].member = tag_p_vlan.mem_ports;
				      dalVlan8021QDelCheck(j);
                             }
  		          }
  	                 }       
	              }
		}
					
	}
	
	shiva_vlan_member_update(0, vid, member, 0);

	if (DAL_VLAN_UNUSED != dal_vlan8228[vid].type)
	{
		dal_vlan8228[vid].type = DAL_VLAN_VTT;
	}
	else  if (DAL_VLAN_MULTICAST != dal_vlan8228[vid].type)
	{
		dal_vlan8228[vid].type = DAL_VLAN_MC_VTT;
	}
	
	dal_vlan8228[vid].portId[atherosPort] = 1;
	dal_vlan8228[vid].member = member;

	dal_vlan_transl_num[atherosPort] = 0;

       dal_vtt_info[atherosPort].vlanMode = VLAN_TAG;
	dal_vtt_info[atherosPort].defaultVlanTag = vid;

	shiva_port_default_svid_set(0, atherosPort, vid);

	iStatus = dalVttTagEntryAdd(0, 0, 0, vid, portId, &entryIndex, DAL_VTT_UC);

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
	shiva_port_default_cvid_get(0, atherosPort, &vid);

	if ((DAL_VLAN_UNUSED != dal_vlan8228[vid].type)
		&& (DAL_VLAN_VTT != dal_vlan8228[vid].type) 
		&& (DAL_VLAN_MULTICAST != dal_vlan8228[vid].type) 
		&& (DAL_VLAN_MC_VTT != dal_vlan8228[vid].type)
		&& (DAL_VLAN_AGGREGATION != dal_vlan8228[vid].type)
		&& (DAL_VLAN_TRUNK != dal_vlan8228[vid].type))
	{
		return OPL_ERROR;
	}

	rv = shiva_vlan_find(0, vid, &p_vlan);
	if (SW_OK == rv)
	{
		member = p_vlan.mem_ports | (1 << atherosPort);
	}
	else
	{
		rv = shiva_vlan_create(0, vid);
		if (SW_OK != rv)
		{
			return OPL_ERROR;
		}
		member = 0x40 + (1 << atherosPort);
	}
	shiva_vlan_member_update(0, vid, member, 0);
	
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
		}
		else if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TAG) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRUNK))
		{
		        if (dal_vtt_info[atherosPort].vlanMode == VLAN_TRANSPARENT)
		        {
		              shiva_port_default_cvid_get(0, ar8228Port, &tagVid);
				rv = shiva_vlan_find(0, tagVid, &tag_p_vlan);
	                     if (SW_OK == rv)
	                     {     
		                     tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
					shiva_vlan_member_update(0, tagVid, tag_p_vlan.mem_ports, 0);
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
				             rv = shiva_vlan_find(0, j, &tag_p_vlan);
					      if (SW_OK == rv)
	                                  {     
		                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
					          shiva_vlan_member_update(0, j, tag_p_vlan.mem_ports, 0);
					          dal_vlan8228[j].portId[atherosPort] = 0;
	                                      dal_vlan8228[j].member = tag_p_vlan.mem_ports;
	                                  }
				        }
			           }
				}

				if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION)
				{
				    for(j = 0; j < ODM_NUM_OF_VTT; j++)
				    {
				        if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))
				        {
				            rv = shiva_vlan_find(0, sVlan, &tag_p_vlan);
					      if (SW_OK == rv)
	                                  {     
		                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
					          shiva_vlan_member_update(0, sVlan, tag_p_vlan.mem_ports, 0);
					          dal_vlan8228[sVlan].portId[atherosPort] = 0;
	                                      dal_vlan8228[sVlan].member = tag_p_vlan.mem_ports;
	                                  }
				        }
			           }
				}
				if(dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION)
				{
				     for(j = 1; j <= 4094; j++)
			            {
				         if ((OPL_OK != dalVttVlanAggFind(ar8228Port, j)) | (j == tagVid))
		                       {
			                    continue;
		                       }
				         else
				        {
				             rv = shiva_vlan_find(0, j, &tag_p_vlan);
					      if (SW_OK == rv)
	                                  {     
		                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
					          shiva_vlan_member_update(0, j, tag_p_vlan.mem_ports, 0);
					          dal_vlan8228[j].portId[atherosPort] = 0;
	                                      dal_vlan8228[j].member = tag_p_vlan.mem_ports;
	                                  }
				        }
			           }
				}

		        }
		}

		if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSPARENT) & 
			((dal_vtt_info[atherosPort].vlanMode == VLAN_TAG) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_TRUNK) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_AGGREGATION) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_TRANSLATION)))
		{   
                         tagVid = dal_vtt_info[atherosPort].defaultVlanTag;
		     rv = shiva_vlan_find(0, tagVid, &tag_p_vlan);
		     if (SW_OK == rv)
	              {     
		             tag_p_vlan.mem_ports  &= ~(1 << ar8228Port);
                                 shiva_vlan_member_update(0, tagVid, tag_p_vlan.mem_ports, 0);
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
  		               rv = shiva_vlan_find(0, j, &tag_p_vlan);
  			        if (SW_OK == rv)
                             {     
                                  tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
  			             shiva_vlan_member_update(0, j, tag_p_vlan.mem_ports, 0);
  			             dal_vlan8228[j].portId[ar8228Port] = 0;
                                  dal_vlan8228[j].member = tag_p_vlan.mem_ports;
				      dalVlan8021QDelCheck(j);
                             }
  		          }
  	                 }       
	              }

			if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION)
		       {
		           for(j = 0; j < ODM_NUM_OF_VTT; j++)
			    {
			        if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))
				 {
				       rv = shiva_vlan_find(0, sVlan, &tag_p_vlan);
					if (SW_OK == rv)
	                            {     
		                           tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
					      shiva_vlan_member_update(0, sVlan, tag_p_vlan.mem_ports, 0);
					      dal_vlan8228[sVlan].portId[ar8228Port] = 0;
	                                  dal_vlan8228[sVlan].member = tag_p_vlan.mem_ports;
					      dalVlan8021QDelCheck(sVlan);
                             }
  		          }
  	                 }       
	              }
			if(dal_vtt_info[atherosPort].vlanMode == VLAN_AGGREGATION)
		      {
		          for(j = 1; j <= 4094; j++)
  	                 {
  		          if ((OPL_OK != dalVttVlanAggFind(atherosPort, j)) | (j == tagVid))
                        {
  	                    continue;
                        }
  		          else
  		          {
  		               rv = shiva_vlan_find(0, j, &tag_p_vlan);
  			        if (SW_OK == rv)
                             {     
                                  tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
  			             shiva_vlan_member_update(0, j, tag_p_vlan.mem_ports, 0);
  			             dal_vlan8228[j].portId[ar8228Port] = 0;
                                  dal_vlan8228[j].member = tag_p_vlan.mem_ports;
				      dalVlan8021QDelCheck(j);
                             }
  		          }
  	                 }       
	              }
		}
					
	}
	shiva_vlan_member_update(0, vid, member, 0);

	if (DAL_VLAN_MC_VTT != dal_vlan8228[vid].type)
	{
		dal_vlan8228[vid].type = DAL_VLAN_VTT;
	}

	dal_vlan8228[vid].portId[atherosPort] = 1;
	dal_vlan8228[vid].member = member;

	dal_vlan_transl_num[atherosPort] = 0;

       dal_vtt_info[atherosPort].vlanMode = VLAN_TRANSLATION;
	dal_vtt_info[atherosPort].defaultVlanTag = vid;

	shiva_port_default_svid_set(0, atherosPort, vid);

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
	shiva_port_default_cvid_get(0, atherosPort, &vid);

	if ((DAL_VLAN_UNUSED != dal_vlan8228[vid].type)
		&& (DAL_VLAN_VTT != dal_vlan8228[vid].type) 
		&& (DAL_VLAN_MULTICAST != dal_vlan8228[vid].type) 
		&& (DAL_VLAN_MC_VTT != dal_vlan8228[vid].type)
		&& (DAL_VLAN_AGGREGATION != dal_vlan8228[vid].type)
		&& (DAL_VLAN_TRUNK != dal_vlan8228[vid].type))
	{
		return OPL_ERROR;
	}

	rv = shiva_vlan_find(0, vid, &p_vlan);
	if (SW_OK == rv)
	{
		member = p_vlan.mem_ports | (1 << atherosPort);
	}
	else
	{
		rv = shiva_vlan_create(0, vid);
		if (SW_OK != rv)
		{
			return OPL_ERROR;
		}
		member = 0x40 + (1 << atherosPort);
	}
	shiva_vlan_member_update(0, vid, member, 0);
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
		}
		else if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TAG) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRUNK))
		{
		        if (dal_vtt_info[atherosPort].vlanMode == VLAN_TRANSPARENT)
		        {
		              shiva_port_default_cvid_get(0, ar8228Port, &tagVid);
				rv = shiva_vlan_find(0, tagVid, &tag_p_vlan);
	                     if (SW_OK == rv)
	                     {    
		                     tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
					shiva_vlan_member_update(0, tagVid, tag_p_vlan.mem_ports, 0);
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
				             rv = shiva_vlan_find(0, j, &tag_p_vlan);
					      if (SW_OK == rv)
	                                  {     
		                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
					          shiva_vlan_member_update(0, j, tag_p_vlan.mem_ports, 0);
					          dal_vlan8228[j].portId[atherosPort] = 0;
	                                      dal_vlan8228[j].member = tag_p_vlan.mem_ports;
	                                  }
				        }
			           }
				}

				if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION)
				{
				    for(j = 0; j < ODM_NUM_OF_VTT; j++)
				    {
				        if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))
				        {
				            rv = shiva_vlan_find(0, sVlan, &tag_p_vlan);
					      if (SW_OK == rv)
	                                  {     
		                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
					          shiva_vlan_member_update(0, sVlan, tag_p_vlan.mem_ports, 0);
					          dal_vlan8228[sVlan].portId[atherosPort] = 0;
	                                      dal_vlan8228[sVlan].member = tag_p_vlan.mem_ports;
	                                  }
				        }
			           }
				}

				if(dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION)
				{
				     for(j = 1; j <= 4094; j++)
			            {
				         if ((OPL_OK != dalVttVlanAggFind(ar8228Port, j)) | (j == tagVid))
		                       {
			                    continue;
		                       }
				         else
				        {
				             rv = shiva_vlan_find(0, j, &tag_p_vlan);
					      if (SW_OK == rv)
	                                  {     
		                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
					          shiva_vlan_member_update(0, j, tag_p_vlan.mem_ports, 0);
					          dal_vlan8228[j].portId[atherosPort] = 0;
	                                      dal_vlan8228[j].member = tag_p_vlan.mem_ports;
	                                  }
				        }
			           }
				}

		        }
		}

		if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSPARENT) & 
			((dal_vtt_info[atherosPort].vlanMode == VLAN_TAG) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_TRANSLATION) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_AGGREGATION) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_TRUNK)))
		{   
                         tagVid = dal_vtt_info[atherosPort].defaultVlanTag;
		     rv = shiva_vlan_find(0, tagVid, &tag_p_vlan);
		     if (SW_OK == rv)
	              {     
		             tag_p_vlan.mem_ports  &= ~(1 << ar8228Port);
                           shiva_vlan_member_update(0, tagVid, tag_p_vlan.mem_ports, 0);
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
  		               rv = shiva_vlan_find(0, j, &tag_p_vlan);
  			        if (SW_OK == rv)
                             {     
                                  tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
  			             shiva_vlan_member_update(0, j, tag_p_vlan.mem_ports, 0);
  			             dal_vlan8228[j].portId[ar8228Port] = 0;
                                  dal_vlan8228[j].member = tag_p_vlan.mem_ports;
				      dalVlan8021QDelCheck(j);
                             }
  		          }
  	                 }       
	              }

			if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION)
		       {
		           for(j = 0; j < ODM_NUM_OF_VTT; j++)
			    {
			        if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))
				 {
				       rv = shiva_vlan_find(0, sVlan, &tag_p_vlan);
					if (SW_OK == rv)
	                            {     
		                           tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
					      shiva_vlan_member_update(0, sVlan, tag_p_vlan.mem_ports, 0);
					      dal_vlan8228[sVlan].portId[ar8228Port] = 0;
	                                  dal_vlan8228[sVlan].member = tag_p_vlan.mem_ports;
					      dalVlan8021QDelCheck(sVlan);
                             }
  		          }
  	                 }       
	              }
			if(dal_vtt_info[atherosPort].vlanMode == VLAN_AGGREGATION)
		      {
		          for(j = 1; j <= 4094; j++)
  	                 {
  		          if ((OPL_OK != dalVttVlanAggFind(atherosPort, j)) | (j == tagVid))
                        {
  	                    continue;
                        }
  		          else
  		          {
  		               rv = shiva_vlan_find(0, j, &tag_p_vlan);
  			        if (SW_OK == rv)
                             {     
                                  tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
  			             shiva_vlan_member_update(0, j, tag_p_vlan.mem_ports, 0);
  			             dal_vlan8228[j].portId[ar8228Port] = 0;
                                  dal_vlan8228[j].member = tag_p_vlan.mem_ports;
				      dalVlan8021QDelCheck(j);
                             }
  		          }
  	                 }       
	              }

		}
					
	}
	shiva_vlan_member_update(0, vid, member, 0);
	dal_vlan8228[vid].type = DAL_VLAN_TRUNK;
	dal_vlan8228[vid].portId[atherosPort] = 1;
	dal_vlan8228[vid].member = member;

	dal_vlan_trunk_num[atherosPort] = 0;

	dal_vtt_info[atherosPort].vlanMode = VLAN_TRUNK;
	dal_vtt_info[atherosPort].defaultVlanTag = vid;

	shiva_port_default_svid_set(0, atherosPort, vid);

	return  OPL_OK;
}



OPL_STATUS dalToAggregationModeHandle(UINT8 portId)
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

	//printf("%s port %d called.\n", __FUNCTION__, portId);

	/* add vlan, its vid = this port pcvid. for odm does not create such vlan */
	shiva_port_default_cvid_get(0, atherosPort, &vid);

	if ((DAL_VLAN_UNUSED != dal_vlan8228[vid].type)
		&& (DAL_VLAN_VTT != dal_vlan8228[vid].type) 
		&& (DAL_VLAN_MULTICAST != dal_vlan8228[vid].type) 
		&& (DAL_VLAN_MC_VTT != dal_vlan8228[vid].type)
		&& (DAL_VLAN_AGGREGATION != dal_vlan8228[vid].type)
		&& (DAL_VLAN_TRUNK != dal_vlan8228[vid].type))
	{
		return OPL_ERROR;
	}

	rv = shiva_vlan_find(0, vid, &p_vlan);
	if (SW_OK == rv)
	{
		member = p_vlan.mem_ports | (1 << atherosPort);
	}
	else
	{
		rv = shiva_vlan_create(0, vid);
		if (SW_OK != rv)
		{
			return OPL_ERROR;
		}
		member = 0x40 + (1 << atherosPort);
	}
	shiva_vlan_member_update(0, vid, member, 0);
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
		}
		else if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TAG) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRUNK))
		{
		        if (dal_vtt_info[atherosPort].vlanMode == VLAN_TRANSPARENT)
		        {
		              shiva_port_default_cvid_get(0, ar8228Port, &tagVid);
				rv = shiva_vlan_find(0, tagVid, &tag_p_vlan);
	                     if (SW_OK == rv)
	                     {    
		                     tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
					shiva_vlan_member_update(0, tagVid, tag_p_vlan.mem_ports, 0);
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
				             rv = shiva_vlan_find(0, j, &tag_p_vlan);
					      if (SW_OK == rv)
	                                  {     
		                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
					          shiva_vlan_member_update(0, j, tag_p_vlan.mem_ports, 0);
					          dal_vlan8228[j].portId[atherosPort] = 0;
	                                      dal_vlan8228[j].member = tag_p_vlan.mem_ports;
	                                  }
				        }
			           }
				}

				if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION)
				{
				    for(j = 0; j < ODM_NUM_OF_VTT; j++)
				    {
				        if(OPL_OK == dalVttTagEntryGet(j, &cVlan, &sVlan, i))
				        {
				            rv = shiva_vlan_find(0, sVlan, &tag_p_vlan);
					      if (SW_OK == rv)
	                                  {     
		                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
					          shiva_vlan_member_update(0, sVlan, tag_p_vlan.mem_ports, 0);
					          dal_vlan8228[sVlan].portId[atherosPort] = 0;
	                                      dal_vlan8228[sVlan].member = tag_p_vlan.mem_ports;
	                                  }
				        }
			           }
				}

				if(dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION)
				{
				     for(j = 1; j <= 4094; j++)
			            {
				         if ((OPL_OK != dalVttVlanAggFind(ar8228Port, j)) | (j == tagVid))
		                       {
			                    continue;
		                       }
				         else
				        {
				             rv = shiva_vlan_find(0, j, &tag_p_vlan);
					      if (SW_OK == rv)
	                                  {     
		                               tag_p_vlan.mem_ports &=  (~(1 << atherosPort));
					          shiva_vlan_member_update(0, j, tag_p_vlan.mem_ports, 0);
					          dal_vlan8228[j].portId[atherosPort] = 0;
	                                      dal_vlan8228[j].member = tag_p_vlan.mem_ports;
	                                  }
				        }
			           }
				}
				
		        }
		}

		if ((dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSPARENT) & 
			((dal_vtt_info[atherosPort].vlanMode == VLAN_TAG) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_TRANSLATION) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_AGGREGATION) |
			(dal_vtt_info[atherosPort].vlanMode == VLAN_TRUNK)))
		{   
                         tagVid = dal_vtt_info[atherosPort].defaultVlanTag;
		     rv = shiva_vlan_find(0, tagVid, &tag_p_vlan);
		     if (SW_OK == rv)
	              {     
		             tag_p_vlan.mem_ports  &= ~(1 << ar8228Port);
                           shiva_vlan_member_update(0, tagVid, tag_p_vlan.mem_ports, 0);
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
  		               rv = shiva_vlan_find(0, j, &tag_p_vlan);
  			        if (SW_OK == rv)
                             {     
                                  tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
  			             shiva_vlan_member_update(0, j, tag_p_vlan.mem_ports, 0);
  			             dal_vlan8228[j].portId[ar8228Port] = 0;
                                  dal_vlan8228[j].member = tag_p_vlan.mem_ports;
				      dalVlan8021QDelCheck(j);
                             }
  		          }
  	                 }       
	              }

			if(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION)
		       {
		           for(j = 0; j < ODM_NUM_OF_VTT; j++)
			    {
			        if(OPL_OK == dalVttTagEntryGet(atherosPort, &cVlan, &sVlan, i))
				 {
				       rv = shiva_vlan_find(0, sVlan, &tag_p_vlan);
					if (SW_OK == rv)
	                            {     
		                           tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
					      shiva_vlan_member_update(0, sVlan, tag_p_vlan.mem_ports, 0);
					      dal_vlan8228[sVlan].portId[ar8228Port] = 0;
	                                  dal_vlan8228[sVlan].member = tag_p_vlan.mem_ports;
					      dalVlan8021QDelCheck(sVlan);
                             }
  		          }
  	                 }       
	              }

			if(dal_vtt_info[ar8228Port].vlanMode == VLAN_AGGREGATION)
		     {
		          for(j = 1; j <= 4094; j++)
  	                 {
  		          if ((OPL_OK != dalVttVlanAggFind(i, j)) | (j == tagVid))
                        {
  	                    continue;
                        }
  		          else
  		          {
  		               rv = shiva_vlan_find(0, j, &tag_p_vlan);
  			        if (SW_OK == rv)
                             {     
                                  tag_p_vlan.mem_ports &=  (~(1 << ar8228Port));
  			             shiva_vlan_member_update(0, j, tag_p_vlan.mem_ports, 0);
  			             dal_vlan8228[j].portId[ar8228Port] = 0;
                                  dal_vlan8228[j].member = tag_p_vlan.mem_ports;
				      dalVlan8021QDelCheck(j);
                             }
  		          }
  	                 }       
	              }
		}
					
	}
	shiva_vlan_member_update(0, vid, member, 0);
	dal_vlan8228[vid].type = DAL_VLAN_AGGREGATION;
	dal_vlan8228[vid].portId[atherosPort] = 1;
	dal_vlan8228[vid].member = member;

	dal_vlan_trunk_num[atherosPort] = 0;

	dal_vtt_info[atherosPort].vlanMode = VLAN_AGGREGATION;
	dal_vtt_info[atherosPort].defaultVlanTag = vid;

	shiva_port_default_svid_set(0, atherosPort, vid);

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

	dalSoftArlMacFlush(portId);

	switch(mode)
	{
		case VLAN_TRANSPARENT:
			iStatus = shiva_port_1qmode_set(0, atherosPort, FAL_1Q_FALLBACK);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = shiva_port_egvlanmode_set(0, atherosPort, FAL_EG_UNMODIFIED);//fixed bug 3483 
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = shiva_port_invlan_mode_set(0, atherosPort, FAL_INVLAN_ADMIT_ALL);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = shiva_port_force_default_vid_set(0, atherosPort, 0);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = shiva_port_tls_set(0, atherosPort, 1);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = shiva_port_arp_leaky_set(0, atherosPort, 1);//fixed bug 3483 
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			/* clone and prop */
			shiva_port_vlan_propagation_set(0, atherosPort, FAL_VLAN_PROPAGATION_CLONE);

			/* added by cylan for bug3027*/
			dalToTransparentModeHandle(portId);

			/* get GE's pcvid and set it to portId's pcvid */
			shiva_port_default_cvid_get(0, 6, &vid);
			shiva_port_default_cvid_set(0, atherosPort, vid);

			shiva_vlan_find(0, 4095, &p_vlan);
			p_vlan.mem_ports |=  (1 << atherosPort);
			shiva_vlan_member_update(0, 4095, p_vlan.mem_ports, 0);
			dal_vlan8228[4095].member = p_vlan.mem_ports;
			dal_vlan8228[4095].portId[atherosPort] = 1;

			/* add this port to GE port based vlan member */	
			shiva_portvlan_member_add(0, 6, atherosPort);
			
            dal_vtt_info[atherosPort].vlanMode = VLAN_TRANSPARENT;
            dal_vtt_info[atherosPort].defaultVlanTag = 0xFFF;

			break;
		case VLAN_TAG:
			iStatus = shiva_port_1qmode_set(0,atherosPort,FAL_1Q_SECURE);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = shiva_port_invlan_mode_set(0, atherosPort, FAL_INVLAN_ADMIT_UNTAGGED);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = shiva_port_egvlanmode_set(0, atherosPort, FAL_EG_UNMODIFIED);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = shiva_port_force_default_vid_set(0, atherosPort, 0);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = shiva_port_tls_set(0, atherosPort, 0);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = shiva_port_arp_leaky_set(0, atherosPort, 1);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			/* clone and prop */
			shiva_port_vlan_propagation_set(0, atherosPort, FAL_VLAN_PROPAGATION_DISABLE);

			/* delete existing vlan or records */
			dalTagDelvlan(portId);
			dalVttDelVlan(portId);
			dalVttDelRec(portId);
			dalTrunkDelvlan(portId);
			dalVttVlanAggDelvlan(portId);

			shiva_vlan_find(0, 4095, &p_vlan);
			p_vlan.mem_ports &=  ~(1 << atherosPort);
			shiva_vlan_member_update(0, 4095, p_vlan.mem_ports, 0);
			dal_vlan8228[4095].member = p_vlan.mem_ports;
			dal_vlan8228[4095].portId[atherosPort] = 0;

			/* del this port from GE port based vlan member */	
			shiva_portvlan_member_del(0, 6, atherosPort);

			/* add vlan, its vid = this port pcvid. for odm does not create such vlan */
			dalToTagModeHandle(portId);

			break;
		case VLAN_TRANSLATION:
			iStatus = shiva_port_1qmode_set(0,atherosPort,FAL_1Q_SECURE);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = shiva_port_invlan_mode_set(0, atherosPort, FAL_INVLAN_ADMIT_ALL);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = shiva_port_egvlanmode_set(0, atherosPort, FAL_EG_HYBRID);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = shiva_port_force_default_vid_set(0, atherosPort, 0);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = shiva_port_tls_set(0, atherosPort, 0);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = shiva_port_arp_leaky_set(0, atherosPort, 1);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			/* clone and prop */
			shiva_port_vlan_propagation_set(0, atherosPort, FAL_VLAN_PROPAGATION_DISABLE);

			/* delete existing vlan */
			dalTagDelvlan(portId);
			dalVttDelVlan(portId);
			dalVttDelRec(portId);
			dalTrunkDelvlan(portId);
			dalVttVlanAggDelvlan(portId);

			shiva_vlan_find(0, 4095, &p_vlan);
			p_vlan.mem_ports &=  ~(1 << atherosPort);
			shiva_vlan_member_update(0, 4095, p_vlan.mem_ports, 0);
			dal_vlan8228[4095].member = p_vlan.mem_ports;
			dal_vlan8228[4095].portId[atherosPort] = 0;

			/* del this port from GE port based vlan member */	
			shiva_portvlan_member_del(0, 6, atherosPort);

			/* added by cylan for bug3027*/
			dalToTranslationModeHandle(portId);
			
			break;
		case VLAN_TRUNK:

			iStatus = shiva_port_1qmode_set(0,atherosPort,FAL_1Q_SECURE);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}
		
			iStatus = shiva_port_invlan_mode_set(0, atherosPort, FAL_INVLAN_ADMIT_ALL);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}
		
			iStatus = shiva_port_egvlanmode_set(0, atherosPort, FAL_EG_HYBRID);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}
		
			iStatus = shiva_port_force_default_vid_set(0, atherosPort, 0);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}
		
			iStatus = shiva_port_tls_set(0, atherosPort, 0);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = shiva_port_arp_leaky_set(0, atherosPort, 1);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}
		
			/* clone and prop */
			shiva_port_vlan_propagation_set(0, atherosPort, FAL_VLAN_PROPAGATION_CLONE);

			/* delete existing vlan or records */
			dalTagDelvlan(portId);
			dalVttDelVlan(portId);
			dalVttDelRec(portId);
			dalTrunkDelvlan(portId);
			dalVttVlanAggDelvlan(portId);

			if (SW_OK == shiva_vlan_find(0, 4095, &p_vlan))
			{
				p_vlan.mem_ports &=  ~(1 << atherosPort);

				if (SW_OK == shiva_vlan_member_update(0, 4095, p_vlan.mem_ports, 0))
				{
					dal_vlan8228[4095].member = p_vlan.mem_ports;
					dal_vlan8228[4095].portId[atherosPort] = 0;
				}
				else
				{
					return OPL_ERROR;
				}
			}
			else
			{
				return OPL_ERROR;
			}
	
			/* delete this port from GE port based vlan member */	
			shiva_portvlan_member_del(0, 6, atherosPort);

			/* added by cylan for bug3027*/
			dalToTrunkModeHandle(portId);
			
			break;
		case VLAN_AGGREGATION:
			dalArlMacPortLockCpuEn(portId, TRUE);
		    dalVttVlanAggIni(portId, 0);
			dalArlFlushAllByPort(portId);
			
			iStatus = shiva_port_1qmode_set(0,atherosPort,FAL_1Q_SECURE);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}
		
			iStatus = shiva_port_invlan_mode_set(0, atherosPort, FAL_INVLAN_ADMIT_ALL);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}
		
			iStatus = shiva_port_egvlanmode_set(0, atherosPort, FAL_EG_HYBRID);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}
		
			iStatus = shiva_port_force_default_vid_set(0, atherosPort, 0);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}
		
			iStatus = shiva_port_tls_set(0, atherosPort, 0);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}

			iStatus = shiva_port_arp_leaky_set(0, atherosPort, 1);
			if(OPL_OK != iStatus)
			{
				return iStatus;
			}
		
			/* clone and prop */
			shiva_port_vlan_propagation_set(0, atherosPort, FAL_VLAN_PROPAGATION_CLONE);

			/* delete existing vlan or records */
			dalTagDelvlan(portId);
			dalVttDelVlan(portId);
			dalVttDelRec(portId);
			dalTrunkDelvlan(portId);
			dalVttVlanAggDelvlan(portId);

			if (SW_OK == shiva_vlan_find(0, 4095, &p_vlan))
			{
				p_vlan.mem_ports &=  ~(1 << atherosPort);

				if (SW_OK == shiva_vlan_member_update(0, 4095, p_vlan.mem_ports, 0))
				{
					dal_vlan8228[4095].member = p_vlan.mem_ports;
					dal_vlan8228[4095].portId[atherosPort] = 0;
				}
				else
				{
					return OPL_ERROR;
				}
			}
			else
			{
				return OPL_ERROR;
			}
	
			/* del this port from GE port based vlan member */	
			shiva_portvlan_member_del(0, 6, atherosPort);

			dalToAggregationModeHandle(portId);
			
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
	return shiva_fdb_port_learn_set(0, atherosPort, enable);
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


