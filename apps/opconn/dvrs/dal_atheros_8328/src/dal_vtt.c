/*
=============================================================================
     Header Name: dal_vtt.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/11/9		Initial Version	
----------------   ------------  ----------------------------------------------
*/

#include <dal_lib.h>
#include <dal_acl.h>
#include "errors.h"
#include "fal_portvlan.h"
#include "fal_vlan.h"
#include "isis_portvlan.h"
#include "mc_control.h"

VLAN_CONFI_INFO_t dal_vtt_info[NUM_OF_PORTS +1];
UINT32 dalVttInitFinished = OPL_ZERO;
VTT_RECORD_8228_t dal_vtt8228[DAL_VTT_8228_NUM];
VTT_VLAN_8228_t	dal_vlan8228[4096];
UINT8	dal_vlan_transl_num[7];
UINT8	dal_vtt_num = 0;

/*******************************************************************************
* dal_vtt_mode_set
*
* DESCRIPTION:
*		opconn support three vlan mode,unmodified mode,tag mode,translation mode.
*	INPUT:
*		mode:0x00:transparent mode,0x01:tag mode,0x02:translation mode.
*
*		 TRANSPARENT mode
*      --------------------------------------------------------------------------------------
*		 direction				|   tag type       | action
*      --------------------------------------------------------------------------------------
*		 							|   with tag        |  forward the ethernet packet without an modification
*      upstream         -------------------------------------------------------------------------
*		 							|   without tag   |  forward the ethernet packet without an modification
*		---------------------------------------------------------------------------------------	
*		 							|   with tag        |  forward the ethernet packet without an modification
*      downstream     -------------------------------------------------------------------------
*		 							|   without tag   |  forward the ethernet packet without an modification
*     ---------------------------------------------------------------------------------------
*
*		 vlan tag mode
*      ---------------------------------------------------------------------------------------
*		 direction				|   tag type       | action
*      ---------------------------------------------------------------------------------------
*		 							|   with tag        |  DROP
*      upstream         -------------------------------------------------------------------------
*		 							|   without tag   |  tagged with default VID
*		---------------------------------------------------------------------------------------	
*		 							|   with tag        |  forward the ethernet packet and strip the tag
*      downstream     -------------------------------------------------------------------------
*		 							|   without tag   |  drop
*     ---------------------------------------------------------------------------------------
*
*		 vlan translation mode
*      ----------------------------------------------------------------------------------------
*		 direction				|   tag type       | action
*      ----------------------------------------------------------------------------------------
*		 							|   with tag        |  lookup the vlan translation table,if there is match entry then replace
*                           |                      |  the vid and forward,else drop the packet. 
*      upstream         ---------------------------------------------------------------------------
*		 							|   without tag   |  tagged with default VID
*		-----------------------------------------------------------------------------------------
*		 							|   with tag        |  lookup the vlan translation table, if there is match entry then replace
*									|                      |  the vid and forward,else drop the packet.		
*      downstream     ---------------------------------------------------------------------------
*		 							|   without tag   |  drop
*     -----------------------------------------------------------------------------------------
*
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttModeSet(UINT8 mode,UINT32 portId)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    dal_vtt_info[atherosPort].vlanMode = mode;
    return OPL_OK;
}

/*******************************************************************************
* dal_vtt_mode_get
*
* DESCRIPTION:
*		opconn support three vlan mode,unmodified mode,tag mode,translation mode.
*
*	INPUT:
*		mode:0x00:transparent mode,0x01:tag mode,0x02:translation mode.
*
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttModeGet(UINT8 *mode,UINT32 portId)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    *mode = dal_vtt_info[atherosPort].vlanMode;
    return OPL_OK;
}



/*******************************************************************************
* dal_vtt_tpid_set
*
* DESCRIPTION:
*		this function is uset to set the tpid to check the vlan.
*	INPUT:
*		tpid:0x8100 ...
*
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttTpidSet(UINT16 tpid,UINT32 portId)
{
    UINT32 atherosPort = dalToAtherosPortMap[portId];
    return isis_nestvlan_tpid_set(0, tpid);
}



/*******************************************************************************
* dal_vtt_tpid_get
*
* DESCRIPTION:
*		this function is uset to get the tpid to check the vlan.
*	INPUT:
*		tpid:0x8100 ...
*
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttTpidGet(UINT16 *tpid,UINT32 portId)
{
	a_uint32_t tmpTpid;
    UINT32 atherosPort = dalToAtherosPortMap[portId];

	if (NULL == tpid)
	{
		return OPL_ERROR;
	}
	
    isis_nestvlan_tpid_get(0, &tmpTpid);

	*tpid = tmpTpid;

	return OPL_OK;
}



/*******************************************************************************
* dal_vtt_vid_set
*
* DESCRIPTION:
*		this function is uset to set the default vlan id for untag packet.
*	INPUT:
*		pvid:1-4095
*
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttVidSet(UINT16 pvid,UINT32 portId)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
    return isis_port_default_cvid_set(0, atherosPort, pvid);
}

	
/*******************************************************************************
* dal_vtt_vid_get
*
* DESCRIPTION:
*		this function is uset to get the default vlan id for untag packet.
*	INPUT:
*
* OUTPUTS:
*   pvid:1-4095
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttVidGet(UINT16 *pvid,UINT32 portId)
{
	a_uint32_t vid;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if (NULL == pvid)
	{
		return OPL_ERROR;
	}
	
    isis_port_default_cvid_get(0, atherosPort, &vid);

	*pvid = vid;

	return OPL_OK;
}



/*******************************************************************************
* dal_vtt_default_pri_set
*
* DESCRIPTION:
*		this function is uset to set the default ether priority  for untag packet.
*	INPUT:
*
* OUTPUTS:
*   etherPri:0-7
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttDefaultPriSet(UINT8  etherPri,UINT32 portId)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	return isis_qos_port_default_cpri_set(0, atherosPort, etherPri);
}



/*******************************************************************************
* dal_vtt_default_pri_get
*
* DESCRIPTION:
*		this function is uset to get the default ether priority  for untag packet.
*	INPUT:
*
* OUTPUTS:
*		etherPri:0-7
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttDefaultPriGet(UINT8  *etherPri,UINT32 portId)
{
	a_uint32_t up;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if (NULL == etherPri)
	{
		return OPL_OK;
	}
	
	isis_qos_port_default_cpri_get(0, atherosPort, &up);

	*etherPri = up;

	return OPL_OK;	
}


OPL_STATUS dalVttFind(UINT32 portId, UINT32 cVlan, UINT32 sVlan, UINT32 *pEntryIndex)
{
	UINT8	i;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	*pEntryIndex = 0xFFF;

	for (i = 0; i < ODM_NUM_OF_VTT; i++)
	{
		if (0 == dal_vtt8228[i].valid)
		{
			*pEntryIndex = i;
			continue;
		}

		if ((atherosPort != dal_vtt8228[i].portId) 
			|| (cVlan != dal_vtt8228[i].o_vid)
			|| (sVlan != dal_vtt8228[i].c_vid))
		{
			continue;
		}

		*pEntryIndex = i;

		return OPL_OK;
	}

	return OPL_ERROR;
}


/* delete vlan translation entry on chip */
OPL_STATUS dalVttDelRec(UINT8 portId)
{
	UINT8	i;
	fal_vlan_trans_entry_t entry;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	for (i = 0; i < DAL_VTT_8228_NUM; i++)
	{
		if (0 == dal_vtt8228[i].valid)
		{
			continue;
		}

		if (atherosPort != dal_vtt8228[i].portId) 
		{
			continue;
		}

		if (DAL_VTT_MC == dal_vtt8228[i].type)
		{
			continue;
		}

		entry.bi_dir = dal_vtt8228[i].bi_dir;
		entry.o_vid = dal_vtt8228[i].o_vid;
		entry.c_vid = dal_vtt8228[i].c_vid;
		entry.s_vid = dal_vtt8228[i].s_vid;
		
		isis_port_vlan_trans_del(0, atherosPort, &entry);
		dal_vtt8228[i].valid = 0;
		dal_vtt_num--;
	}

	return OPL_OK;
}


/* when port's vlan moded changes, its vlan used for vtt should be deleted */
OPL_STATUS dalVttDelVlan(UINT8 portId)
{
	UINT16 vid;
	int rv;
	fal_vlan_t p_vlan;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	for (vid = 0; vid <= DAL_VLAN_VID_MAX; vid++)
	{
		if ((DAL_VLAN_VTT == dal_vlan8228[vid].type) 
			&& dal_vlan8228[vid].portId[atherosPort])
		{
			if (SW_OK == isis_vlan_find(0, vid, &p_vlan))
			{
				if (~(0x40 | (1 << atherosPort)) & p_vlan.mem_ports)
				{
					/* other ports exist */
					p_vlan.mem_ports &= ~(1 << atherosPort);
					//isis_vlan_member_add(0, vid, p_vlan.mem_ports,0);
					isis_vlan_member_del(0, vid, atherosPort);
					dal_vlan8228[vid].member = p_vlan.mem_ports;
					dal_vlan8228[vid].portId[atherosPort] = 0;
				}
				else
				{
					rv = isis_vlan_delete(0, vid);
					if (SW_OK == rv)
					{
						memset(&dal_vlan8228[vid], 0 , sizeof(VTT_VLAN_8228_t));				
						dal_vlan8228[vid].type = DAL_VLAN_UNUSED;
						dal_vlan8228[vid].member = 0;
					}
				}
			}
		}
	}

	return OPL_OK;
}

/*******************************************************************************
* dalVttTagEntryAdd
*
* DESCRIPTION:
*  there are 16 vlan translation table in opconn,and have two types of vlan translation table  one is untag 
*  record ,the others is tag record.
*  this function is used to del a tag record for vlan translation table 
*
* INPUTS:
*	portId	: 	port ID
*	cValn   	:	0-4095:vid in the packet which received from the subscrible port
*     sVlan	:	0-4095:vid at system side.
*
*	    
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
#define ISIS_MAX_VLAN_TRANS  64
OPL_STATUS dalVttTagEntryAdd(UINT8 pStrip, UINT8 gStrip, UINT32 cVlan, 
					UINT32 sVlan,UINT32 portId,UINT32 *pEntryIndex, UINT8 type)
{
	fal_vlan_trans_entry_t entry;
	a_uint32_t vid;
	sw_error_t rv;
	fal_vlan_t p_vlan;
	fal_pbmp_t member;
	fal_pbmp_t u_member = 0;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if (NULL == pEntryIndex || sVlan >= 0xFFF)
	{
		return OPL_BAD_PARA;
	}

	if (ISIS_MAX_VLAN_TRANS <= dal_vtt_num)
	{
		return OPL_FULL;
	}
	
	if (OPL_OK == dalVttFind(portId, cVlan, sVlan, pEntryIndex))
	{
		return OPL_EXIST;
	}
	else
	{
		if ( 0xFFF == *pEntryIndex )
		{
			return OPL_FULL;
		}
	}

	if (DAL_VTT_UC == type)
	{
	/* create internal vlan = svlan, member: GE and portId */
	if ((DAL_VLAN_UNUSED != dal_vlan8228[sVlan].type)
		&& (DAL_VLAN_VTT != dal_vlan8228[sVlan].type) 
		&& (DAL_VLAN_MULTICAST != dal_vlan8228[sVlan].type)
		&& (DAL_VLAN_MC_VTT != dal_vlan8228[sVlan].type))
	{
		return OPL_ERROR;
	}

     
	if (SW_OK == isis_vlan_find(0, sVlan, &p_vlan))
	{
		/* sVlan exists */
		member = p_vlan.mem_ports | (1 << atherosPort);
	}
	else
	{
		if (SW_OK != isis_vlan_create(0, sVlan))
		{
			return OPL_ERROR;
		}
		member = 0x1 | (1 << atherosPort);
		
	}
    
	isis_vlan_member_add(0, sVlan, 0, FAL_EG_UNMODIFIED);
	rv = isis_vlan_member_add(0, sVlan, atherosPort, FAL_EG_UNMODIFIED);
		
	//isis_vlan_member_add(0, sVlan, member, 0);

	dal_vlan8228[sVlan].type = DAL_VLAN_VTT;
	dal_vlan8228[sVlan].member = member;
	dal_vlan8228[sVlan].portId[atherosPort] = 1;
	}

	memset(&entry, 0, sizeof(fal_vlan_trans_entry_t));
	
	/* double direction downsteram */
	entry.bi_dir = 1;
	entry.o_vid = cVlan;
	entry.c_vid = sVlan;
	entry.s_vid = sVlan;
	//entry.s_vid_enable = 1; /*use o_vid<->c_vid.*/
	entry.c_vid_enable = 1;
	entry.o_vid_is_cvid = 1;
//printf("%s o_vid %d, c_vid %d, s_vid %d.\n", __FUNCTION__, entry.o_vid, entry.c_vid, entry.s_vid);
	rv = isis_port_vlan_trans_add(0, atherosPort, &entry);
	if (SW_OK == rv)
	{
		dal_vtt8228[*pEntryIndex].bi_dir = 1;
		dal_vtt8228[*pEntryIndex].o_vid = cVlan;
		dal_vtt8228[*pEntryIndex].c_vid = sVlan;
		dal_vtt8228[*pEntryIndex].s_vid = sVlan;
		dal_vtt8228[*pEntryIndex].valid = 1;
		dal_vtt8228[*pEntryIndex].portId = atherosPort;

		if (DAL_VTT_UC == type)
		{
			dal_vtt8228[*pEntryIndex].type = DAL_VTT_UC;
		}
		else
		{
			dal_vtt8228[*pEntryIndex].type = DAL_VTT_MC;
		}

		if (DAL_VTT_UC == type)
		{
			dal_vlan_transl_num[atherosPort] += 1;
		}

		dal_vtt_num++;
		
		return OPL_OK;
	}
	else
	{
		return rv;
	}
}



/*******************************************************************************
* dal_vtt_tag_entry_get
*
* DESCRIPTION:
*  there are 16 vlan translation table in opconn,and have two types of vlan translation table  one is untag 
*  record ,the others is tag record.
*  this function is used to del a tag record for vlan translation table 
*
* INPUTS:
*        pStrip		: 	0:strip disable,1: strip enable   for pon port.
*        gStrip	 	:  0:strip disable,1: strip enable 	for ge port.
*			cValn   	:	0-4095:vid in the packet which received from the subscrible port
*        sVlan		:	0-4095:vid at system side.
*	    
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttTagEntryGet(UINT8 ValidRecordIndex,UINT32 *cVlan,UINT32 *sVlan,UINT32 portId)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if ((NULL == cVlan) || (NULL == sVlan))
	{
		return OPL_ERROR;
	}

	if (ValidRecordIndex > DAL_VTT_8228_NUM)
	{
		return OPL_ERROR;
	}
	
    if (dal_vtt8228[ValidRecordIndex].valid)
    {
		*cVlan = dal_vtt8228[ValidRecordIndex].o_vid;
		*sVlan = dal_vtt8228[ValidRecordIndex].c_vid;
		return OPL_OK;
    }
	else
	{
		return OPL_ERROR;
	}
}



/*******************************************************************************
* dal_vtt_tag_entry_del
*
* DESCRIPTION:
*  there are 16 vlan translation table in opconn,and have two types of vlan translation table  one is untag 
*  record ,the others is tag record.
*  this function is used to del a tag record for vlan translation table 
*
* INPUTS:
*        pStrip		: 	0:strip disable,1: strip enable   for pon port.
*        gStrip	 	:  0:strip disable,1: strip enable 	for ge port.
*			cValn   	:	0-4095:vid in the packet which received from the subscrible port
*        sVlan		:	0-4095:vid at system side.
*	    
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttTagEntryDel(UINT8 pStrip,UINT8 gStrip,UINT32 cVlan,
			UINT32 sVlan,UINT32 portId,UINT32 *pEntryIndex, UINT8 type)
{
	fal_vlan_trans_entry_t entry;
	a_uint32_t vid;
	sw_error_t rv;
	fal_vlan_t p_vlan;
	fal_pbmp_t member;
	fal_pbmp_t u_member = 0;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if (NULL == pEntryIndex)
	{
		return OPL_ERROR;
	}

	if (OPL_OK != dalVttFind(portId, cVlan, sVlan, pEntryIndex))
	{
		return OPL_OK;
	}

	if ((DAL_VTT_UC == type)
		&& (DAL_VLAN_VTT != dal_vlan8228[sVlan].type))
	{
		return OPL_ERROR;
	}

	if (DAL_VTT_UC == type)
	{
	if (SW_OK == isis_vlan_find(0, sVlan, &p_vlan))
	{
		if (~(0x1 | (1 << atherosPort)) & p_vlan.mem_ports)
		{
			/* other ports exist */
			p_vlan.mem_ports &= ~(1 << atherosPort);
			//shiva_vlan_member_update(0, sVlan, p_vlan.mem_ports,0);
			isis_vlan_member_del(0, sVlan, atherosPort);
			dal_vlan8228[sVlan].member = p_vlan.mem_ports;
			dal_vlan8228[sVlan].portId[atherosPort] = 0;
		}
		else
		{
	           rv = isis_vlan_delete(0, sVlan);
		    if (SW_OK == rv)
			{
				memset(&dal_vlan8228[sVlan], 0 , sizeof(VTT_VLAN_8228_t));				
				dal_vlan8228[sVlan].type = DAL_VLAN_UNUSED;
				dal_vlan8228[sVlan].member = 0;
			}
		}
	}
	}	

	memset(&entry, 0, sizeof(fal_vlan_trans_entry_t));
	
	entry.bi_dir = 1;
	entry.o_vid = cVlan;
	entry.c_vid = sVlan;
	entry.s_vid = sVlan;
	entry.c_vid_enable = 1;
	entry.o_vid_is_cvid = 1;

	rv = isis_port_vlan_trans_del(0, atherosPort, &entry);
	if (SW_OK == rv)
	{
		dal_vtt8228[*pEntryIndex].valid = 0;
		if (DAL_VTT_UC == type)
		{
			dal_vlan_transl_num[atherosPort] -= 1;
		}
		
		dal_vtt_num--;
		
		return OPL_OK;
	}
	else
	{
		return OPL_ERROR;
	}
}



/*******************************************************************************
* dal_vtt_num_of_record_get
*
* DESCRIPTION:
*		this function is used to init the vlan translaion table. both the software database and hardware table.
*
* INPUTS:
*        pStrip		: 	0:strip disable,1: strip enable   for pon port.
*        gStrip	 	:  0:strip disable,1: strip enable 	for ge port.
*			cValn   	:	0-4095:vid in the packet which received from the subscrible port
*        sVlan		:	0-4095:vid at system side.
*	    
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttNumOfRecordGet(UINT8 *numOfRecords,UINT32 portId)
{
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if (atherosPort >= 7 )
	{
		return OPL_ERROR;
	}

	if (NULL == numOfRecords)
	{
		return OPL_ERROR;
	}

	*numOfRecords = dal_vlan_transl_num[atherosPort];
	
	return OPL_OK;   
}



/*******************************************************************************
* dal_vtt_init
*
* DESCRIPTION:
*		this function is used to init the vlan translaion table. both the software database and hardware table.
*
* INPUTS:
*        pStrip		: 	0:strip disable,1: strip enable   for pon port.
*        gStrip	 	:  0:strip disable,1: strip enable 	for ge port.
*			cValn   	:	0-4095:vid in the packet which received from the subscrible port
*        sVlan		:	0-4095:vid at system side.
*	    
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttInit(void)
{
    OPL_STATUS iStatus = OPL_OK;
    UINT32 portId = 0;
	BRG_T_VTT_ENTRY_t pvidVttEntry;

    OPL_MEMSET(&dal_vtt_info[0],0X00,sizeof(VLAN_CONFI_INFO_t)*(NUM_OF_PORTS +1));
	OPL_MEMSET(&pvidVttEntry,0X00,sizeof(BRG_T_VTT_ENTRY_t));

	pvidVttEntry.active = OPL_TRUE;
	pvidVttEntry.g_strip = OPL_TRUE;
	pvidVttEntry.p_strip = OPL_TRUE;
	pvidVttEntry.c_vid = 0XFFF;
	pvidVttEntry.s_vid = 0XFFF;

    iStatus = brgVttInit();

    if(OPL_OK != iStatus)
    {
        return iStatus;
    }

    /*init onu port to transparent*/
    iStatus = brgVttLookupFailActionHwWrite(OPL_TRUE);
    if(OPL_OK != iStatus)
    {
        return iStatus;
    }

	iStatus = brgVttEntryHwWrite(0, (BRG_VTT_ENTRY_t *)&pvidVttEntry);
	if(OPL_OK != iStatus)
    {
        return iStatus;
    }

    OPL_MEMSET(dal_vtt8228, 0, sizeof(VTT_RECORD_8228_t)*DAL_VTT_8228_NUM);

    OPL_MEMSET(dal_vlan8228, 0, sizeof(VTT_VLAN_8228_t) * 4096);

	/* 4095 is used for untag mode */
	isis_vlan_create(0, 4095);
	//isis_vlan_member_add(0, 4095, 0x40, 0);
	isis_vlan_member_add(0, 4095, 0, 0);
	dal_vlan8228[4095].type = DAL_VLAN_VTT;
	dal_vlan8228[4095].member = 0x1;

	for (portId = 0; portId < 7; portId++)
	{
		dal_vlan_transl_num[portId] = 0;
	}

	dal_vtt_num = 0;
	
	return OPL_OK;
}



OPL_STATUS dalVttVlanAggDel(UINT16 egressPort, UINT16 targetVlan, UINT16 aggedVlan,
  UINT8 *aggedMac)
{

	UINT32 entryIndex;
	
	dalVttTagEntryDel(0,0, targetVlan, aggedVlan, egressPort, &entryIndex, DAL_VTT_UC);
	return OPL_OK;
}

/*
 * N:1 VLAN Aggregation mode support
 */
OPL_STATUS dalVttVlanAggAdd(UINT16 portId, UINT16 sVlan, UINT16 cVlan,
  UINT8 *aggedMac, UINT16* ruleId)
{
    fal_vlan_trans_entry_t entry;
	a_uint32_t vid;
	sw_error_t rv;
	fal_vlan_t p_vlan;
	fal_pbmp_t member;
	fal_pbmp_t u_member = 0;
    UINT32  atherosPort = dalToAtherosPortMap[portId];
	UINT32  ar8228Port, i;
	UINT32 entryIndex;

	//return OPL_OK;

	if (sVlan >= 0xFFF)
	{
		return OPL_BAD_PARA;
	}

	if (ISIS_MAX_VLAN_TRANS <= dal_vtt_num)
	{
		return OPL_FULL;
	}
	
	if (OPL_OK == dalVttFind(portId, cVlan, sVlan, &entryIndex))
	{
		return OPL_EXIST;
	}
	else
	{
		if ( 0xFFF == entryIndex )
		{
			return OPL_FULL;
		}
	}

	//if (DAL_VTT_UC == type)
	{
	/* create internal vlan = svlan, member: GE and portId */
	/*
	if ((DAL_VLAN_UNUSED != dal_vlan8228[sVlan].type)
		&& (DAL_VLAN_VTT != dal_vlan8228[sVlan].type) 
		&& (DAL_VLAN_MULTICAST != dal_vlan8228[sVlan].type)
		&& (DAL_VLAN_MC_VTT != dal_vlan8228[sVlan].type))
	{
		return OPL_ERROR;
	}
	*/

	if (SW_OK == isis_vlan_find(0, sVlan, &p_vlan))
	{
		/* sVlan exists */
		member = p_vlan.mem_ports | (1 << atherosPort);
	}
	else
	{
		if (SW_OK != isis_vlan_create_svl(0, sVlan))/*use svl vlan.*/
		{
			return OPL_ERROR;
		}
		member = 0x1 | (1 << atherosPort);
		isis_vlan_member_add(0, sVlan, 0, FAL_EG_UNMODIFIED); /*6 to 0*/
	}
	
	rv = isis_vlan_member_add(0, sVlan, atherosPort, FAL_EG_UNMODIFIED);
       #if 0
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
			dal_vlan8228[sVlan].portId[ar8228Port] = 1;
			isis_vlan_member_add(0, sVlan, ar8228Port, FAL_EG_UNMODIFIED);
		}					
	}
	#endif
	//isis_vlan_member_add(0, sVlan, member, 0);

	dal_vlan8228[sVlan].type = DAL_VLAN_VTT;
	dal_vlan8228[sVlan].member = member;
	dal_vlan8228[sVlan].portId[atherosPort] = 1;
	}

	memset(&entry, 0, sizeof(fal_vlan_trans_entry_t));
	
	/* double direction downsteram */
	entry.forward_dir = 1;
	entry.o_vid = cVlan;
	entry.c_vid = sVlan;
	entry.s_vid = sVlan;
	//entry.s_vid_enable = 1; /*use o_vid<->c_vid.*/
	entry.c_vid_enable = 1;
	entry.o_vid_is_cvid = 1;
	rv = isis_port_vlan_trans_add(0, atherosPort, &entry);
	if (SW_OK == rv)
	{
		dal_vtt8228[entryIndex].bi_dir = 1;
		dal_vtt8228[entryIndex].o_vid = cVlan;
		dal_vtt8228[entryIndex].c_vid = sVlan;
		dal_vtt8228[entryIndex].s_vid = sVlan;
		dal_vtt8228[entryIndex].valid = 1;
		dal_vtt8228[entryIndex].portId = atherosPort;

		dal_vtt8228[entryIndex].type = DAL_VTT_UC;
		dal_vlan_transl_num[atherosPort] += 1;

		dal_vtt_num++;
		
		return OPL_OK;
	}
	else
	{
		return rv;
	}
}



OPL_STATUS dalVttShow(void)
{
  int i;
  BRG_T_VTT_ENTRY_t *e;
  UINT32 val;

  memset(&e, 0, sizeof(e));

  printf("Show Vtt Table:\n");
  for(i = 0; i < 16; i++) {
    oplRegRead((0x3080+i)*4, &val);
    if(val!=0) {
      e = (BRG_T_VTT_ENTRY_t *)&val;
      printf("%d: ACTIVE=%d,P_STRIP=%x,G_STRIP=%x,CVID=%d,SVID=%d\n",
        i, e->active, e->p_strip, e->g_strip, e->c_vid, e->s_vid);
    }
  }

  return 0;
}

OPL_STATUS dalArVttShow(UINT32 portId)
{
	fal_vlan_trans_entry_t entry;
	UINT32	cnt = 0;
	int rv;
	a_uint32_t  iterator = 0;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	printf("Vlan Translation Entry for Port(%d)\n", portId);
	printf("    Ovid     Cvid     Svid     Bi-Direc    forward   reverse\n");
	printf("------------------------------------------------------------\n");
	
	while(1)
	{
		rv = isis_port_vlan_trans_iterate(0, atherosPort, &iterator, &entry);

		if (SW_OK != rv)
		{
			break;
		}

		printf("%8d %8d %8d %8d %8d %8d\n", entry.o_vid, entry.c_vid,
			entry.s_vid, entry.bi_dir, entry.forward_dir, entry.reverse_dir);
		cnt++;
	}

	printf("The number of entry: %d\n", cnt);

	return OPL_OK;
}

