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
#include <dal_mib.h>



/*******************************************************************************
* dalMibGet
*
* DESCRIPTION:
*		get the mib information of port
*	INPUTS:
*		portId:	port number[0 -6]
*				CPU port:0
*				UNI 1: 1, ..., UNI 4: 4
*				GE port: 6
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success

* 	SEE ALSO: 
*/
OPL_STATUS dalMibGet(UINT8 portId, void * mib)
{
	fal_mib_info_t mib_info;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if (NULL == mib)
	{
		return OPL_ERROR;
	}
	
	return shiva_get_mib_info(0, atherosPort, mib);
}

