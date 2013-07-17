/*
=============================================================================
     File Name: dal_mac.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author 	 	              Date              	Description of Changes
----------------   ------------  ----------------------------------------------
 zran 				 2009/05/20		Initial Version	
----------------   ------------  ----------------------------------------------
*/

#include <dal_lib.h>
#include "odm_port.h"
#include "odm_fdb.h"
#include "dal_mac.h"
#include "shiva_misc.h"
#include "shiva_fdb.h"
#include <aos_mem.h>

UINT32 ARLMACNUMLIMIT = PON_BRG_MAC_ENTRY_NUM;

FDB_MAC_DROP_INFO_s macDrop[DAL_MAC_TOTAL_NUM];

char tmpStr[10];

OPL_STATUS dalArlInit()
{
   
    aos_mem_zero(macDrop, sizeof(FDB_MAC_DROP_INFO_s) * DAL_MAC_TOTAL_NUM);

    dalArlMgmRdtCpu();

	/*use the switch acl to do this.*/
    //dalArlDiscardZeroMacAdd();

    return OPL_OK;   

}


/*******************************************************************************
* dalArlAgeTimeSet
*
* DESCRIPTION:
*  		this function is used to set the arl age time interval
*
*	INPUTS:
*			ageingTime:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlAgeTimeSet(UINT32 ageingTime)
{
	UINT32 tmpAgingTime;
    OPL_STATUS ulRet;

    /* for AR8228, the stepping of aging time is 7 seconds */
    tmpAgingTime = ageingTime / 7;
    
	if (0 == tmpAgingTime)
	{
		ulRet = shiva_fdb_age_ctrl_set(0, 0);
        if (SW_OK != ulRet)
        {
            printf("%s: shiva_fdb_age_ctrl_set error %u\r\n", __FUNCTION__, ulRet);
            return ulRet;
        }

	}
	else
	{
		ulRet = shiva_fdb_age_ctrl_set(0, 1);
        if (SW_OK != ulRet)
        {
            printf("%s: shiva_fdb_age_ctrl_set error %u\r\n", __FUNCTION__, ulRet);
            return ulRet;
        }
        
		ulRet = shiva_fdb_age_time_set(0, &ageingTime);
        if (SW_OK != ulRet)
        {
            printf("%s: shiva_fdb_age_time_set error %u\r\n", __FUNCTION__, ulRet);
            return ulRet;
        }
	}

    return OPL_OK;
}



/*******************************************************************************
* dalArlAgeTimeGet
*
* DESCRIPTION:
*  		this function is used to get the arl age time interval
*
*	INPUTS:
*
*	OUTPUTS:
*		ageingTime	     
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlAgeTimeGet(UINT32 *ageingTime)
{
    OPL_STATUS ulRet;
    
	ulRet = shiva_fdb_age_time_get(0, ageingTime);
    if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_age_time_get error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

    *ageingTime = *ageingTime;

    return OPL_OK;
}



/*******************************************************************************
* dalArlDiscardMacAdd
*
* DESCRIPTION:
*  		this function is used to add one FDB entry to 
*			discard the frame which SA equals to the macAddress
*	multicast.
*
*	INPUTS:
*		portId: 		portId
*		macAddress:	mac
*		vlanIndex:	vlanIndex
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlDiscardMacAdd(UINT8 *macAddress)
{
	fal_fdb_entry_t entry;
    sw_error_t ulRet;

	if (NULL == macAddress)
	{
		return OPL_ERROR;
	}
	
	aos_mem_zero(&entry, sizeof(fal_fdb_entry_t));
	aos_mem_copy((void *)entry.addr.uc, macAddress, 6);
    entry.dacmd = FAL_MAC_FRWRD;
	entry.sacmd = FAL_MAC_DROP;
    entry.port.map = 0x0;
    entry.portmap_en = 1;
    entry.static_en = 1;
	
	ulRet = shiva_fdb_add(0, &entry);
    if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_add error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

    return OPL_OK;
}

/*******************************************************************************
* dalArlDiscardZeroMacAdd
*
* DESCRIPTION:
*  		this function is used to add one FDB entry to 
*			discard the frame which SA equals to the Zero
*	multicast.
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
OPL_STATUS dalArlDiscardZeroMacAdd()
{

    //UINT8 zeroMacAddress[MAC_LENGTH] = {0,0,0,0,0xee,0xee};
	fal_fdb_entry_t entry;
    sw_error_t ulRet;

    aos_mem_zero(&entry, sizeof(fal_fdb_entry_t));
	//aos_mem_copy((void *)entry.addr.uc, zeroMacAddress, 6);
    //entry.dacmd = FAL_MAC_DROP;
	entry.sacmd = FAL_MAC_DROP;
       entry.port.map = 0x1E;
       entry.portmap_en = 1;
    entry.static_en = 1;
	
	ulRet = shiva_fdb_add(0, &entry);
    if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_add error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

    return OPL_OK;
}


/*******************************************************************************
* dalArlDiscardMacDel
*
* DESCRIPTION:
*  		this function is used to delete one FDB entry to 
*			discard the frame which SA equals to the macAddress
*	multicast.
*
*	INPUTS:
*		portId: 		portId
*		macAddress:	mac
*		vlanIndex:	vlanIndex
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlDiscardMacDel(UINT8 *macAddress)
{
	fal_fdb_entry_t entry;
    sw_error_t ulRet;

	if (NULL == macAddress)
	{
		return OPL_ERROR;
	}

	aos_mem_zero(&entry, sizeof(fal_fdb_entry_t));
	aos_mem_copy((void *)entry.addr.uc, macAddress, 6);

    ulRet = shiva_fdb_find(0, &entry);
    if (SW_NOT_FOUND == ulRet)
    {
        return OPL_OK;
    }
    else if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_find error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

    /* this mac is not for discarding */
    if (FAL_MAC_DROP != entry.sacmd)
    {
        return OPL_OK;
    }
	
	ulRet = shiva_fdb_del_by_mac(0, &entry);
    if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_del_by_mac error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

    return OPL_OK;
}



/*******************************************************************************
* dalArlMacAdd
*
* DESCRIPTION:
*  		this function is used to add a port-mac map to arl table.the mac address may be unicast or 
*	multicast.
*
*	INPUTS:
*		portId: 		portId
*		macAddress:	mac
*		vlanIndex:	vlanIndex
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacAdd(UINT8 portId,UINT8 *macAddress,UINT16 vlanIndex)
{
	fal_fdb_entry_t entry;    
    sw_error_t ulRet;
	UINT32 ulPortBitmap;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	char macStr[32];
	vosMacToStr(macAddress, macStr);

	//printf("%s(%d, %d, %s)\n", __FUNCTION__, portNum, vlanIndex, macStr);

	aos_mem_zero(&entry, sizeof(fal_fdb_entry_t));
	aos_mem_copy((void *)entry.addr.uc, macAddress, 6);	

	ulPortBitmap = (1 << atherosPort);
	
	ulRet = shiva_fdb_find(0, &entry);
    if (SW_OK == ulRet)
    {
   		ulPortBitmap |= entry.port.map;
    }

	aos_mem_zero(&entry, sizeof(fal_fdb_entry_t));
	aos_mem_copy((void *)entry.addr.uc, macAddress, 6);	
    entry.vid = vlanIndex;
    entry.dacmd = FAL_MAC_FRWRD;
    entry.sacmd = FAL_MAC_FRWRD;
	entry.port.map = ulPortBitmap;
	entry.portmap_en = 1;
    entry.static_en = 1;
	
	ulRet = shiva_fdb_add(0, &entry);
    if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_add error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

    return OPL_OK;
}



/*******************************************************************************
* dalArlMacDel
*
* DESCRIPTION:
*  		this function is used to del a port-mac map from arl table.the mac address may be unicast or 
*	multicast.if the macAddress is a unicast mac address,then remove the mac from arl,or only remove 
*	the portVec from the arl records.
*
*	INPUTS:
*		portId: 		portId
*		macAddress:	mac
*		vlanIndex:	vlanIndex
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacDel(UINT8 portId,UINT8 *macAddress,UINT16 vlanIndex)
{	
	fal_fdb_entry_t entry;
    UINT32 ulPortBitmap;
    sw_error_t ulRet;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	char macStr[32];
	vosMacToStr(macAddress, macStr);

	//printf("%s(%d, %d, %s)\n", __FUNCTION__, portNum, vlanIndex, macStr);

	aos_mem_zero(&entry, sizeof(fal_fdb_entry_t));
	aos_mem_copy((void *)entry.addr.uc, macAddress, 6);	

	ulRet = shiva_fdb_find(0, &entry);
    if (SW_NOT_FOUND == ulRet)
    {
        return OPL_OK;
    }
    else if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_find error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

    ulPortBitmap = ~(1 << atherosPort);

    entry.port.map = entry.port.map & ulPortBitmap;

    /* no port exist in the arl entry */
    if (0 == entry.port.map)
    {
        ulRet = shiva_fdb_del_by_mac(0, &entry);
        if (SW_OK != ulRet)
        {
            printf("%s: shiva_fdb_del_by_mac error %u\r\n", __FUNCTION__, ulRet);
            return ulRet;
        }
    }
    /* just remove the port from the arl entry */
    else
    {
        ulRet = shiva_fdb_add(0, &entry);
        if (SW_OK != ulRet)
        {
            printf("%s: shiva_fdb_add error %u\r\n", __FUNCTION__, ulRet);
            return ulRet;
        }
    }
    
    return OPL_OK;
}



/*******************************************************************************
* dalArlMacRemove
*
* DESCRIPTION:
*  		this function is used to remove a mac address from the arl table of the vlanIndex.
*
*	INPUTS:
*		macAddress:	mac
*		vlanIndex:	    vlanIndex
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacRemove(UINT8 *macAddress,UINT16 vlanIndex)
{
	fal_fdb_entry_t entry;
    sw_error_t ulRet;

	aos_mem_copy((void *)entry.addr.uc, macAddress, 6);
	
	ulRet = shiva_fdb_del_by_mac(0, &entry);
    if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_del_by_mac error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

    return OPL_OK;
}


/*******************************************************************************
* dalArlFlushDynamic
*
* DESCRIPTION:
*  		this function is used to flush all unicast mac address learn by the arl.
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
OPL_STATUS dalArlFlushDynamic(void)
{    
    sw_error_t ulRet;
    
	ulRet = shiva_fdb_del_all(0, ARL_FLUSH_ALL_UNLOCK);
    if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_del_all error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

    return OPL_OK;
}
	

/*******************************************************************************
* dalArlFlushAll
*
* DESCRIPTION:
*  		this function is used to flush all mac address from arl table.
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
OPL_STATUS dalArlFlushAll(void)
{
	sw_error_t ulRet;
    
	ulRet = shiva_fdb_del_all(0, ARL_FLUSH_ALL);
    if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_del_all error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

	ulRet = dalArlMgmRdtCpu();
    if (OPL_OK != ulRet)
    {
        printf("%s: dalArlMgmRdtCpu error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

	/*use the switch acl to do this.*/
    // dalArlDiscardZeroMacAdd();
	return OPL_OK;
}


OPL_STATUS dalArlFlushAllByPort(UINT8 portId)
{
	sw_error_t ulRet;
	UINT32  atherosPort = dalToAtherosPortMap[portId];

	//printf("dalArlFlushAllByPort port %d.", portId);
    
	ulRet = shiva_fdb_del_by_port(0, atherosPort, FAL_FDB_DEL_STATIC);
    if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_del_by_port error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

	return OPL_OK;
}

/*******************************************************************************
* dalArlLearnEnSet
*
* DESCRIPTION:
*  		this function is used to enable or disable arl leanning function
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
OPL_STATUS dalArlLearnEnSet(UINT8 enable)
{
	int portId;
    sw_error_t ulRet;

	for (portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		ulRet = shiva_fdb_port_learn_set(0, portId, enable);
        if (SW_OK != ulRet)
        {
            printf("%s: shiva_fdb_port_learn_set port %u error %u\r\n", __FUNCTION__, portId, ulRet);
            return ulRet;
        }
	}

	return OPL_OK;
}



/*******************************************************************************
* dalArlLearnEnGet
*
* DESCRIPTION:
*  		this function is used to get the arl learning function status
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
OPL_STATUS dalArlLearnEnGet(UINT8 *enable)
{
    sw_error_t ulRet;
    
	ulRet = shiva_fdb_port_learn_get(0, 1, (a_bool_t *)enable);
    if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_port_learn_get error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

    return OPL_OK;
}



/*******************************************************************************
* dalArlSoftLearnEnSet
*
* DESCRIPTION:
*  		this function is used to enable or disable arl leanning function
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
OPL_STATUS dalArlSoftLearnEnSet(UINT8 enable)
{
    int portId;
    a_bool_t bHwLearnEn;
    UINT32 ulUnkSaCmd;
    sw_error_t ulRet;

    if (OPL_TRUE == enable)
    {
        bHwLearnEn = A_FALSE;
        ulUnkSaCmd = FAL_MAC_RDT_TO_CPU;
    }
    else
    {
        bHwLearnEn = A_TRUE;
        ulUnkSaCmd = FAL_MAC_FRWRD;
    }

	for (portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
        ulRet = shiva_fdb_port_learn_set(0, portId, bHwLearnEn);
        if (SW_OK != ulRet)
        {
            printf("%s: shiva_fdb_port_learn_set port %u error %u\r\n", __FUNCTION__, portId, ulRet);
            return ulRet;
        }
        
		ulRet = shiva_port_unk_sa_cmd_set(0, portId, ulUnkSaCmd);
        if (SW_OK != ulRet)
        {
            printf("%s: garuda_port_unk_sa_cmd_set port %u error %u\r\n", __FUNCTION__, portId, ulRet);
            return ulRet;
        }
	}
    
	return OPL_OK;
}



/*******************************************************************************
* dalArlSoftLearnEnGet
*
* DESCRIPTION:
*  		this function is used to get the arl learning function status
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
OPL_STATUS dalArlSoftLearnEnGet(UINT8 *enable)
{
    a_bool_t bHwLearnEn;
    UINT32 ulUnkSaCmd;
    sw_error_t ulRet;
    
    ulRet = shiva_fdb_port_learn_get(0, 1, &bHwLearnEn);
    if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_port_learn_get error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

    if (A_TRUE == bHwLearnEn)
    {
        *enable = A_FALSE;
    }
    else
    {
        ulRet = shiva_port_unk_sa_cmd_get(0, 1, &ulUnkSaCmd);
        if (SW_OK != ulRet)
        {
            printf("%s: garuda_port_unk_sa_cmd_get error %u\r\n", __FUNCTION__, ulRet);
            return ulRet;
        }

        if (FAL_MAC_RDT_TO_CPU == ulUnkSaCmd)
        {
            *enable = OPL_TRUE;
        }
        else
        {
            *enable = OPL_FALSE;
        }
    }
    
	return OPL_OK;
}



/*******************************************************************************
* dalArlAgeEnableSet
*
* DESCRIPTION:
*  		this function is used to enable or disable arl age function
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
OPL_STATUS dalArlAgeEnableSet(UINT8 enable)
{
    sw_error_t ulRet;
    
	ulRet = shiva_fdb_age_ctrl_set(0, enable);
    if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_age_ctrl_set error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

    return OPL_OK;
}



/*******************************************************************************
* dalArlSoftLearnEnGet
*
* DESCRIPTION:
*  		this function is used to get the arl age function status
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
OPL_STATUS dalArlAgeEnableGet(UINT8 *enable)
{
	sw_error_t ulRet;
    
	ulRet = shiva_fdb_age_ctrl_get(0, (a_bool_t *)enable);
    if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_age_ctrl_get error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

    return OPL_OK;
}



/*******************************************************************************
* dalArlMacNumLimitSet
*
* DESCRIPTION:
*  		this function is used to set num of arl mac limit
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
OPL_STATUS dalArlMacNumLimitSet(UINT32 portId, UINT8 enable, UINT32 num)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	return OPL_OK;
}



/*******************************************************************************
* dalArlMultiPortMacAdd
*
* DESCRIPTION:
*  		this function is used to add an atu entry with multiple ports
*	if the atu entry exists already, add the portlist to this atu entry
*	if the atu entry not exists, add atu entry with the portlist
*
*	INPUTS:
*		uiPortNum	Port number for Port List
*		auiPortlist	the Port List associated with the mac
*		macAddress	the mac address of atu entry
*		usVlanId		Vlanid associated with the mac
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK 		success
* 			OPL_ERROR 	failed
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMultiPortMacAdd(UINT32 portNum, UINT32 *portlist,
	UINT8 *macAddress, UINT16 vlanId)
{
	fal_fdb_entry_t entry;
	fal_pbmp_t tmpPortMap;
	UINT8	i;
	sw_error_t ulRet;

	tmpPortMap = 0;
	if (portNum > 0)
	{
		for(i = 0; i < portNum; i++)
		{
			tmpPortMap |= (1 << dalToAtherosPortMap[portlist[i]]);
		}
	}
	else
	{
		return OPL_ERROR;
	}

	aos_mem_zero(&entry, sizeof(fal_fdb_entry_t));
	aos_mem_copy((void *)entry.addr.uc, macAddress, 6);

	ulRet = shiva_fdb_find(0, &entry);
	if (SW_OK == ulRet)
	{
		entry.port.map |= tmpPortMap;
	}
    else if (SW_NOT_FOUND == ulRet)
    {
        aos_mem_zero(&entry, sizeof(fal_fdb_entry_t));
        aos_mem_copy((void *)entry.addr.uc, macAddress, 6);
        entry.port.map = tmpPortMap;
    }
	else
	{
		printf("%s: shiva_fdb_find error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
	}

    entry.dacmd = FAL_MAC_FRWRD;
    entry.sacmd = FAL_MAC_FRWRD;
    entry.portmap_en = 1;
	entry.static_en = 1;
	
	ulRet =  shiva_fdb_add(0, &entry);
    if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_add error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

    return OPL_OK;
}



/*******************************************************************************
* dalArlMultiPortMacDel
*
* DESCRIPTION:
*  		this function is used to delete multiple ports from the desired atu entry
*	if the atu entry exists, delete the portlist of this atu entry
*	if the mac not exists, just return OK
*
*	INPUTS:
*		uiPortNum	Port num for Port List
*		auiPortlist		the Port List associated with the mac
*		macAddress	the mac address needed to be added
*		usVlanId		Vlanid associated with the mac
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK 		success
* 			OPL_ERROR 	failed
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMultiPortMacDel(UINT32 portNum, UINT32 *portlist,
	UINT8 *macAddress, 	UINT16 vlanId)
{
	fal_fdb_entry_t entry;
	fal_pbmp_t tmpPortMap;
	UINT8	i;
    sw_error_t ulRet;

	tmpPortMap = 0;
	if (portNum > 0)
	{
		for(i = 0; i < portNum; i++)
		{
			tmpPortMap |= (1 << dalToAtherosPortMap[portlist[i]]);
		}
	}
    else 
	{
        return OPL_ERROR;
    }

	aos_mem_zero(&entry, sizeof(fal_fdb_entry_t));
	aos_mem_copy((void *)entry.addr.uc, macAddress, 6);

	ulRet = shiva_fdb_find(0, &entry);
	if (SW_NOT_FOUND == ulRet)
	{
		return OPL_OK;
	}
    else if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_add error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

    tmpPortMap = ~tmpPortMap;
    entry.port.map = entry.port.map & tmpPortMap;
    if (0 == entry.port.map)
    {
        ulRet = shiva_fdb_del_by_mac(0, &entry);
        if (SW_OK != ulRet)
        {
            printf("%s: shiva_fdb_del_by_mac error %u\r\n", __FUNCTION__, ulRet);
            return ulRet;
        }
    }
    else
    {
        entry.dacmd = FAL_MAC_FRWRD;
        entry.sacmd = FAL_MAC_FRWRD;
        entry.portmap_en = 1;
    	entry.static_en = 1;
    
        ulRet = shiva_fdb_add(0, &entry);
        if (SW_OK != ulRet)
        {
            printf("%s: shiva_fdb_add error %u\r\n", __FUNCTION__, ulRet);
            return ulRet;
        }
    }

	return OPL_OK;
}



/*******************************************************************************
* dalArlMacPortClear
*
* DESCRIPTION:
*  		this function is used to set the portlist of the desired atu entry 0
*	if the atu entry exists, clear the portlist of this atu entry
*	if the mac not exists, just return OK
*
*	INPUTS:
*		macAddress	the mac address needed to be added
*		usVlanId		Vlanid associated with the mac
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK 		success
* 			OPL_ERROR 	failed
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacPortClear(UINT8 *aucMacAddress, UINT16 usVlanId)
{
    fal_fdb_entry_t entry;
    sw_error_t ulRet;
    
    aos_mem_zero(&entry, sizeof(fal_fdb_entry_t));
	aos_mem_copy((void *)entry.addr.uc, aucMacAddress, 6);

	ulRet = shiva_fdb_find(0, &entry);
    if (SW_NOT_FOUND == ulRet)
    {
        return OPL_OK;
    }
	else if (SW_OK != ulRet)
	{
		printf("%s: shiva_fdb_find error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
	}

    aos_mem_zero(&entry, sizeof(fal_fdb_entry_t));
    aos_mem_copy((void *)entry.addr.uc, aucMacAddress, 6);
    entry.port.map = 0;
    entry.dacmd = FAL_MAC_FRWRD;
    entry.sacmd = FAL_MAC_FRWRD;
    entry.portmap_en = 1;
	entry.static_en = 1;
	
	ulRet =  shiva_fdb_add(0, &entry);
    if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_add error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

    return OPL_OK;
}

char *
dalArlPortmap2Str(UINT32 portMap)
{
	UINT32 i;

	vosMemSet(tmpStr,0, 10);
/*-------- Modified by zhangjj 2013-6-27 -------*/
#if 0
	for(i = 0; i < 7; i++)
	{
		if (1 & (portMap >> i))
		{
			tmpStr[atherosToDalPortMap[i]] = '1';
		}
		else
		{
			tmpStr[atherosToDalPortMap[i]] = '-';
		}		
	}
	return tmpStr;
#else
	UINT32 j=0;
	UINT8 flag = 0;
	UINT8 portmap[15];

	memset(portmap,0,15);
	for(i=0;i<7;i++)
	{
		if(1 & (portMap >> i))
		{
			if(j+2 < 15)
			{
				if(0 != flag)
				{
					portmap[j++] = ',';
					portmap[j++] = i + '0';
				}
				else
				{
					flag = 1;
					portmap[j++] = i + '0';
				}
			}
			else
			{
				printf("dalArlPortmap2Str array overflow...\r\n");
			}
		}
	}

return portmap;
#endif
}

UINT32 dalArlPortmapReverse(UINT32 portMap)
{
	UINT32 i, newPortMap;

	newPortMap = 0;
	for(i = 0; i < 7; i++)
	{
		if (1 & (portMap >> i))
		{
			newPortMap |= (1 << atherosToDalPortMap[i]);
		}
	}

	return newPortMap;
}


/*******************************************************************************
* dalArlMacEntryShowOne
*
* DESCRIPTION:
*		show the information of desired mac entry
*		if the mac entry not exists, this function will print "no such mac entry"
*	INPUTS:
*		lFd				WriteFd of CLI Env
*		aucMacAddress	the mac address needed to delete
*		usType			FDB_MAC_STATIC	for static
*						FDB_MAC_DYNAMIC for dynamic
*						FDB_MAC_TYPE_MAX for not care
*		uiPortNum	Port number for Port List 
*					0 for not care
*		auiPortlist	the Port List associated with the mac
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success
* 		FDB_MAC_SHOW_ONE_ERROR	show one mac entry error
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacEntryShowOne(IN INT32 lFd, IN UINT8 *aucMacAddress,
	IN UINT16 usType, IN UINT32 uiPortNum, IN UINT32 *auiPortlist)
{
	UINT8 szType[10];
	UINT8 szFwdCmd[10];
	UINT8 szSaFwdCmd[10];
	UINT8 szMacaddress[20];
	fal_fdb_entry_t entry;
	UINT32	cnt = 0;
	int rv;

	if (FDB_MAC_INVALID == usType)
	{
		return OPL_OK;
	}

	vosMemSet(&entry, 0, sizeof(fal_fdb_entry_t));	
	vosMemCpy(entry.addr.uc, aucMacAddress,6);
	
	rv = shiva_fdb_find(0, &entry);
	if (SW_OK != rv)
	{
		vosPrintf(lFd, "\r\nno mac entry exists.\r\n");
		return OPL_OK;
	}

	vosPrintf(lFd, "Port Map(CFFFF-G): Cpu Fe1 Fe2 Fe3 Fe4 - GE\r\n");
	vosPrintf(lFd, "Mac Address		PortMap(CFFFFG)		Type	      SaFwdCmd\r\n");
	vosPrintf(lFd, "----------------------------------------------------------------------\r\n");
		
	do
	{
		if (FDB_MAC_TYPE_MAX != usType)
		{
			if ((FDB_MAC_DYNAMIC == usType) && entry.static_en)
			{
				break;
			}

			if ((FDB_MAC_STATIC == usType) && !entry.static_en)
			{
				break;
			}
			
			if ((FDB_MAC_STATIC == usType) && (entry.sacmd == FAL_MAC_DROP))
			{
				break;
			}
			
			if ((FDB_MAC_DISCARD == usType) && !(entry.sacmd == FAL_MAC_DROP))
			{
				break;
			}
		}

		if (entry.static_en)
		{
			OPL_MEMCPY(szType, "Static", 10);
		}
		else
		{
			OPL_MEMCPY(szType, "Dynamic", 10);
		}

#if 0
		switch (entry.dacmd)
		{
			case FAL_MAC_FRWRD:
				OPL_MEMCPY(szFwdCmd, "Forwad", 10);
				break;
			case FAL_MAC_DROP:
				OPL_MEMCPY(szFwdCmd, "Drop", 10);
				break;
			case FAL_MAC_CPY_TO_CPU:
				OPL_MEMCPY(szFwdCmd, "Copy2Cpu", 10);
				break;
			case FAL_MAC_RDT_TO_CPU:
				OPL_MEMCPY(szFwdCmd, "Redir2Cpu", 10);
				break;
			default:
				break;
		}				
#endif

		switch (entry.sacmd)
		{
			case FAL_MAC_FRWRD:
				OPL_MEMCPY(szSaFwdCmd, "Forwad", 10);
				break;
			case FAL_MAC_DROP:
				OPL_MEMCPY(szSaFwdCmd, "Drop", 10);
				break;
			case FAL_MAC_CPY_TO_CPU:
				OPL_MEMCPY(szSaFwdCmd, "Copy2Cpu", 10);
				break;
			case FAL_MAC_RDT_TO_CPU:
				OPL_MEMCPY(szSaFwdCmd, "Redir2Cpu", 10);
				break;
			default:
				break;
		}				

		cliMacToStr(entry.addr.uc, szMacaddress);

		vosPrintf(lFd, "%18s\t%8s\t%12s\t%12s\r\n", szMacaddress, 
			dalArlPortmap2Str(entry.port.id), szType, szSaFwdCmd);
		cnt++;
	} while (0);

	vosPrintf(lFd, "The number of FDB: %d\r\n", cnt);
	
	return OPL_OK;
}




/*******************************************************************************
* dalArlMacEntryShowAll
*
* DESCRIPTION:
*		show the information of all mac entry
*	INPUTS:
*		lFd				WriteFd of CLI Env
*		usType			FDB_MAC_STATIC		for static
*						FDB_MAC_DYNAMIC	for dynamic
*						FDB_MAC_TYPE_MAX	for not care
*		uiPortNum	Port number for Port List 
*					0 for not care
*		auiPortlist	the Port List associated with the mac
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success
* 		FDB_MAC_SHOW_ONE_ERROR	show one mac entry error
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacEntryShowAll(IN INT32 lFd,	IN UINT16 usType,
		IN UINT32 uiPortNum, IN UINT32 *auiPortlist)
{
	UINT8 szType[10];
	UINT8 szFwdCmd[10];
	UINT8 szSaFwdCmd[10];
	UINT8 szMacaddress[20];
	UINT8 tmpPortMap = 0;
	fal_fdb_entry_t entry;
	UINT32	cnt = 0;
    UINT32 i;
	int rv;
	a_uint32_t  iterator = 0;

	if (FDB_MAC_INVALID == usType)
	{
		return OPL_OK;
	}

	vosMemSet(&entry, 0, sizeof(fal_fdb_entry_t));	

	if (uiPortNum > 0)
	{
		tmpPortMap = 0;
		for(i = 0; i < uiPortNum; i++)
		{
			tmpPortMap |= (1 << dalToAtherosPortMap[auiPortlist[i]]);
		}
	}

	vosPrintf(lFd, "Port Map(CFFFF-G): Cpu Fe1 Fe2 Fe3 Fe4 - GE\r\n");
	vosPrintf(lFd, "Mac Address		PortMap(CFFFFG)		Type	      SaFwdCmd      Vid\r\n");
	vosPrintf(lFd, "---------------------------------------------------------------------------------\r\n");

	while(1)
	{
		rv = shiva_fdb_iterate(0, &iterator, &entry);

		if (SW_OK != rv)
		{
			break;
		}

		if (FDB_MAC_TYPE_MAX != usType)
		{
			if ((FDB_MAC_DYNAMIC == usType) && entry.static_en)
			{
				continue;
			}

			if ((FDB_MAC_STATIC == usType) && !entry.static_en)
			{
				continue;
			}
			
			if ((FDB_MAC_STATIC == usType) && (entry.sacmd == FAL_MAC_DROP))
			{
				continue;
			}
			
			if ((FDB_MAC_DISCARD == usType) && !(entry.sacmd == FAL_MAC_DROP))
			{
				continue;
			}
		}

		if (uiPortNum > 0)
		{
			if (!(entry.port.map & tmpPortMap))
			{
				continue;
			}
		}

		if (entry.static_en)
		{
			OPL_MEMCPY(szType, "Static", 10);
		}
		else
		{
			OPL_MEMCPY(szType, "Dynamic", 10);
		}

#if 0
		switch (entry.dacmd)
		{
			case FAL_MAC_FRWRD:
				OPL_MEMCPY(szFwdCmd, "Forwad", 10);
				break;
			case FAL_MAC_DROP:
				OPL_MEMCPY(szFwdCmd, "Drop", 10);
				break;
			case FAL_MAC_CPY_TO_CPU:
				OPL_MEMCPY(szFwdCmd, "Copy2Cpu", 10);
				break;
			case FAL_MAC_RDT_TO_CPU:
				OPL_MEMCPY(szFwdCmd, "Redir2Cpu", 10);
				break;
			default:
				break;
		}				
#endif 

		switch (entry.sacmd)
		{
			case FAL_MAC_FRWRD:
				OPL_MEMCPY(szSaFwdCmd, "Forwad", 10);
				break;
			case FAL_MAC_DROP:
				OPL_MEMCPY(szSaFwdCmd, "Drop", 10);
				break;
			case FAL_MAC_CPY_TO_CPU:
				OPL_MEMCPY(szSaFwdCmd, "Copy2Cpu", 10);
				break;
			case FAL_MAC_RDT_TO_CPU:
				OPL_MEMCPY(szSaFwdCmd, "Redir2Cpu", 10);
				break;
			default:
				break;
		}				

		cliMacToStr(entry.addr.uc, szMacaddress);

		vosPrintf(lFd, "%18s\t%8s\t%12s\t%12s%10d\r\n", szMacaddress,
			dalArlPortmap2Str(entry.port.id), szType, szSaFwdCmd,entry.vid);
		cnt++;
	}

	vosPrintf(lFd, "The number of FDB: %d\r\n", cnt);

	return OPL_OK;
}

OPL_STATUS dalArlMacEntryShowDiscard(IN UINT8 *total_num, IN UINT8 *buf)
{
	UINT8 szType[10];
	UINT8 szFwdCmd[10];
	UINT8 szSaFwdCmd[10];
	UINT8 szMacaddress[20];
	UINT8 tmpPortMap = 0;
	fal_fdb_entry_t entry;
	UINT32	cnt = 0;
	UINT8 mac[6];
    UINT32 i;
	int rv;
	a_uint32_t  iterator = 0;

	vosMemSet(&entry, 0, sizeof(fal_fdb_entry_t));	

	while(1)
	{
		rv = shiva_fdb_iterate(0, &iterator, &entry);

		if (SW_OK != rv)
		{
			break;
		}

		if (!(entry.sacmd == FAL_MAC_DROP))
		{
		  continue;
		}

		cliMacToStr(entry.addr.uc, szMacaddress);
		vosStrCpy(&buf[cnt*22+4], szMacaddress);

		cnt++;
	}
	*total_num = cnt;

	return OPL_OK;
}

OPL_STATUS dalArlMacEntryNumGet(UINT32 filter, UINT32 page_num, 
    UINT32 port_id, UINT32* ptotal_num)
{
    fal_fdb_entry_t entry;
    UINT32	cnt = 0;
    UINT8 tmpPortMap;
    int rv;
    UINT32 i;
    a_uint32_t  iterator = 0;
  
    if (port_id > 0)
    {
        tmpPortMap = 0;
        tmpPortMap |= (0x01 << port_id);
    }
    else
    {
        tmpPortMap = 0x1E; /* 0b0011110 */
    }
  
    while (1)
    {
        rv = shiva_fdb_iterate(0, &iterator, &entry);
        if (SW_OK != rv)
		{
			break;
		}
		
		if (filter == 1)
		{
            if (entry.static_en)
            {
                continue;
            }
		}
		else if (filter == 2)
		{
            if (!entry.static_en)
            {
		        continue;
		    }
		}
		
	    if (!(dalArlPortmapReverse(entry.port.map) & tmpPortMap))
		{
			continue;
		}
	  
	    cnt++;
    }
  
    *ptotal_num = cnt;
    return OPL_OK;
}


OPL_STATUS dalArlMacEntryShowWeb(UINT32 filter, UINT32 page_num, 
    UINT32 port_id, UINT32* ptotal_num, UINT8 *pdata)
{
    fal_fdb_entry_t entry;
    sn_macentry *pentry;
    UINT32	cnt = 0;
    UINT8 tmpPortMap;
    int rv;
    UINT32 i;
    UINT32 offset = 0;
    a_uint32_t  iterator = 0;
  
    if (port_id > 0)
    {
        tmpPortMap = 0;
        tmpPortMap |= (0x01 << port_id);
    }
    else
    {
        tmpPortMap = 0x1E; /* 0b0011110 */
    }

    while (1)
    {
        rv = shiva_fdb_iterate(0, &iterator, &entry);
        if (SW_OK != rv)
		{
			break;
		}
		
		if (filter == 1)
		{
            if (entry.static_en)
            {
                continue;
            }
		}
		else if (filter == 2)
		{
            if (!entry.static_en)
            {
		        continue;
		    }
		}
		
	    if (!(dalArlPortmapReverse(entry.port.map) & tmpPortMap))
		{
			continue;
		}
	  
	    cnt++;
	    if (cnt > ((page_num-1) * 50) && cnt <= (page_num * 50))
	    {
	        pentry = (sn_macentry *)(pdata + offset*sizeof(sn_macentry));
            pentry->type = (int)entry.static_en;
            pentry->port_vec = (int)dalArlPortmapReverse(entry.port.map);
            pentry->mac[0] = entry.addr.uc[0];
            pentry->mac[1] = entry.addr.uc[1];
            pentry->mac[2] = entry.addr.uc[2];
            pentry->mac[3] = entry.addr.uc[3];
            pentry->mac[4] = entry.addr.uc[4];
            pentry->mac[5] = entry.addr.uc[5];
            offset++;
        }
        else {
            break;
        }
    }
  
    *ptotal_num = offset;
    return OPL_OK;
}

/*******************************************************************************
* dalArlMacMove
*
* DESCRIPTION:
*		move fdb entry from one port to another
*	INPUTS:
*		fromPortId:	Port number[1-4]
*		toPortId:		Port number[1-4]
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success
* 		FDB_MAC_SHOW_ONE_ERROR	show one mac entry error
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacMove(UINT32 port, UINT32 toPort)
{
	fal_fdb_entry_t entry;
	int rv;
	a_uint32_t  iterator = 0;
    UINT32 atherosPort = dalToAtherosPortMap[port];
    UINT32 toPhyPort = dalToAtherosPortMap[toPort];

	if ((1 > atherosPort) || ( 4 < atherosPort))
	{
		return OPL_ERROR;
	}

	if ((1 > toPhyPort) || ( 4 < toPhyPort))
	{
		return OPL_ERROR;
	}

	if (atherosPort == toPhyPort)
	{
		return OPL_ERROR;
	}

	memset(&entry, 0, sizeof(fal_fdb_entry_t));	

	while(1)
	{
		rv = shiva_fdb_iterate(0, &iterator, &entry);
		if (SW_OK != rv)
		{
			break;
		}

		if ((1 << atherosPort) & entry.port.map)
		{
			entry.port.map &= ~(1 << atherosPort);
			entry.port.map |= (1 << toPhyPort);
			if (SW_OK != shiva_fdb_add(0, &entry))
			{
				return OPL_ERROR;
			}
		}
	}

	return OPL_OK;
}



/* redirect management frame to CPU port */
OPL_STATUS dalArlMgmRdtCpu(void)
{
	fal_fdb_entry_t entry;
	int rv;

	/* STP multicast address 01:80:C2:00:00:00 */
	UINT8	stpMac[6] = {0x01,0x80,0xC2,0x00,0x00,0x00}; 	
	UINT8	hwAddr[6];

#if 1
	aos_mem_zero(&entry, sizeof(fal_fdb_entry_t));
	aos_mem_copy((void *)entry.addr.uc, stpMac, 6);
	entry.static_en = 1;
    entry.sacmd = FAL_MAC_FRWRD;
	entry.dacmd = FAL_MAC_RDT_TO_CPU;
	entry.port.map = 0x1;
	entry.portmap_en = 1;
    entry.cross_pt_state = 1;

	shiva_fdb_add(0, &entry);
#endif

	vosHWAddrGet("eth0", &hwAddr[0]);
	aos_mem_zero(&entry, sizeof(fal_fdb_entry_t));
	aos_mem_copy((void *)entry.addr.uc, hwAddr, 6);
	entry.static_en = 1;
	entry.dacmd = FAL_MAC_FRWRD;
	entry.port.map = 0x1;
	entry.portmap_en = 1;

	shiva_fdb_add(0, &entry);

	return OPL_OK;
}


OPL_STATUS dalArlFindbyMac(UINT8 * mac, UINT32 * index)
{
	UINT32 i;
	
	for (i = 0; i < DAL_MAC_TOTAL_NUM; i++)
	{
		if ((macDrop[i].type)
			&& ( 0 == aos_mem_cmp(mac, macDrop[i].mac, 6)))
		{
			*index = i;
			return OPL_OK;
		}		
	}

	return OPL_ERROR;
}

OPL_STATUS dalArlFindFreeRsc(UINT32 * index)
{
	UINT32 i;
	
	for (i = 0; i < DAL_MAC_TOTAL_NUM; i++)
	{
		if (macDrop[i].type)
		{
			continue;
		}		
	}

	if (i >= DAL_MAC_TOTAL_NUM)
	{
		return OPL_ERROR;
	}

	*index = i;
	return OPL_ERROR;
}

/*******************************************************************************
* dalArlMacFilterAdd
*
* DESCRIPTION:
*  		this function is used to add one FDB entry to 
*			discard the frame which SA equals to the macAddress
*	multicast.
*
*	INPUTS:
*		portId: 		portId
*		vlanId:	        vlan id
*		macAddress:	    mac address
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacFilterAdd(UINT8 portId, UINT16 vlanId, UINT8 *macAddress)
{
	fal_fdb_entry_t entry;
    sw_error_t ulRet;
    UINT32 atherosPort = dalToAtherosPortMap[portId];
	
	char macStr[32];
	vosMacToStr(macAddress, macStr);

	//printf("%s(%d, %d, %s)\n", __FUNCTION__, portId, vlanId, macStr);

	if (NULL == macAddress)
	{
		return OPL_ERROR;
	}
	
	aos_mem_zero(&entry, sizeof(fal_fdb_entry_t));
	aos_mem_copy((void *)entry.addr.uc, macAddress, 6);
    entry.dacmd = FAL_MAC_FRWRD;
	entry.sacmd = FAL_MAC_DROP;
    //entry.port.map = 1 << atherosPort ;
    entry.port.map = 0x0 ;
    entry.portmap_en = 1;
    entry.static_en = 1;
	
	ulRet = shiva_fdb_add(0, &entry);
    if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_add error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

    return OPL_OK;
}



/*******************************************************************************
* dalArlMacFilterDel
*
* DESCRIPTION:
*  		this function is used to delete one FDB entry to 
*			discard the frame which SA equals to the macAddress
*	multicast.
*
*	INPUTS:
*		portId: 		portId
*		vlanId:	        vlan id
*		macAddress:	    mac address
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacFilterDel(UINT8 portId, UINT16 vlanId, UINT8 *macAddress)
{
	fal_fdb_entry_t entry;
    sw_error_t ulRet;
    UINT32 atherosPort = dalToAtherosPortMap[portId];

	char macStr[32];
	vosMacToStr(macAddress, macStr);

	//printf("%s(%d, %d, %s)\n", __FUNCTION__, portId, vlanId, macStr);


	if (NULL == macAddress)
	{
		return OPL_ERROR;
	}

	aos_mem_zero(&entry, sizeof(fal_fdb_entry_t));
	aos_mem_copy((void *)entry.addr.uc, macAddress, 6);
    entry.port.map = 1 << atherosPort ;

    ulRet = shiva_fdb_find(0, &entry);
    if (SW_NOT_FOUND == ulRet)
    {
        return OPL_OK;
    }
    else if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_find error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

    /* this mac is not for discarding */
    if (FAL_MAC_DROP != entry.sacmd)
    {
        return OPL_OK;
    }
	
	ulRet = shiva_fdb_del_by_mac(0, &entry);
    if (SW_OK != ulRet)
    {
        printf("%s: shiva_fdb_del_by_mac error %u\r\n", __FUNCTION__, ulRet);
        return ulRet;
    }

    return OPL_OK;
}

OPL_STATUS dalArlMacDlfFilterEn(UINT8 portId, UINT8 enable)
{
  UINT32 atherosPort = dalToAtherosPortMap[portId];
  
  shiva_port_unk_uc_filter_set(0, atherosPort, enable);
  return OPL_OK;
}

OPL_STATUS dalArlMacPortLockDropEn(UINT8 portId, UINT8 enable)
{
  UINT32 atherosPort = dalToAtherosPortMap[portId];
  UINT32 ulUnkSaCmd;
//printf("%s enable %d.\n", __FUNCTION__, enable);
  if (OPL_TRUE == enable)
   {
        ulUnkSaCmd = FAL_MAC_DROP;
    }
    else
    {
        ulUnkSaCmd = FAL_MAC_FRWRD;
    }
  
  shiva_port_unk_sa_cmd_set(0, atherosPort, ulUnkSaCmd);
  return OPL_OK;
}

OPL_STATUS dalArlMacPortLockCpuEn(UINT8 portId, UINT8 enable)
{
  UINT32 atherosPort = dalToAtherosPortMap[portId];
  UINT32 ulUnkSaCmd;
  a_bool_t bHwLearnEn;
//printf("%s enable %d.\n", __FUNCTION__, enable);
    if (OPL_TRUE == enable)
    {
        bHwLearnEn = A_FALSE;
        ulUnkSaCmd = FAL_MAC_RDT_TO_CPU;
    }
    else
    {
        bHwLearnEn = A_TRUE;
        ulUnkSaCmd = FAL_MAC_FRWRD;
    }
	shiva_fdb_port_learn_set(0, atherosPort, bHwLearnEn);
	shiva_port_unk_sa_cmd_set(0, atherosPort, ulUnkSaCmd);
  return OPL_OK;
}

OPL_STATUS dalArlMgmtMacRdt2Cpu(UINT8 * oldMac, UINT8 * newMac)
{
	fal_fdb_entry_t entry;
	int rv;

	aos_mem_zero(&entry, sizeof(fal_fdb_entry_t));
	aos_mem_copy((void *)entry.addr.uc, oldMac, 6);	

	rv = shiva_fdb_find(0, &entry);
    if (SW_OK == rv)
    {
        shiva_fdb_del_by_mac(0, &entry);
    }

	aos_mem_zero(&entry, sizeof(fal_fdb_entry_t));
	aos_mem_copy((void *)entry.addr.uc, newMac, 6);
	entry.static_en = 1;
    entry.sacmd = FAL_MAC_FRWRD;
	entry.dacmd = FAL_MAC_RDT_TO_CPU;
	entry.port.map = 0x1;
	entry.portmap_en = 1;
    entry.leaky_en = 1;

	shiva_fdb_add(0, &entry);
    return OPL_OK;
}

SOFT_LEARN_MAC_t *arlsoftLearnMacEntry = NULL;
OPL_STATUS dalSoftArlMacAdd(UINT16 portId,UINT16 vlanId,UINT8 *mac)
{
  UINT8 Flg = 0;
  SOFT_LEARN_MAC_t *ptmp1;
  SOFT_LEARN_MAC_t *ptmp2;

  ptmp1 = arlsoftLearnMacEntry;
  ptmp2 = arlsoftLearnMacEntry;
  while (ptmp1 != NULL)
  {
    if ((ptmp1->vlanId==vlanId) && (ptmp1->portId == portId) &&
		(!memcmp(&(ptmp1->mac[0]), mac, ETH_MAC_ADDR_LEN)))
    {
      Flg = 1;
      break;
    }
	ptmp2 = ptmp1;
    ptmp1 = ptmp1->next;
  }

  if (1 == Flg)
  {
  /*	if (ptmp1->portId == portId){
		return OPL_OK;
  	}
    ptmp1->portId = portId;*/
  }
  else
  {
     ptmp1 = (SOFT_LEARN_MAC_t *)malloc(sizeof(SOFT_LEARN_MAC_t));
     if (NULL == ptmp1)
     {
       return OPL_ERROR;
     }
     ptmp1->portId = portId;
     ptmp1->vlanId = vlanId;
	 ptmp1->next = NULL;
     memcpy(&(ptmp1->mac[0]), mac, ETH_MAC_ADDR_LEN);
	 if (arlsoftLearnMacEntry == NULL)
	 {
	   arlsoftLearnMacEntry = ptmp1;
     }
	 else
	 {
	   ptmp2->next = ptmp1;
	 }

  }
  
  return dalArlMacAdd(portId, mac, vlanId);
  
}

OPL_STATUS dalSoftArlMacDel(UINT16 portId, UINT16 vlanId,UINT8 *mac)
{
  SOFT_LEARN_MAC_t *ptmp;
  SOFT_LEARN_MAC_t *ptmp1;
  SOFT_LEARN_MAC_t *ptmp2;

  //printf("dalSoftArlMacDel port %d vlan %d mac[5] %x.\n", portId, vlanId, mac[5]);

  ptmp = arlsoftLearnMacEntry;
  ptmp1 = arlsoftLearnMacEntry;
  ptmp2 = arlsoftLearnMacEntry;
  while (ptmp1 != NULL)
  {
    if ((ptmp1->vlanId==vlanId) && (ptmp1->portId == portId) &&
		(!memcmp(&(ptmp1->mac[0]), mac, ETH_MAC_ADDR_LEN)))
    {
      dalArlMacDel(ptmp1->portId,&(ptmp1->mac[0]), ptmp1->vlanId);
      if (ptmp1 == arlsoftLearnMacEntry){
  	  	ptmp = ptmp1->next;
  	  }
  	  else{
  		ptmp2->next = ptmp1->next;
  	  }
      free(ptmp1);
      break;
    }
	ptmp2 = ptmp1;
    ptmp1 = ptmp1->next;
  }
  
  arlsoftLearnMacEntry = ptmp;

  //printf("%s arlsoftLearnMacEntry %x\n", __FUNCTION__, arlsoftLearnMacEntry);
  return OPL_OK;
}


OPL_STATUS dalSoftArlMacFlush(UINT16 portId)
{
  SOFT_LEARN_MAC_t *ptmp;
  SOFT_LEARN_MAC_t *ptmp1;
  SOFT_LEARN_MAC_t *ptmp2;

  ptmp = NULL;
  ptmp1 = arlsoftLearnMacEntry;
  ptmp2 = arlsoftLearnMacEntry;
  while (ptmp2!= NULL)
  {
    ptmp2 = ptmp2->next;
	if (ptmp1->portId == portId){
	    dalArlMacDel(ptmp1->portId,&(ptmp1->mac[0]), ptmp1->vlanId);
	    free(ptmp1);
	}
	else{
		if (ptmp){
			ptmp->next = ptmp1;
		}
		ptmp = ptmp1;
	}
	
	ptmp1 = ptmp2;
  }
  
  arlsoftLearnMacEntry = ptmp;
    //printf("%s arlsoftLearnMacEntry %x\n", __FUNCTION__, arlsoftLearnMacEntry);

  return OPL_OK;
}


