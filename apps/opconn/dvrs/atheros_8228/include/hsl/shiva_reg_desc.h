/*
 * Copyright (c) 2007-2009 Atheros Communications, Inc.
 * All rights reserved.
 *
 */
 
#ifndef _SHIVA_REG_DESC_H_
#define _SHIVA_REG_DESC_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "reg_desc.h"
#include "shiva_reg.h"

static sw_reg_desc_t reg_desc[] = {
    sw_reg_entry_des(MASK_CTL),
    sw_reg_entry_des(GLOBAL_INT),
    sw_reg_entry_des(GLOBAL_INT_MASK),
    sw_reg_entry_des(GLOBAL_MAC_ADDR0),
    sw_reg_entry_des(GLOBAL_MAC_ADDR1),
    sw_reg_entry_des(FLOOD_MASK),
    sw_reg_entry_des(GLOBAL_CTL),
    sw_reg_entry_des(FLOW_CTL0),
    sw_reg_entry_des(FLOW_CTL1),
    sw_reg_entry_des(QM_CTL),
    sw_reg_entry_des(VLAN_TABLE_FUNC0),
    sw_reg_entry_des(VLAN_TABLE_FUNC1),
    sw_reg_entry_des(ADDR_TABLE_FUNC0),
    sw_reg_entry_des(ADDR_TABLE_FUNC1),
    sw_reg_entry_des(ADDR_TABLE_FUNC2),
    sw_reg_entry_des(ADDR_TABLE_CTL),
    sw_reg_entry_des(IP_PRI_MAPPING0),
    sw_reg_entry_des(IP_PRI_MAPPING1),
    sw_reg_entry_des(IP_PRI_MAPPING2),
    sw_reg_entry_des(IP_PRI_MAPPING3),
    sw_reg_entry_des(TAG_PRI_MAPPING),
    sw_reg_entry_des(SERVICE_TAG),
    sw_reg_entry_des(CPU_PORT),
    sw_reg_entry_des(MIB_FUNC),
    sw_reg_entry_des(PORT_STATUS),
    sw_reg_entry_des(PORT_CTL),
    sw_reg_entry_des(PORT_DOT1AD),
    sw_reg_entry_des(PORT_BASE_VLAN),
    sw_reg_entry_des(RATE_LIMIT0),
    sw_reg_entry_des(PRI_CTL),
    sw_reg_entry_des(STORM_CTL),
    sw_reg_entry_des(QUEUE_CTL),
    sw_reg_entry_des(RATE_LIMIT1),
    sw_reg_entry_des(RATE_LIMIT2),
};

static sw_field_desc_t field_desc[] = {
    /* Garuda Mask Control Register */
    sw_reg_field_des(MASK_CTL, SOFT_RST),
    sw_reg_field_des(MASK_CTL, MII_CLK5_SEL),
    sw_reg_field_des(MASK_CTL, MII_CLK0_SEL),
    sw_reg_field_des(MASK_CTL, LOAD_EEPROM),
    sw_reg_field_des(MASK_CTL, DEVICE_ID),
    sw_reg_field_des(MASK_CTL, REV_ID),

    /* Global Interrupt Register */
    sw_reg_field_des(GLOBAL_INT, GLB_QM_ERR_CNT),
    sw_reg_field_des(GLOBAL_INT, GLB_LOOP_CHECK),
    sw_reg_field_des(GLOBAL_INT, GLB_LOOKUP_ERR),
    sw_reg_field_des(GLOBAL_INT, GLB_QM_ERR),
    sw_reg_field_des(GLOBAL_INT, GLB_HW_INI_DONE),
    sw_reg_field_des(GLOBAL_INT, GLB_MIB_INI),
    sw_reg_field_des(GLOBAL_INT, GLB_MIB_DONE),
    sw_reg_field_des(GLOBAL_INT, GLB_BIST_DONE),
    sw_reg_field_des(GLOBAL_INT, GLB_VT_MISS_VIO),
    sw_reg_field_des(GLOBAL_INT, GLB_VT_MEM_VIO),
    sw_reg_field_des(GLOBAL_INT, GLB_VT_DONE),
    sw_reg_field_des(GLOBAL_INT, GLB_QM_INI),
    sw_reg_field_des(GLOBAL_INT, GLB_AT_INI),
    sw_reg_field_des(GLOBAL_INT, GLB_ARL_FULL),
    sw_reg_field_des(GLOBAL_INT, GLB_ARL_DONE),
    sw_reg_field_des(GLOBAL_INT, GLB_MDIO_DONE),
    sw_reg_field_des(GLOBAL_INT, GLB_PHY_INT),
    sw_reg_field_des(GLOBAL_INT, GLB_EEPROM_ERR),
    sw_reg_field_des(GLOBAL_INT, GLB_EEPROM_INT),

    /* Global Interrupt Mask Register */
    sw_reg_field_des(GLOBAL_INT_MASK, GLBM_LOOP_CHECK),
    sw_reg_field_des(GLOBAL_INT_MASK, GLBM_LOOKUP_ERR),
    sw_reg_field_des(GLOBAL_INT_MASK, GLBM_QM_ERR),
    sw_reg_field_des(GLOBAL_INT_MASK, GLBM_HW_INI_DONE),
    sw_reg_field_des(GLOBAL_INT_MASK, GLBM_MIB_INI),
    sw_reg_field_des(GLOBAL_INT_MASK, GLBM_MIB_DONE),
    sw_reg_field_des(GLOBAL_INT_MASK, GLBM_BIST_DONE),
    sw_reg_field_des(GLOBAL_INT_MASK, GLBM_VT_MISS_VIO),
    sw_reg_field_des(GLOBAL_INT_MASK, GLBM_VT_MEM_VIO),
    sw_reg_field_des(GLOBAL_INT_MASK, GLBM_VT_DONE),
    sw_reg_field_des(GLOBAL_INT_MASK, GLBM_QM_INI),
    sw_reg_field_des(GLOBAL_INT_MASK, GLBM_AT_INI),
    sw_reg_field_des(GLOBAL_INT_MASK, GLBM_ARL_FULL),
    sw_reg_field_des(GLOBAL_INT_MASK, GLBM_ARL_DONE),
    sw_reg_field_des(GLOBAL_INT_MASK, GLBM_MDIO_DONE),
    sw_reg_field_des(GLOBAL_INT_MASK, GLBM_PHY_INT),
    sw_reg_field_des(GLOBAL_INT_MASK, GLBM_EEPROM_ERR),
    sw_reg_field_des(GLOBAL_INT_MASK, GLBM_EEPROM_INT),    

    /* Global MAC Address Register */
    sw_reg_field_des(GLOBAL_MAC_ADDR0, GLB_BYTE4),
    sw_reg_field_des(GLOBAL_MAC_ADDR0, GLB_BYTE5),
    sw_reg_field_des(GLOBAL_MAC_ADDR1, GLB_BYTE0),
    sw_reg_field_des(GLOBAL_MAC_ADDR1, GLB_BYTE1),
    sw_reg_field_des(GLOBAL_MAC_ADDR1, GLB_BYTE2),
    sw_reg_field_des(GLOBAL_MAC_ADDR1, GLB_BYTE3),

    /* Flood Mask Register */
    sw_reg_field_des(FLOOD_MASK, BC_FLOOD_DP),
    sw_reg_field_des(FLOOD_MASK, ARL_MUL_LEAKY),
    sw_reg_field_des(FLOOD_MASK, ARL_UNI_LEAKY),
    sw_reg_field_des(FLOOD_MASK, MUL_FLOOD_DP),
    sw_reg_field_des(FLOOD_MASK, UNI_FLOOD_DP),

    /* Global Control Register */
    sw_reg_field_des(GLOBAL_CTL, RATE_DROP_EN),
    sw_reg_field_des(GLOBAL_CTL, QM_PRI_MODE),
    sw_reg_field_des(GLOBAL_CTL, RATE_CRE_LIMIT),
    sw_reg_field_des(GLOBAL_CTL, RATE_TIME_SLOT),
    sw_reg_field_des(GLOBAL_CTL, RELOAD_TIMER),
    sw_reg_field_des(GLOBAL_CTL, QM_CNT_LOCK),
    sw_reg_field_des(GLOBAL_CTL, BROAD_DROP_EN),
    sw_reg_field_des(GLOBAL_CTL, MAX_FRAME_SIZE),

    /* Flow Control Register */
    sw_reg_field_des(FLOW_CTL0, TEST_PAUSE),
    sw_reg_field_des(FLOW_CTL0, GOL_PAUSE_OFF_THRES),
    sw_reg_field_des(FLOW_CTL0, GOL_PAUSE_ON_THRES),
    sw_reg_field_des(FLOW_CTL1, PORT_PAUSE_OFF_THRES),
    sw_reg_field_des(FLOW_CTL1, PORT_PAUSE_ON_THRES),

    /* QM control Register */
    sw_reg_field_des(QM_CTL, QM_ERR_RST_EN),
    sw_reg_field_des(QM_CTL, LOOKUP_ERR_RST_EN),
    sw_reg_field_des(QM_CTL, IGMP_COPY_EN),
    sw_reg_field_des(QM_CTL, QM_FUNC_TEST),
    sw_reg_field_des(QM_CTL, MS_FC_EN),
    sw_reg_field_des(QM_CTL, FLOW_DROP_EN),
    sw_reg_field_des(QM_CTL, FLOW_DROP_CNT),

    /* Vlan Table Function Register */
    sw_reg_field_des(VLAN_TABLE_FUNC0, VT_PRI_EN),
    sw_reg_field_des(VLAN_TABLE_FUNC0, VT_PRI),
    sw_reg_field_des(VLAN_TABLE_FUNC0, VLAN_ID),
    sw_reg_field_des(VLAN_TABLE_FUNC0, VT_PORT_NUM),
    sw_reg_field_des(VLAN_TABLE_FUNC0, VT_FULL_VIO),
    sw_reg_field_des(VLAN_TABLE_FUNC0, VT_BUSY),
    sw_reg_field_des(VLAN_TABLE_FUNC0, VT_FUNC),

    sw_reg_field_des(VLAN_TABLE_FUNC1, VT_VALID),
    sw_reg_field_des(VLAN_TABLE_FUNC1, VID_MEM),

    /* Address Table Function Register */
    sw_reg_field_des(ADDR_TABLE_FUNC0, AT_ADDR_BYTE4),
    sw_reg_field_des(ADDR_TABLE_FUNC0, AT_ADDR_BYTE5),
    sw_reg_field_des(ADDR_TABLE_FUNC0, AT_FULL_VIO),
    sw_reg_field_des(ADDR_TABLE_FUNC0, AT_PORT_NUM),
    sw_reg_field_des(ADDR_TABLE_FUNC0, AT_BUSY),
    sw_reg_field_des(ADDR_TABLE_FUNC0, AT_FUNC),
    
    sw_reg_field_des(ADDR_TABLE_FUNC1, AT_ADDR_BYTE0),
    sw_reg_field_des(ADDR_TABLE_FUNC1, AT_ADDR_BYTE1),
    sw_reg_field_des(ADDR_TABLE_FUNC1, AT_ADDR_BYTE2),
    sw_reg_field_des(ADDR_TABLE_FUNC1, AT_ADDR_BYTE3),
    
    sw_reg_field_des(ADDR_TABLE_FUNC2, COPY_TO_CPU),
    sw_reg_field_des(ADDR_TABLE_FUNC2, REDRCT_TO_CPU),
    sw_reg_field_des(ADDR_TABLE_FUNC2, LEAKY_EN),
    sw_reg_field_des(ADDR_TABLE_FUNC2, AT_STATUS),
    sw_reg_field_des(ADDR_TABLE_FUNC2, SA_DROP_EN),
    sw_reg_field_des(ADDR_TABLE_FUNC2, MIRROR_EN),
    sw_reg_field_des(ADDR_TABLE_FUNC2, AT_PRI_EN),
    sw_reg_field_des(ADDR_TABLE_FUNC2, AT_PRI),
    sw_reg_field_des(ADDR_TABLE_FUNC2, DES_PORT),

    /* Address Table Control Register */
    sw_reg_field_des(ADDR_TABLE_CTL, ARL_INI_EN),
    sw_reg_field_des(ADDR_TABLE_CTL, LEARN_CHANGE_EN),
    sw_reg_field_des(ADDR_TABLE_CTL, AGE_EN),
    sw_reg_field_des(ADDR_TABLE_CTL, AGE_TIME),

    /* IP Priority Mapping Register */
    sw_reg_field_des(IP_PRI_MAPPING0, IP_0X3C),
    sw_reg_field_des(IP_PRI_MAPPING0, IP_0X38),
    sw_reg_field_des(IP_PRI_MAPPING0, IP_0X34),
    sw_reg_field_des(IP_PRI_MAPPING0, IP_0X30),
    sw_reg_field_des(IP_PRI_MAPPING0, IP_0X2C),
    sw_reg_field_des(IP_PRI_MAPPING0, IP_0X28),
    sw_reg_field_des(IP_PRI_MAPPING0, IP_0X24),
    sw_reg_field_des(IP_PRI_MAPPING0, IP_0X20),
    sw_reg_field_des(IP_PRI_MAPPING0, IP_0X1C),
    sw_reg_field_des(IP_PRI_MAPPING0, IP_0X18),
    sw_reg_field_des(IP_PRI_MAPPING0, IP_0X14),
    sw_reg_field_des(IP_PRI_MAPPING0, IP_0X10),
    sw_reg_field_des(IP_PRI_MAPPING0, IP_0X0C),
    sw_reg_field_des(IP_PRI_MAPPING0, IP_0X08),
    sw_reg_field_des(IP_PRI_MAPPING0, IP_0X04),
    sw_reg_field_des(IP_PRI_MAPPING0, IP_0X00),
    
    sw_reg_field_des(IP_PRI_MAPPING1, IP_0X7C),
    sw_reg_field_des(IP_PRI_MAPPING1, IP_0X78),
    sw_reg_field_des(IP_PRI_MAPPING1, IP_0X74),
    sw_reg_field_des(IP_PRI_MAPPING1, IP_0X70),
    sw_reg_field_des(IP_PRI_MAPPING1, IP_0X6C),
    sw_reg_field_des(IP_PRI_MAPPING1, IP_0X68),
    sw_reg_field_des(IP_PRI_MAPPING1, IP_0X64),
    sw_reg_field_des(IP_PRI_MAPPING1, IP_0X60),
    sw_reg_field_des(IP_PRI_MAPPING1, IP_0X5C),
    sw_reg_field_des(IP_PRI_MAPPING1, IP_0X58),
    sw_reg_field_des(IP_PRI_MAPPING1, IP_0X54),
    sw_reg_field_des(IP_PRI_MAPPING1, IP_0X50),
    sw_reg_field_des(IP_PRI_MAPPING1, IP_0X4C),
    sw_reg_field_des(IP_PRI_MAPPING1, IP_0X48),
    sw_reg_field_des(IP_PRI_MAPPING1, IP_0X44),
    sw_reg_field_des(IP_PRI_MAPPING1, IP_0X40),
    
    sw_reg_field_des(IP_PRI_MAPPING2, IP_0XBC),
    sw_reg_field_des(IP_PRI_MAPPING2, IP_0XB8),
    sw_reg_field_des(IP_PRI_MAPPING2, IP_0XB4),
    sw_reg_field_des(IP_PRI_MAPPING2, IP_0XB0),
    sw_reg_field_des(IP_PRI_MAPPING2, IP_0XAC),
    sw_reg_field_des(IP_PRI_MAPPING2, IP_0XA8),
    sw_reg_field_des(IP_PRI_MAPPING2, IP_0XA4),
    sw_reg_field_des(IP_PRI_MAPPING2, IP_0XA0),
    sw_reg_field_des(IP_PRI_MAPPING2, IP_0X9C),
    sw_reg_field_des(IP_PRI_MAPPING2, IP_0X98),
    sw_reg_field_des(IP_PRI_MAPPING2, IP_0X94),
    sw_reg_field_des(IP_PRI_MAPPING2, IP_0X90),
    sw_reg_field_des(IP_PRI_MAPPING2, IP_0X8C),
    sw_reg_field_des(IP_PRI_MAPPING2, IP_0X88),
    sw_reg_field_des(IP_PRI_MAPPING2, IP_0X84),
    sw_reg_field_des(IP_PRI_MAPPING2, IP_0X80),
    
    sw_reg_field_des(IP_PRI_MAPPING3, IP_0XFC),
    sw_reg_field_des(IP_PRI_MAPPING3, IP_0XF8),
    sw_reg_field_des(IP_PRI_MAPPING3, IP_0XF4),
    sw_reg_field_des(IP_PRI_MAPPING3, IP_0XF0),
    sw_reg_field_des(IP_PRI_MAPPING3, IP_0XEC),
    sw_reg_field_des(IP_PRI_MAPPING3, IP_0XE8),
    sw_reg_field_des(IP_PRI_MAPPING3, IP_0XE4),
    sw_reg_field_des(IP_PRI_MAPPING3, IP_0XE0),
    sw_reg_field_des(IP_PRI_MAPPING3, IP_0XDC),
    sw_reg_field_des(IP_PRI_MAPPING3, IP_0XD8),
    sw_reg_field_des(IP_PRI_MAPPING3, IP_0XD4),
    sw_reg_field_des(IP_PRI_MAPPING3, IP_0XD0),
    sw_reg_field_des(IP_PRI_MAPPING3, IP_0XCC),
    sw_reg_field_des(IP_PRI_MAPPING3, IP_0XC8),
    sw_reg_field_des(IP_PRI_MAPPING3, IP_0XC4),
    sw_reg_field_des(IP_PRI_MAPPING3, IP_0XC0),

    /* Tag Priority Mapping Register */
    sw_reg_field_des(TAG_PRI_MAPPING, TAG_0X07),
    sw_reg_field_des(TAG_PRI_MAPPING, TAG_0X06),
    sw_reg_field_des(TAG_PRI_MAPPING, TAG_0X05),
    sw_reg_field_des(TAG_PRI_MAPPING, TAG_0X04),
    sw_reg_field_des(TAG_PRI_MAPPING, TAG_0X03),
    sw_reg_field_des(TAG_PRI_MAPPING, TAG_0X02),
    sw_reg_field_des(TAG_PRI_MAPPING, TAG_0X01),
    sw_reg_field_des(TAG_PRI_MAPPING, TAG_0X00),

    /* Service tag Register */
    sw_reg_field_des(SERVICE_TAG, TAG_VALUE),

    /* Cpu Port Register */
    sw_reg_field_des(CPU_PORT, CPU_PORT_EN),
    sw_reg_field_des(CPU_PORT, MIRROR_PORT_NUM),

    /* MIB Function Register */
    sw_reg_field_des(MIB_FUNC, MAC_CRC_EN),
    sw_reg_field_des(MIB_FUNC, MIB_FUN),
    sw_reg_field_des(MIB_FUNC, MIB_BUSY),
    sw_reg_field_des(MIB_FUNC, MIB_AT_HALF_EN),
    sw_reg_field_des(MIB_FUNC, MIB_TIMER),

    /* Port Status Register */
    sw_reg_field_des(PORT_STATUS, LINK_ASYN_PAUSE),
    sw_reg_field_des(PORT_STATUS, LINK_PAUSE),
    sw_reg_field_des(PORT_STATUS, LINK_EN),
    sw_reg_field_des(PORT_STATUS, LINK),
    sw_reg_field_des(PORT_STATUS, DUPLEX_MODE),
    sw_reg_field_des(PORT_STATUS, RX_FLOW_EN),
    sw_reg_field_des(PORT_STATUS, TX_FLOW_EN),
    sw_reg_field_des(PORT_STATUS, RXMAC_EN),
    sw_reg_field_des(PORT_STATUS, TXMAC_EN),
    sw_reg_field_des(PORT_STATUS, SPEED_MODE),

    /* Port Control Register */
    sw_reg_field_des(PORT_CTL, ARP_LEAKY_EN),
    sw_reg_field_des(PORT_CTL, ING_MIRROR_EN),
    sw_reg_field_des(PORT_CTL, EG_MIRROR_EN),
    sw_reg_field_des(PORT_CTL, DTAG_EN),
    sw_reg_field_des(PORT_CTL, LEARN_EN),
    sw_reg_field_des(PORT_CTL, SINGLE_VLAN_EN),
    sw_reg_field_des(PORT_CTL, MAC_LOOP_BACK),
    sw_reg_field_des(PORT_CTL, HEAD_EN),
    sw_reg_field_des(PORT_CTL, IGMP_MLD_EN),
    sw_reg_field_des(PORT_CTL, EG_VLAN_MODE),
    sw_reg_field_des(PORT_CTL, LEARN_ONE_LOCK),
    sw_reg_field_des(PORT_CTL, PORT_LOCK_EN),
    sw_reg_field_des(PORT_CTL, LOCK_DROP_EN),
    sw_reg_field_des(PORT_CTL, PORT_STATE),

    /* Port Dot1ad Register */
    sw_reg_field_des(PORT_DOT1AD, ING_PRI),
    sw_reg_field_des(PORT_DOT1AD, FORCE_PVLAN),
    sw_reg_field_des(PORT_DOT1AD, DEF_CVID),
    sw_reg_field_des(PORT_DOT1AD, CLONE),
    sw_reg_field_des(PORT_DOT1AD, PROPAGATION_EN),
    sw_reg_field_des(PORT_DOT1AD, TLS_EN),
    sw_reg_field_des(PORT_DOT1AD, FORCE_DEF_VID),
    sw_reg_field_des(PORT_DOT1AD, DEF_SVID),

    /* Port Based Vlan Register */
    sw_reg_field_des(PORT_BASE_VLAN, DOT1Q_MODE),
    sw_reg_field_des(PORT_BASE_VLAN, COREP_EN),
    sw_reg_field_des(PORT_BASE_VLAN, IN_VLAN_MODE),
    sw_reg_field_des(PORT_BASE_VLAN, PRI_PROPAGATION),
    sw_reg_field_des(PORT_BASE_VLAN, PORT_VID_MEM),
    sw_reg_field_des(PORT_BASE_VLAN, UNI_LEAKY_EN),
    sw_reg_field_des(PORT_BASE_VLAN, MUL_LEAKY_EN),

    /* Rate Limit0 Register */
    sw_reg_field_des(RATE_LIMIT0, ADD_RATE_BYTE),
    sw_reg_field_des(RATE_LIMIT0, EG_RATE_EN),
    sw_reg_field_des(RATE_LIMIT0, EG_MNG_RATE_EN),
    sw_reg_field_des(RATE_LIMIT0, IN_MNG_RATE_EN),
    sw_reg_field_des(RATE_LIMIT0, IN_MUL_RATE_EN),
    sw_reg_field_des(RATE_LIMIT0, ING_RATE),
    sw_reg_field_des(RATE_LIMIT1, EG_Q1_RATE),
    sw_reg_field_des(RATE_LIMIT1, EG_Q0_RATE),
    sw_reg_field_des(RATE_LIMIT2, EG_Q3_RATE),
    sw_reg_field_des(RATE_LIMIT2, EG_Q2_RATE),

    /* Priority Control Register */
    sw_reg_field_des(PRI_CTL, PORT_PRI_EN),
    sw_reg_field_des(PRI_CTL, DA_PRI_EN),
    sw_reg_field_des(PRI_CTL, VLAN_PRI_EN),
    sw_reg_field_des(PRI_CTL, IP_PRI_EN),
    sw_reg_field_des(PRI_CTL, DA_PRI_SEL),
    sw_reg_field_des(PRI_CTL, VLAN_PRI_SEL),
    sw_reg_field_des(PRI_CTL, IP_PRI_SEL),
    sw_reg_field_des(PRI_CTL, PORT_PRI_SEL),

    /* Storm Control Register */
    sw_reg_field_des(STORM_CTL, UNIT),
    sw_reg_field_des(STORM_CTL, MUL_EN),
    sw_reg_field_des(STORM_CTL, UNI_EN),
    sw_reg_field_des(STORM_CTL, BRO_EN),
    sw_reg_field_des(STORM_CTL, RATE),

    /* Queue Control Register */
    sw_reg_field_des(QUEUE_CTL, PORT_DESC_EN),
    sw_reg_field_des(QUEUE_CTL, QUEUE_DESC_EN),
    sw_reg_field_des(QUEUE_CTL, PORT_DESC_NR),
    sw_reg_field_des(QUEUE_CTL, QUEUE0_DESC_NR),
    sw_reg_field_des(QUEUE_CTL, QUEUE1_DESC_NR),
    sw_reg_field_des(QUEUE_CTL, QUEUE2_DESC_NR),
    sw_reg_field_des(QUEUE_CTL, QUEUE3_DESC_NR),

    /* Rate Limit1 Register */
    sw_reg_field_des(RATE_LIMIT1, EG_Q1_RATE),
    sw_reg_field_des(RATE_LIMIT1, EG_Q0_RATE),

    /* Rate Limit2 Register */
    sw_reg_field_des(RATE_LIMIT2, EG_Q3_RATE),
    sw_reg_field_des(RATE_LIMIT2, EG_Q2_RATE),
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SHIVA_REG_DESC_H_ */

