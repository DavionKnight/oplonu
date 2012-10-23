/*
=============================================================================
     Header Name: dal_igmp.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved


Revision History:
Author                		Date              	Description of Changes
----------------   ------------  ----------------------------------------------
 zran 				 2009/07/22		Initial Version
----------------   ------------  ----------------------------------------------
*/

#include <dal_lib.h>
#include <fal_vlan.h>
#include <isis_portvlan.h>
#include <fal_rate.h>

#define MV_MAX_DBNUM  64

UINT16           dbnum_flag[64];
PORT_MC_VLAN_t   port_mcvlan[DAL_NUM_OF_PORTS];

GROUP_ADDR_CLS_INDEX_MAP_t  groupAddrClsIndexMapTab[MAX_GROUP_NUM];
static UINT16 mcDataFilterRuleId = 0xFFFF;
extern VLAN_CONFI_INFO_t dal_vtt_info[];
extern VTT_VLAN_8228_t	dal_vlan8228[];


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
OPL_STATUS dalMulticastInit(void)
{
	int i;
	OPL_STATUS retVal = OPL_OK;

	for(i = 0;i < MV_MAX_DBNUM;i++)
	{
		dbnum_flag[i] = 0xffff;
	}
	OPL_MEMSET(port_mcvlan,0x0,DAL_NUM_OF_PORTS*sizeof(PORT_MC_VLAN_t));
	OPL_MEMSET(&groupAddrClsIndexMapTab[0],0x00,MAX_GROUP_NUM*sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));

#if 1

	/* drop all igmp packets */
	retVal = dalClsTranActCfg(MULTCAST_CLS_STARTID -1,CLASSIFY_EII_IP,DROP_PKTS);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	retVal = dalClsDestMacAddrCfg(MULTCAST_CLS_STARTID -1, CLASSIFY_EII_IP, "01:00:5e:00:00:00", "ff:ff:ff:00:00:00", OPL_ENABLE);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
#endif
	/*send pon igmp packets cpu*/
	retVal = dalClsTranActCfg(MULTCAST_CLS_STARTID -2,CLASSIFY_EII_IP,COPY_PKTS_TO_CPU|DROP_PKTS);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

 #if 0
	retVal = dalClsTypeLenCfg(MULTCAST_CLS_STARTID -2, CLASSIFY_EII_IP,0x0800, 0xffff, OPL_ENABLE);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	
	retVal = dalClsIpTypeCfg(MULTCAST_CLS_STARTID -2, CLASSIFY_EII_IP, 2, 0xff, OPL_ENABLE);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
#endif

       retVal = dalClsTypeLenCfg(MULTCAST_CLS_STARTID -2, CLASSIFY_EII_IP,0x9100, 0xffff, OPL_ENABLE);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	
	retVal = dalClsDestMacAddrCfg(MULTCAST_CLS_STARTID -2, CLASSIFY_EII_IP, "01:00:5e:00:00:00", "ff:ff:ff:00:00:00", OPL_ENABLE);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

#if 0 /* yxzhao added, called at mcInitFromConfigFile(), 2010-01-04, 16:17:51 */
	dalMulticastEnable();
#endif

	return OPL_OK;
}


OPL_STATUS dalMulticastEnable()
{
    OPL_STATUS retVal = OPL_OK;
	UINT8	port_id;

/*	fal_port_policer_t policer;
	
	memset(&policer, 0, sizeof(fal_port_policer_t));
	isis_rate_port_policer_get(0,6,&policer);

	policer.e_rate_flag |= FAL_INGRESS_POLICING_UNK_MUL;
	policer.eir = 32;
	policer.e_enable = A_TRUE;

	isis_rate_port_policer_set(0,6,&policer); */
	
    OPL_MEMSET(&groupAddrClsIndexMapTab[0],0x00,MAX_GROUP_NUM*sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));

    retVal = dalClsTranActCfg(MULTCAST_CLS_STARTID -2,CLASSIFY_EII_IP,COPY_PKTS_TO_CPU|DROP_PKTS);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

#if 1
	retVal = dalClsTranActCfg(MULTCAST_CLS_STARTID -1,CLASSIFY_EII_IP,DROP_PKTS);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

    retVal = dalMulticastDataFilterAdd();
    if(OPL_OK != retVal)
    {
    	return retVal;
    }
#endif
#if 1
	/*forbit mulcast flooding when igmp enabled */
	for(port_id = 0; port_id < 7; port_id++)
	{
		isis_port_unk_mc_filter_set(0, port_id, 1);
	}
	//isis_port_unk_mc_filter_set(0,6,1);
#endif
	isis_igmp_mld_cmd_set(0, FAL_MAC_RDT_TO_CPU);

	dalAclIgmpEnable(A_TRUE);

	return retVal;
}



OPL_STATUS dalMulticastDisable()
{
    OPL_STATUS retVal = OPL_OK;
	UINT8	port_id;
/*	fal_port_policer_t policer;

	memset(&policer, 0, sizeof(fal_port_policer_t));
	isis_rate_port_policer_get(0,6,&policer);

	policer.e_rate_flag &= ~FAL_INGRESS_POLICING_UNK_MUL;
	policer.e_enable = A_FALSE;

	isis_rate_port_policer_set(0,6,&policer); */
	
    OPL_MEMSET(&groupAddrClsIndexMapTab[0],0x00,MAX_GROUP_NUM*sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));

    retVal = dalClsTranActCfg(MULTCAST_CLS_STARTID -2,CLASSIFY_EII_IP,0);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
#if 1
	retVal = dalClsTranActCfg(MULTCAST_CLS_STARTID -1,CLASSIFY_EII_IP,0);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
    
    retVal = dalMulticastDataFilterDel();
    if(OPL_OK != retVal)
    {
    	return retVal;
    }
#endif
#if 1
	/*forbit mulcast flooding when igmp disabled */
	for(port_id = 0; port_id < 7; port_id++)
	{
		isis_port_unk_mc_filter_set(0, port_id, 0);
	}
	//isis_port_unk_mc_filter_set(0, 6, 0);
#endif
	isis_igmp_mld_cmd_set(0, FAL_MAC_CPY_TO_CPU);

	dalAclIgmpEnable(A_FALSE);

	for(port_id = 1; port_id < 5; port_id++)
	{
		/* restore the configuration from port vlan mode */
		if (VLAN_TRANSPARENT == dal_vtt_info[dalToAtherosPortMap[port_id]].vlanMode)
		{
			return isis_port_egvlanmode_set(0, dalToAtherosPortMap[port_id], FAL_EG_UNMODIFIED); /*FAL_EG_HYBRID to FAL_EG_UNMODIFIED*/
		}
		else if(VLAN_TAG == dal_vtt_info[dalToAtherosPortMap[port_id]].vlanMode)
		{
			return isis_port_egvlanmode_set(0, dalToAtherosPortMap[port_id], FAL_EG_UNTAGGED);
		}
		else if (VLAN_TRANSLATION == dal_vtt_info[dalToAtherosPortMap[port_id]].vlanMode|
			VLAN_AGGREGATION == dal_vtt_info[dalToAtherosPortMap[port_id]].vlanMode)
		{
			return isis_port_egvlanmode_set(0, dalToAtherosPortMap[port_id], FAL_EG_UNMODIFIED); /*FAL_EG_HYBRID to FAL_EG_UNMODIFIED*/
		}
	}

	return retVal;
}



/*******************************************************************************
* dalMallocVlanDBNUM
*
* DESCRIPTION:
*  		this function malloc a new Vlan DBNumber.
*
*	INPUTS:
           vlanId:VLAN ID.
*
*	OUTPUTS:
           dsnum:VLAN DBNumber.
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMallocVlanDBNUM(UINT16 vlanId,UINT8 *dbnum)
{
    UINT8 i;
    UINT8 emptydbnum = 0xff;

    if (NULL == dbnum) return OPL_ERROR;


    for (i = 1;i < 64;i++)
    {
        if (dbnum_flag[i] == vlanId)
        {
            *dbnum = i;
            return OPL_OK;
        }
        else if (dbnum_flag[i] == 0xffff)
        {
            emptydbnum  = i;
        }
    }

    if ((64 == i)&&(emptydbnum < 64))
    {
        *dbnum = emptydbnum;
        dbnum_flag[emptydbnum] = vlanId;
        return OPL_OK;
    }

    return OPL_ERROR;
}



/*******************************************************************************
* dalGetVlanDBNUM
*
* DESCRIPTION:
*  		this function get Vlan DBNumber.
*
*	INPUTS:
           vlanId:VLAN ID.
*
*	OUTPUTS:
           dsnum:VLAN DBNumber.
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalGetVlanDBNUM(UINT16 vlanId,UINT8 *dbnum)
{
    UINT8 i;
    UINT8 emptydbnum = 0xff;
    /*get dbnum*/
    if (NULL == dbnum) return OPL_ERROR;

    for (i = 1;i < 64;i++)
    {
        if (dbnum_flag[i] == vlanId)
        {
            *dbnum = i;
            return OPL_OK;
        }
    }

    *dbnum = 0;
    return OPL_OK;
}



/*******************************************************************************
* dalClearVlanDBNUM
*
* DESCRIPTION:
*  		this function clear a Vlan DBNumber.
*
*	INPUTS:
           vlanId:VLAN ID.
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalClearVlanDBNUM(UINT16 vlanId)
{
    UINT8 i;
    UINT8 emptydbnum = 0xff;


    for (i = 1;i < 64;i++)
    {
        if (dbnum_flag[i] == vlanId)
        {
            dbnum_flag[i] = 0xffff;
            return OPL_OK;
        }
    }
    return OPL_ERROR;
}



/*******************************************************************************
* dalMulticastVlanCreate
*
* DESCRIPTION:
*  		this function create a multicast vlan.
*
*	INPUTS:
           vlanId:VLAN ID.
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastVlanCreate(UINT16 vlanId)
{
	int rv;

	if (DAL_VLAN_UNUSED != dal_vlan8228[vlanId].type)
	{
		return OPL_ERROR;
	}
	
	rv = isis_vlan_create(0, vlanId);
	if (SW_OK == rv)
	{
		memset(&dal_vlan8228[vlanId], 0 , sizeof(VTT_VLAN_8228_t));
		dal_vlan8228[vlanId].type = DAL_VLAN_MULTICAST;
		dal_vlan8228[vlanId].member = 0;

		return OPL_OK;
	}
	else
	{
		return OPL_ERROR;
	}

	return OPL_OK;
}



/*******************************************************************************
* dalMulticastVlanDelete
*
* DESCRIPTION:
*  		this function delete a multicast vlan.
*
*	INPUTS:
           vlanId:VLAN ID.
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastVlanDelete(UINT16 vlanId)
{
	int rv;

	if (DAL_VLAN_UNUSED == dal_vlan8228[vlanId].type) 
	{
		return OPL_OK;
	}
	
	if ((DAL_VLAN_MULTICAST != dal_vlan8228[vlanId].type) 
		&& (DAL_VLAN_MC_VTT != dal_vlan8228[vlanId].type))
	{
		return OPL_ERROR;
	}

	rv = isis_vlan_delete(0, vlanId);
	if (SW_OK == rv)
	{
		memset(&dal_vlan8228[vlanId], 0 , sizeof(VTT_VLAN_8228_t));
		dal_vlan8228[vlanId].type = DAL_VLAN_UNUSED;
		dal_vlan8228[vlanId].member = 0;

		return OPL_OK;
	}
	else
	{
		return OPL_ERROR;
	}	
}



/*******************************************************************************
* dalMulticastVlanAddPort
*
* DESCRIPTION:
*  		this function used to add a port to vlan,default is MEMBER_EGRESS_UNTAGGED
*
*	INPUTS:
*			portId:
*			vlanId:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/

//    FAL_EG_UNMODIFIED = 0,  /**<  egress transmit packets unmodified */
//    FAL_EG_UNTAGGED,        /**<   egress transmit packets without vlan tag*/
//    FAL_EG_TAGGED,          /**<  egress transmit packets with vlan tag     */
OPL_STATUS dalMulticastVlanAddPort(UINT8 portId,UINT16 vlanId, UINT32 tag)
{
	fal_vlan_t vlan;
	fal_pbmp_t member;
	fal_pbmp_t u_member = 0;
	int rv;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if ((DAL_VLAN_MULTICAST != dal_vlan8228[vlanId].type)
		&&(DAL_VLAN_MC_VTT != dal_vlan8228[vlanId].type))
	{
		return OPL_ERROR;
	}

	u_member = tag;
	
	if (SW_OK == isis_vlan_find(0, vlanId, &vlan))
	{
		member = vlan.mem_ports;
		member |= (1 << atherosPort);

		rv = isis_vlan_member_add(0, vlanId, atherosPort, u_member);
		if (SW_OK == rv)
		{
			dal_vlan8228[vlanId].member = member;
			dal_vlan8228[vlanId].portId[atherosPort] = 1;
			return OPL_OK;
		}
		else
		{
			return OPL_ERROR;
		}
	}
	else
	{
		return OPL_ERROR;
	}
}



/*******************************************************************************
* dalMulticastVlanDelPort
*
* DESCRIPTION:
*  		this function used to remove a port from a vlan.
*
*	INPUTS:
*			portId:
*			vlanId:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastVlanDelPort(UINT8 portId,UINT16 vlanId)
{
	fal_vlan_t vlan;
	fal_pbmp_t member;
	fal_pbmp_t u_member = 0;
	int rv;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if ((DAL_VLAN_MULTICAST != dal_vlan8228[vlanId].type) 
		&& (DAL_VLAN_MC_VTT != dal_vlan8228[vlanId].type))
	{
		return OPL_ERROR;
	}

	if (SW_OK == isis_vlan_find(0, vlanId, &vlan))
	{
		member = vlan.mem_ports;
		member &= ~(1 << atherosPort);
		
//		rv =  isis_vlan_member_update(0, vlanId, member, u_member);
		if (SW_OK == rv)
		{			
			dal_vlan8228[vlanId].member = member;
			dal_vlan8228[vlanId].portId[atherosPort] = 0;
			return OPL_OK;
		}
		else
		{
			return OPL_ERROR;
		}
	}
	else
	{
		return OPL_ERROR;
	}
}



/*******************************************************************************
* dalMulticastVlanTagStripe
*
* DESCRIPTION:
*  		this function used to disable or enable the egress with or without tag information
*
*	INPUTS:
*			portId:
*			enable:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastVlanTagStripe(UINT8 portId,UINT8 enable)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if (enable)
	{
		return isis_port_egvlanmode_set(0, atherosPort, FAL_EG_UNTAGGED);
	}
	else
	{
		return isis_port_egvlanmode_set(0, atherosPort, FAL_EG_UNMODIFIED);
	}
}

	

/*******************************************************************************
* dalMulticastArlAddPort
*
* DESCRIPTION:
*  		this function used to add a arl records(unicast),or add a port to a multicast records.
*
*	INPUTS:
*			portId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastArlAddPort(UINT8 portId,UINT8 *macAddr)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    return OPL_OK;
}

OPL_STATUS dalMulticastArlAddPortTest(UINT8 portId,UINT8 *macString)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    return OPL_OK;
}



/*******************************************************************************
* dalMulticastArlPortDel
*
* DESCRIPTION:
*  		this function used to remove a arl records(unicast),or remove a port from a multicast group.
*
*	INPUTS:
*			portId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastArlPortDel(UINT8 portId,UINT8 *macAddr)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    return OPL_OK;
}



/*******************************************************************************
* dalMulticastArlVlanAdd
*
* DESCRIPTION:
*  		this function used add a multicast group:port+vlan+mac
*
*	INPUTS:
*			portId:
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
OPL_STATUS dalMulticastArlVlanAdd(UINT8 portId,UINT16 vlanId,UINT8 *macAddr)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    return OPL_OK;
}



/*******************************************************************************
* dalMulticastArlVlanDel
*
* DESCRIPTION:
*  		this function used remove a multicast group:port+vlan+mac
*
*	INPUTS:
*			portId:
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
OPL_STATUS dalMulticastArlVlanDel(UINT8 portId,UINT16 vlanId,UINT8 *macAddr)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    return OPL_OK;
}



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
OPL_STATUS dalMulticastArlTabFlush(void)
{
    return OPL_OK;
}

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
OPL_STATUS dalMulticastPortMacAdd(UINT8 portNum, UINT8 *macAddr)
{
    INT32 index;
    OPL_STATUS retVal = OPL_OK;

    if(OPL_NULL == macAddr)
    {
        return OPL_ERR_NULL_POINTER;
    }
    
    OPL_DAL_PRINTF(("dalMulticastPortMacAdd Input Parameters: portNum=%d macAddr=%02x%02x%02x%02x%02x%02x.\n", 
        portNum, macAddr[0], macAddr[1], 
        macAddr[2], macAddr[3], macAddr[4], macAddr[5]));

	index = portNum;

	return dalArlMultiPortMacAdd(1, &portNum, macAddr, 0);

#if 0
    for (index = 0; index < MC_MAX_GROUP_NUM; index++)
    {
        /* 01:00:5e:xx:xx:xx */
        if ((!OPL_MEMCMP(groupAddrClsIndexMapTab[index].mac, macAddr, MAC_LENGTH)) 
            && (groupAddrClsIndexMapTab[index].valid == OPL_TRUE))
        {
            OPL_DAL_PRINTF(("%s[%d]: Multicast Classify Rule Exist.\n", __FUNCTION__, __LINE__));
            return OPL_OK;
        }
    }

    for (index = 0; index < MC_MAX_GROUP_NUM; index++)
    {
        if (groupAddrClsIndexMapTab[index].valid != OPL_TRUE)
        {
            CLS_CONFIG_INFO_t stClassCfg;
            UINT16 ruleId;
            
            OPL_MEMSET((char *)&stClassCfg, 0, sizeof(stClassCfg));

            /* Valid */
            stClassCfg.valid = 1;

            /* Action */
            stClassCfg.t_act = DONOT_DROP;

            /* SRC Port, not used for single port mode */
#if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)
            stClassCfg.ports[0] = portNum;
#endif

            /* GDA MAC, 01:00:5e:xx:xx:xx */
            stClassCfg.dstMacFlag = 1;
            stClassCfg.dstMac.lowRange[0] = macAddr[0];
            stClassCfg.dstMac.lowRange[1] = macAddr[1];
            stClassCfg.dstMac.lowRange[2] = macAddr[2];
            stClassCfg.dstMac.lowRange[3] = macAddr[3];
            stClassCfg.dstMac.lowRange[4] = macAddr[4];
            stClassCfg.dstMac.lowRange[5] = macAddr[5];
            stClassCfg.dstMac.highRange[0] = 0xff;
            stClassCfg.dstMac.highRange[1] = 0xff;
            stClassCfg.dstMac.highRange[2] = 0xff;
            stClassCfg.dstMac.highRange[3] = 0xff;
            stClassCfg.dstMac.highRange[4] = 0xff;
            stClassCfg.dstMac.highRange[5] = 0xff;
            
            /* Type or Length, 0x0800 */
            stClassCfg.lenOrTypeFlag = 1;
            stClassCfg.lenOrType.lowRange = 0x0800;
            stClassCfg.lenOrType.highRange = 0xffff;

            /* IP Type, UDP */
            stClassCfg.ipTypeFlag = 1;
            stClassCfg.ipType.lowRange = 17;
            stClassCfg.ipType.highRange = 0xff;

            /* Do not change COS value */
            stClassCfg.cosMapVal = CTC_ETHER_PRI_END;
            stClassCfg.queueMapId = CTC_QUEUE_MAP_END;

#if 0
            stClassCfg.bingress=0x01;
            stClassCfg.ingressmask=0x01;
#endif
      
            retVal = dalVoipClsRuleCtcAdd(&stClassCfg, &ruleId);
            if (retVal != OPL_OK)
            {
                OPL_MEMSET(&groupAddrClsIndexMapTab[index], 0x0, sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));
                OPL_DAL_PRINTF(("%s[%d]: dalVoipClsRuleCtcAdd Fail, retVal=%d.\n", __FUNCTION__, __LINE__, retVal));
                return retVal;
            }
            
            groupAddrClsIndexMapTab[index].valid = OPL_TRUE;
            groupAddrClsIndexMapTab[index].clsIndex = ruleId;
            OPL_MEMCPY(groupAddrClsIndexMapTab[index].mac,macAddr,MAC_LENGTH);
                        
            OPL_DAL_PRINTF(("Multicast Classify Rule Added: valid=%d index=%d macAddr=%02x%02x%02x%02x%02x%02x\n", 
                groupAddrClsIndexMapTab[index].valid, groupAddrClsIndexMapTab[index].clsIndex, 
                groupAddrClsIndexMapTab[index].mac[0], groupAddrClsIndexMapTab[index].mac[1], 
                groupAddrClsIndexMapTab[index].mac[2], groupAddrClsIndexMapTab[index].mac[3], 
                groupAddrClsIndexMapTab[index].mac[4], groupAddrClsIndexMapTab[index].mac[5]));

            return OPL_OK;
        }
    }
    
    if (index == MC_MAX_GROUP_NUM)
    {
        OPL_DAL_PRINTF(("multicast address table is full.\n"));
    }
    #endif
    return OPL_OK;
}

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
OPL_STATUS dalMulticastPortMacDel(UINT8 portNum, UINT8 *macAddr)
{
    INT32 index;
    OPL_STATUS retVal = OPL_OK;
    
    if (OPL_NULL == macAddr)
    {
        return OPL_ERR_NULL_POINTER;
    }
    
    OPL_DAL_PRINTF(("dalMulticastPortMacDel Input Parameters: portNum=%d macAddr=%02x%02x%02x%02x%02x%02x.\n", 
        portNum, macAddr[0], macAddr[1], 
        macAddr[2], macAddr[3], macAddr[4], macAddr[5]));

	index = portNum;
	
	return dalArlMultiPortMacDel(1, &index, macAddr, 0);

#if 0
    for (index = 0; index < MC_MAX_GROUP_NUM; index++)
    {
    	if ((!OPL_MEMCMP(groupAddrClsIndexMapTab[index].mac, macAddr, MAC_LENGTH)) 
            && (groupAddrClsIndexMapTab[index].valid == OPL_TRUE))
        {
            retVal = dalVoipClsDelEntry(groupAddrClsIndexMapTab[index].clsIndex);
            if (retVal != OPL_OK)
            {
                OPL_DAL_PRINTF(("%s[%d]: dalVoipClsDelEntry Fail, retVal=%d.\n", __FUNCTION__, __LINE__, retVal));
                return retVal;
            }        

            OPL_MEMSET(&groupAddrClsIndexMapTab[index], 0x0, sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));
            return OPL_OK;
        }
    }

    OPL_DAL_PRINTF(("%s[%d]: Multicast Classify Rule Not Exist.\n", __FUNCTION__, __LINE__));
#endif
    return OPL_OK;
}

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
OPL_STATUS dalMulticastPortVlanMacAdd(UINT8 portNum, UINT16 vlanId, UINT8 *macAddr, OPL_BOOL fuzzy)
{
    INT32 index;
    OPL_STATUS retVal = OPL_OK;
    CLS_CONFIG_INFO_t stClassCfg;
    UINT16 ruleId;
    
    if(OPL_NULL == macAddr)
    {
        return OPL_ERR_NULL_POINTER;
    }
    
    OPL_DAL_PRINTF(("dalMulticastPortVlanMacAdd Input Parameters: portNum=%d vlanId=%d macAddr=%02x%02x%02x%02x%02x%02x.\n", 
        portNum, vlanId, macAddr[0], macAddr[1], 
        macAddr[2], macAddr[3], macAddr[4], macAddr[5]));

	index = portNum;
	//dalArlMultiPortMacAdd(1, &index, macAddr, vlanId);

#if 1
    for (index = 0; index < MC_MAX_GROUP_NUM; index++)
    {
        /* 01:00:5e:xx:xx:xx */
        if(((fuzzy && (!OPL_MEMCMP(groupAddrClsIndexMapTab[index].mac, macAddr, MAC_LENGTH/2))) 
            || (!fuzzy && (!OPL_MEMCMP(groupAddrClsIndexMapTab[index].mac, macAddr, MAC_LENGTH))))
            && (groupAddrClsIndexMapTab[index].vid == vlanId) 
            && (groupAddrClsIndexMapTab[index].valid == OPL_TRUE))
        {
            OPL_DAL_PRINTF(("%s[%d]: Multicast Classify Rule Exist.\n", __FUNCTION__, __LINE__));
            return OPL_OK;
        }
    }

    for (index = 0; index < MC_MAX_GROUP_NUM; index++)
    {
        if (groupAddrClsIndexMapTab[index].valid != OPL_TRUE)
        {
            OPL_MEMSET((char *)&stClassCfg, 0, sizeof(stClassCfg));

            /* Valid */
            stClassCfg.valid = 1;

            /* Action */
            stClassCfg.t_act = DONOT_DROP;

            /* SRC Port, not used for single port mode */
#if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)
            stClassCfg.ports[0] = portNum;
#endif

            /* GDA MAC, 01:00:5e:xx:xx:xx */
            stClassCfg.dstMacFlag = 1;
            stClassCfg.dstMac.lowRange[0] = macAddr[0];
            stClassCfg.dstMac.lowRange[1] = macAddr[1];
            stClassCfg.dstMac.lowRange[2] = macAddr[2];
            stClassCfg.dstMac.lowRange[3] = macAddr[3];
            stClassCfg.dstMac.lowRange[4] = macAddr[4];
            stClassCfg.dstMac.lowRange[5] = macAddr[5];
            stClassCfg.dstMac.highRange[0] = 0xff;
            stClassCfg.dstMac.highRange[1] = 0xff;
            stClassCfg.dstMac.highRange[2] = 0xff;
            if (fuzzy)
            {
                stClassCfg.dstMac.highRange[3] = 0x0;
                stClassCfg.dstMac.highRange[4] = 0x0;
                stClassCfg.dstMac.highRange[5] = 0x0;
            }
            else
            {
                stClassCfg.dstMac.highRange[3] = 0xff;
                stClassCfg.dstMac.highRange[4] = 0xff;
                stClassCfg.dstMac.highRange[5] = 0xff;
            }
            
            /* VID */
            stClassCfg.vlanIdFlag = 1;
            stClassCfg.vlanId.lowRange = vlanId;
            stClassCfg.vlanId.highRange = 0xffff;

            /* Type or Length, 0x0800 */
            stClassCfg.lenOrTypeFlag = 1;
            stClassCfg.lenOrType.lowRange = 0x0800;
            stClassCfg.lenOrType.highRange = 0xffff;

            /* IP Type, UDP */
            stClassCfg.ipTypeFlag = 1;
            stClassCfg.ipType.lowRange = 17;
            stClassCfg.ipType.highRange = 0xff;
			
            /* Do not change COS value */
            stClassCfg.cosMapVal = CTC_ETHER_PRI_END;
            stClassCfg.queueMapId = CTC_QUEUE_MAP_END;
		
#if 0
            stClassCfg.bingress=0x01;
            stClassCfg.ingressmask=0x01;
#endif
      
            retVal = dalVoipClsRuleCtcAdd(&stClassCfg, &ruleId);
            if (retVal != OPL_OK)
            {
                OPL_MEMSET(&groupAddrClsIndexMapTab[index], 0x0, sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));
                OPL_DAL_PRINTF(("%s[%d]: dalVoipClsRuleCtcAdd Fail, retVal=%d.\n", __FUNCTION__, __LINE__, retVal));
                return retVal;
            }
            
            groupAddrClsIndexMapTab[index].valid = OPL_TRUE;
            groupAddrClsIndexMapTab[index].clsIndex = ruleId;
            OPL_MEMCPY(groupAddrClsIndexMapTab[index].mac,macAddr,MAC_LENGTH);
            groupAddrClsIndexMapTab[index].vid = vlanId;
                        
            OPL_DAL_PRINTF(("Multicast Classify Rule Added: valid=%d index=%d macAddr=%02x%02x%02x%02x%02x%02x vlanId=%d\n", 
                groupAddrClsIndexMapTab[index].valid, groupAddrClsIndexMapTab[index].clsIndex, 
                groupAddrClsIndexMapTab[index].mac[0], groupAddrClsIndexMapTab[index].mac[1], 
                groupAddrClsIndexMapTab[index].mac[2], groupAddrClsIndexMapTab[index].mac[3], 
                groupAddrClsIndexMapTab[index].mac[4], groupAddrClsIndexMapTab[index].mac[5], 
                groupAddrClsIndexMapTab[index].vid));

            return OPL_OK;
        }
    }
    
    if (index == MC_MAX_GROUP_NUM)
    {
        OPL_DAL_PRINTF(("multicast address table is full.\n"));
    }
#endif    
    return OPL_OK;
}

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
OPL_STATUS dalMulticastPortVlanMacDel(UINT8 portNum, UINT16 vlanId, UINT8 *macAddr, OPL_BOOL fuzzy)
{
    INT32 index;
    OPL_STATUS retVal = OPL_OK;
    
    if (OPL_NULL == macAddr)
    {
        return OPL_ERR_NULL_POINTER;
    }
    
    OPL_DAL_PRINTF(("dalMulticastPortVlanMacDel Input Parameters: portNum=%d vlanId=%d macAddr=%02x%02x%02x%02x%02x%02x.\n", 
        portNum, vlanId, macAddr[0], macAddr[1], 
        macAddr[2], macAddr[3], macAddr[4], macAddr[5]));

	index = portNum;
	
	//dalArlMultiPortMacDel(1, &index, macAddr, 0);
	
#if 1
    for (index = 0; index < MC_MAX_GROUP_NUM; index++)
    {
    	if (((fuzzy && (!OPL_MEMCMP(groupAddrClsIndexMapTab[index].mac, macAddr, MAC_LENGTH/2))) 
            || (!fuzzy && (!OPL_MEMCMP(groupAddrClsIndexMapTab[index].mac, macAddr, MAC_LENGTH))))
            && (groupAddrClsIndexMapTab[index].vid == vlanId) 
            && (groupAddrClsIndexMapTab[index].valid == OPL_TRUE))
        {
            retVal = dalVoipClsDelEntry(groupAddrClsIndexMapTab[index].clsIndex);
            if (retVal != OPL_OK)
            {
                OPL_PRINTF(("%s[%d]: dalVoipClsDelEntry Fail, retVal=%d.\n", __FUNCTION__, __LINE__, retVal));
                return retVal;
            }        

            OPL_MEMSET(&groupAddrClsIndexMapTab[index], 0x0, sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));
            return OPL_OK;
        }
    }

    OPL_DAL_PRINTF(("%s[%d]: Multicast Classify Rule Not Exist.\n", __FUNCTION__, __LINE__));
#endif
    return OPL_OK;
}

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
OPL_STATUS dalMulticastRuleDelAll(void)
{
    INT32 index;
    OPL_STATUS retVal = OPL_OK;
    
    for (index = 0; index < MC_MAX_GROUP_NUM; index++)
    {
    	if (groupAddrClsIndexMapTab[index].valid == OPL_TRUE)
        {
            retVal = dalVoipClsDelEntry(groupAddrClsIndexMapTab[index].clsIndex);
            if (retVal != OPL_OK)
            {
                OPL_DAL_PRINTF(("%s[%d]: dalVoipClsDelEntry Fail, retVal=%d.\n", __FUNCTION__, __LINE__, retVal));
                return retVal;
            }  

            OPL_MEMSET(&groupAddrClsIndexMapTab[index], 0x0, sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));
        }
    }

    OPL_DAL_PRINTF(("%s[%d]: Multicast Classify Rule Del All.\n", __FUNCTION__, __LINE__));

    return OPL_OK;
}

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
OPL_STATUS dalMulticastDataFilterAdd(void)
{
    INT32 index;
    OPL_STATUS retVal = OPL_OK;
    CLS_CONFIG_INFO_t stClassCfg;

    if (mcDataFilterRuleId != 0xFFFF)
    {
        OPL_DAL_PRINTF(("dalMulticastDataFilterAdd, already exist.\r\n"));
        return OPL_OK;
    }
    
    OPL_MEMSET((char *)&stClassCfg, 0, sizeof(stClassCfg));
    
    /* Valid */
    stClassCfg.valid = 1;

    /* Action */
    stClassCfg.t_act = DROP_PKTS;

    /* GDA MAC, 01:00:5e:xx:xx:xx */
    stClassCfg.dstMacFlag = 1;
    stClassCfg.dstMac.lowRange[0] = 0x01;
    stClassCfg.dstMac.lowRange[1] = 0x00;
    stClassCfg.dstMac.lowRange[2] = 0x5e;
    stClassCfg.dstMac.lowRange[3] = 0x00;
    stClassCfg.dstMac.lowRange[4] = 0x00;
    stClassCfg.dstMac.lowRange[5] = 0x00;
    stClassCfg.dstMac.highRange[0] = 0xff;
    stClassCfg.dstMac.highRange[1] = 0xff;
    stClassCfg.dstMac.highRange[2] = 0xff;
    stClassCfg.dstMac.highRange[3] = 0x0;
    stClassCfg.dstMac.highRange[4] = 0x0;
    stClassCfg.dstMac.highRange[5] = 0x0;

    /* Type or Length, 0x0800 */
    stClassCfg.lenOrTypeFlag = 1;
    stClassCfg.lenOrType.lowRange = 0x0800;
    stClassCfg.lenOrType.highRange = 0xffff;

    /* IP Type, UDP */
    stClassCfg.ipTypeFlag = 1;
    stClassCfg.ipType.lowRange = 17;
    stClassCfg.ipType.highRange = 0xff;
	
    /* Do not change COS value */
    stClassCfg.cosMapVal = CTC_ETHER_PRI_END;
    stClassCfg.queueMapId = CTC_QUEUE_MAP_END;
	
    retVal = dalVoipClsRuleCtcAdd(&stClassCfg, &mcDataFilterRuleId);
    if (retVal != OPL_OK)
    {
        OPL_DAL_PRINTF(("%s[%d]: dalVoipClsRuleCtcAdd Fail, retVal=%d.\n", __FUNCTION__, __LINE__, retVal));
        return retVal;
    }            
    
    OPL_DAL_PRINTF(("dalMulticastDataFilterAdd succeed, mcDataFilterRuleId=0x%x.\r\n", mcDataFilterRuleId));
    
    return OPL_OK;
}

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
OPL_STATUS dalMulticastDataFilterDel(void)
{
    INT32 index;
    OPL_STATUS retVal = OPL_OK;

    if (mcDataFilterRuleId == 0xFFFF)
    {
        OPL_DAL_PRINTF(("dalMulticastDataFilterDel, not exist.\r\n"));
        return OPL_OK;
    }

    retVal = dalVoipClsDelEntry(mcDataFilterRuleId);
    if (retVal != OPL_OK)
    {
        OPL_DAL_PRINTF(("%s[%d]: dalVoipClsDelEntry Fail, retVal=%d.\n", __FUNCTION__, __LINE__, retVal));
        return retVal;
    }

    OPL_DAL_PRINTF(("dalMulticastDataFilterDel succeed, mcDataFilterRuleId=0x%x.\r\n", mcDataFilterRuleId));

    mcDataFilterRuleId = 0xFFFF;
    
    return OPL_OK;
}

