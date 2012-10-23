/*
=============================================================================
     File Name: dal_mac.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     
NOTES: opconn ONU chip not support 802.1q vlan function.
so implement by switch chip. 
Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
wangliang 			2008/8/19		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif /*__cplusplus*/
#endif /*__cplusplus*/

#include <dal_lib.h>

OPL_STATUS dalSetLocalSwitch
(
    UINT8  lport,
    UINT8  enable
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalSetPortPVid
*
* DESCRIPTION:
*  		this function support set the port default vlan id (PVID).
*
*	INPUTS:
          lport:user logic port.
          vid  :vlan Id
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalSetPortPVid
(
    UINT8  lport,
    UINT16 vid
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalGetPortPVid
*
* DESCRIPTION:
*  		this function support get the port default vlan id (PVID).
*
*	INPUTS:
          lport:user logic port.          
*		
*	OUTPUTS:
          vid  :vlan Id
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalGetPortPVid
(
    UINT8  lport,
    UINT16 *vid
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalSetPortBaseVlanMember
*
* DESCRIPTION:
*  		this function support set port base vlan port member.
*
*	INPUTS:
          lport:user logic port.
          memlist:member list.
          memnum:member number.
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalSetPortBaseVlanMember
(
    UINT8 lport,
    UINT8 *memlist,
    UINT8 memnum
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalGetPortBaseVlanMember
*
* DESCRIPTION:
*  		this function support get port base vlan port member.
*
*	INPUTS:
          lport:user logic port.
*		
*	OUTPUTS:
          memlist:member list.
          memnum:member number.
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalGetPortBaseVlanMember
(
    UINT8 lport,
    UINT8 *memlist,
    UINT8 *memnum
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalSetPortDot1qMode
*
* DESCRIPTION:
*  		this function support set port 802.1q check mode.
*
*	INPUTS:
          lport:user logic port. 
          mode :port 802.1q check mode.
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalSetPortDot1qMode
(
    UINT8 lport,
    SW_DOT1Q_MODE mode
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalGetPortDot1qMode
*
* DESCRIPTION:
*  		this function support get port 802.1q check mode.
*
*	INPUTS:
          lport:user logic port. 
*		
*	OUTPUTS:
          mode :port 802.1q check mode.
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalGetPortDot1qMode
(
    UINT8  lport,
    SW_DOT1Q_MODE *mode
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalCreateVlan
*
* DESCRIPTION:
*  		this function support create a vlan.
*
*	INPUTS:
          vid:vlan id. 
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalCreateVlanID
(
    UINT16 vid
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalDeleteVlan
*
* DESCRIPTION:
*  		this function support delete a vlan.
*
*	INPUTS:
          vid:vlan id. 
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalDeleteVlanID
(
    UINT16 vid
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalFlushVlanAll
*
* DESCRIPTION:
*  		this function support flush all vlan .
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
OPL_STATUS dalFlushVlanAll
(
    void
)
{
    UINT8 recordIndex;
	BRG_T_VTT_ENTRY_t oldEntry;
    OPL_STATUS retVal = OPL_OK;

    OPL_MEMSET(&oldEntry,0X00,sizeof(BRG_T_VTT_ENTRY_t));
    
    for(recordIndex = OPL_ZERO;recordIndex < DAL_PORT_MCAST_ENTRY_NUM;recordIndex++)
    {
        
        brgVttEntryHwWrite(DAL_PORT_MCAST_ENTRY_START_NUM + recordIndex, &oldEntry);
    }
    return OPL_OK;
}
/*******************************************************************************
* dalAddVlanPort
*
* DESCRIPTION:
*  		this function support add a port to vlan.
*
*	INPUTS:
          lport:user logic port.
          vid:vlan id.
          type:port vlan egress type.
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalAddVlanPort
(
    UINT8  lport,
    UINT16 vid,
    VLAN_EGRESS_TYPE_E type
)
{
    return OPL_OK;
}
/*******************************************************************************
* dalMoveVlanPort
*
* DESCRIPTION:
*  		this function support move a port to vlan.
*
*	INPUTS:
          lport:user logic port.
          vid:vlan id.
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalMoveVlanPort
(
    UINT8  lport,
    UINT16 vid
)
{
    return OPL_OK;
}

OPL_STATUS dalGetVlanPortEGMode
(
    UINT16 vid,
    UINT32 lport,
    VLAN_EGRESS_TYPE_E *type
)
{
    return OPL_OK;
}

OPL_STATUS dalSetPortEgressMode
(
    UINT8 lport,
    PORT_EGRESS_MODE mode
)
{
    return OPL_OK;
}
OPL_STATUS dalGetPortEgressMode
(
    UINT8 lport,
    PORT_EGRESS_MODE *mode
)
{
    return OPL_OK;
}
OPL_STATUS dalShowChipVlanEntry
(
    void
)
{
    return OPL_OK;
}

int dalSetPortDefPri
(
    UINT8 lport,
    UINT8 pri
)
{
    return OPL_OK;
}

OPL_STATUS dalTrunkEntryAdd(UINT8 portId, UINT32 vlanId)
{
	UINT32 vttIndex;
	OPL_STATUS retVal;
	
	retVal = dalVttTagEntryAdd(0, 0, vlanId, vlanId, portId, &vttIndex, DAL_VTT_UC);

    return retVal;
}

OPL_STATUS dalTrunkEntryDel(UINT8 portId, UINT32 vlanId)
{
	UINT32 vttIndex;
	OPL_STATUS retVal;

	retVal = dalVttTagEntryDel(0, 0, vlanId, vlanId, portId, &vttIndex, DAL_VTT_UC);

    return retVal;
}

OPL_STATUS dalTrunkEntryValueGet(UINT8 portId, UINT16 entryIndex, UINT16 *pVlanId)
{
	UINT8 i;
	UINT32 cVlan;
	UINT32 sVlan;
	OPL_STATUS retVal;
	UINT16 validCount;

	validCount = 0;
	
	retVal = dalVttTagEntryGet(entryIndex, &cVlan, &sVlan, portId);

	if(retVal == OPL_OK)
	{
		*pVlanId = (UINT16)cVlan;
	}

	return retVal;
}

OPL_STATUS dalTrunkEntryNumGet(UINT8 portId, UINT32 *pNum)
{
	UINT8 recordNum;
	OPL_STATUS retVal;
	
	retVal = dalVttNumOfRecordGet(&recordNum, portId);

	if(retVal != OPL_OK)
		return retVal;

	*pNum = (UINT32)recordNum;
	
    return OPL_OK;
}

OPL_STATUS dalTrunkEntryClear(UINT8 portId)
{
	UINT8 i;
	BRG_VTT_ENTRY_t brg_vtt_entry;
	 
	OPL_MEMSET(&brg_vtt_entry,0X00,sizeof(BRG_VTT_ENTRY_t));

	for(i = 0; i < DAL_PORT_VTT_ENTRY_NUM; i++)
	{
		brgVttEntryHwWrite(i, &brg_vtt_entry);
	}

    return OPL_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /*__cplusplus*/
#endif /*__cplusplus*/

