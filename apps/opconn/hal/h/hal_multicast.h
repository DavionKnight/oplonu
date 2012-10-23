/*
=============================================================================
     Header Name: hal_multicast.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved


Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 xfan    2009/03/17   Initial Version
----------------   ------------  ----------------------------------------------
*/
#ifndef _HAL_MULTICAST_H_
#define _HAL_MULTICAST_H_

typedef struct HAL_MC_CFG_INFO_s{
  UINT8 port;
  UINT8 mac[6];
} HAL_MC_CFG_INFO_t;

#define HAL_MC_PRINTF(x)  printf x
#define HAL_MC_TRACE() HAL_MC_PRINTF(("%s,%s,%d\n",__FILE__,__FUNCTION__,__LINE__))

#endif /* _HAL_MULTICAST_H_ */