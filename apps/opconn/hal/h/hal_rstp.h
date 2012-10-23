/*
=============================================================================
     Header Name: hal_rstp.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 jiangmingli	   2009/03/17		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#ifndef _HAL_RSTP_H_
#define _HAL_RSTP_H_

#include "opl_driver.h"
#include "opconn_api.h"

#ifndef INOUT
#define IN      /* consider as comments near 'input' parameters */
#define OUT     /* consider as comments near 'output' parameters */
#define INOUT   /* consider as comments near 'input/output' parameters */
#endif

/* hal api parameter struct define */
typedef struct tagHAL_API_RSTP_PORT_STATE_SET
{
    UINT32 ulPortId;
    UINT8 ucPortState;
}HAL_API_RSTP_PORT_STATE_SET_S;

typedef struct tagHAL_API_RSTP_PORT_STATE_GET
{
    UINT32 ulPortId;
    UINT8 *pucPortState;
}HAL_API_RSTP_PORT_STATE_GET_S;

typedef struct tagHAL_API_RSTP_PORT_MAC_FLUSH
{
    UINT32 ulPortid;
    BOOL_T bOnlyThisPort;
}HAL_API_RSTP_PORT_MAC_FLUSH_S;


#endif      /* _HAL_RSTP_H_ */


