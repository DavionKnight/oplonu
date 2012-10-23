/*
=============================================================================
     Header Name: dal_acl.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                		Date              	Description of Changes
----------------   ------------  ----------------------------------------------
 zran 				 2009/06/25		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#ifndef DAL_ACL_H
#define DAL_ACL_H

#include "opl_driver.h"
#include "opconn_api.h"
#include "isis_acl.h"

/* ACL list number */
#define DAL_ACL_NUM_PER_PORT			96
#define DAL_ACL_TOTAL_NUM				96

#define DAL_ACL_PORT_FLOWCTRL_PORT1_LIST_ID   103
#define DAL_ACL_PORT_FLOWCTRL_PORT2_LIST_ID   104
#define DAL_ACL_PORT_FLOWCTRL_PORT3_LIST_ID   105
#define DAL_ACL_PORT_FLOWCTRL_PORT4_LIST_ID   106
#define DAL_ACL_INVALID_LIST_ID			0xFF

#define DAL_ACL_UPLINK_PORT				6

#define DAL_ACL_POLICER_NUM                   32
#define DAL_ACL_INGRESS_RATELIMIT_PORT1_POLICY_ID  0
#define DAL_ACL_INGRESS_RATELIMIT_PORT2_POLICY_ID  1
#define DAL_ACL_INGRESS_RATELIMIT_PORT3_POLICY_ID  2
#define DAL_ACL_INGRESS_RATELIMIT_PORT4_POLICY_ID  3

#define DAL_ACL_MAC_SAME(D,S)  (vosMemCmp ((D), (S), 6) == 0)


typedef enum DAL_ACL_TYPE_s
{
	DAL_ACL_UNUSED = 0,
	DAL_ACL_1TO1_VT = 1,		/* 1:1 Vlan Translation */
	DAL_ACL_NTO1_VA_COS = 2,	/* N:1 Vlan Aggregation by Cos */
	DAL_ACL_NTO1_VA_MAC = 3,	/* N:1 Vlan Aggregation by MAC */
	DAL_ACL_CLASSIFICATION = 4,	/* Classification */
	DAL_ACL_IGMP = 5,		/* Management Frame */
	DAL_ACL_CTC_MC_VT = 6,		/* CTC mc VT Frame */
	DAL_ACL_FDB = 7,
	DAL_ACL_RSTP = 8,		/* RSTP Frame */
	DAL_ACL_RSVD_VLAN_FILTER = 9,		/* Reserved Vlan */
	DAL_ACL_ZERO_MAC= 10,		/* zero mac Frame */
	DAL_ACL_END
}DAL_ACL_TYPE_e;


typedef struct DAL_ACL_INFO_s
{
	DAL_ACL_TYPE_e		type;
	UINT8		name[64];	/* for class name */
	UINT8		cos[DAL_ACL_NUM_PER_PORT];
	UINT8		mac[DAL_ACL_NUM_PER_PORT * 6];
	UINT8		port;	/* for cls, it's port bitmap; otherwise port ID */
	UINT16		toVid[DAL_ACL_NUM_PER_PORT];		/* Service side Vid */
	UINT16		fromVid[DAL_ACL_NUM_PER_PORT];		/* Customer side Vid */
	UINT8		aclIdUs[DAL_ACL_NUM_PER_PORT];	/* Upstream ACL list ID */		/* 0xFF is invalid ID */
	UINT8		aclIdDs[DAL_ACL_NUM_PER_PORT];	/* Downstream ACL list ID */	
	fal_acl_rule_t  rule;
}DAL_ACL_INFO_s;


OPL_STATUS	dalAclFind1to1VT(UINT32 portId,UINT32 cVid,UINT32 sVid,UINT32 * index);
OPL_STATUS  dalAclIngressRatelimitSet(a_uint8_t portId, a_uint32_t acl_policer_id,a_uint32_t kbits_sec);
OPL_STATUS  dalPortFlowctrlAclSet(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);

#endif
