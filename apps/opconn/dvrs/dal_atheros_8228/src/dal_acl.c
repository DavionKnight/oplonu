/*
=============================================================================
     Header Name: dal_acl.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                		Date              	Description of Changes
----------------   ------------  ----------------------------------------------
 zran 				2009/06/25		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#include <dal_lib.h>
#include <odm_port.h>

#include <dal_acl.h>
#include <classify.h>

DAL_ACL_INFO_s dalAclPool[DAL_ACL_TOTAL_NUM];	/* at most DAL_ACL_TOTAL_NUM entries */

UINT8	igmpListId;
UINT8	RstplistId;

extern shiva_acl_list_t *list_ent[];
	
OPL_STATUS dalConvertClsParam(CLS_CONFIG_INFO_t *stClassCfg,fal_acl_rule_t * rule)
{
		UINT8	i;

		if ((stClassCfg == NULL)||(rule == NULL))
		{
			return OPL_ERROR;
		}

		rule->rule_type = FAL_ACL_RULE_MAC;
	
		if (stClassCfg->dstMacFlag)
		{
			for (i = 0; i < 6; i++) {
				rule->dest_mac_val.uc[i] = stClassCfg->dstMac.lowRange[i];
				rule->dest_mac_mask.uc[i] = 0xFF;
			}		
			FAL_FIELD_FLG_SET(rule->field_flg,FAL_ACL_FIELD_MAC_DA);
		}
	
		if (stClassCfg->srcMacFlag)
		{
			for (i = 0; i < 6; i++) {
				rule->src_mac_val.uc[i] = stClassCfg->srcMac.lowRange[i];
				rule->src_mac_mask.uc[i] = 0xFF;
			}		
			FAL_FIELD_FLG_SET(rule->field_flg,FAL_ACL_FIELD_MAC_SA);
		}
	
		if (stClassCfg->etherPriFlag)
		{
			rule->up_val = stClassCfg->etherPri.lowRange;
			rule->up_mask = 0xFF;
			FAL_FIELD_FLG_SET(rule->field_flg,FAL_ACL_FIELD_MAC_UP);
		}
	
		if (stClassCfg->vlanIdFlag)
		{
			rule->vid_val = stClassCfg->vlanId.lowRange;
			rule->vid_mask = 0xFFF;
			FAL_FIELD_FLG_SET(rule->field_flg,FAL_ACL_FIELD_MAC_VID);
		}
	
		if (stClassCfg->lenOrTypeFlag)
		{
			rule->ethtype_val = stClassCfg->lenOrType.lowRange;
			rule->ethtype_mask = 0xFFFF;
			FAL_FIELD_FLG_SET(rule->field_flg,FAL_ACL_FIELD_MAC_ETHTYPE);
		}
	
		if (stClassCfg->dstIpFlag)
		{
			rule->dest_ip4_val = stClassCfg->dstIp.lowRange;
			rule->dest_ip4_mask = 0xFFFFFFFF;
			rule->rule_type = FAL_ACL_RULE_IP4;
			FAL_FIELD_FLG_SET(rule->field_flg,FAL_ACL_FIELD_IP4_DIP);
		}
	
		if (stClassCfg->srcIpFlag)
		{
			rule->src_ip4_val = stClassCfg->srcIp.lowRange;
			rule->src_ip4_mask = 0xFFFFFFFF;
			rule->rule_type = FAL_ACL_RULE_IP4;
			FAL_FIELD_FLG_SET(rule->field_flg,FAL_ACL_FIELD_IP4_SIP);
		}
	
		if (stClassCfg->ipTypeFlag)
		{
			rule->ip_proto_val = stClassCfg->ipType.lowRange;
			rule->ip_proto_mask = 0xFF;
			rule->rule_type = FAL_ACL_RULE_IP4;
			FAL_FIELD_FLG_SET(rule->field_flg,FAL_ACL_FIELD_IP_PROTO);
		}
	
		if (stClassCfg->ipV4DscpFlag)
		{
			rule->ip_dscp_val = stClassCfg->ipV4Dscp.lowRange;
			rule->ip_dscp_mask = 0xFF;
			rule->rule_type = FAL_ACL_RULE_IP4;
			FAL_FIELD_FLG_SET(rule->field_flg,FAL_ACL_FIELD_IP_DSCP);
		}

		/* use IP DSCP to support IP Precedence */
		if (stClassCfg->ipV6PreFlag)
		{
			rule->ip_dscp_val = stClassCfg->ipV6Pre.lowRange;
			rule->ip_dscp_mask = 0xFF;
			rule->rule_type = FAL_ACL_RULE_IP4;
			FAL_FIELD_FLG_SET(rule->field_flg,FAL_ACL_FIELD_IP_DSCP);
		}
	
		if (stClassCfg->srcL4PortFlag)
		{
			rule->src_l4port_val = stClassCfg->srcL4Port.lowRange;
			rule->src_l4port_mask = 0xFFFF;
			rule->rule_type = FAL_ACL_RULE_IP4;
			FAL_FIELD_FLG_SET(rule->field_flg,FAL_ACL_FIELD_L4_SPORT);
		}
	
		if (stClassCfg->dstL4PortFlag)
		{
			rule->dest_l4port_val = stClassCfg->dstL4Port.lowRange;
			rule->dest_l4port_mask = 0xFFFF;
			rule->rule_type = FAL_ACL_RULE_IP4;
			FAL_FIELD_FLG_SET(rule->field_flg,FAL_ACL_FIELD_L4_DPORT);
		}
		
		if (stClassCfg->rmkvidOp==RMK_CHANGE_VID)
		{
			rule->vid = stClassCfg->rmkvid;
			rule->vid_val = 0;
			rule->vid_mask = 0xFFF;
			rule->vid_op = 1;
			FAL_FIELD_FLG_SET(rule->field_flg,FAL_ACL_FIELD_MAC_VID);
			rule->ports = 0x40;
			FAL_ACTION_FLG_SET(rule->action_flg, FAL_ACL_ACTION_REDPT);	
			FAL_ACTION_FLG_SET(rule->action_flg,FAL_ACL_ACTION_MODIFY_VLAN);	
		}

	
		/* four queues per port */
		rule->queue = stClassCfg->queueMapId;
		FAL_ACTION_FLG_SET(rule->action_flg, FAL_ACL_ACTION_REMARK_QUEUE);
	
		/* change user priority */
		rule->up = stClassCfg->cosMapVal;
		FAL_ACTION_FLG_SET(rule->action_flg, FAL_ACL_ACTION_REMARK_UP);
	
		FAL_ACTION_FLG_SET(rule->action_flg, FAL_ACL_ACTION_PERMIT);
		if (stClassCfg->t_act & DROP_PKTS)
		{
			if (stClassCfg->t_act & COPY_PACKETS_TO_CPU) 
			{
				/* redirect to CPU */
				FAL_ACTION_FLG_SET(rule->action_flg, FAL_ACL_ACTION_RDTCPU);
			}
			else
			{
				/* drop */
				FAL_ACTION_FLG_SET(rule->action_flg, FAL_ACL_ACTION_DENY);
			}
		}
		else
		{
			/* pass and copy */
			if (stClassCfg->t_act & COPY_PKTS_TO_CPU)
			{
				FAL_ACTION_FLG_SET(rule->action_flg, FAL_ACL_ACTION_CPYCPU);
			}
	
			/* pass through */
			if (stClassCfg->t_act & DONOT_COPY_TO_CPU) 
			{
				rule->ports = 0x40;
				FAL_ACTION_FLG_SET(rule->action_flg, FAL_ACL_ACTION_REDPT);			
			}		
		}
		return OPL_OK;

}

OPL_STATUS dalAclClsFindName(CLS_CONFIG_INFO_t *stClassCfg,UINT8 * name)
{
	UINT32	i;
	fal_acl_rule_t	rule;
	UINT8 	portListBuff[255];
	UINT32 	portArry[ODM_NUM_OF_PORTS+ 1];
	UINT8 	tmpPortMap;
	
	if ((stClassCfg == NULL)||(name == NULL))
    {
		return OPL_ERROR;
    }
	vosMemSet(&rule, 0, sizeof (fal_acl_rule_t));
	dalConvertClsParam(stClassCfg,&rule);
	
	vosStrCpy(portListBuff,stClassCfg->ports);
	vosMemSet(portArry, 0, sizeof(UINT32)*(ODM_NUM_OF_PORTS+ 1));
	convertPortListString2PortArry(portListBuff,portArry,ODM_NUM_OF_PORTS+1);	
	tmpPortMap = 0;
	for(i = 1; i < DAL_ACL_UPLINK_PORT; i++)
	{
		if (portArry[i])
		{			
			tmpPortMap |= (1 << dalToAtherosPortMap[i]);
		}
	}

	for (i = 0; i < DAL_ACL_TOTAL_NUM; i++)
	{
		if ( (DAL_ACL_CLASSIFICATION == dalAclPool[i].type)
			&& (0 == vosMemCmp(&(dalAclPool[i].rule), &rule,sizeof(rule)))
			&&(tmpPortMap==dalAclPool[i].port)
			)
		{			
			vosStrCpy(name,dalAclPool[i].name);
			return OPL_OK;
		}	
		continue;
	}
	return OPL_ERROR;
}

OPL_STATUS dalAclFreeRsc(UINT32 *hsl_f_rsc)
{
	UINT32 dev_id = 0;
	sw_error_t rv;

	*hsl_f_rsc = 0;
	
	hsl_acl_free_rsc_get(dev_id, hsl_f_rsc);

	return OPL_OK;
}



OPL_STATUS dalAclFindFreeAclList(UINT8 * listId)
{
	UINT32	i, j;
	UINT8	k[DAL_ACL_TOTAL_NUM];

	for (i = 0; i < DAL_ACL_TOTAL_NUM; i++)
	{	
		k[i] = 0;
	}
	
	for (i = 0; i < DAL_ACL_TOTAL_NUM; i++)
	{
		if (DAL_ACL_UNUSED != dalAclPool[i].type)
		{
			for (j = 0; j < DAL_ACL_NUM_PER_PORT; j++)
			{
				if (DAL_ACL_INVALID_LIST_ID != dalAclPool[i].aclIdUs[j])
				{
					/* mask off the bit of this acl list id */
					k[dalAclPool[i].aclIdUs[j]] = 1;
				}

				if (DAL_ACL_INVALID_LIST_ID != dalAclPool[i].aclIdDs[j])
				{
					/* mask off the bit of this acl list id */
					k[dalAclPool[i].aclIdDs[j]] = 1;
				}
			}
		}		
	}

	for (i = 0; i < DAL_ACL_TOTAL_NUM; i++)
	{
		if (0 == k[i])
		{
			*listId = i;
			return OPL_OK;
		}
	}

	return OPL_ERROR;
}



OPL_STATUS dalAclFindFreeAclPool(UINT16	* index)
{
	UINT8 i;

	for (i = 0; i < DAL_ACL_TOTAL_NUM; i++)
	{
		if (DAL_ACL_UNUSED == dalAclPool[i].type)
		{
			*index = i;
			return OPL_OK;
		}
	}

	return OPL_ERROR;
}



OPL_STATUS dalAclFind1to1VT(UINT32 portId, UINT32 cVid, UINT32 sVid, UINT32 * index)
{
	UINT32	i, j;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
	for (i = 0; i < DAL_ACL_TOTAL_NUM; i++)
	{
		if ( (DAL_ACL_1TO1_VT != dalAclPool[i].type)
			|| (dalAclPool[i].port != atherosPort))
		{
			continue;
		}

		for (j = 0; j < DAL_ACL_NUM_PER_PORT; j++)
		{
			if ((dalAclPool[i].fromVid[j] != cVid)
				|| (dalAclPool[i].toVid[j] != sVid))
			{
				continue;
			}

			/* to compatible back with Opconn VTT index */
			*index = i * DAL_ACL_NUM_PER_PORT + j;

			return OPL_OK;
		}		
	}

	return OPL_ERROR;
}



OPL_STATUS dalAclFindNto1MacVT(UINT32 portId, UINT32 cVid, UINT32 sVid, UINT8 * mac, UINT32 * index)
{
	UINT32	i, j;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
	for (i = 0; i < DAL_ACL_TOTAL_NUM; i++)
	{
		if ( (DAL_ACL_NTO1_VA_MAC != dalAclPool[i].type)
			|| (dalAclPool[i].port != atherosPort))
		{
			continue;
		}

		for (j = 0; j < DAL_ACL_NUM_PER_PORT; j++)
		{
			if ((dalAclPool[i].fromVid[j] != cVid)
				|| (dalAclPool[i].toVid[j] != sVid)
				|| !DAL_ACL_MAC_SAME(&(dalAclPool[i].mac[j*6]),mac))
			{
				continue;
			}

			/* to compatible back with Opconn VTT index */
			*index = i * DAL_ACL_NUM_PER_PORT + j;

			return OPL_OK;
		}		
	}

	return OPL_ERROR;
}



OPL_STATUS dalAclFindCls(UINT8 * name, UINT16 * index)
{
	UINT32	i, j;
	
	for (i = 0; i < DAL_ACL_TOTAL_NUM; i++)
	{
		if ( (DAL_ACL_CLASSIFICATION != dalAclPool[i].type)
			|| (0 != vosStrCmp(dalAclPool[i].name, name)))
		{
			continue;
		}

		*index = i;
		return OPL_OK;
	}

	return OPL_ERROR;
}

/* find dalAclPool index */
OPL_STATUS dalAclFind1to1VtPort(UINT32 portId, UINT16	 * loc)
{
	UINT8	i;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
	for (i = 0; i < DAL_ACL_TOTAL_NUM; i++)
	{
		if ( (dalAclPool[i].type != DAL_ACL_1TO1_VT)
			|| (dalAclPool[i].port != atherosPort))
		{
			continue;
		}

		*loc = i;

		return OPL_OK;
	}

	return OPL_ERROR;
}



/* find dalAclPool index */
OPL_STATUS dalAclFindNto1VtPort(UINT32 portId, UINT32 type, UINT16 * loc)
{
	UINT8	i;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
	for (i = 0; i < DAL_ACL_TOTAL_NUM; i++)
	{
		if ( (dalAclPool[i].type != type)
			|| (dalAclPool[i].port != atherosPort))
		{
			continue;
		}

		*loc = i;

		return OPL_OK;
	}

	return OPL_ERROR;
}



OPL_STATUS dalAcl1to1VtGet(UINT32 portId, UINT32 *cVlan, UINT32 *sVlan, UINT32 index)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if ((DAL_ACL_1TO1_VT != dalAclPool[index/DAL_ACL_NUM_PER_PORT].type)
		|| (atherosPort != dalAclPool[index/DAL_ACL_NUM_PER_PORT].port))
	{
		return OPL_ERROR;
	}

	*cVlan = dalAclPool[index/DAL_ACL_NUM_PER_PORT].fromVid[index % DAL_ACL_NUM_PER_PORT];
	*sVlan = dalAclPool[index/DAL_ACL_NUM_PER_PORT].toVid[index % DAL_ACL_NUM_PER_PORT];

	return OPL_OK;
}



OPL_STATUS dalAcl1to1VtAdd(UINT32 portId, UINT32 cVlan, UINT32 sVlan, UINT32 * index)
{
	UINT32	i;
	UINT32  hsl_f_rsc = 0;
	UINT8	listId, usListid;
	UINT16	loc = 0;;
	int		rv;
	fal_acl_rule_t  rule;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
 	if (OPL_OK == dalAclFind1to1VT(portId, cVlan, sVlan, index))
	{
		return OPL_OK;
	}

	/* one VT entry needs two ACL rules */
	dalAclFreeRsc(&hsl_f_rsc);	
	if ( hsl_f_rsc < 3)
	{
		return DAL_VTT_ACL_RULE_FULL;
	}
	
	if (OPL_OK == dalAclFind1to1VtPort(portId, &loc))
	{
		/* other VT entries exist on this port */

		/* find free location of aclIdus to contain new ACL list */
		for (i = 0; i < DAL_ACL_NUM_PER_PORT; i++)
		{
			if (DAL_ACL_INVALID_LIST_ID == dalAclPool[loc].aclIdUs[i])
			{
				break;
			}
		}

		if (i >= DAL_ACL_NUM_PER_PORT)
		{
			return DAL_VTT_ACL_UPLIST_ERR;
		}		
	}
	else
	{
		/* to add first ACL list on this port */
		if (OPL_OK != dalAclFindFreeAclPool(&loc))
		{
			return DAL_VTT_ACL_POOL_FULL;
		}

		/* first ACL list for this port */
		i = 0;
	}

	/* create upstream ACL list first */
	if (OPL_OK != dalAclFindFreeAclList(&listId))
	{
		return DAL_VTT_ACL_LIST_FULL;
	}

	/* 1:1 VT ACL list priority is always 0*/
	rv = shiva_acl_list_creat(0, listId, 0);
	if (SW_OK != rv)
	{
		return rv;
	}

	/* add ACL rule for ACL list */
    vosMemSet(&rule, 0, sizeof (fal_acl_rule_t));		
	rule.rule_type = FAL_ACL_RULE_MAC;
	rule.vid_val = cVlan;
	rule.vid_mask = 0xFFF;
	rule.vid_op = FAL_ACL_FIELD_MASK;
	FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_MAC_VID);
	FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_MODIFY_VLAN);
	rule.vid = sVlan;

	/* one ACL list contains only one ACL rule, so ruleId is always 0 */
	rv = shiva_acl_rule_add(0, listId, 0, 1, &rule);
	if (SW_OK != rv)
	{
		shiva_acl_list_destroy(0, listId);
		return rv;
	}


	/* bind ACL list to portId */
	rv = shiva_acl_list_bind(0, listId, 0, 0, atherosPort);
	if (SW_OK != rv)
	{
		shiva_acl_rule_delete(0, listId, 0, 1);
		shiva_acl_list_destroy(0, listId);
		return rv;
	}		

	dalAclPool[loc].type = DAL_ACL_1TO1_VT;	
	dalAclPool[loc].aclIdUs[i] = listId;
	dalAclPool[loc].fromVid[i] = cVlan;
	dalAclPool[loc].toVid[i] = sVlan;	
	dalAclPool[loc].port = atherosPort;
	usListid = listId;

	/* create downstream ACL list first  on uplink port */
	if (OPL_OK != dalAclFindFreeAclList(&listId))
	{
		return DAL_VTT_ACL_LIST_FULL;
	}

	/* 1:1 VT ACL list priority is always 0*/
	rv = shiva_acl_list_creat(0, listId, 0);
	if (SW_OK != rv)
	{
		/* delete upstream acl list */
		shiva_acl_rule_delete(0, usListid, 0, 1);
		shiva_acl_list_unbind(0, usListid, 0, 0, atherosPort);
		shiva_acl_list_destroy(0, usListid);

		dalAclPool[loc].type = DAL_ACL_UNUSED; 
		dalAclPool[loc].aclIdUs[i] = DAL_ACL_INVALID_LIST_ID;
		dalAclPool[loc].fromVid[i] = 0;
		dalAclPool[loc].toVid[i] = 0;	
		dalAclPool[loc].port = 0;
		
		return rv;
	}

	/* add ACL rule for ACL list */
    vosMemSet(&rule, 0, sizeof (fal_acl_rule_t));		
	rule.rule_type = FAL_ACL_RULE_MAC;
	rule.vid_val = sVlan;	/* for downstream */
	rule.vid_mask = 0xFFF;
	rule.vid_op = FAL_ACL_FIELD_MASK;
	FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_MAC_VID);
	FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_MODIFY_VLAN);
	rule.vid = cVlan;
		
	/* one ACL list contains only one ACL rule, so ruleId is always 0 */
	rv = shiva_acl_rule_add(0, listId, 0, 1, &rule);
	if (SW_OK != rv)
	{
		shiva_acl_list_destroy(0, listId);
		return rv;
	}

	/* bind ACL list to uplink port */
	rv = shiva_acl_list_bind(0, listId, 0, 0, DAL_ACL_UPLINK_PORT);
	if (SW_OK != rv)
	{
		shiva_acl_rule_delete(0, listId, 0, 1);
		shiva_acl_list_destroy(0, listId);
		return rv;
	}		

	dalAclPool[loc].aclIdDs[i] = listId;

	*index = loc * DAL_ACL_NUM_PER_PORT + i;

	return OPL_OK;
}



OPL_STATUS dalAcl1to1VtDel(UINT32 portId, UINT32 cVlan, UINT32 sVlan, UINT32 * index)
{
	UINT8	i;
	UINT16  loc = 0;
	UINT8	listId;
	int		rv;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

 	if (OPL_OK != dalAclFind1to1VT(portId, cVlan, sVlan, index))
	{
		return DAL_VTT_ACL_NOT_FOUND;
	}

	dalAclFind1to1VtPort(portId, &loc);
	for (i = 0; i < DAL_ACL_NUM_PER_PORT; i++)
	{
		if ((cVlan == dalAclPool[loc].fromVid[i])
			&&(sVlan == dalAclPool[loc].toVid[i]))
		{
			break;
		}
	}

	if (i >= DAL_ACL_NUM_PER_PORT)
	{
		return DAL_VTT_ACL_UPLIST_ERR;
	}		

	/* destroy upstream ACL list first */
	listId = dalAclPool[loc].aclIdUs[i];
	shiva_acl_list_unbind(0, listId, 0, 0, atherosPort);
	shiva_acl_rule_delete(0, listId, 0, 1);
	shiva_acl_list_destroy(0, listId);
	dalAclPool[loc].aclIdUs[i] = DAL_ACL_INVALID_LIST_ID;
	dalAclPool[loc].fromVid[i] = 0;
	dalAclPool[loc].toVid[i] = 0;

	/* destroy downstream ACL list first */
	listId = dalAclPool[loc].aclIdDs[i];
	shiva_acl_list_unbind(0, listId, 0, 0, DAL_ACL_UPLINK_PORT);
	shiva_acl_rule_delete(0, listId, 0, 1);
	shiva_acl_list_destroy(0, listId);
	dalAclPool[loc].aclIdDs[i] = DAL_ACL_INVALID_LIST_ID;

	*index = loc * DAL_ACL_NUM_PER_PORT + i;

	for (i = 0; i < DAL_ACL_NUM_PER_PORT; i++)
	{
		if (dalAclPool[loc].aclIdUs[i] != DAL_ACL_INVALID_LIST_ID)
		{
			break;
		}
	}

	if (i >= DAL_ACL_NUM_PER_PORT)
	{
		/* no more acl lists */
		vosMemSet(&dalAclPool[loc], 0, sizeof (DAL_ACL_INFO_s));
		dalAclPool[loc].type = DAL_ACL_UNUSED;	
	}

	return OPL_OK;
}



OPL_STATUS dalAclNto1MacVtAdd(UINT32 portId, UINT32 cVlan, UINT32 sVlan, UINT8 * mac, UINT32 * index)
{
	UINT32	i;
	UINT32  hsl_f_rsc = 0;
	UINT8	listId;
	UINT16	loc = 0;
	int		rv;
	fal_acl_rule_t  rule;
	fal_mac_addr_t tmpMac = {0xff,0xff,0xff,0xff,0xff,0xff};
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
 	if (OPL_OK == dalAclFindNto1MacVT(portId, cVlan, sVlan, mac, index))
	{
		return OPL_OK;
	}

	/* one VT entry needs two ACL rules */
	dalAclFreeRsc(&hsl_f_rsc);	
	if ( hsl_f_rsc < 2) /*modified*/
	{
		return DAL_VTT_ACL_RULE_FULL;
	}
	
	if (OPL_OK == dalAclFindNto1VtPort(portId, DAL_ACL_NTO1_VA_MAC, &loc))
	{
		/* other VT entries exist on this port */

		/* find free location of aclIdus to contain new ACL list */
		for (i = 0; i < DAL_ACL_NUM_PER_PORT; i++)
		{
			if (DAL_ACL_INVALID_LIST_ID == dalAclPool[loc].aclIdUs[i])
			{
				break;
			}
		}

		if (i >= DAL_ACL_NUM_PER_PORT)
		{
			return DAL_VTT_ACL_UPLIST_ERR;
		}		
	}
	else
	{
		/* to add first ACL list on this port */
		if (OPL_OK != dalAclFindFreeAclPool(&loc))
		{
			return DAL_VTT_ACL_POOL_FULL;
		}

		/* first ACL list for this port */
		i = 0;
	}

#if 1
	/* create upstream ACL list first */
	if (OPL_OK != dalAclFindFreeAclList(&listId))
	{
		return DAL_VTT_ACL_LIST_FULL;
	}

	/* N:1 VT ACL list priority is always 0*/
	rv = shiva_acl_list_creat(0, listId, 0);
	if (SW_OK != rv)
	{
		return rv;
	}

	/* add ACL rule for ACL list */
    vosMemSet(&rule, 0, sizeof (fal_acl_rule_t));		

	rule.rule_type = FAL_ACL_RULE_MAC;
	rule.src_mac_val = *(fal_mac_addr_t *)mac;
	rule.src_mac_mask = tmpMac;
	FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_MAC_SA);

	rule.vid_val = cVlan;
	rule.vid_mask = 0xFFF;
	rule.vid_op = FAL_ACL_FIELD_MASK;
	FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_MAC_VID);
	rule.ports = 0x40;
	FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_MODIFY_VLAN);
	rule.vid = sVlan;
	//FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_REDPT);
	//rule.ports = 8;

	/* one ACL list contains only one ACL rule, so ruleId is always 0 */
	rv = shiva_acl_rule_add(0, listId, 0, 1, &rule);
	if (SW_OK != rv)
	{
		shiva_acl_list_destroy(0, listId);
		return rv;
	}

	/* bind ACL list to portId */
	rv = shiva_acl_list_bind(0, listId, 0, 0, atherosPort);
	if (SW_OK != rv)
	{
		shiva_acl_rule_delete(0, listId, 0, 1);
		shiva_acl_list_destroy(0, listId);
		return rv;
	}		
#endif 

	dalAclPool[loc].type = DAL_ACL_NTO1_VA_MAC;	
	dalAclPool[loc].aclIdUs[i] = listId;
	dalAclPool[loc].fromVid[i] = cVlan;
	dalAclPool[loc].toVid[i] = sVlan;	
	dalAclPool[loc].port = atherosPort;

	/* create downstream ACL list first  on uplink port */
	if (OPL_OK != dalAclFindFreeAclList(&listId))
	{
		return DAL_VTT_ACL_LIST_FULL;
	}

	/* N:1 VT ACL list priority is always 0*/
	rv = shiva_acl_list_creat(0, listId, 0);
	if (SW_OK != rv)
	{
		return rv;
	}

	/* add ACL rule for ACL list */
	vosMemSet(&rule, 0, sizeof (fal_acl_rule_t));		

	rule.rule_type = FAL_ACL_RULE_MAC;
	rule.dest_mac_val = *(fal_mac_addr_t *)mac;
	rule.dest_mac_mask = tmpMac;
	FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_MAC_DA);

	rule.vid_val = sVlan;	/* for downstream */
	rule.vid_mask = 0xFFF;
	rule.vid_op = FAL_ACL_FIELD_MASK;
	FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_MAC_VID);
	rule.ports = 0x1e;
	FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_MODIFY_VLAN);
	rule.vid = cVlan;
		
	/* one ACL list contains only one ACL rule, so ruleId is always 0 */
	rv = shiva_acl_rule_add(0, listId, 0, 1, &rule);
	if (SW_OK != rv)
	{
		/* delete upstream acl list */
		shiva_acl_rule_delete(0, dalAclPool[loc].aclIdUs[i], 0, 1);
		shiva_acl_list_unbind(0, dalAclPool[loc].aclIdUs[i], 0, 0, atherosPort);
		shiva_acl_list_destroy(0, dalAclPool[loc].aclIdUs[i]);

		//dalAclPool[loc].type = DAL_ACL_UNUSED; 
		dalAclPool[loc].aclIdUs[i] = DAL_ACL_INVALID_LIST_ID;
		dalAclPool[loc].fromVid[i] = 0;
		dalAclPool[loc].toVid[i] = 0;	
		//dalAclPool[loc].port = 0;
		shiva_acl_list_destroy(0, listId);
		return rv;
	}

	/* bind ACL list to uplink port */
	rv = shiva_acl_list_bind(0, listId, 0, 0, DAL_ACL_UPLINK_PORT);
	if (SW_OK != rv)
	{
		shiva_acl_rule_delete(0, listId, 0, 1);
		shiva_acl_list_destroy(0, listId);
		return rv;
	}		
	
	dalAclPool[loc].aclIdDs[i] = listId;
	vosMemCpy(&(dalAclPool[loc].mac[i*6]), mac, 6);

	*index = loc * DAL_ACL_NUM_PER_PORT + i;
	
	return OPL_OK;
}



OPL_STATUS dalAclNto1MacVtDel(UINT32 portId, UINT32 cVlan, UINT32 sVlan, UINT8 * mac, UINT32 * index)
{
	UINT8	i;
	UINT16	loc = 0;
	UINT8	listId;
	int		rv;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
 	if (OPL_OK != dalAclFindNto1MacVT(portId, cVlan, sVlan, mac, index))
	{
		return DAL_VTT_ACL_NOT_FOUND;
	}

	dalAclFindNto1VtPort(portId, DAL_ACL_NTO1_VA_MAC, &loc);
	for (i = 0; i < DAL_ACL_NUM_PER_PORT; i++)
	{
		if ((cVlan == dalAclPool[loc].fromVid[i])
			&&(sVlan == dalAclPool[loc].toVid[i])
			&& DAL_ACL_MAC_SAME(&(dalAclPool[loc].mac[i*6]),mac))
		{
			break;
		}
	}

	if (i >= DAL_ACL_NUM_PER_PORT)
	{
		return DAL_VTT_ACL_UPLIST_ERR;
	}		

#if 1
	/* destroy upstream ACL list first */
	listId = dalAclPool[loc].aclIdUs[i];
	shiva_acl_list_unbind(0, listId, 0, 0, atherosPort);
	shiva_acl_rule_delete(0, listId, 0, 1);
	shiva_acl_list_destroy(0, listId);
#endif

	/* destroy downstream ACL list first */
	listId = dalAclPool[loc].aclIdDs[i];
	shiva_acl_list_unbind(0, listId, 0, 0, DAL_ACL_UPLINK_PORT);
	shiva_acl_rule_delete(0, listId, 0, 1);
	rv = shiva_acl_list_destroy(0, listId);
	if (SW_OK != rv){
		printf("%s fail to destroy acl.(rv %d size %d)\n", __FUNCTION__, rv, list_ent[0][listId].size);
	}
	
	dalAclPool[loc].aclIdUs[i] = DAL_ACL_INVALID_LIST_ID;
	dalAclPool[loc].aclIdDs[i] = DAL_ACL_INVALID_LIST_ID;
	dalAclPool[loc].fromVid[i] = 0;
	dalAclPool[loc].toVid[i] = 0;	
	vosMemSet(&(dalAclPool[loc].mac[i*6]), 0, 6);

	for (i = 0; i < DAL_ACL_NUM_PER_PORT; i++)
	{
		if (DAL_ACL_INVALID_LIST_ID == dalAclPool[loc].aclIdUs[i] && 
			DAL_ACL_INVALID_LIST_ID == dalAclPool[loc].aclIdDs[i]){
			continue;
		}
		else {
			break;
		}
	}

	if (i >= DAL_ACL_NUM_PER_PORT){
		dalAclPool[loc].port = 0;
		dalAclPool[loc].type = DAL_ACL_UNUSED; 
		//vosMemSet(&dalAclPool[loc], 0, sizeof (DAL_ACL_INFO_s));
	}

	*index = loc * DAL_ACL_NUM_PER_PORT + i;
	
	return OPL_OK;
}



OPL_STATUS dalAclClsAdd(CLS_CONFIG_INFO_t *stClassCfg, UINT16 *ruleId)
{
	UINT8	listId, i;
	fal_acl_rule_t  rule;
	UINT32  hsl_f_rsc = 0;
	int		rv;
	UINT8 	portListBuff[255];
	UINT32 	portArry[ODM_NUM_OF_PORTS+ 1];
	UINT8 	tmpPortMap;
	UINT16	tmpRuleId;

	/* one class entry needs two ACL rules */
	dalAclFreeRsc(&hsl_f_rsc);	
	if ( hsl_f_rsc < 2)
	{
		return DAL_VTT_ACL_RULE_FULL;
	}

	vosStrCpy(portListBuff,stClassCfg->ports);

	vosMemSet(portArry, 0, sizeof(UINT32)*(ODM_NUM_OF_PORTS+ 1));
	rv = convertPortListString2PortArry(portListBuff,portArry,ODM_NUM_OF_PORTS+1);	

	if (OPL_OK == dalAclFindCls(stClassCfg->name, &tmpRuleId))
	{
		/* old rule exists,  delete it */
		listId = dalAclPool[tmpRuleId].aclIdUs[0];
		shiva_acl_rule_delete(0, listId, 0, 1);
		shiva_acl_list_unbind(0, listId, 0, 0, dalAclPool[tmpRuleId].port);
		shiva_acl_list_destroy(0, listId);		

		/* add new rule */
	}
	else
	{
		/* to add first ACL list on this port */
		if (OPL_OK != dalAclFindFreeAclPool(&tmpRuleId))
		{
			return DAL_VTT_ACL_POOL_FULL;
		}

		/* add new rule */
		if (OPL_OK != dalAclFindFreeAclList(&listId))
		{
			return DAL_VTT_ACL_POOL_FULL;
		}
	}

	/* add new rule */
	/* classify ACL list priority is always 0*/
	rv = shiva_acl_list_creat(0, listId, 0);
	if (SW_OK != rv)
	{
		return rv;
	}

	/* add ACL rule for ACL list */
    vosMemSet(&rule, 0, sizeof (fal_acl_rule_t));
	dalConvertClsParam(stClassCfg, &rule);
	
	#if 0
	rule.rule_type = FAL_ACL_RULE_MAC;

	if (stClassCfg->dstMacFlag)
	{
        for (i = 0; i < 6; i++) {
			rule.dest_mac_val.uc[i] = stClassCfg->dstMac.lowRange[i];
            rule.dest_mac_mask.uc[i] = 0xFF;
        }		
		FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_MAC_DA);
	}

	if (stClassCfg->srcMacFlag)
	{
        for (i = 0; i < 6; i++) {
			rule.src_mac_val.uc[i] = stClassCfg->srcMac.lowRange[i];
            rule.src_mac_mask.uc[i] = 0xFF;
        }		
		FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_MAC_SA);
	}

	if (stClassCfg->etherPriFlag)
	{
		rule.up_val = stClassCfg->etherPri.lowRange;
		rule.up_mask = 0xFF;
		FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_MAC_UP);
	}

	if (stClassCfg->vlanIdFlag)
	{
		rule.vid_val = stClassCfg->vlanId.lowRange;
		rule.vid_mask = 0xFFF;
		FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_MAC_VID);
	}

	if (stClassCfg->lenOrTypeFlag)
	{
		rule.ethtype_val = stClassCfg->lenOrType.lowRange;
		rule.ethtype_mask = 0xFFFF;
		FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_MAC_ETHTYPE);
	}

	if (stClassCfg->dstIpFlag)
	{
		rule.dest_ip4_val = stClassCfg->dstIp.lowRange;
		rule.dest_ip4_mask = 0xFFFFFFFF;
		rule.rule_type = FAL_ACL_RULE_IP4;
		FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_IP4_DIP);
	}

	if (stClassCfg->srcIpFlag)
	{
		rule.src_ip4_val = stClassCfg->srcIp.lowRange;
		rule.src_ip4_mask = 0xFFFFFFFF;
		rule.rule_type = FAL_ACL_RULE_IP4;
		FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_IP4_SIP);
	}

	if (stClassCfg->ipTypeFlag)
	{
		rule.ip_proto_val = stClassCfg->ipType.lowRange;
		rule.ip_proto_mask = 0xFF;
		rule.rule_type = FAL_ACL_RULE_IP4;
		FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_IP_PROTO);
	}

	if (stClassCfg->ipV4DscpFlag)
	{
		rule.ip_dscp_val = stClassCfg->ipV4Dscp.lowRange;
		rule.ip_dscp_mask = 0xFF;
		rule.rule_type = FAL_ACL_RULE_IP4;
		FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_IP_DSCP);
	}

	if (stClassCfg->srcL4PortFlag)
	{
		rule.src_l4port_val = stClassCfg->srcL4Port.lowRange;
		rule.src_l4port_mask = 0xFFFF;
		rule.rule_type = FAL_ACL_RULE_IP4;
		FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_L4_SPORT);
	}

	if (stClassCfg->dstL4PortFlag)
	{
		rule.dest_l4port_val = stClassCfg->dstL4Port.lowRange;
		rule.dest_l4port_mask = 0xFFFF;
		rule.rule_type = FAL_ACL_RULE_IP4;
		FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_L4_DPORT);
	}
	
	if (stClassCfg->rmkvidOp==RMK_CHANGE_VID)
	{
		rule.vid=stClassCfg->rmkvid;
		FAL_ACTION_FLG_SET(rule.action_flg,FAL_ACL_ACTION_MODIFY_VLAN);
	}
	if (stClassCfg->rmkvidOp==RMK_INSERT_VID)
	{
		rule.vid=stClassCfg->rmkvid;
		FAL_ACTION_FLG_SET(rule.action_flg,FAL_ACL_ACTION_NEST_VLAN);
	}

	/* four queues per port */
	rule.queue = stClassCfg->queueMapId;
	FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_REMARK_QUEUE);

	/* change user priority */
	rule.up = stClassCfg->cosMapVal;
	FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_REMARK_UP);

	FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_PERMIT);
	if (stClassCfg->t_act & DROP_PKTS)
	{
		if (stClassCfg->t_act & COPY_PACKETS_TO_CPU) 
		{
			/* redirect to CPU */
			FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_RDTCPU);
		}
		else
		{
			/* drop */
			FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_DENY);
		}
	}
	else
	{
		/* pass and copy */
		if (stClassCfg->t_act & COPY_PKTS_TO_CPU)
		{
			FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_CPYCPU);
		}

		/* pass through */
		if (stClassCfg->t_act & DONOT_COPY_TO_CPU) 
		{
			rule.ports = DAL_ACL_UPLINK_PORT;
			FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_REDPT);			
		}		
	}
#endif
	/* one ACL list contains only one ACL rule, so ruleId is always 0 */
	rv = shiva_acl_rule_add(0, listId, 0, 1, &rule);
	if (SW_OK != rv)
	{
		shiva_acl_list_destroy(0, listId);
		return rv;
	}

	/* bind ACL list to portId */
	tmpPortMap = 0;
	for(i = 1; i < DAL_ACL_UPLINK_PORT; i++)
	{
		if (portArry[i])
		{			
			tmpPortMap |= (1 << dalToAtherosPortMap[i]);
			rv = shiva_acl_list_bind(0, listId, 0, 0, dalToAtherosPortMap[i]);
		}
	}

	if (SW_OK != rv)
	{
		shiva_acl_rule_delete(0, listId, 0, 1);
		shiva_acl_list_destroy(0, listId);
		return rv;
	}		

	dalAclPool[tmpRuleId].type = DAL_ACL_CLASSIFICATION; 
	dalAclPool[tmpRuleId].aclIdUs[0] = listId;
	dalAclPool[tmpRuleId].port = tmpPortMap;
	vosStrCpy(dalAclPool[tmpRuleId].name, stClassCfg->name);
	vosMemCpy(&(dalAclPool[tmpRuleId].rule), &rule,sizeof(rule));

	*ruleId = tmpRuleId;

	return OPL_OK;	
}



OPL_STATUS dalAclClsDel2(UINT16 ruleId)
{
	UINT8	listId, j;
	UINT8	tmpPortMap;

	if (DAL_ACL_CLASSIFICATION != dalAclPool[ruleId].type)
	{
		return OPL_ERROR;
	}

	/* old rule exists,  delete it */
	listId = dalAclPool[ruleId].aclIdUs[0];
	tmpPortMap = dalAclPool[ruleId].port;

	j = 1;
	while(j < DAL_ACL_UPLINK_PORT)
	{
		if ((tmpPortMap >> j) & 1)
		{
			shiva_acl_list_unbind(0, listId, 0, 0,j);
		}
		j++;
	}
	shiva_acl_rule_delete(0, listId, 0, 1);
	shiva_acl_list_destroy(0, listId);
	
	vosMemSet(&dalAclPool[ruleId], 0, sizeof (DAL_ACL_INFO_s));
	dalAclPool[ruleId].type = DAL_ACL_UNUSED;	
	for (j = 0; j < DAL_ACL_NUM_PER_PORT; j++)
	{
		dalAclPool[ruleId].aclIdUs[j] = DAL_ACL_INVALID_LIST_ID;
		dalAclPool[ruleId].aclIdDs[j] = DAL_ACL_INVALID_LIST_ID;
	}

	return OPL_OK;
}



OPL_STATUS dalAclClsDel(CLS_CONFIG_INFO_t *stClassCfg, UINT16 *ruleId)
{
	if (OPL_OK == dalAclFindCls(stClassCfg->name, ruleId))
	{
		return dalAclClsDel2(*ruleId);
	}
	else
	{
		return OPL_ERROR;
	}	
}



OPL_STATUS dalAclIgmpAdd(void)
{
	UINT8	listId, i;
	fal_acl_rule_t  rule;
	UINT32  hsl_f_rsc = 0;
	int		rv;
	UINT16	tmpRuleId;
	UINT8	igmpMac[6] = {0x01,0x00,0x5e,0x00,0x00,0x00};
	UINT8	igmpMacMask[6] = {0xFF,0xFF,0xFF,0x00,0x00,0x00};

	/* one class entry needs two ACL rules */
	dalAclFreeRsc(&hsl_f_rsc);	
	if ( hsl_f_rsc < 3)
	{
		return DAL_VTT_ACL_RULE_FULL;
	}

	/* to add first ACL list on this port */
	if (OPL_OK != dalAclFindFreeAclPool(&tmpRuleId))
	{
		return DAL_VTT_ACL_POOL_FULL;
	}

	/* add new rule */
	if (OPL_OK != dalAclFindFreeAclList(&listId))
	{
		return DAL_VTT_ACL_POOL_FULL;
	}

	/* add new rule */
	/* classify ACL list priority is always 0*/
	rv = shiva_acl_list_creat(0, listId, 0);
	if (SW_OK != rv)
	{
		return rv;
	}

	/* add ACL rule for ACL list */
    vosMemSet(&rule, 0, sizeof (fal_acl_rule_t));
	rule.rule_type = FAL_ACL_RULE_IP4;

    for (i = 0; i < 6; i++) {
		rule.dest_mac_val.uc[i] = igmpMac[i];
        rule.dest_mac_mask.uc[i] = igmpMacMask[i];
    }		
	FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_MAC_DA);

	rule.ip_proto_val = 2;	/* igmp */
	rule.ip_proto_mask = 0xFF;
	FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_IP_PROTO);

	FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_PERMIT);
	/* redirect to CPU */
	FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_RDTCPU);

	/* one ACL list contains only one ACL rule, so ruleId is always 0 */
	rv = shiva_acl_rule_add(0, listId, 0, 1, &rule);
	if (SW_OK != rv)
	{
		shiva_acl_list_destroy(0, listId);
		return rv;
	}

	/* bind ACL list to portId */
	for(i = 1; i < DAL_ACL_UPLINK_PORT; i++)
	{
		/* binding operation is up to dalAclIgmpEnable */
		/* rv = shiva_acl_list_bind(0, listId, 0, 0, i); */
	}

	if (SW_OK != rv)
	{
		shiva_acl_rule_delete(0, listId, 0, 1);
		shiva_acl_list_destroy(0, listId);
		return rv;
	}		

	dalAclPool[tmpRuleId].type = DAL_ACL_IGMP; 
	dalAclPool[tmpRuleId].aclIdUs[0] = listId;
	dalAclPool[tmpRuleId].port = 0x1E;

	igmpListId = listId;

	return OPL_OK;	
}



OPL_STATUS dalAclRstpAdd(UINT8 flg)  //0:cpu 1:ge
{
	UINT8	listId, i;
	fal_acl_rule_t  rule;
	UINT32  hsl_f_rsc = 0;
	int		rv;
	UINT16	tmpRuleId;
	UINT8	rstpMac[6] = {0x01,0x80,0xc2,0x00,0x00,0x00};
	UINT8	rstpMacMask[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

	/* one class entry needs two ACL rules */
	dalAclFreeRsc(&hsl_f_rsc);	
	if ( hsl_f_rsc < 3)
	{
		return DAL_VTT_ACL_RULE_FULL;
	}

	if (0xFF != RstplistId)
	{
    listId = RstplistId;
	  shiva_acl_rule_delete(0, listId, 0, 1);
	  for(i = 1; i < DAL_ACL_UPLINK_PORT; i++)
	  {
		  shiva_acl_list_unbind(0, listId, 0, 0, i);
		}
		shiva_acl_list_destroy(0, listId);
	  RstplistId = listId;
	}
	else
	{
	  /* to add first ACL list on this port */
	  if (OPL_OK != dalAclFindFreeAclPool(&tmpRuleId))
	  {
	  	return DAL_VTT_ACL_POOL_FULL;
	  }
    
	  /* add new rule */
	  if (OPL_OK != dalAclFindFreeAclList(&listId))
	  {
	  	return DAL_VTT_ACL_POOL_FULL;
	  }
	  RstplistId = listId;
	}

	/* add new rule */
	/* classify ACL list priority is always 0*/
	rv = shiva_acl_list_creat(0, listId, 0);
	if (SW_OK != rv)
	{
		return rv;
	}

	/* add ACL rule for ACL list */
    vosMemSet(&rule, 0, sizeof (fal_acl_rule_t));
	rule.rule_type = FAL_ACL_RULE_MAC;

    for (i = 0; i < 6; i++) {
		rule.dest_mac_val.uc[i] = rstpMac[i];
        rule.dest_mac_mask.uc[i] = rstpMacMask[i];
    }		
	FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_MAC_DA);

	FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_PERMIT);
	/* redirect to CPU */
	#if 0
	FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_RDTCPU);
	#else
	if (0 == flg)
	{
	  rule.ports = 0x1;
	}
	else
	{
	  rule.ports = 0x40;
	}
	
	FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_REDPT);
	#endif

	/* highest prioroty queue */
	rule.queue = 3;
	FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_REMARK_QUEUE);	

	/* one ACL list contains only one ACL rule, so ruleId is always 0 */
	rv = shiva_acl_rule_add(0, listId, 0, 1, &rule);
	if (SW_OK != rv)
	{
		shiva_acl_list_destroy(0, listId);
		return rv;
	}

	/* bind ACL list to portId */
	for(i = 1; i < DAL_ACL_UPLINK_PORT; i++)
	{
		rv = shiva_acl_list_bind(0, listId, 0, 0, i);
	}

	if (SW_OK != rv)
	{
		shiva_acl_rule_delete(0, listId, 0, 1);
		shiva_acl_list_destroy(0, listId);
		return rv;
	}		

	dalAclPool[tmpRuleId].type = DAL_ACL_RSTP; 
	dalAclPool[tmpRuleId].aclIdUs[0] = listId;
	dalAclPool[tmpRuleId].port = 0x1E;

	return OPL_OK;	
}



OPL_STATUS dalAclIgmpEnable(a_bool_t enable)
{
	UINT8 i;
	
	for(i = 1; i < DAL_ACL_UPLINK_PORT; i++)
	{
		/* un/binding operation is up to dalAclIgmpEnable */
		if (enable)
		{
			shiva_acl_list_bind(0, igmpListId, 0, 0, i); 
		}
		else
		{
			shiva_acl_list_unbind(0, igmpListId, 0, 0, i); 
		}
	}

	return OPL_OK;
}



OPL_STATUS dalAclCtcVTAdd(UINT16 oldVid, UINT16 newVid, UINT8 portID)
{
	UINT8	listId, i, j;
	fal_acl_rule_t  rule;
	UINT32  hsl_f_rsc = 0;
	int		rv;
	UINT16	tmpRuleId;
	UINT8	igmpMac[6] = {0x01,0x00,0x5e,0x00,0x00,0x00};
	UINT8	igmpMacMask[6] = {0xFF,0xFF,0xFF,0x00,0x00,0x00};

	for (i = 0; i < DAL_ACL_TOTAL_NUM; i++)
	{
		if (DAL_ACL_CTC_MC_VT == dalAclPool[i].type)
		{
			for (j = 0; j < DAL_ACL_NUM_PER_PORT; j++)
			{
				if ( (oldVid == dalAclPool[i].fromVid[0])
					&& (newVid == dalAclPool[i].toVid[0])
					&& (portID == dalAclPool[i].port))
				{
					return OPL_ERROR;
				}
			}
		}		
	}

	/* one class entry needs two ACL rules */
	dalAclFreeRsc(&hsl_f_rsc);	
	if ( hsl_f_rsc < 3)
	{
		return DAL_VTT_ACL_RULE_FULL;
	}

	/* to add first ACL list on this port */
	if (OPL_OK != dalAclFindFreeAclPool(&tmpRuleId))
	{
		return DAL_VTT_ACL_POOL_FULL;
	}

	/* add new rule */
	if (OPL_OK != dalAclFindFreeAclList(&listId))
	{
		return DAL_VTT_ACL_POOL_FULL;
	}

	/* add new rule */
	/* classify ACL list priority is always 0*/
	rv = shiva_acl_list_creat(0, listId, 0);
	if (SW_OK != rv)
	{
		return rv;
	}

	/* add ACL rule for ACL list */
    vosMemSet(&rule, 0, sizeof (fal_acl_rule_t));
	rule.rule_type = FAL_ACL_RULE_MAC;

	/* match DA */
    for (i = 0; i < 6; i++) {
		rule.dest_mac_val.uc[i] = igmpMac[i];
        rule.dest_mac_mask.uc[i] = igmpMacMask[i];
    }		
	FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_MAC_DA);

	/* match VID */
	rule.vid_val = oldVid;
	rule.vid_mask = 0xFFF;
	rule.vid_op = 0;
	FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_MAC_VID);

	/* remard vid */
	rule.vid = newVid;
	FAL_ACTION_FLG_SET(rule.action_flg,FAL_ACL_ACTION_MODIFY_VLAN);

	FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_PERMIT); 
	FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_VID_MEM_DIS);

	/* one ACL list contains only one ACL rule, so ruleId is always 0 */
	rv = shiva_acl_rule_add(0, listId, 0, 1, &rule);
	if (SW_OK != rv)
	{
		shiva_acl_list_destroy(0, listId);
		return rv;
	}

	/* bind ACL list to GE port */
	rv = shiva_acl_list_bind(0, listId, 0, 0, 6);
	if (SW_OK != rv)
	{
		shiva_acl_rule_delete(0, listId, 0, 1);
		shiva_acl_list_destroy(0, listId);
		return rv;
	}		

	dalAclPool[tmpRuleId].type = DAL_ACL_CTC_MC_VT; 
	dalAclPool[tmpRuleId].aclIdDs[0] = listId;
	dalAclPool[tmpRuleId].fromVid[0] = oldVid;
	dalAclPool[tmpRuleId].toVid[0] = newVid;
	dalAclPool[tmpRuleId].port = portID;

	return OPL_OK;	
}



OPL_STATUS dalAclCtcVTDel(UINT16 oldVid, UINT16 newVid, UINT8 portID)
{
	UINT8	listId = 255, i, j, ruleId = 255;
	int rv;
	
	for (i = 0; i < DAL_ACL_TOTAL_NUM; i++)
	{
		if (DAL_ACL_CTC_MC_VT == dalAclPool[i].type)
		{
			for (j = 0; j < DAL_ACL_NUM_PER_PORT; j++)
			{
				if ( (oldVid == dalAclPool[i].fromVid[0])
					&& (newVid == dalAclPool[i].toVid[0])
					&& (portID == dalAclPool[i].port))
				{
					ruleId = i;
					listId = dalAclPool[i].aclIdDs[0];
				}
			}
		}		
	}

	if ((255 == listId) || (255 == ruleId))
	{
		return OPL_ERROR;
	}

	rv = shiva_acl_list_unbind(0, listId, 0, 0,6);
	if (SW_OK != rv)
	{
		printf("dalAclCtcDel unbind failed\n");
		return rv;
	}

	shiva_acl_rule_delete(0, listId, 0, 1);
	if (SW_OK != rv)
	{
		printf("dalAclCtcDel rule deleting failed\n");
		return rv;
	}

	shiva_acl_list_destroy(0, listId);
	if (SW_OK != rv)
	{
		printf("dalAclCtcDel rule destroying failed\n");
		return rv;
	}

	dalAclPool[ruleId].type = DAL_ACL_UNUSED; 
	dalAclPool[ruleId].port = 0;

	for (j = 0; j < DAL_ACL_NUM_PER_PORT; j++)
	{
		dalAclPool[ruleId].aclIdUs[j] = DAL_ACL_INVALID_LIST_ID;
		dalAclPool[ruleId].aclIdDs[j] = DAL_ACL_INVALID_LIST_ID;
	}

	return OPL_OK;	
}



OPL_STATUS dalAclRsvdVlanFilter(void)
{
	UINT8	listId, i;
	fal_acl_rule_t  rule;
	UINT32  hsl_f_rsc = 0;
	int		rv;
	UINT16	tmpRuleId;

	/* one class entry needs two ACL rules */
	dalAclFreeRsc(&hsl_f_rsc);	
	if ( hsl_f_rsc < 3)
	{
		return DAL_VTT_ACL_RULE_FULL;
	}

	/* to add first ACL list on this port */
	if (OPL_OK != dalAclFindFreeAclPool(&tmpRuleId))
	{
		return DAL_VTT_ACL_POOL_FULL;
	}

	/* add new rule */
	if (OPL_OK != dalAclFindFreeAclList(&listId))
	{
		return DAL_VTT_ACL_POOL_FULL;
	}

	/* add new rule */
	/* classify ACL list priority is always 0*/
	rv = shiva_acl_list_creat(0, listId, 0);
	if (SW_OK != rv)
	{
		return rv;
	}

	/* add ACL rule for ACL list */
    vosMemSet(&rule, 0, sizeof (fal_acl_rule_t));
	rule.rule_type = FAL_ACL_RULE_MAC;

	rule.vid_val = 4091;
	rule.vid_mask = 4095;
	rule.vid_op = FAL_ACL_FIELD_RANGE;
	FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_MAC_VID);

	FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_DENY);

	/* one ACL list contains only one ACL rule, so ruleId is always 0 */
	rv = shiva_acl_rule_add(0, listId, 0, 1, &rule);
	if (SW_OK != rv)
	{
		shiva_acl_list_destroy(0, listId);
		return rv;
	}

	/* bind ACL list to all UNI ports and GE port */
	for(i = 1; i <= DAL_ACL_UPLINK_PORT; i++)
	{
		rv = shiva_acl_list_bind(0, listId, 0, 0, i); 
	}

	if (SW_OK != rv)
	{
		shiva_acl_rule_delete(0, listId, 0, 1);
		shiva_acl_list_destroy(0, listId);
		return rv;
	}		

	dalAclPool[tmpRuleId].type = DAL_ACL_RSVD_VLAN_FILTER; 
	dalAclPool[tmpRuleId].aclIdUs[0] = listId;
	dalAclPool[tmpRuleId].port = 0x7E;

	igmpListId = listId;

	return OPL_OK;	
}


OPL_STATUS dalAclZeroMacDiscard(void)
{
	UINT8	listId, i, j;
	fal_acl_rule_t  rule;
	UINT32  hsl_f_rsc = 0;
	int		rv;
	UINT16	tmpRuleId;

	for (j=0; j<2; j++)
	{
		/* one class entry needs two ACL rules */
		dalAclFreeRsc(&hsl_f_rsc);	
		if ( hsl_f_rsc < 3)
		{
			printf("dalAclFreeRsc failed %d.\n", hsl_f_rsc);
			return DAL_VTT_ACL_RULE_FULL;
		}

		/* to add first ACL list on this port */
		if (OPL_OK != dalAclFindFreeAclPool(&tmpRuleId))
		{
			printf("dalAclFindFreeAclPool failed %d.\n", rv);
			return DAL_VTT_ACL_POOL_FULL;
		}

		/* add new rule */
		if (OPL_OK != dalAclFindFreeAclList(&listId))
		{
			printf("dalAclFindFreeAclList failed %d.\n", rv);
			return DAL_VTT_ACL_POOL_FULL;
		}

		/* add new rule */
		/* classify ACL list priority is always 0*/
		rv = shiva_acl_list_creat(0, listId, 0);
		if (SW_OK != rv)
		{
			printf("shiva_acl_list_creat failed %d.\n", rv);
			return rv;
		}

		/* add ACL rule for ACL list */
		vosMemSet(&rule, 0, sizeof (fal_acl_rule_t));
		if (j == 0)/*des mac*/
		{	
			//printf("add des mac 0 acl.\n");
			rule.rule_type = FAL_ACL_RULE_MAC;
			vosMemSet(rule.dest_mac_mask.uc, 0xFF, sizeof(fal_mac_addr_t));
			FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_MAC_DA);
		}
		else /* src mac*/
		{
			//printf("add src mac 0 acl.\n");
			rule.rule_type = FAL_ACL_RULE_MAC;
			vosMemSet(rule.src_mac_mask.uc, 0xFF, sizeof(fal_mac_addr_t));
			FAL_FIELD_FLG_SET(rule.field_flg,FAL_ACL_FIELD_MAC_SA);
		}
		FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_DENY);

		/* one ACL list contains only one ACL rule, so ruleId is always 0 */
		rv = shiva_acl_rule_add(0, listId, 0, 1, &rule);
		if (SW_OK != rv)
		{
			shiva_acl_list_destroy(0, listId);
			//printf("shiva_acl_rule_add failed %d.\n", rv);
			return rv;
		}

		/* bind ACL list to all UNI ports and GE port */
		for(i = 1; i <= DAL_ACL_UPLINK_PORT; i++)
		{
			rv = shiva_acl_list_bind(0, listId, 0, 0, i); 
		}

		if (SW_OK != rv)
		{
			shiva_acl_rule_delete(0, listId, 0, 1);
			shiva_acl_list_destroy(0, listId);
			//printf("shiva_acl_list_bind failed %d.\n", rv);
			return rv;
		}		

		dalAclPool[tmpRuleId].type = DAL_ACL_ZERO_MAC; 
		dalAclPool[tmpRuleId].aclIdUs[0] = listId;
		dalAclPool[tmpRuleId].port = 0x7E;
	}


	
	return OPL_OK;	
}


/*******************************************************************************
* dalAclInit
*
* DESCRIPTION:
*  	Initialize the ACL management data structure.
*
*	INPUTS:
*
*	OUTPUTS:

* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalAclInit()
{
	UINT32 i,j;
	
	RstplistId = 0xFF;
	vosMemSet(dalAclPool, 0, (DAL_ACL_TOTAL_NUM) * sizeof (DAL_ACL_INFO_s));

	for (i = 0; i < DAL_ACL_TOTAL_NUM; i++)
	{
		dalAclPool[i].type = DAL_ACL_UNUSED;
		
		for (j = 0; j < DAL_ACL_NUM_PER_PORT; j++)
		{
			dalAclPool[i].aclIdUs[j] = DAL_ACL_INVALID_LIST_ID;
			dalAclPool[i].aclIdDs[j] = DAL_ACL_INVALID_LIST_ID;
		}
	}	

	//dalAclIgmpAdd();
	dalAclRstpAdd(0);
	//deleted by cylan for bug3027
	//dalAclRsvdVlanFilter();

	dalAclZeroMacDiscard();
	return OPL_OK;
}



OPL_STATUS dalAclShow()
{
	int rv;
	UINT8	i, j;

	for (i = 0; i < DAL_ACL_TOTAL_NUM; i++)
	{
		printf("index = %d, acl type = %d, port = %d, name = %s\n", 
			i, dalAclPool[i].type, dalAclPool[i].port, dalAclPool[i].name);

		printf("fromvid:");
		for (j = 0; j < DAL_ACL_NUM_PER_PORT; j++)
		{
			printf("%04d ", dalAclPool[i].fromVid[j]);
		}
		printf("\n");
		
		printf("tovid:");
		for (j = 0; j < DAL_ACL_NUM_PER_PORT; j++)
		{
			printf("%04d ", dalAclPool[i].toVid[j]);
		}
		printf("\n");			

		printf("aclIdUs:");
		for (j = 0; j < DAL_ACL_NUM_PER_PORT; j++)
		{
			printf("%04d ", dalAclPool[i].aclIdUs[j]);
		}
		printf("\n");

		printf("aclIdDs:");
		for (j = 0; j < DAL_ACL_NUM_PER_PORT; j++)
		{
			printf("%04d ", dalAclPool[i].aclIdDs[j]);
		}
		printf("\n");
	}
	
	return OPL_OK;
}

