/*
=============================================================================
Header Name:dal_igmp.h

  General Description:
  
	This file define all opcomm chip regisger and table address.
	===============================================================================
	Opulan Confidential Proprietary                     
	ID and version: xxxxxxxxxxxxxx  Version 1.00
	(c) Copyright Opulan XXXX - XXXX, All Rights Reserved
	
	  
		Revision History:
		Modification
		Author			Date				Description of Changes
		---------------	---------------	----------------------------------------------
		zzhu			2007/11/09		Initial Version
		---------------	---------------	----------------------------------------------
*/
#ifndef DAL_IGMP_H
#define DAL_IGMP_H

#include <opl_driver.h>
#include <opconn_api.h>

typedef struct GROUP_ADDR_CLS_INDEX_MAP_s{
	UINT8 	valid;
	UINT16 clsIndex;
	UINT8   mac[MAC_LENGTH];
       UINT16 vid;
}GROUP_ADDR_CLS_INDEX_MAP_t;

typedef struct PORT_MC_VLAN_s
{
    UINT16 mc_vlan[64];
    UINT8  tagstrip;
}__attribute__ ((packed)) PORT_MC_VLAN_t;

#define MULTCAST_CLS_STARTID 		100
#define MAX_GROUP_NUM 			    32 //multi-port no need use classify to switch multicast traffic.
#define MC_MAX_GROUP_NUM 			    64*4

/*******************************************************************************
* dalMulticastInit
*
* DESCRIPTION:
*  		this function drop all multicast packets from olt except the destma equeal the table.
*		send all oam packet to cpu.
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
OPL_STATUS dalMulticastInit(void);
/*******************************************************************************
* dalMulticastVlanAddPort
*
* DESCRIPTION:
*  		this function used to add a port to vlan,default is MEMBER_EGRESS_UNTAGGED
*
*	INPUTS:
*			portNum:
*			vlanId:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalMulticastVlanAddPort(UINT8 portNum,UINT16 vlanId, UINT32 tag);

/*******************************************************************************
* dalMulticastVlanDelPort
*
* DESCRIPTION:
*  		this function used to remove a port from a vlan.
*
*	INPUTS:
*			portNum:
*			vlanId:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalMulticastVlanDelPort(UINT8 portNum,UINT16 vlanId);

/*******************************************************************************
* dalMulticastVlanTagStripe
*
* DESCRIPTION:
*  		this function used to disable or enable the egress with or without tag information
*
*	INPUTS:
*			portNum:
*			enable:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalMulticastVlanTagStripe(UINT8 portNum,UINT8 enable);

/*******************************************************************************
* dalMulticastArlAddPort
*
* DESCRIPTION:
*  		this function used to add a arl records(unicast),or add a port to a multicast records.
*
*	INPUTS:
*			portNum:
*			macAddr:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalMulticastArlAddPort(UINT8 portNum,UINT8 *macAddr);

/*******************************************************************************
* dalMulticastArlPortDel
*
* DESCRIPTION:
*  		this function used to remove a arl records(unicast),or remove a port from a multicast group.
*
*	INPUTS:
*			portNum:
*			macAddr:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalMulticastArlPortDel(UINT8 portNum,UINT8 *macAddr);

/*******************************************************************************
* dalMulticastArlVlanAdd
*
* DESCRIPTION:
*  		this function used add a multicast group:port+vlan+mac
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalMulticastArlVlanAdd(UINT8 portNum,UINT16 vlanId,UINT8 *macAddr);

/*******************************************************************************
* dalMulticastArlVlanDel
*
* DESCRIPTION:
*  		this function used remove a multicast group:port+vlan+mac
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalMulticastArlVlanDel(UINT8 portNum,UINT16 vlanId,UINT8 *macAddr);

/*******************************************************************************
* dalMulticastArlTabFlush
*
* DESCRIPTION:
*  		this function used flush all arl table
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
OPL_STATUS dalMulticastArlTabFlush(void);

/*******************************************************************************
* dalMulticastPortMacAdd
*
* DESCRIPTION:
*  		this function used to add a multicast control rule: port+mac
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastPortMacAdd(UINT8 portNum, UINT8 *macAddr);

/*******************************************************************************
* dalMulticastPortMacDel
*
* DESCRIPTION:
*  		this function used to delete a multicast control rule: port+mac
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastPortMacDel(UINT8 portNum, UINT8 *macAddr);

/*******************************************************************************
* dalMulticastPortVlanMacAdd
*
* DESCRIPTION:
*  		this function used to add a multicast control rule: port+vlan+mac
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastPortVlanMacAdd(UINT8 portNum, UINT16 vlanId, UINT8 *macAddr, OPL_BOOL fuzzy);

/*******************************************************************************
* dalMulticastPortVlanMacDel
*
* DESCRIPTION:
*  		this function used to delete a multicast control rule: port+vlan+mac
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastPortVlanMacDel(UINT8 portNum, UINT16 vlanId, UINT8 *macAddr, OPL_BOOL fuzzy);

/*******************************************************************************
* dalMulticastRuleDelAll
*
* DESCRIPTION:
*  		this function used to delete all multicast control rule
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastRuleDelAll(void);

/*******************************************************************************
* dalMulticastDataFilterAdd
*
* DESCRIPTION:
*  		this function used add a multicast control rule: port+vlan+mac
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastDataFilterAdd(void);

/*******************************************************************************
* dalMulticastDataFilterDel
*
* DESCRIPTION:
*  		this function used add a multicast control rule: port+vlan+mac
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastDataFilterDel(void);

#endif

