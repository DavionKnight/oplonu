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

sw_error_t
ssdk_hsl_access_mode_set(a_uint32_t dev_id, hsl_access_mode reg_mode)
{
    sw_error_t rv;

    rv = hsl_access_mode_set(dev_id, reg_mode);
    return rv;
}

void config_init_reg()
{

unsigned int value;
fal_port_t portid = 0;

/*配置可以直接配置交换PHY寄存器*/
	isis_reg_get(0,0x3c,&value,4);
	value &= 0xbfff;
	isis_reg_set(0,0x3c,&value,4);

/*配置MIB计数器使能*/
	value = 0x80000301;
	isis_reg_set(0,0x30,&value,4);

/*配置未知报文的转发端口列表(未知报文转发至所有端口)*/
	value = 0x7f7f7f;
	isis_reg_set(0,0x624,&value,4);

       value = 0x40000000;
	isis_reg_set(0,0x10,&value,4);  
	
/*使能CPU口，使能GE0号口的ATHORES head模式*/
	//value = 0x04f0;
       //value = 0x0400;
       value = 0x4002400;
	isis_reg_set(0,0x620,&value,4);

/* header type = 0X9100 in the atheros header, header length = 4 */
       value = 0x19100;
       isis_reg_set(0,0x98,&value,4);	

/* port header ctrl mode for 4 bytes header*/
	value = 0x5;
	isis_reg_set(0,0x9c,&value,4);	

/*配置4个GE口的组播协议报文TRAP到CPU口*/	
	value = 0x01010100;
	isis_reg_set(0,0x210,&value,4);		
	
	value = 0x01000001;
	isis_reg_set(0,0x214,&value,4);		


/*配置交换芯片依据报文的优先级映射QOS队列*/
	for(portid = 1; portid < 5;  portid ++)
	  {
		isis_reg_get(0,0x664 + (portid * 3 * 4),&value,4);
		value |= 0x20000;
		isis_reg_set(0,0x664 + (portid * 3 * 4),&value,4);
	  }

	

	for(portid = 1; portid < 5;  portid ++)
	  {
		isis_reg_get(0,0x970+ (portid * 2 * 4),&value,4);
		value |= 0xffffff;
		isis_reg_set(0,0x970 + (portid * 2 * 4),&value,4);
	  }



/*配置优先级到4条QOS队列的映射关系*/

	value = 0x55443322;/*WAN口映射到高4条队列*/
	isis_reg_set(0,0x810,&value,4);	
	value = 0x33221100;/*LAN口就4条队列*/
	isis_reg_set(0,0x814,&value,4);	



/*配置端口出口限速基于端口，配置模式burst大小为最大*/
	for(portid = 1; portid < 5;  portid ++)
	  {
		value = 0x77777777;
		isis_reg_set(0,0x8a8 + (portid * 8 * 4),&value,4);

		value = 0xa;
		isis_reg_set(0,0x8ac + (portid * 8 * 4),&value,4);		
	  }


/*配置端口入口限速基于端口，配置模式burst大小为最大*/
	for(portid = 1; portid < 5;  portid ++)
	  {
	  	isis_reg_get(0,0xb00 + (portid * 4 * 4),&value,4);
		value |= 0x38000;
		isis_reg_set(0,0xb00 + (portid * 4 * 4),&value,4);

	  	isis_reg_get(0,0xb04 + (portid * 4 * 4),&value,4);
		value |= 0x38000;
		isis_reg_set(0,0xb04 + (portid * 4 * 4),&value,4);	

        isis_reg_get(0,0xb08 + (portid * 4 * 4),&value,4);
		value |= 0xc000;
		isis_reg_set(0,0xb08 + (portid * 4 * 4),&value,4);			
	  }

}

void config_phy_reg()
{

	
	isis_phy_set(0,1,0xd,0x3);
		
	isis_phy_set(0,1,0xe,0x8003);
	
	isis_phy_set(0,1,0xd,0x4003);
	
	isis_phy_set(0,1,0xe,0xdd19);
	
	isis_phy_set(0,1,0xd,0x3);
	
	isis_phy_set(0,1,0xe,0x8005);
	
	isis_phy_set(0,1,0xd,0x4003);
		
	isis_phy_set(0,1,0xe,0xcb32);
	
	isis_phy_set(0,1,0x1d,0x3d);
	
	isis_phy_set(0,1,0x1e,0xe8e0);
	
	isis_phy_set(0,2,0xd,0x3);
		
	isis_phy_set(0,2,0xe,0x8003);
	
	isis_phy_set(0,2,0xd,0x4003);
	
	isis_phy_set(0,2,0xe,0xdd19);
		
	isis_phy_set(0,2,0x1d,0x3d);
	
	isis_phy_set(0,2,0x1e,0xe8e0);	
	
	isis_phy_set(0,2,0xd,0x3);
	
	isis_phy_set(0,2,0xe,0x8005);
	
	isis_phy_set(0,2,0xd,0x4003);
		
	isis_phy_set(0,2,0xe,0xcb32);
	
	isis_phy_set(0,3,0xd,0x3);
		
	isis_phy_set(0,3,0xe,0x8003);
	
	isis_phy_set(0,3,0xd,0x4003);
	
	isis_phy_set(0,3,0xe,0xdd19);
	
	isis_phy_set(0,3,0xd,0x3);
	
	isis_phy_set(0,3,0xe,0x8005);
	
	isis_phy_set(0,3,0xd,0x4003);
		
	isis_phy_set(0,3,0xe,0xcb32);
	
	isis_phy_set(0,3,0x1d,0x3d);
	
	isis_phy_set(0,3,0x1e,0xe8e0);
	
	isis_phy_set(0,4,0xd,0x3);
		
	isis_phy_set(0,4,0xe,0x8003);
	
	isis_phy_set(0,4,0xd,0x4003);
	
	isis_phy_set(0,4,0xe,0xdd19);
	
	
	isis_phy_set(0,4,0xd,0x3);
	
	isis_phy_set(0,4,0xe,0x8005);
	
	isis_phy_set(0,4,0xd,0x4003);
		
	isis_phy_set(0,4,0xe,0xcb32);
	
	isis_phy_set(0,4,0x1d,0x3d);
	
	isis_phy_set(0,4,0x1e,0xe8e0);
	
	isis_phy_set(0,1,0,0x3300);
	
	isis_phy_set(0,2,0,0x3300);
	
	isis_phy_set(0,3,0,0x3300);
		  
	isis_phy_set(0,4,0,0x3300);


}

sw_error_t ath_isis_initACL()
{

#define ACL_TRAP_BPDU_PACKET_TO_CPU_LIST_ID 101
#define ACL_TRAP_LOOPDETECT_PACKET_TO_CPU_LIST_ID 102
#define ACL_PRI_HIGH 10

a_uint32_t list_id;
fal_acl_rule_t rule;
sw_error_t status = OPL_OK;
fal_port_t portid = 0;

	/*使能芯片的ACL功能*/
	status = isis_acl_status_set(0,A_TRUE);
	if(status != SW_OK)
	{
		printf("\n isis_acl_status_set error, return %d! file %s, line %d\n", status,__FILE__,__LINE__);
		return status;
	}

       list_id = ACL_TRAP_BPDU_PACKET_TO_CPU_LIST_ID;
	   
	memset(&rule,0,sizeof(fal_acl_rule_t));

	status = isis_acl_list_creat(0,list_id,ACL_PRI_HIGH);

	if(status != SW_OK)
	{
		printf("\n isis_acl_list_creat error, return %d! file %s, line %d\n", status,__FILE__,__LINE__);
		return status;
	}

	/*根据目的MAC地址来过滤BPDU报文*/
	rule.rule_type = FAL_ACL_RULE_MAC;
	
	FAL_FIELD_FLG_SET((rule.field_flg),FAL_ACL_FIELD_MAC_DA);

	rule.dest_mac_val.uc[0] = 0x01;
	rule.dest_mac_val.uc[1] = 0x80;
	rule.dest_mac_val.uc[2] = 0xc2;
	rule.dest_mac_val.uc[3] = 0x00;
	rule.dest_mac_val.uc[4] = 0x00;
	rule.dest_mac_val.uc[5] = 0x00;

	rule.dest_mac_mask.uc[0] = 0xff;
	rule.dest_mac_mask.uc[1] = 0xff;
	rule.dest_mac_mask.uc[2] = 0xff;
	rule.dest_mac_mask.uc[3] = 0xff;
	rule.dest_mac_mask.uc[4] = 0xff;
	rule.dest_mac_mask.uc[5] = 0xff;

	FAL_ACTION_FLG_SET((rule.action_flg),FAL_ACL_ACTION_RDTCPU);

/*对TRAP到CPU得报文做限速处理*/
	//FAL_ACTION_FLG_SET((rule.action_flg),FAL_ACL_ACTION_POLICER_EN);
	//rule.policer_ptr = 0;
	

	status = isis_acl_rule_add(0,list_id,0,1,&rule);

	if(status != OPL_OK)
	{
		printf("\n isis_acl_rule_add error, return %d! file %s, line %d\n", status,__FILE__,__LINE__);
		return status;
	}


	/*将该规则绑定到所有FE口的INGRESS方向*/
	status = isis_acl_list_bind(0,list_id,FAL_ACL_DIREC_IN,FAL_ACL_BIND_PORT,1);
	if(status != OPL_OK)
	{
		printf("\n isis_acl_list_bind error, return %d! file %s, line %d\n", status,__FILE__,__LINE__);
		return status;
	}

	status = isis_acl_list_bind(0,list_id,FAL_ACL_DIREC_IN,FAL_ACL_BIND_PORT,2);
	if(status != OPL_OK)
	{
		printf("\n isis_acl_list_bind error, return %d! file %s, line %d\n", status,__FILE__,__LINE__);
		return status;
	}

	status = isis_acl_list_bind(0,list_id,FAL_ACL_DIREC_IN,FAL_ACL_BIND_PORT,3);
	if(status != OPL_OK)
	{
		printf("\n isis_acl_list_bind error, return %d! file %s, line %d\n", status,__FILE__,__LINE__);
		return status;
	}

	status = isis_acl_list_bind(0,list_id,FAL_ACL_DIREC_IN,FAL_ACL_BIND_PORT,4);
	if(status != OPL_OK)
	{
		printf("\n isis_acl_list_bind error, return %d! file %s, line %d\n", status,__FILE__,__LINE__);
		return status;
	}

       list_id = ACL_TRAP_LOOPDETECT_PACKET_TO_CPU_LIST_ID;
	memset(&rule,0,sizeof(fal_acl_rule_t));

	status = isis_acl_list_creat(0,list_id,ACL_PRI_HIGH);
	if(status != SW_OK)
	{
		printf("\n isis_acl_list_creat error, return %d! file %s, line %d\n", status,__FILE__,__LINE__);
		return status;
	}

/*根据以太网类型来过滤环回检测报文*/
	rule.rule_type = FAL_ACL_RULE_MAC;
	FAL_FIELD_FLG_SET((rule.field_flg),FAL_ACL_FIELD_MAC_ETHTYPE);
	rule.ethtype_val = 0x8001;
	rule.ethtype_mask = 0xffff;


	FAL_ACTION_FLG_SET((rule.action_flg),FAL_ACL_ACTION_RDTCPU);

/*对TRAP到CPU得报文做限速处理*/
	//FAL_ACTION_FLG_SET((rule.action_flg),FAL_ACL_ACTION_POLICER_EN);
	//rule.policer_ptr = 0;
	
	status = isis_acl_rule_add(0,list_id,0,1,&rule);

	if(status != SW_OK)
	{
		printf("\n isis_acl_rule_add error, return %d! file %s, line %d\n", status,__FILE__,__LINE__);
		return status;
	}

	/*将该规则绑定到所有FE口的INGRESS方向*/

	status = isis_acl_list_bind(0,list_id,FAL_ACL_DIREC_IN,FAL_ACL_BIND_PORT,1);

	if(status != SW_OK)
	{
		printf("\n isis_acl_list_bind error, return %d! file %s, line %d\n", status,__FILE__,__LINE__);
		return status;
	}

	status = isis_acl_list_bind(0,list_id,FAL_ACL_DIREC_IN,FAL_ACL_BIND_PORT,2);

	if(status != SW_OK)
	{
		printf("\n isis_acl_list_bind error, return %d! file %s, line %d\n", status,__FILE__,__LINE__);
		return status;
	}

	status = isis_acl_list_bind(0,list_id,FAL_ACL_DIREC_IN,FAL_ACL_BIND_PORT,3);

	if(status != SW_OK)
	{
		printf("\n isis_acl_list_bind error, return %d! file %s, line %d\n", status,__FILE__,__LINE__);
		return status;
	}

	status = isis_acl_list_bind(0,list_id,FAL_ACL_DIREC_IN,FAL_ACL_BIND_PORT,4);

	if(status != SW_OK)
	{
		printf("\n isis_acl_list_bind error, return %d! file %s, line %d\n", status,__FILE__,__LINE__);
		return status;
	}
	
	return status;
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
	garuda_init_spec_cfg chip_spec_cfg;

       ssdk_init_cfg     cfg = def_init_cfg;
	memset(&cfg, 0, sizeof(ssdk_init_cfg));
       memset(&chip_spec_cfg, 0, sizeof(garuda_init_spec_cfg));

	cfg.cpu_mode = HSL_CPU_2;
	cfg.reg_mode = HSL_MDIO;
	cfg.nl_prot  = 30;
	cfg.chip_type=CHIP_ISIS;
	cfg.chip_spec_cfg = &chip_spec_cfg;

	/* register mdio APIs */
	cfg.reg_func.mdio_set = mdioRegisterWrite;
	cfg.reg_func.mdio_get = mdioRegisterRead;

    retVal = ssdk_init(0, &cfg);	
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("init Atheros switch failed.\n"));
		return retVal;
	}

	/* delete port base vlan members */
	for(port_id = 1; port_id < 5; port_id++){
		/*exclude GE port and CPU port */
		for (i = 1; i < 5; i++){
			isis_portvlan_member_del(0, port_id, i);
		}
		
		isis_portvlan_member_add(0, port_id, 0);
	}

	/* ummodify any tag */
	isis_port_egvlanmode_set(0, 0, FAL_EG_UNMODIFIED);

	/*station address of swith. used as SA in pause frame or other management frames */
	vosHWAddrGet("eth0", &hwAddr[0]);
	isis_reg_set(0, 0x64, (a_uint8_t *) &hwAddr[0], sizeof(a_uint32_t));
	hwAddr[2] = 0;
	hwAddr[3] = 0;
	isis_reg_set(0, 0x60, (a_uint8_t *) &hwAddr[2], sizeof(a_uint32_t));

    /* disable UNI ports filter */
	for(port_id = 1; port_id < 5; port_id++)
	{
		/* IGMP enable */
		isis_port_igmps_status_set(0, port_id, 1);		

		isis_qos_port_tx_buf_nr_set(0, port_id, &bufNumber);
	}

	/* enable ge port multicast leaky */
	isis_port_mc_leaky_set(0, 0, 1);

    /* enable cpu port arp leaky */
    isis_port_arp_leaky_set(0, 0, 1);
    isis_arp_cmd_set(0, FAL_MAC_CPY_TO_CPU);

	isis_mib_status_set(0, 1);
	isis_acl_status_set(0, 1);

	/* switch stag mode */
	//isis_qinq_mode_set(0, FAL_QINQ_STAG_MODE);

	isis_port_invlan_mode_set(0, 0, FAL_INVLAN_ADMIT_ALL);

    /* cpu port should be set to fallback to support igmp,rstp */
    //isis_port_1qmode_set(0, 0, FAL_1Q_FALLBACK);

	/* GE port ingress fallback */
	isis_port_1qmode_set(0, 0, FAL_1Q_FALLBACK);

	/* strip priority tag */
	isis_port_egvlanmode_set(0, 0, FAL_EG_HYBRID);

	/* GE port edge mode */
	isis_port_qinq_role_set(0, 0, FAL_QINQ_EDGE_PORT);

	/* GE port vlan clone and prop */
	isis_port_vlan_propagation_set(0, 0, FAL_VLAN_PROPAGATION_CLONE);
	isis_port_default_cvid_set(0, 0, 0);

	/* disable flow control for GE port/CPU port*/
	opl_port_flowctrl_set(0, 0, 0); /* disable to avoid UNI rate control problem */
	isis_port_duplex_set(0, 0, FAL_FULL_DUPLEX);

	for(port_id = 1; port_id < 5; port_id++)
	{
		isis_port_egvlanmode_set(0, port_id, FAL_EG_HYBRID);
		isis_port_invlan_mode_set(0, port_id, FAL_INVLAN_ADMIT_ALL);
		isis_port_qinq_role_set(0, port_id, FAL_QINQ_EDGE_PORT);
		isis_port_1qmode_set(0, port_id, FAL_1Q_DISABLE);
	}


	/* delete port base vlan members */
	for(port_id = 1; port_id < 5; port_id++)
	{
		/*exclude GE port and CPU port */
		for (i = 1; i < 6; i++)
		{
			isis_portvlan_member_del(0, port_id, i);
		}
		
		isis_portvlan_member_add(0, port_id, 0);
	}

	/* remove CPU from GE port based vlan membership */
	//isis_portvlan_member_del(0, 6, 0);
	isis_port_tls_set(0, 0, 1);
	isis_port_default_svid_set(0, 0, 4095);

	/* ummodify any tag */
	//isis_port_egvlanmode_set(0, 0, FAL_EG_UNMODIFIED);

	/* shiva_rate_port_egrl_set(0, 0, &speed, 1); */

	//isis_storm_ctrl_frame_set(0, 0, FAL_BROADCAST_STORM, 1);   //temp delete by cylan
	//isis_storm_ctrl_frame_set(0, 0, FAL_MULTICAST_STORM, 1);    //temp delete by cylan	
	//isis_storm_ctrl_frame_set(0, 0, FAL_UNICAST_STORM, 1);	   //temp delete by cylan
	/* 1000 means 1k frame per second */
    rate_in_pps = 1000;
	//isis_storm_ctrl_rate_set(0, 0, &rate_in_pps);    //temp delete by cylan

	/*station address of swith. used as SA in pause frame or other management frames */
	//vosHWAddrGet("eth0", &hwAddr[0]);
	//isis_reg_set(0, 0x24, (a_uint8_t *) &hwAddr[0], sizeof(a_uint32_t));
	//isis_reg_set(0, 0x20, (a_uint8_t *) &hwAddr[2], sizeof(a_uint32_t));

	/* to avoid internal buffer problem*/
	//regVal = 0xb000c0;
    //isis_reg_set(0, 0x34, (a_uint8_t *)&regVal, sizeof(a_uint32_t));
	
	//regVal = 0x400050;
    //isis_reg_set(0, 0x38, (a_uint8_t *)&regVal, sizeof(a_uint32_t));

	/* GE port scheduler */
	dalQosGeInit();
	
	for(port_id = 1; port_id < 5; port_id++)
	{
		isis_qos_port_mode_set(0, port_id, FAL_QOS_UP_MODE, 1);
		isis_qos_port_mode_set(0, port_id, FAL_QOS_DSCP_MODE, 1);
	}

    /* enable 10M led control, 0xca35ca35 -> 0xcb35cb35 */
	//regVal = 0xc936c936;
    //isis_reg_set(0, 0xb0, (a_uint8_t *)&regVal, sizeof(a_uint32_t));
	//regVal = 0xcb36cb36;
    //isis_reg_set(0, 0xb4, (a_uint8_t *)&regVal, sizeof(a_uint32_t));

	/* to support bc/mc/uu strom suppression */
    //isis_reg_get(0, 0x30, (a_uint8_t *)&regVal, sizeof(a_uint32_t));
	//regVal |= 0x40000;	/* enable broadcast_drop_en */
   // isis_reg_set(0, 0x30, (a_uint8_t *)&regVal, sizeof(a_uint32_t));

	/* CPU egress rate limiting */
	//dalPortDsPolicingEnableSet(0, 1);
	//dalPortDsPolicingCirSet(0, 2048);

	/* shiva_mgmt_vid_vio_drop_set */
	//isis_mgmt_vid_vio_drop_set(0, 0);     //temp delete by cylan
	isis_igmp_mld_cmd_set(0, FAL_MAC_RDT_TO_CPU);

    /* clear queue buffer for port 5 */
   // regVal = 0x03000000;
  //  isis_reg_set(0, 0x61c, (a_uint8_t *)&regVal, sizeof(a_uint32_t));

    /* update port buffer to 0x28008888 */
    regVal = 0x28008888;
    isis_reg_set(0, 0x978, (a_uint8_t *)&regVal, sizeof(a_uint32_t));
    isis_reg_set(0, 0x980, (a_uint8_t *)&regVal, sizeof(a_uint32_t));
    isis_reg_set(0, 0x988, (a_uint8_t *)&regVal, sizeof(a_uint32_t));
    isis_reg_set(0, 0x990, (a_uint8_t *)&regVal, sizeof(a_uint32_t));

    /* turn off Wirespeed-downgrade */
    for(i = 0; i < 4; i++)
	{
		//mdioRegisterFieldWrite(i, 0x14, 5, 1, 0);
	}

	config_init_reg();

	config_phy_reg();

	ath_isis_initACL();
    
	return SW_OK;

}


