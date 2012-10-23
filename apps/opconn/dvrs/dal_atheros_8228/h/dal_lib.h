/*
=============================================================================
     Header Name: dal_lib.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2008/1/16		Initial Version	
----------------   ------------  ----------------------------------------------
*/

#ifndef DAL_LIB_H
#define DAL_LIB_H
#include <opconn_lib.h>

#include <dal_ver.h>
#include <dal_header.h>
#include <dal_port.h>
#include <dal_vtt.h>
#include <dal_mac.h>
#include <dal_multicast.h>
#include <dal_cls.h>
#include <dal_dba.h>
#include <dal_pon.h>
#include <dal_vlan.h>
#include <dal_rstp.h>


#define NUM_OF_PORTS        4
#define MAX_NUM_OF_VLAN     4095

extern UINT8 dalToAtherosPortMap[];
extern UINT8 atherosToDalPortMap[];

extern UINT8 logicToPhyPortMap[] ;
extern UINT8 phyToLogicPortMap[];


OPL_STATUS dalChipReset();
OPL_STATUS dalDemonInit(void);

#endif

