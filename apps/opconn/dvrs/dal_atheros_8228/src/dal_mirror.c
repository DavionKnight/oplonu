/*
=============================================================================
     Header Name: dal_mib.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                		Date              	Description of Changes
----------------   ------------  ----------------------------------------------
 zran 				2009/07/17		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#include <dal_lib.h>
#include <odm_port.h>
#include <dal_mirror.h>



/*******************************************************************************
* dalMirPortSet
*
* DESCRIPTION:
*		set mirror port for the switch 
*	INPUTS:
*		portId:	port number[1-4];  to disable mirror, use 0xF as portId.
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success

* 	SEE ALSO: 
*/
OPL_STATUS dalMirPortSet(UINT8 portId)
{
    UINT32 atherosPort;
    if (portId == 0xF)    
        atherosPort = 0xF;
    else
        atherosPort = dalToAtherosPortMap[portId];
	
	return shiva_mirr_analysis_port_set(0, atherosPort);		
}



/*******************************************************************************
* dalMirPortGet
*
* DESCRIPTION:
*		get mirror port for the switch 
*	INPUTS:
*		portId:	port number[1-4]
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success

* 	SEE ALSO: 
*/
OPL_STATUS dalMirPortGet(UINT8 * portId)
{
    UINT32  atherosPort;
    sw_error_t rv;
 
	if (NULL == portId)
	{
		return OPL_ERROR;
	}

	rv = shiva_mirr_analysis_port_get(0, &atherosPort);
    *portId = atherosToDalPortMap[atherosPort];

	return rv;
}



/*******************************************************************************
* dalMirIngressPortSet
*
* DESCRIPTION:
*		set ingress mirror port for the switch 
*	INPUTS:
*		portId:	port number[1-4]
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success

* 	SEE ALSO: 
*/
OPL_STATUS dalMirIngPortSet(UINT8 portId, a_bool_t enable)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	return shiva_mirr_port_in_set(0, atherosPort, enable);
}


/*******************************************************************************
* dalMirIngPortGet
*
* DESCRIPTION:
*		set ingress mirror port for the switch 
*	INPUTS:
*		portId:	port number[1-4]
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success

* 	SEE ALSO: 
*/
OPL_STATUS dalMirIngPortGet(UINT8 portId, a_bool_t * enable)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if (NULL == enable)
	{
		return OPL_ERROR;
	}

	return shiva_mirr_port_in_get(0, atherosPort, enable);
}


/*******************************************************************************
* dalMirEgressPortSet
*
* DESCRIPTION:
*		set Egress mirror port for the switch 
*	INPUTS:
*		portId:	port number[1-4]
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success

* 	SEE ALSO: 
*/
OPL_STATUS dalMirEgPortSet(UINT8 portId, a_bool_t enable)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	return shiva_mirr_port_eg_set(0, atherosPort, enable);
}


/*******************************************************************************
* dalMirEgressPortGet
*
* DESCRIPTION:
*		get Egress mirror port for the switch 
*	INPUTS:
*		portId:	port number[1-4]
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success

* 	SEE ALSO: 
*/
OPL_STATUS dalMirEgPortGet(UINT8 portId, a_bool_t * enable)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if (NULL == enable)
	{
		return OPL_ERROR;
	}

    return shiva_mirr_port_eg_get(0, atherosPort, enable);
}

