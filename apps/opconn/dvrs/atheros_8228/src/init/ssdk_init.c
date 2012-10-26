/*
 * Copyright (c) 2007-2008 Atheros Communications, Inc.
 * All rights reserved.
 *
 */

#include "sw.h"
#include "ssdk_init.h"
#include "fal_init.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "opl_driver.h"

extern OPL_STATUS mdioRegisterRead(UINT8 deviceAddr,UINT8 regAddr,UINT16 *data0);
extern OPL_STATUS mdioRegisterWrite(UINT8 deviceAddr,UINT8 regAddr,UINT16 data0);
extern OPL_STATUS mdioRegisterFieldRead(UINT8 deviceAddr,UINT8 regAddr,UINT8 offset,UINT8 width,UINT16 *data0);
extern OPL_STATUS mdioRegisterFieldWrite(UINT8 deviceAddr,UINT8 regAddr,UINT8 offset,UINT8 width,UINT16 data0);

sw_error_t
ssdk_init(a_uint32_t dev_id, ssdk_init_cfg * cfg)
{
    sw_error_t rv;

#if (defined(KERNEL_MODULE) && defined(USER_MODE))
    rv = hsl_dev_init(dev_id, cfg);
#else
#ifdef HSL_STANDALONG
    rv = hsl_dev_init(dev_id, cfg);
#else
    rv = fal_init(dev_id, cfg);
#endif
#endif

    return rv;
}

sw_error_t
ssdk_reduced_init(a_uint32_t dev_id, hsl_init_mode cpu_mode,
                  hsl_access_mode reg_mode)
{
    sw_error_t rv;

#if (defined(KERNEL_MODULE) && defined(USER_MODE))
    rv = hsl_dev_reduced_init(dev_id, cpu_mode, reg_mode);
#else
#ifdef HSL_STANDALONG
    rv = hsl_dev_reduced_init(dev_id, cpu_mode, reg_mode);
#else
    rv = fal_reduced_init(dev_id, cpu_mode, reg_mode);
#endif
#endif

    return rv;
}

sw_error_t
ssdk_cleanup(void)
{
    sw_error_t rv;

#if (defined(KERNEL_MODULE) && defined(USER_MODE))
    rv = hsl_dev_cleanup();
#else
#ifdef HSL_STANDALONG
    rv = hsl_dev_cleanup();
#else
    rv = fal_cleanup();
#endif
#endif

    return rv;
}


sw_error_t	atheros_init()
{
	sw_error_t retVal = OPL_OK;
	UINT8	port_id, i;
	a_uint32_t speed = 32 * 32;
	a_uint32_t rate_in_pps = 0;
	UINT8	hwAddr[6];
	a_uint32_t	regVal;
	a_uint32_t bufNumber = 0x80;

    ssdk_init_cfg     cfg = def_init_cfg;

	/* register mdio APIs */
	cfg.reg_func.mdio_set = mdioRegisterWrite;
	cfg.reg_func.mdio_get = mdioRegisterRead;
	cfg.reg_mode = HSL_MDIO;
    retVal = ssdk_init(0, &cfg);	
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("init Atheros switch failed.\n"));
		return retVal;
	}

    /* disable all uni port while initialize */
    for (port_id = 1; port_id < 5; port_id++)
    {
		//shiva_port_shutdown_set(0, port_id, 1);
		shiva_port_shutdown_set_only(0, port_id, 1);
    }

	/* enable CPU header function */
	shiva_port_hdr_status_set(0, 0, 1);

	shiva_cpu_port_status_set(0, 1);

    /* disable UNI ports filter */
	for(port_id = 1; port_id < 5; port_id++)
	{
		shiva_port_bc_filter_set(0, port_id, 0);
		shiva_port_unk_uc_filter_set(0, port_id, 0);
		shiva_port_unk_mc_filter_set(0, port_id, 0);

		/* to fne tune rate limiting */
		shiva_rate_add_byte_set(0, port_id, 24);

		/* IGMP enable */
		shiva_port_igmps_status_set(0, port_id, 1);		

		shiva_qos_port_tx_buf_nr_set(0, port_id, &bufNumber);
	}

	/* to fne tune rate limiting */
	shiva_rate_add_byte_set(0, 0, 4);
	shiva_rate_add_byte_set(0, 6, 4);

	/* unknown bc,mc,uc filter for GE port */
	shiva_port_bc_filter_set(0, 6, 0);	/* the 2nd 0 means BC frame could be sent by GE */

    /* unknown bc,mc,uc filter for CPU port */
	shiva_port_bc_filter_set(0, 0, 0);
    shiva_port_unk_uc_filter_set(0, 0, 1);
	shiva_port_unk_mc_filter_set(0, 0, 1);
    /* enable cpu port arp leaky */
    shiva_port_arp_leaky_set(0, 0, 1);
    shiva_arp_status_set(0, 1);
    shiva_arp_cmd_set(0, FAL_MAC_CPY_TO_CPU);

	shiva_mib_status_set(0, 1);
	shiva_acl_status_set(0, 1);

	/* switch stag mode */
	shiva_qinq_mode_set(0, FAL_QINQ_STAG_MODE);

	shiva_port_invlan_mode_set(0, 6, FAL_INVLAN_ADMIT_ALL);

    /* cpu port should be set to fallback to support igmp,rstp */
    shiva_port_1qmode_set(0, 0, FAL_1Q_FALLBACK);

	/* GE port ingress fallback */
	shiva_port_1qmode_set(0, 6, FAL_1Q_FALLBACK);

	/* strip priority tag */
	shiva_port_egvlanmode_set(0, 6, FAL_EG_HYBRID);

	/* GE port edge mode */
	shiva_port_qinq_role_set(0, 6, FAL_QINQ_EDGE_PORT);

	/* GE port vlan clone and prop */
	shiva_port_vlan_propagation_set(0, 6, FAL_VLAN_PROPAGATION_CLONE);
	shiva_port_default_cvid_set(0, 6, 0);

	/* enable flow control for GE port*/
	shiva_port_flowctrl_set(0, 6, 0); /* disable to avoid UNI rate control problem */

	for(port_id = 1; port_id < 5; port_id++)
	{
		shiva_port_egvlanmode_set(0, port_id, FAL_EG_HYBRID);
		shiva_port_invlan_mode_set(0, port_id, FAL_INVLAN_ADMIT_ALL);
		shiva_port_qinq_role_set(0, port_id, FAL_QINQ_EDGE_PORT);
		shiva_port_1qmode_set(0, port_id, FAL_1Q_DISABLE);
	}


	/* delete port base vlan members */
	for(port_id = 1; port_id < 5; port_id++)
	{
		/*exclude GE port and CPU port */
		for (i = 1; i < 6; i++)
		{
			shiva_portvlan_member_del(0, port_id, i);
		}
		
		shiva_portvlan_member_add(0, port_id, 0);
	}

	/* remove CPU from GE port based vlan membership */
	shiva_portvlan_member_del(0, 6, 0);
	shiva_port_tls_set(0, 0, 1);
	shiva_port_default_svid_set(0, 6, 4095);

	/* ummodify any tag */
	shiva_port_egvlanmode_set(0, 0, FAL_EG_UNMODIFIED);

	/* shiva_rate_port_egrl_set(0, 0, &speed, 1); */

	shiva_storm_ctrl_frame_set(0, 0, FAL_BROADCAST_STORM, 1);
	shiva_storm_ctrl_frame_set(0, 0, FAL_MULTICAST_STORM, 1);	
	shiva_storm_ctrl_frame_set(0, 0, FAL_UNICAST_STORM, 1);	
	/* 1000 means 1k frame per second */
    rate_in_pps = 1000;
	shiva_storm_ctrl_rate_set(0, 0, &rate_in_pps);

	/* disable flow control for CPU port*/
	shiva_port_flowctrl_set(0, 0, 0);
	shiva_port_duplex_set(0, 0, FAL_FULL_DUPLEX);

	/*station address of swith. used as SA in pause frame or other management frames */
	vosHWAddrGet("eth0", &hwAddr[0]);
	shiva_reg_set(0, 0x24, (a_uint8_t *) &hwAddr[0], sizeof(a_uint32_t));
	shiva_reg_set(0, 0x20, (a_uint8_t *) &hwAddr[2], sizeof(a_uint32_t));

	/* to avoid internal buffer problem*/
	regVal = 0xb000c0;
    shiva_reg_set(0, 0x34, (a_uint8_t *)&regVal, sizeof(a_uint32_t));
	
	regVal = 0x400050;
    shiva_reg_set(0, 0x38, (a_uint8_t *)&regVal, sizeof(a_uint32_t));

	/* GE port scheduler */
	dalQosGeInit();
	
	for(port_id = 1; port_id < 5; port_id++)
	{
		shiva_qos_port_mode_set(0, port_id, FAL_QOS_UP_MODE, 1);
		shiva_qos_port_mode_set(0, port_id, FAL_QOS_DSCP_MODE, 1);
	}

    /* enable 10M led control, 0xca35ca35 -> 0xcb35cb35 */
/*	regVal = 0xc936c936;*/
	regVal = 0xcb35cb35;
    shiva_reg_set(0, 0xb0, (a_uint8_t *)&regVal, sizeof(a_uint32_t));
/*	regVal = 0xcb36cb36;*/
	regVal = 0xcb05cb05;
    shiva_reg_set(0, 0xb4, (a_uint8_t *)&regVal, sizeof(a_uint32_t));

	/* to support bc/mc/uu strom suppression */
    shiva_reg_get(0, 0x30, (a_uint8_t *)&regVal, sizeof(a_uint32_t));
	regVal |= 0x40000;	/* enable broadcast_drop_en */
    shiva_reg_set(0, 0x30, (a_uint8_t *)&regVal, sizeof(a_uint32_t));

	/* CPU egress rate limiting */
	dalPortDsPolicingEnableSet(0, 1);
	dalPortDsPolicingCirSet(0, 2048);

	/* shiva_mgmt_vid_vio_drop_set */
	shiva_mgmt_vid_vio_drop_set(0, 0);
	shiva_igmp_mld_cmd_set(0, FAL_MAC_RDT_TO_CPU);
	shiva_port_unk_mc_filter_set(0, 6, 1);

    /* clear queue buffer for port 5 */
    regVal = 0x03000000;
    shiva_reg_set(0, 0x61c, (a_uint8_t *)&regVal, sizeof(a_uint32_t));

    /* update port buffer to 0x2334aaaa */
    regVal = 0x2334aaaa;
    shiva_reg_set(0, 0x21c, (a_uint8_t *)&regVal, sizeof(a_uint32_t));
    shiva_reg_set(0, 0x31c, (a_uint8_t *)&regVal, sizeof(a_uint32_t));
    shiva_reg_set(0, 0x41c, (a_uint8_t *)&regVal, sizeof(a_uint32_t));
    shiva_reg_set(0, 0x51c, (a_uint8_t *)&regVal, sizeof(a_uint32_t));
    #if 0
    /* clear queue buffer for port 5 */
    regVal = 0x03000000;
    shiva_reg_set(0, 0x61c, (a_uint8_t *)&regVal, sizeof(a_uint32_t));

    /* update port buffer to 0x2334aaaa */
    regVal = 0x2334aaaa;
    shiva_reg_set(0, 0x21c, (a_uint8_t *)&regVal, sizeof(a_uint32_t));
    shiva_reg_set(0, 0x31c, (a_uint8_t *)&regVal, sizeof(a_uint32_t));
    shiva_reg_set(0, 0x41c, (a_uint8_t *)&regVal, sizeof(a_uint32_t));
    shiva_reg_set(0, 0x51c, (a_uint8_t *)&regVal, sizeof(a_uint32_t));
    #endif

    /* turn off Wirespeed-downgrade */
    for(i = 0; i < 4; i++)
	{
		mdioRegisterFieldWrite(i, 0x14, 5, 1, 0);
	}

    /* Modified global MAC_CTRL0 Register, global IPG from 0x60 to 0x58 */
    regVal = 0x58;
    shiva_reg_set(0, 0xe8, (a_uint8_t *)&regVal, sizeof(a_uint32_t));

	return SW_OK;
}

