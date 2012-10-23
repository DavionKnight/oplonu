/*
=============================================================================
     file Name: dal_qos.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                		Date              	Description of Changes
----------------   ------------  ----------------------------------------------
 zran 				2009/05/20		Initial Version	
----------------   ------------  ----------------------------------------------
*/

#include <dal_lib.h>
#include <aos_types.h>
#include <fal_qos.h>

OPL_STATUS dalQosGeInit(void)
{
	const a_uint32_t weight [4] = {1,2,4,8};

	isis_qos_port_mode_set(0, 0, FAL_QOS_UP_MODE, 1);
	isis_qos_port_mode_set(0, 0, FAL_QOS_DSCP_MODE, 1);

	/* to support downstream SP mode */
	isis_port_pri_propagation_set(0, 0, 1);
	
	return isis_qos_port_sch_mode_set(0, 0, FAL_SCH_SP_MODE, weight);
}



OPL_STATUS dalQosSchModeSet(a_uint32_t portId, fal_sch_mode_t mode)
{
	a_uint32_t weight [4] = {0};
	fal_sch_mode_t tmpMode;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if ( atherosPort > 6)
	{
		return OPL_ERROR;
	}
	
	isis_qos_port_sch_mode_get(0, atherosPort, &tmpMode, weight);
	
	return isis_qos_port_sch_mode_set(0, atherosPort, mode, weight);
}

OPL_STATUS dalQosSchModeGet(a_uint32_t portId, fal_sch_mode_t * mode)
{
	a_uint32_t weight [4] = {0};
	
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if ( atherosPort > 6)
	{
		return OPL_ERROR;
	}
	
	return isis_qos_port_sch_mode_get(0, atherosPort, mode, weight);
}


OPL_STATUS dalQosSchWtSet(a_uint32_t portId, a_uint32_t weight[])
{
	a_uint32_t tmpWeight [4] = {0};
	fal_sch_mode_t mode;
    UINT32  atherosPort = dalToAtherosPortMap[portId];

	if ( atherosPort > 6)
	{
		return OPL_ERROR;
	}
	
	isis_qos_port_sch_mode_get(0, atherosPort, &mode, tmpWeight);
	
	return isis_qos_port_sch_mode_set(0, atherosPort, mode, weight);
}



OPL_STATUS dalQosTagPri2QSet(a_uint32_t up, fal_queue_t queue)
{
       return 0;
	//return isis_cosmap_up_queue_set(0, up, queue);
}



OPL_STATUS dalQosTagPri2QGet(a_uint32_t up, fal_queue_t * queue)
{
	if (NULL == queue)
	{
		return OPL_ERROR;
	}

	return 0;
	//return isis_cosmap_up_queue_get(0, up, queue);
}



OPL_STATUS dalQosIpPri2QSet(a_uint32_t dscp, fal_queue_t queue)
{
       return 0;
	//return isis_cosmap_dscp_queue_set(0, dscp, queue);
}



OPL_STATUS dalQosIpPri2QGet(a_uint32_t dscp, fal_queue_t * queue)
{
	if (NULL == queue)
	{
		return OPL_ERROR;
	}
       return 0;
	//return isis_cosmap_dscp_queue_set(0, dscp, queue);
}


OPL_STATUS dalQosPortCosRemapSet(a_uint32_t portId, a_uint32_t oldup,a_uint32_t newup)
{
	return 0;
}

OPL_STATUS dalQosSchWtSetTest(a_uint32_t portId, 
	a_uint32_t w0, a_uint32_t w1, a_uint32_t w2, a_uint32_t w3)
{
	a_uint32_t tmpWeight [4] = {0};

	tmpWeight[0] = w0;
	tmpWeight[1] = w1;
	tmpWeight[2] = w2;
	tmpWeight[3] = w3;

	return dalQosSchWtSet(portId, tmpWeight);
}

