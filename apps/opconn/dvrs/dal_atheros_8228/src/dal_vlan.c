/*
=============================================================================
     File Name: dal_vlan.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     
NOTES: opconn ONU chip not support 802.1q vlan function.
so implement by switch chip. 
Revision History:
Author                Date              		Description of Changes
----------------   ------------  ----------------------------------------------
zran 			2009/05/20		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif /*__cplusplus*/
#endif /*__cplusplus*/

#include <dal_lib.h>
#include "aos_types.h"
#include "fal_type.h"
#include "fal_vlan.h"
#include "dal_vtt.h"

extern VTT_VLAN_8228_t dal_vlan8228[];
extern VLAN_CONFI_INFO_t dal_vtt_info[];
UINT16	dal_vlan_trunk_num[7];	/* 7 ports on 8228 */

/*******************************************************************************
* dalSetLocalSwitch
*
* DESCRIPTION:
*  		this function support set local switch enable/disable.
*
*	INPUTS:
          lport:user logic port.
          enable  :enable/disable.
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalSetLocalSwitch(UINT8 portId, UINT8  enable)
{
	UINT8 i;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if (enable)
	{
		for (i = 1; i < 5; i++)
		{
		       if (atherosPort != i)
		       {
		            shiva_portvlan_member_add(0, atherosPort, i);
		       }		
		}
//		shiva_port_arp_leaky_set(0, atherosPort, 1);
		/* added by cylan for bug3027 */
		/* add the force_port_vlan_en=0 for the port not isolation */
		shiva_port_force_portvlan_set(0, atherosPort, 0);

              /* deleted by cylan for bug3027 */
		/* add port to special vlan */
		/*switch(atherosPort)
		{
			case 1:
				shiva_vlan_member_update(0, 4094, 0x5F, 0);
				dal_vlan8228[4094].member = 0x5F;
				break;
			case 2:
				shiva_vlan_member_update(0, 4093, 0x5F, 0);
				dal_vlan8228[4093].member = 0x5F;
				break;
			case 3:
				shiva_vlan_member_update(0, 4092, 0x5F, 0);
				dal_vlan8228[4092].member = 0x5F;
				break;
			case 4:
				shiva_vlan_member_update(0, 4091, 0x5F, 0);
				dal_vlan8228[4091].member = 0x5F;
				break;
			default:
				break;
		}*/
	}
	else
	{
		for (i = 1; i < 5; i++)
		{
				shiva_portvlan_member_del(0, atherosPort, i);
			}

		/* added by cylan for bug3027 */
		/* add the force_port_vlan_en=1 for the port isolation */
//shiva_uc_leaky_mode_set(0,atherosPort,0);
//printf("uc leaky mode 0\r\n");
		shiva_port_arp_leaky_set(0, atherosPort, 0);
		shiva_port_uc_leaky_set(0,atherosPort,0);
		/* add portvlan member = UNI + Mac6 */
//		shiva_portvlan_member_add(0, atherosPort, 6);
		//shiva_portvlan_member_add(0, 6, atherosPort);

              /* deleted by cylan for bug3027 */
		/* delete port from special vlan */
		/*switch(atherosPort)
		{
			case 1:
				shiva_vlan_member_update(0, 4094, 0x43, 0);
				dal_vlan8228[4094].member = 0x43;				
				break;
			case 2:
				shiva_vlan_member_update(0, 4093, 0x45, 0);
				dal_vlan8228[4093].member = 0x45;
				break;
			case 3:
				shiva_vlan_member_update(0, 4092, 0x49, 0);
				dal_vlan8228[4092].member = 0x49;
	
				break;
			case 4:
				shiva_vlan_member_update(0, 4091, 0x51, 0);
				dal_vlan8228[4091].member = 0x51;
				break;
			default:
				break;
		}*/

	}

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
OPL_STATUS dalSetPortPVid(UINT8 portId, UINT16 vid)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	return shiva_port_default_cvid_set(0, atherosPort, vid);
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
OPL_STATUS dalGetPortPVid(UINT8 portId, UINT16 *vid)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	return shiva_port_default_cvid_get(0, atherosPort, vid);
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
OPL_STATUS dalSetPortBaseVlanMember(UINT8 portId, UINT8 *memlist, UINT8 memnum)
{
	UINT8 num;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	for (num = 1; num < memnum; num++)
	{
	    if (memlist[num])
	    {
			shiva_portvlan_member_add(0, atherosPort, 1 << num);
	    }
	}
	
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
OPL_STATUS dalGetPortBaseVlanMember(UINT8 portId, UINT8 *memlist, UINT8 *memnum)
{
	fal_pbmp_t mem_port_map;
	UINT8 num = 0;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	shiva_portvlan_member_get(0, atherosPort, &mem_port_map);

	*memnum = 0;
	while(mem_port_map)
	{
		if (mem_port_map && 0x1)
		{
			memlist[*memnum] = num;
			(*memnum)++;
		}

		mem_port_map = mem_port_map >> 1;
		num++;
	}

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
OPL_STATUS dalSetPortDot1qMode(UINT8 portId, SW_DOT1Q_MODE mode)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	return shiva_port_1qmode_set(0, atherosPort, mode);
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
OPL_STATUS dalGetPortDot1qMode(UINT8 portId, SW_DOT1Q_MODE *mode)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	return shiva_port_1qmode_get(0, atherosPort, mode);
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
OPL_STATUS dalCreateVlanID(UINT16 vid)
{
	int rv;
	
	if (DAL_VLAN_UNUSED != dal_vlan8228[vid].type)
	{
		return OPL_ERROR;
	}
	
	rv = shiva_vlan_create(0, vid);
	if (SW_OK == rv)
	{
		memset(&dal_vlan8228[vid], 0 , sizeof(VTT_VLAN_8228_t));
		dal_vlan8228[vid].type = DAL_VLAN_NORMAL;
		dal_vlan8228[vid].member = 0;
		return OPL_OK;
	}
	else
	{
		return OPL_ERROR;
	}
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
OPL_STATUS dalDeleteVlanID(UINT16 vid)
{
	int rv;
	
	if (DAL_VLAN_UNUSED == dal_vlan8228[vid].type)
	{
		return OPL_ERROR;
	}

	if (DAL_VLAN_NORMAL != dal_vlan8228[vid].type)
	{
		return OPL_ERROR;
	}
	
	if (SW_OK == shiva_vlan_delete(0, vid))
	{
		memset(&dal_vlan8228[vid], 0 , sizeof(VTT_VLAN_8228_t));
		dal_vlan8228[vid].type = DAL_VLAN_UNUSED;
		dal_vlan8228[vid].member = 0;
		
		return OPL_OK;
	}
	else
	{
		return OPL_ERROR;
	}
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
OPL_STATUS dalFlushVlanAll(void)
{
	UINT16	vid;
	int rv;

	for (vid = 0; vid <= DAL_VLAN_VID_MAX; vid++)
	{
		if (DAL_VLAN_UNUSED == dal_vlan8228[vid].type)
		{
			continue;
		}

		if (DAL_VLAN_NORMAL == dal_vlan8228[vid].type)
		{
			rv = shiva_vlan_delete(0, vid);
			if (SW_OK == rv)
			{
				memset(&dal_vlan8228[vid], 0 , sizeof(VTT_VLAN_8228_t));
				dal_vlan8228[vid].type = DAL_VLAN_UNUSED;
				dal_vlan8228[vid].member = 0;
			}
			else
			{
				return OPL_ERROR;
			}
		}
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
OPL_STATUS dalAddVlanPort(UINT8 portId, UINT16 vid, VLAN_EGRESS_TYPE_E type)
{
	fal_vlan_t vlan;
	fal_pbmp_t member;
	fal_pbmp_t u_member = 0;
	int rv;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if (DAL_VLAN_UNUSED == dal_vlan8228[vid].type)
	{
		return OPL_ERROR;
	}

	if (DAL_VLAN_NORMAL != dal_vlan8228[vid].type)
	{
		return OPL_ERROR;
	}
	
	if (SW_OK == shiva_vlan_find(0, vid, &vlan))
	{
		member = vlan.mem_ports;
		member |= ((1 << atherosPort) + 0x40);	/* include GE port */
		
		rv =  shiva_vlan_member_update(0, vid, member, u_member);
		if (SW_OK == rv)
		{
			dal_vlan8228[vid].member = member;
			dal_vlan8228[vid].portId[atherosPort] = 1;
				
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
* dalMoveVlanPort
*
* DESCRIPTION:
*  		this function deletes a port from a vlan.
*
* INPUTS:
*          portId:user logic port.
*          vid:vlan id.
*		
* OUTPUTS:
*		
*  RETURNS:
* 		OPL_OK if the access is success.
* 		OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalMoveVlanPort(UINT8 portId, UINT16 vid)
{
	fal_vlan_t vlan;
	fal_pbmp_t member;
	fal_pbmp_t u_member = 0;	
	int rv;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if (DAL_VLAN_NORMAL != dal_vlan8228[vid].type)
	{
		return OPL_ERROR;
	}
	
	if (SW_OK == shiva_vlan_find(0, vid, &vlan))
	{
		member = vlan.mem_ports;
		member &= ~(1 << atherosPort);

		rv = shiva_vlan_member_update(0, vid, member, u_member);
		if (SW_OK == rv)
		{
			dal_vlan8228[vid].member = member;
			dal_vlan8228[vid].portId[atherosPort] = 0;
			
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



OPL_STATUS dalGetVlanPortEGMode(UINT16 vid, UINT32 portId, VLAN_EGRESS_TYPE_E *type)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	return OPL_OK;
}



OPL_STATUS dalSetPortEgressMode(UINT8 portId, PORT_EGRESS_MODE mode)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	return shiva_port_egvlanmode_set(0, atherosPort, mode);
}



OPL_STATUS dalGetPortEgressMode(UINT8 portId, PORT_EGRESS_MODE *mode)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	return shiva_port_egvlanmode_get(0, atherosPort, mode);
}



OPL_STATUS dalShowChipVlanEntry(void)
{
	return OPL_OK;
}



int dalSetPortDefPri(UINT8 portId, UINT8 pri)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	return OPL_OK;
}



/* delete all vlan used for the port tag mode */
OPL_STATUS dalTagDelvlan(UINT8 portId)
{
	UINT32 vid;
	int rv;
	fal_vlan_t vlan;
	fal_pbmp_t member;
	fal_pbmp_t u_member = 0;	
	UINT32 i, j;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
	for (vid = 0; vid <= DAL_VLAN_VID_MAX; vid++)
	{
		/*DAL_VLAN_MC_VTT is up to IGMP to manage */
		if ((DAL_VLAN_TAG == dal_vlan8228[vid].type) 			
			&& dal_vlan8228[vid].portId[atherosPort])
		{
			if (SW_OK == shiva_vlan_find(0, vid, &vlan))
			{
				member = vlan.mem_ports;
				member &= ~(1 << atherosPort);
			
				rv = shiva_vlan_member_update(0, vid, member, u_member);
				if (SW_OK == rv)
				{
					dal_vlan8228[vid].member = member;
					dal_vlan8228[vid].portId[atherosPort] = 0;
				}
			}

			j = 0;
			for (i = 1; i < 5; i++)
			{
				if (i == atherosPort )
				{
					continue;
				}
				j += dal_vlan8228[vid].portId[i];
			}

			if (0 == j)
			{
				rv = shiva_vlan_delete(0, vid);
				if (SW_OK == rv)
				{
					memset(&dal_vlan8228[vid], 0 , sizeof(VTT_VLAN_8228_t));
					dal_vlan8228[vid].type = DAL_VLAN_UNUSED;
					dal_vlan8228[vid].member = 0;
				}
				else
				{
					return OPL_ERROR;
				}
			}
		}
	}

	return OPL_OK;
}



OPL_STATUS dalTrunkFind(UINT8 portId, UINT32 VlanId)
{	
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	if ((dal_vtt_info[atherosPort].vlanMode == VLAN_TRUNK)
		&& dal_vlan8228[VlanId].portId[atherosPort])
	{
		return OPL_OK;
	}
	else
	{
		return OPL_ERROR;
	}
}



OPL_STATUS dalTrunkEntryAdd(UINT8 portId, UINT32 VlanId)
{
	fal_vlan_t p_vlan;
	fal_pbmp_t member;
	fal_pbmp_t u_member = 0;	
	sw_error_t rv;
	UINT32	pcvid;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	UINT32 ar8228Port, i;

	if (OPL_OK == dalTrunkFind(portId, VlanId))
	{
		return OPL_OK;
	}

	shiva_port_default_cvid_get(0, atherosPort, &pcvid);
	if (VlanId == pcvid)
	{
		return OPL_ERROR;
	}

       /*
	if ((DAL_VLAN_UNUSED != dal_vlan8228[VlanId].type)
		&& (DAL_VLAN_TRUNK != dal_vlan8228[VlanId].type))
	{     
		return OPL_ERROR;
	}
       */

	rv = shiva_vlan_find(0, VlanId, &p_vlan);
	if (SW_OK == rv)
	{
		member = p_vlan.mem_ports | (1 << atherosPort);
	}
	else
	{
		rv = shiva_vlan_create(0, VlanId);
		if (SW_OK != rv)
		{
			return OPL_ERROR;
		}
		member = 0x40 + (1 << atherosPort);
	}

	/* added by cylan for bug3027*/
	for(i = DAL_MIN_PORT_NUM; i <= DAL_MAX_PORT_NUM; i++)
	{
              ar8228Port = dalToAtherosPortMap[i];
		if (ar8228Port == atherosPort)
	       {
	              continue;
	       }

		if (dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSPARENT)
		{
			member |=  (1 << ar8228Port);
			dal_vlan8228[VlanId].portId[ar8228Port] = 1;
		}					
	}
	
	shiva_vlan_member_update(0, VlanId, member, 0);
	dal_vlan8228[VlanId].type = DAL_VLAN_TRUNK;
	dal_vlan8228[VlanId].portId[atherosPort] = 1;
	dal_vlan8228[VlanId].member = member;

	dal_vlan_trunk_num[atherosPort] += 1;

	return OPL_OK;
}



OPL_STATUS dalTrunkEntryDel(UINT8 portId, UINT32 VlanId)
{
	int rv;
	fal_vlan_t vlan;
	fal_pbmp_t member;
	fal_pbmp_t u_member = 0;	
	int i, j, flag;
	UINT32	pcvid;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	UINT32 ar8228Port, temPort;

	shiva_port_default_cvid_get(0, atherosPort, &pcvid);
	if (VlanId == pcvid)
	{
		return OPL_ERROR;
	}

	if (OPL_OK != dalTrunkFind(portId, VlanId))
	{
		return OPL_ERROR;
	}
	
	if (SW_OK == shiva_vlan_find(0, VlanId, &vlan))
	{
		member = vlan.mem_ports;
		member &= ~(1 << atherosPort);

		/* added by cylan for bug3027*/
	      for(i = DAL_MIN_PORT_NUM; i <= DAL_MAX_PORT_NUM; i++)
	      {
                  ar8228Port = dalToAtherosPortMap[i];
		    if (ar8228Port == atherosPort)
	           {
	                  continue;
	           }

		    if (dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSPARENT)
		    {
			    if(~(0x40 | (1 << atherosPort)) & member)
		           {
		               flag = 0;
		               /* this vlan exists on other ports */
			        for(j = DAL_MIN_PORT_NUM; j <= DAL_MAX_PORT_NUM; j++)
			        {
			            temPort = dalToAtherosPortMap[j];
				     if((temPort != ar8228Port) && (temPort != atherosPort))
				     {					 
				         if(((dal_vtt_info[temPort].vlanMode == VLAN_TAG) ||
					     (dal_vtt_info[temPort].vlanMode == VLAN_TRANSLATION) ||
					     (dal_vtt_info[temPort].vlanMode == VLAN_TRUNK)) && dal_vlan8228[VlanId].portId[temPort])
				         {
				              flag = 1;
				         }
				     }
			        }
				 if(flag == 0)
				 {
				     member &=  ~(1 << ar8228Port);
				     dal_vlan8228[VlanId].portId[ar8228Port] = 0;
				 }
		           }
		    }					
	       }
	
		rv = shiva_vlan_member_update(0, VlanId, member, u_member);
		if (SW_OK == rv)
		{
			dal_vlan8228[VlanId].member = member;
			dal_vlan8228[VlanId].portId[atherosPort] = 0;
			dal_vlan_trunk_num[atherosPort] -= 1;			
		}
	}
	else
	{
		return OPL_ERROR;
	}

	j = 0;
	for (i = 1; i < 5; i++)
	{
		j += dal_vlan8228[VlanId].portId[i];
	}

	if (0 == j)
	{
		rv = shiva_vlan_delete(0, VlanId);
		if (SW_OK == rv)
		{
			memset(&dal_vlan8228[VlanId], 0 , sizeof(VTT_VLAN_8228_t));
			dal_vlan8228[VlanId].type = DAL_VLAN_UNUSED;
			dal_vlan8228[VlanId].member = 0;
		}
	}

	return OPL_OK;
}


/* delete all vlan used for the port trunk mode */
OPL_STATUS dalTrunkDelvlan(UINT8 portId)
{
	UINT32 vid;
	
	for (vid = 0; vid <= DAL_VLAN_VID_MAX; vid++)
	{
	       dalTrunkEntryDel(portId, vid);
	}

	return OPL_OK;
}

/*******************************************************************************
* dalTrunkEntryValueGet
*
* DESCRIPTION:
*  		get trunk vlan Id for a certain port
*
*	INPUTS:
*           portId:	user  port[1-4]
*		EntryIndex:	index[0-dal_vlan_trunk_num[portId]]
*		pVlanId:		trunk vlan Id
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/

OPL_STATUS dalTrunkEntryValueGet(UINT8 portId, UINT16 EntryIndex, UINT16 *pVlanId)
{
	UINT32	vid;
	UINT16	tmpIndex = 0;
	UINT32	pcvid;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	
	if ((atherosPort < 1) || (atherosPort > 4))
	{
		return OPL_ERROR;
	}

	if ((EntryIndex >= dal_vlan_trunk_num[atherosPort])
		|| (NULL == pVlanId))
	{
		return OPL_ERROR;
	}

	shiva_port_default_cvid_get(0, atherosPort, &pcvid);
	
	for (vid = 0; vid <= DAL_VLAN_VID_MAX; vid++)
	{
		if (pcvid == vid)
		{
			continue;
		}
		
		if ((DAL_VLAN_TRUNK == dal_vlan8228[vid].type)
			&& dal_vlan8228[vid].portId[atherosPort])
		{
			if (EntryIndex == tmpIndex)
			{
				*pVlanId = vid;
				return OPL_OK;
			}
			
			tmpIndex++;
			if (tmpIndex >= dal_vlan_trunk_num[atherosPort])
			{
				return OPL_ERROR;
			}
		}
	}

	return OPL_ERROR;
}



OPL_STATUS dalTrunkEntryNumGet(UINT8 portId, UINT32 *pNum)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	if (NULL == pNum)
	{
		return OPL_ERROR;
	}

	*pNum = dal_vlan_trunk_num[atherosPort];

	return OPL_OK;
}



OPL_STATUS dalTrunkEntryClear(UINT8 portId)
{
	/* return dalTrunkDelvlan(portId); */
	/* DAL will delete the vlan for trunk mode */

	return OPL_OK;
}



dalVlanShow(UINT32	vid)
{
	fal_vlan_t p_vlan;
	int i,j;

	if (SW_OK == shiva_vlan_find(0, vid, &p_vlan))
	{
		printf("On chip: vid = %d, type = %d, port member = %X, uport = %X\n", 
			vid, dal_vlan8228[vid].type, p_vlan.mem_ports, p_vlan.u_ports);
	}

	j = 0;
	for (i = 0; i < 7; i++)
	{		
		if (dal_vlan8228[vid].portId[i])
		{
			j += (1 << i);
		}
	}

	printf("In  Mem: vid = %d, type = %d, port member = %X, port = %X\n", 
		vid, dal_vlan8228[vid].type, dal_vlan8228[vid].member, j);

	return OPL_OK;
}




dalVlanTrunkShow(UINT8 portId)
{
	UINT16 vid;
	fal_vlan_t p_vlan;
	int i,j;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	for (vid = 0; vid <= DAL_VLAN_VID_MAX; vid++)
	{
		if (DAL_VLAN_TRUNK != dal_vlan8228[vid].type)
		{
			continue;
		}
		
		if (SW_OK == shiva_vlan_find(0, vid, &p_vlan))
		{
			if ((1 << atherosPort) & p_vlan.mem_ports)
			{
				printf("On chip: vid = %d, type = %d, port member = %X, uport = %X\n", 
					vid, dal_vlan8228[vid].type, p_vlan.mem_ports, p_vlan.u_ports);

				j = 0;
				for (i = 0; i < 7; i++)
				{		
					if (dal_vlan8228[vid].portId[i])
					{
						j += (1 << i);
					}
				}

				printf("In  Mem: vid = %d, type = %d, port member = %X, port = %X\n", 
					vid, dal_vlan8228[vid].type, dal_vlan8228[vid].member, j);
			}
		}
	}

	printf("total vlan trunk for port(%d): %d\n", portId, dal_vlan_trunk_num[atherosPort]);
	
	return OPL_OK;
}


