/*
=============================================================================
     File Name: dal_mac.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                		   Date              	Description of Changes
----------------   ------------  ----------------------------------------------
 zran				   2009/05/20	Initial Version	
----------------   ------------  ----------------------------------------------
*/
#include <dal_lib.h>
#include "uid_stp.h"
#include "fal_stp.h"

/*******************************************************************************
* dalRstpModeSet
*
* DESCRIPTION:
*  		this function is used to enable or disable rstp globally
*
*	INPUTS:
*		bEnable: 		enable or disable rstp
*
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalRstpModeSet(IN OPL_BOOL bEnable)
{
	return OPL_OK;
}


/*******************************************************************************
* dalRstpPortStateGet
*
* DESCRIPTION:
*  		this function is used to get port state of ulPortId
*
*	INPUTS:
*		ulPortId: 		logical portid
*
*	OUTPUTS:
*		pucPortState:	port state
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalRstpPortStateGet(IN UINT32 portId, OUT UINT8 * portState)
{
    UINT32 ulState;
    sw_error_t ulRet; 
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	ulRet = isis_stp_port_state_get(0, FAL_SINGLE_STP_ID, atherosPort, &ulState);
    if (SW_OK != ulRet)
    {
        printf("get rstp port %u state error %u\r\n", atherosPort, ulRet);
        return ulRet;
    }
    
    switch (ulState)
    {
        case FAL_STP_DISABLED:
        {
            *portState = UID_PORT_DISABLED;
            break;
        }
        case FAL_STP_BLOKING:
        {
            *portState = UID_PORT_DISCARDING;
            break;
        }
        case FAL_STP_LISTENING:
        {
            *portState = UID_PORT_LEARNING;
            break;
        }
        case FAL_STP_LEARNING:
        {
            *portState = UID_PORT_LEARNING;
            break;
        }
        case FAL_STP_FARWARDING:
        {
            *portState = UID_PORT_FORWARDING;
            break;
        }
        default:
        {
            return OPL_ERR_INVALID_PARAMETERS;
        }
    }
   
    return OPL_OK;
}



/*******************************************************************************
* dalRstpPortStateSet
*
* DESCRIPTION:
*  		this function is used to set port state of ulPortId
*
*	INPUTS:
*		ulPortId: 		logical portid
*		ucPortState:	port state
*
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalRstpPortStateSet(IN UINT32 portId, IN UINT8 portState)
{
    UINT32 ulState;
    sw_error_t ulRet; 
    UINT32  atherosPort = dalToAtherosPortMap[portId];

    switch (portState)
    {
        case UID_PORT_DISABLED:
        {
            ulState = FAL_STP_DISABLED;
            break;
        }
        case UID_PORT_DISCARDING:
        {
            ulState = FAL_STP_BLOKING;
            break;
        }
        case UID_PORT_LEARNING:
        {
            ulState = FAL_STP_LEARNING;
            break;
        }
        case UID_PORT_FORWARDING:
        {
            ulState = FAL_STP_FARWARDING;
            break;
        }
        case UID_PORT_NON_STP:
        {
            ulState = FAL_STP_FARWARDING;
            break;
        }
        default:
        {
            return OPL_ERR_INVALID_PARAMETERS;
        }
    }

    ulRet = isis_stp_port_state_set(0, FAL_SINGLE_STP_ID, atherosPort, ulState);

    if (SW_OK != ulRet)
    {
        printf("set port %u fdb error %u\r\n", portId, ulRet);
        return ulRet;
    }

    return OPL_OK;
}



/*******************************************************************************
* dalRstpPortMacFlush
*
* DESCRIPTION:
*  		this function is used to flush mac of only this port or all except this port
*
*	INPUTS:
*		portId: 		port id
*		bOnlyThisPort	1 --- only this port
*					0 --- all except this port
*
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalRstpPortMacFlush(UINT32 portId, BOOL_T bOnlyThisPort)
{
    UINT32 ulCurPortid;
    sw_error_t ulRet; 
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    
    if (OPL_TRUE == bOnlyThisPort)
	{
		ulRet = isis_fdb_del_by_port(0, atherosPort, 0);
        if (SW_OK != ulRet)
        {
            printf("flush port %u fdb error %u\r\n", portId, ulRet);
            return ulRet;
        }
	}
    else
    {
        for (ulCurPortid = 1; ulCurPortid < 6; ulCurPortid++)
        {
            if (ulCurPortid == atherosPort)
            {
                continue;
            }

            ulRet = isis_fdb_del_by_port(0, ulCurPortid, 0);
            if (SW_OK != ulRet)
            {
                printf("flush port %u fdb error %u\r\n", portId, ulRet);
                return ulRet;
            }
        }
    }

    return OPL_OK;
}

