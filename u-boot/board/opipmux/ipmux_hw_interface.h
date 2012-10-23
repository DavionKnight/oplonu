/*
=============================================================================
     Header Name: ipmux_hw_interface.h

     General Description:
general definition for IPMux device hardware interface in Opulan driver.
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 yzhang            07/04/2005      Initial Version
  Zhihua Lu         04/02/2007      add arl cap information
----------------   ------------  ----------------------------------------------
*/


#ifndef IPMUX_HW_INTERFACE_INCLUDE
#define	IPMUX_HW_INTERFACE_INCLUDE 

/* includes */
#include <opl_driver.h>

/* definitions */

#define IPMUX_NUM_OF_PHY_SL	72
#define IPMUX_NUM_OF_PHY_DL	144
#define IPMUX_NUM_OF_PHY_DL2	96
#define IPMUX_NUM_OF_SCHEDULE_Q	8
#define IPMUX_TM_NUM_OF_FLOW_PER_PORT_SL	32
#define IPMUX_TM_NUM_OF_FLOW_PER_PORT_DL	16

#define IPMUX_PORT_NUM_OF_PVC_PER_PHY_SL		8
#define IPMUX_PORT_NUM_OF_PVC_PER_PHY_DL		4

#define IPMUX_PORT_ID_PVCx_SL(phyId, pvc)		((phyId) * IPMUX_PORT_NUM_OF_PVC_PER_PHY_SL + pvc)
#define IPMUX_PORT_ID_PVCx_DL(phyId, pvc)		((phyId) * IPMUX_PORT_NUM_OF_PVC_PER_PHY_DL + pvc)
#define IPMUX_PORT_NUM_OF_PVC		768 
#define IPMUX_PORT_ID_OAMx(phyId)	(IPMUX_PORT_NUM_OF_PVC + (phyId))	/* phyId: from 0 - 575 */
#define IPMUX_PORT_ID_CPU			1008
#define IPMUX_PORT_ID_GEW			1009
#define IPMUX_PORT_ID_GEW0		1010
#define IPMUX_PORT_ID_GEW1		1011
/*#define IPMUX_PORT_ID_GEL			1012*/
#define IPMUX_PORT_ID_NULL			1022		/* PORT ID packe with twhich can pass CLS and RMK, but be dropped before TM */
#define IPMUX_PORT_ID_INVALID		1023		/* PORT ID packe with twhich is dropped before CLS and RMK */

#define IPMUX_FLOW_UPSTREAM		0
#define IPMUX_FLOW_DOWNSTREAM	1
#define IPMUX_FLOW_SHAPER_MODE			1
#define IPMUX_FLOW_SHAPER_BYPASS_MODE	0
#define IPMUX_FLOW_SCHEDULE_WFQ_MODE	1
#define IPMUX_FLOW_SCHEDULE_WRR_MODE	0

	/* register and bitwise operation */
#define IPMUX_REG_SIZE				32			/* 32 bits register for IPMux */
#define REG_ADDR_ALLIGNED		4			/* 32bits word */
#define FIELD_BITS(n)				(0xffffffff >> (IPMUX_REG_SIZE - (n)))

#define IPMUX_REG_BIT0			(0x1 << 0)
#define IPMUX_REG_BIT1			(0x1 << 1)
#define IPMUX_REG_BIT2			(0x1 << 2)
#define IPMUX_REG_BIT3			(0x1 << 3)
#define IPMUX_REG_BIT4			(0x1 << 4)
#define IPMUX_REG_BIT5			(0x1 << 5)
#define IPMUX_REG_BIT6			(0x1 << 6)
#define IPMUX_REG_BIT7			(0x1 << 7)
#define IPMUX_REG_BIT8			(0x1 << 8)
#define IPMUX_REG_BIT9			(0x1 << 9)
#define IPMUX_REG_BIT10		(0x1 << 10)
#define IPMUX_REG_BIT11		(0x1 << 11)
#define IPMUX_REG_BIT12		(0x1 << 12)
#define IPMUX_REG_BIT13		(0x1 << 13)
#define IPMUX_REG_BIT14		(0x1 << 14)
#define IPMUX_REG_BIT15		(0x1 << 15)
#define IPMUX_REG_BIT16		(0x1 << 16)
#define IPMUX_REG_BIT17		(0x1 << 17)
#define IPMUX_REG_BIT18		(0x1 << 18)
#define IPMUX_REG_BIT19		(0x1 << 19)
#define IPMUX_REG_BIT20		(0x1 << 20)
#define IPMUX_REG_BIT21		(0x1 << 21)
#define IPMUX_REG_BIT22		(0x1 << 22)
#define IPMUX_REG_BIT23		(0x1 << 23)
#define IPMUX_REG_BIT24		(0x1 << 24)
#define IPMUX_REG_BIT25		(0x1 << 25)
#define IPMUX_REG_BIT26		(0x1 << 26)
#define IPMUX_REG_BIT27		(0x1 << 27)
#define IPMUX_REG_BIT28		(0x1 << 28)
#define IPMUX_REG_BIT29		(0x1 << 29)
#define IPMUX_REG_BIT30		(0x1 << 30)
#define IPMUX_REG_BIT31		(0x1 << 31)

/*
#define OPL_IPMUX_MODULE_TEST
*/
#ifdef OPL_IPMUX_MODULE_TEST
extern UINT32* pRegTest ;

#define IPMUX_REG_BASE			pRegTest
#else
#define IPMUX_REG_BASE			0xBF000000	/* default IPMux register address space */
#endif

	/* Register Section Address Map */
#define IPMUX_REG_BASE_GLOBAL   	0x0000 /* Global register               */ 
#define IPMUX_REG_BASE_SWD      	0x2800 /* Watch dog timer               */ 
#define IPMUX_REG_BASE_GPIO     		0x2880 /* General purpose IO block      */ 
#define IPMUX_REG_BASE_RTC      		0x2980 /* Real time clock               */ 
#define IPMUX_REG_BASE_INTC     		0x2A00 /* Interrupt Controller          */ 
#define IPMUX_REG_BASE_DMA0    		0x2B40 /* DMA0 channel interface, Host bus  interface*/ 
#define IPMUX_REG_BASE_HOST    		IPMUX_REG_BASE_DMA0
#define IPMUX_REG_BASE_DMA1    		0x2BC0 /* DMA1 channel interface         */ 
#define IPMUX_REG_BASE_HDLC    		0x2C00 /* HDLC interface                */ 
#define IPMUX_REG_BASE_UART     	0x4000 /* Serial UART                   */ 
#define IPMUX_REG_BASE_I2C      		0x4400 /* I2C interface                 */ 
#define IPMUX_REG_BASE_DDR       	0x4800 /* DDR controller register access*/ 
#define IPMUX_REG_BASE_RESERVED0	0x4C00 /*                               */ 
#define IPMUX_REG_BASE_GE0      		0x8000 /* Gigbits Ethernet 0            */ 
#define IPMUX_REG_BASE_GE1      		0xA000 /* Gigbits Ethernet 1            */ 
#define IPMUX_REG_BASE_GEL      		0xC000 /* Gigbits Ethernet Local        */ 
#define IPMUX_REG_BASE_UTOPIA   		0x10000 /* UTOPIA interface              */ 
#define IPMUX_REG_BASE_POSPHY  		0x10000 /* POS-PHY interface             */ 
#define IPMUX_REG_BASE_TRAFFIC  		0x14000 /* Traffic management block      */ 
#define IPMUX_REG_BASE_RESERVED1		0x16000 /*                               */ 
#define IPMUX_REG_BASE_CLASSIFY 		0x18000 /* Classfy engine block          */ 
#define IPMUX_REG_BASE_RESERVED2		0x1A000 /*                               */ 
#define IPMUX_REG_BASE_REMARK   		0x1C000 /* Remark block                  */ 
#define IPMUX_REG_BASE_RESERVED3		0x1E000 /*                               */ 
#define IPMUX_REG_BASE_MACBRIDGE		0x20000 /* Mac learning bridge block     */ 
#define IPMUX_REG_BASE_RESERVED4		0x40000 /*                               */ 

/********* Here below only support in IPMUX-E ********/

#define IPMUX_REG_MAC_CFG1_GE0      	(IPMUX_REG_BASE_GE0 + 0x200)	/*  MAC Configuration 1 */
#define IPMUX_REG_MAC_CFG1_GE1      	(IPMUX_REG_BASE_GE1 + 0x200)	/*  MAC Configuration 1 */
#define IPMUX_REG_MAC_CFG1_GEL      	(IPMUX_REG_BASE_GEL + 0x200)	/*  MAC Configuration 1 */

#define IPMUX_REG_MAC_CFG2_GE0      	(IPMUX_REG_BASE_GE0 + 0x204)	/*  MAC Configuration 2 */
#define IPMUX_REG_MAC_CFG2_GE1      	(IPMUX_REG_BASE_GE1 + 0x204)	/*  MAC Configuration 2 */
#define IPMUX_REG_MAC_CFG2_GEL      	(IPMUX_REG_BASE_GEL + 0x204)	/*  MAC Configuration 2 */

/********* Here above only support in IPMUX-E ********/

/********* Here below only support in IPMUX-E2 ********/

/* Link control register */
#define IPMUX_REG_GMAC_LAG_CONTROL  	(IPMUX_REG_BASE_GE0 + 0xC)

/* GE0 Daisy Buffer EPD Threshold Config */
#define IPMUX_REG_GMAC_DAISY_EPD_THRESHOLD_0 	(IPMUX_REG_BASE_GE0 + 0x4C)

/* GE1 Daisy Buffer EPD Threshold Config */
#define IPMUX_REG_GMAC_DAISY_EPD_THRESHOLD_1 	(IPMUX_REG_BASE_GE1 + 0x4C)

/* GE0 Daisy Buffer Pause Frame Threshold Config */
#define IPMUX_REG_GMAC_DAISY_PAUSE_THRESHOLD_0 	(IPMUX_REG_BASE_GE0 + 0x50)

/* GE1 Daisy Buffer Pause Frame Threshold Config */
#define IPMUX_REG_GMAC_DAISY_PAUSE_THRESHOLD_1 	(IPMUX_REG_BASE_GE1 + 0x50)

/* Daisy Path Maximum Packet Lentgh */
#define IPMUX_REG_GMAC_DAISY_MAX_PKT_LEN 	(IPMUX_REG_BASE_GE1 + 0x54)

/* ARL Aging Time Configure */
#define IPMUX_ARL_AGING_TIME_CFG		0x20004

/*Reserved MAC Receive Control */
#define IPMUX_REG_RSVD_MAC_RECEIVE		0x20050

/* GE Port Control */
#define IPMUX_REG_GE_PORT_CFG			0x200A8

/* DHCP/ARP/PPPoE Send to CPU control Register */
#define IPMUX_REG_PKT_TO_CPU      		0x200CC

 /* Bridge GEW Default VLAN ID and S-COS Config register*/
#define IPMUX_REG_GEW_DEFAULT_VLAN_SCOS  0x20128

/********* Here above only support in IPMUX-E2 ********/

	/* IPMux Table IDs */

#define IPMUX_TAB_DATA_UNIT_SIZE		IPMUX_REG_SIZE	/* 32 bits data unit for IPMux tables */



#if 1 /* just for compiling backward-compatibility */
#define IPMUX_TAB_VLAN_PTR	 		201 /* 2,  Bridge Vlan Pointer Table */
#define IPMUX_TAB_VLAN_HASH_B	 	202 /* 3,  Bridge Vlan Hash Table B */
#define IPMUX_TAB_VLAN_HASH_A	 	203 /* 4,  Bridge Vlan Hash Table A */
#define IPMUX_TAB_VLAN_REP	 		205 /* 6,  Bridge CTAG replacement table */
#define IPMUX_TAB_VLAN_MEM	 	204 /* 7,  Bridge VLAN Cross Connection Enable table */
#define IPMUX_TAB_SACT	 			206 /* 8,  Bridge MAC Filter Table, also source address control talbe, SACT */
#define IPMUX_TAB_LIMIT	 			207 /* 13, Bridge MAC limitation table */
#define IPMUX_TAB_LIMIT_CNT	 	208	/* 14, Bridge MAC limitation counter table */
#define IPMUX_TAB_PRIORITY_TAG	209	/* 15, Bridge VLAN priority tag table */
#endif
	/* table alias */
#define IPMUX_TAB_DS_FLOW_R1	IPMUX_TAB_DS_FPSEQ
#define IPMUX_TAB_DS_FLOW_R2	IPMUX_TAB_DS_FPWPRI
#define IPMUX_TAB_DS_FLOW_R3	IPMUX_TAB_DS_FPWSUM

	/* IPMux Table dimensions: width, in 32-bit word */
#if 1 /* just for compiling backward-compatibility */
#define IPMUX_TAB_VLAN_PTR_WIDTH 		1 /* 2,  Bridge Vlan Pointer Table */
#define IPMUX_TAB_VLAN_HASH_B_WIDTH 	2 /* 3,  Bridge Vlan Hash Table B */
#define IPMUX_TAB_VLAN_HASH_A_WIDTH 	2 /* 4,  Bridge Vlan Hash Table A */
#define IPMUX_TAB_VLAN_REP_WIDTH 		1 /* 6,  Bridge CTAG replacement table */
#define IPMUX_TAB_VLAN_MEM_WIDTH 	4 /* 7,  Bridge VLAN Cross Connection Enable table */
#define IPMUX_TAB_SACT_WIDTH 			2 /* 8,  Bridge MAC Filter Table, also source address control talbe, SACT */
#define IPMUX_TAB_LIMIT_WIDTH 			1 /* 13, Bridge MAC limitation table */
#define IPMUX_TAB_LIMIT_CNT_WIDTH 	1	/* 14, Bridge MAC limitation counter table */
#define IPMUX_TAB_PRIORITY_TAG_WIDTH 1	/* 15, Bridge VLAN priority tag table */
#endif

#define IPMUX_TAB_PORT_WIDTH			4 /* 0,  Bridge Port table (PTable) */
#define IPMUX_TAB_ARL_WIDTH			3 /* 1,  Bridge Address Resolution logic table  */
#define IPMUX_TAB_VLAN_WIDTH			1 /* 2,  Bridge Vlan Table */
#define IPMUX_TAB_VLAN_ENTRY_WIDTH	1 /* 3,  Bridge Vlan Entry Table */
#define IPMUX_TAB_US_PVC_BUNDLE_WIDTH 1/* 4,  Bridge Upstream PVC Bundle Table */
#define IPMUX_TAB_DS_PVC_BUNDLE_WIDTH 1/* 5,  Bridge Downstream PVC Bundle Table */
#define IPMUX_TAB_VLAN_CC_PTR_WIDTH	1 /* 6,  Bridge VLAN Cross Connection Pointer table */
#define IPMUX_TAB_VLAN_CC_ENTRY_WIDTH 2/* 7,  Bridge VLAN Cross Connection Entry table */
#define IPMUX_TAB_L2_SECURITY_WIDTH	2 /* 8,  Bridge MAC Filter Table, also Layer 2 Security talbe */
#define IPMUX_TAB_REMARK_WIDTH		1 /* 9,  Bridge remark table */
#define IPMUX_TAB_MC_PTR_WIDTH		1 /* 10, Bridge MC pointer table, Multicast Group table */
#define IPMUX_TAB_MC_WIDTH				1 /* 11, Bridge MC Table, Multicast Group member table */
#define IPMUX_TAB_EGRESS_PORT_WIDTH	1 /* 12, Bridge Port Egress Contrl table (ECT) */
#define IPMUX_TAB_MATT_WIDTH			4 /* 13, Bridge MAC Adress Translation table */
#define IPMUX_TAB_VTT_WIDTH			2 /* 14, Bridge VLAN Translation table */
#define IPMUX_TAB_L3_SECURITY_WIDTH	4 /* 15, Bridge IP Binding table, also Layer 3 Security table */


#define IPMUX_TAB_RULE_MASK_WIDTH 	10 /* 16, Classify Rule Mask table */
#define IPMUX_TAB_RULE_HASH_WIDTH 	2 /* 17, Classify rule hash address table */
#define IPMUX_TAB_RULE_WIDTH 			10 /* 18, Classify rule table */
#define IPMUX_TAB_FLOW_MASK_WIDTH 	2 /* 20, Classify Flow ID mask table */
#define IPMUX_TAB_FLOW_HASH_WIDTH 	1 /* 21, Classify flow address pointer (hash) table */
#define IPMUX_TAB_FLOW_WIDTH 			2 /* 22, Classify Flow table */
#define IPMUX_TAB_FLOW_RMK_WIDTH 	1 /* 23, Classify Flow Remark rule table */
#define IPMUX_TAB_CLS_CNT_HIT_WIDTH	1 /* 24, Classify Hit Counter table */

#define IPMUX_TAB_CLS_METER_WIDTH   1 /* 28, Classify meter table */
#define IPMUX_TAB_CLS_COUNTER_WIDTH 2 /* 29, Classify counter table */

#define IPMUX_TAB_US_FSHP_WIDTH 		1 /* 32, Upstream Flow Shaper Table */
#define IPMUX_TAB_US_FWRED_WIDTH 	1	/* 34, Upstream Flow WRED Table */
#define IPMUX_TAB_US_FLOW_R_WIDTH	1 /* 36, Upstream Flow Table (Read Only) */
#define IPMUX_TAB_US_FLOW_W_WIDTH	1 /* 38, Upstream Flow Table (Write Only) */
#define IPMUX_TAB_US_FCNT_WIDTH		1 /* 40, Upstream Flow Queue Counter table */
#define IPMUX_TAB_US_FSTATUS_WIDTH	1 /* 42, Upstream Flow Queue Status Table */
#define IPMUX_TAB_US_FCNT_DROP_WIDTH 1	/* 44, Upstream Flow Rx Packet Drop Counter Table */

#define IPMUX_TAB_DS_FSHP_WIDTH 		1 /* 49, Downstream Flow Shaper Table */
#define IPMUX_TAB_DS_FWRED_WIDTH 	1 /* 51, Downstream Flow WRED Table */
#define IPMUX_TAB_DS_FPSEQ_WIDTH		1 /* 53, Downstream Flow Port Sequence Table (Read Only) */
#define IPMUX_TAB_DS_FPWPRI_WIDTH	10 /* 55, Downstream Port Weight and Priority Table (Read Only) */
#define IPMUX_TAB_DS_FPWSUM_WIDTH	4 /* 57, Downstream Port Weight Sum Table */
#define IPMUX_TAB_DS_FLOW_W_WIDTH	1 /* 59, Downstream Flow Table (Write Only) */
#define IPMUX_TAB_DS_PSEQ_R_WIDTH	1 /*45, Downstream Port Sequence Table (Read Only) */
#define IPMUX_TAB_DS_FCNT_WIDTH		1 /* 47, Downstream Flow Queue Counter table */
#define IPMUX_TAB_DS_FSTATUS_WIDTH	1 /* 49, Downstream Flow Queue Status Table */
#define IPMUX_TAB_DS_FCNT_DROP_WIDTH 1	/* 51, Downstream Flow Rx Packet Drop Counter Table */

#define IPMUX_TAB_UPPT_WIDTH 			1 /* 64, SAR UPPT */
#define IPMUX_TAB_EPPT_WIDTH 			3 /* 65, SAR EPPT */
#define IPMUX_TAB_SAR_QS_WIDTH		1 /* 67, SAR Queue Status Table */
#define IPMUX_TAB_SAR_CNT_APCTRL_WIDTH		1 /* 68, SAR Access Port Tx/Rx Counter Control Table */
#define IPMUX_TAB_SAR_CNT_APGRPRX_WIDTH		8 /* 69, SAR Access Port Rx Counter Group Table */
#define IPMUX_TAB_SAR_CNT_APGRPTX_WIDTH		7 /* 70, SAR Access Port Tx Counter Group Table */
#define IPMUX_TAB_SAR_CNT_PVPTR_WIDTH		1 /* 71, SAR Port VLAN Counter Pointer Table */
#define IPMUX_TAB_SAR_CNT_PVHASH_WIDTH		2 /* 72, SAR Port VLAN Counter Hash Table */
#define IPMUX_TAB_SAR_CNT_PVIN_WIDTH			1 /* 73, SAR Port VLAN In Frame Statistics Group Table */
#define IPMUX_TAB_SAR_CNT_PVOUT_WIDTH		1 /* 74, SAR Port VLAN Out Frame Statistics Group Table */
#define IPMUX_TAB_SAR_CNT_VLANPTR_WIDTH		1 /* 75, SAR GEW VLAN Counter Pointer Table */
#define IPMUX_TAB_SAR_CNT_VLANHASH_WIDTH	2 /* 76, SAR GEW VLAN Counter Hash Table */
#define IPMUX_TAB_SAR_CNT_VLANIN_WIDTH		1 /* 77, SAR GEW VLAN In Frame Statistics Group Table */
#define IPMUX_TAB_SAR_CNT_VLANOUT_WIDTH		1 /* 78, SAR GEW VLAN Out Frame Statistics Group Table */
#define IPMUX_TAB_SAR_GIT_WIDTH				4 /* 79, SAR Gateway Information Table */

#define IPMUX_TAB_HOST_RX_WIDTH 		1 /* 80, HOST Rx BD table, DMA0 BD table */
#define IPMUX_TAB_HOST_TX_WIDTH 		1 /* 81, HOST Tx BD table, DMA0 BD table */
#define IPMUX_TAB_FE_RX_WIDTH 		1 /* 96, Fast Ethernet Rx BD table, DMA1 BD table */
#define IPMUX_TAB_FE_TX_WIDTH 		1 /* 97, Fast Ethernet Tx BD table, DMA1 BD table */
#define IPMUX_TAB_GEW0_TX_WIDTH		1 /* 113, GEW0 Tx BD table, DMA2 Tx BD table */
#define IPMUX_TAB_GEW1_TX_WIDTH		1 /* 129, GEW1 Tx BD table, DMA3 Tx BD table */
#define IPMUX_TAB_EXCEPTION_CONTEXT_WIDTH	1/* 144, MIPS Exception Context table */


#define IPMUX_TAB_ATB_CNT_RXCL_WIDTH		1 /* 112, Rx cells per Link,except asm */
#define IPMUX_TAB_ATB_CNT_RXCG_WIDTH	1 /* 113, Rx cells per Group,except asm */
#define IPMUX_TAB_ATB_CNT_RXAL_WIDTH	1 /* 114, Rx asm cells per Link */
#define IPMUX_TAB_ATB_CNT_TXCL_WIDTH		1 /* 115, Tx cells per Link,except asm */
#define IPMUX_TAB_ATB_CNT_TXCG_WIDTH	1 /* 116, Tx cells per Group,except asm */
#define IPMUX_TAB_ATB_CNT_TXAL_WIDTH	1 /* 117, Tx asm cells per Link */

	/* IPMux Table dimensions: length, in records */
#if 1 /* just for compiling backward-compatibility */
#define IPMUX_TAB_VLAN_PTR_LEN 		512 /* 2,  Bridge Vlan Pointer Table */
#define IPMUX_TAB_VLAN_HASH_B_LEN 	512 /* 3,  Bridge Vlan Hash Table B */
#define IPMUX_TAB_VLAN_HASH_A_LEN 	512 /* 4,  Bridge Vlan Hash Table A */
#define IPMUX_TAB_VLAN_REP_LEN 		4608 /* 6,  Bridge CTAG replacement table */
#define IPMUX_TAB_VLAN_MEM_LEN 		64 /* 7,  Bridge VLAN Cross Connection Enable table */
#define IPMUX_TAB_SACT_LEN 			4608 /* 8,  Bridge MAC Filter Table, also source address control talbe, SACT */
#define IPMUX_TAB_LIMIT_LEN 			4096 /* 13, Bridge MAC limitation table */
#define IPMUX_TAB_LIMIT_CNT_LEN 		576	/* 14, Bridge MAC limitation counter table */
#define IPMUX_TAB_PRIORITY_TAG_LEN 	36	/* 15, Bridge VLAN priority tag table */
#endif

#define IPMUX_TAB_PORT_LEN			576 /* 0,  Bridge Port table (PTable) */
#define IPMUX_TAB_ARL_LEN			4096 /* 1,  Bridge Address Resolution logic table  */
#define IPMUX_TAB_VLAN_LEN			4096 /* 2,  Bridge Vlan Table */
#define IPMUX_TAB_VLAN_ENTRY_LEN	4096 /* 3,  Bridge Vlan Entry Table */
#define IPMUX_TAB_US_PVC_BUNDLE_LEN 576 /* 4,  Bridge Upstream PVC Bundle Table */
#define IPMUX_TAB_DS_PVC_BUNDLE_LEN 4608 /* 5,  Bridge Downstream PVC Bundle Table */
#define IPMUX_TAB_VLAN_CC_PTR_LEN	1024 /* 6,  Bridge VLAN Cross Connection Pointer table */
#define IPMUX_TAB_VLAN_CC_ENTRY_LEN 1024 /* 7,  Bridge VLAN Cross Connection Entry table */
#define IPMUX_TAB_L2_SECURITY_LEN	1024 /* 8,  Bridge MAC Filter Table, also Layer 2 Security talbe */
#define IPMUX_TAB_REMARK_LEN		512 /* 9,  Bridge remark table */
#define IPMUX_TAB_MC_PTR_LEN		1024 /* 10, Bridge MC pointer table, Multicast Group table */
#define IPMUX_TAB_MC_LEN			1024 /* 11, Bridge MC Table, Multicast Group member table */
#define IPMUX_TAB_EGRESS_PORT_LEN	576 /* 12, Bridge Port Egress Contrl table (ECT) */
#define IPMUX_TAB_MATT_LEN			1024 /* 13, Bridge MAC Adress Translation table */
#define IPMUX_TAB_VTT_LEN			1024 /* 14, Bridge VLAN Translation table */
#define IPMUX_TAB_L3_SECURITY_LEN	1024 /* 15, Bridge IP Binding table, also Layer 3 Security table */

#define IPMUX_TAB_RULE_MASK_LEN 	32 /* 16, Classify Rule Mask table */
#define IPMUX_TAB_RULE_HASH_LEN 	256	/* 17, Classify rule hash address table */
#define IPMUX_TAB_RULE_LEN 		512  /* 18, Classify rule table */
#define IPMUX_TAB_FLOW_MASK_LEN 	16 /* 20, Classify Flow ID mask table */
#define IPMUX_TAB_FLOW_HASH_LEN 	(1024*2) 	/* 21, Classify flow address pointer (hash) table */
#define IPMUX_TAB_FLOW_LEN 		(1024*2)	 /* 22, Classify Flow table */
#define IPMUX_TAB_FLOW_RMK_LEN 	512	/* 23, Classify Flow Remark rule table */
#define IPMUX_TAB_CLS_CNT_HIT_LEN	512	/* 24, Classify Hit Counter table */

#define IPMUX_TAB_CLS_METER_LEN     512 /* 28, Classify meter table */
#define IPMUX_TAB_CLS_COUNTER_LEN   512 /* 29, Classify counter table */

#if 0
#define IPMUX_TAB_US_FSHP_LEN 		512 /* 32, Upstream Flow Shaper Table */
#define IPMUX_TAB_US_FWRED_LEN 	512	/* 34, Upstream Flow WRED Table */
#define IPMUX_TAB_DS_FSHP_LEN 		1024 /* 49, Downstream Flow Shaper Table */
#define IPMUX_TAB_DS_FWRED_LEN 	1024 /* 51, Downstream Flow WRED Table */
#endif

#define IPMUX_TAB_US_FSHP_LEN 	512 /* 32, Upstream Flow Shaper Table */
#define IPMUX_TAB_US_FWRED_LEN 	512	/* 34, Upstream Flow WRED Table */
#define IPMUX_TAB_US_FLOW_R_LEN	512 /* 36, Upstream Flow Table (Read Only) */
#define IPMUX_TAB_US_FLOW_W_LEN	1 /* 38, Upstream Flow Table (Write Only) */
#define IPMUX_TAB_US_FCNT_LEN		512 /* 40, Upstream Flow Queue Counter table */
#define IPMUX_TAB_US_FSTATUS_LEN	512 /* 42, Upstream Flow Queue Status Table */
#define IPMUX_TAB_US_FCNT_DROP_LEN 512	/* 44, Upstream Flow Rx Packet Drop Counter Table */

#define IPMUX_TAB_DS_FSHP_LEN 	1024 /* 49, Downstream Flow Shaper Table */
#define IPMUX_TAB_DS_FWRED_LEN 	1024 /* 51, Downstream Flow WRED Table */
#define IPMUX_TAB_DS_FPSEQ_LEN	1024 /* 53, Downstream Flow Port Sequence Table (Read Only) */
#define IPMUX_TAB_DS_FPWPRI_LEN	72 /* 55, Downstream Port Weight and Priority Table (Read Only) */
#define IPMUX_TAB_DS_FPWSUM_LEN	144 /* 57, Downstream Port Weight Sum Table */
#define IPMUX_TAB_DS_FLOW_W_LEN	1 /* 59, Downstream Flow Table (Write Only) */
#define IPMUX_TAB_DS_PSEQ_R_LEN	(72*32) /*45, Downstream Port Sequence Table (Read Only) */
#define IPMUX_TAB_DS_FCNT_LEN		1024 /* 47, Downstream Flow Queue Counter table */
#define IPMUX_TAB_DS_FSTATUS_LEN	1024 /* 49, Downstream Flow Queue Status Table */
#define IPMUX_TAB_DS_FCNT_DROP_LEN 1024	/* 51, Downstream Flow Rx Packet Drop Counter Table */

#define IPMUX_TAB_UPPT_LEN 			576 /* 64, SAR UPPT */
#define IPMUX_TAB_EPPT_LEN 			576 /* 65, SAR EPPT */
#define IPMUX_TAB_SAR_QS_LEN			578 /* 67, SAR Queue Status Table */
#define IPMUX_TAB_SAR_CNT_APCTRL_LEN		576 /* 68, SAR Access Port Tx/Rx Counter Control Table */
#define IPMUX_TAB_SAR_CNT_APGRPRX_LEN	256 /* 69, SAR Access Port Rx Counter Group Table */
#define IPMUX_TAB_SAR_CNT_APGRPTX_LEN	256 /* 70, SAR Access Port Tx Counter Group Table */
#define IPMUX_TAB_SAR_CNT_PVPTR_LEN		1024 /* 71, SAR Port VLAN Counter Pointer Table */
#define IPMUX_TAB_SAR_CNT_PVHASH_LEN		1024 /* 72, SAR Port VLAN Counter Hash Table */
#define IPMUX_TAB_SAR_CNT_PVIN_LEN		1024 /* 73, SAR Port VLAN In Frame Statistics Group Table */
#define IPMUX_TAB_SAR_CNT_PVOUT_LEN		1024 /* 74, SAR Port VLAN Out Frame Statistics Group Table */
#define IPMUX_TAB_SAR_CNT_VLANPTR_LEN	1024 /* 75, SAR GEW VLAN Counter Pointer Table */
#define IPMUX_TAB_SAR_CNT_VLANHASH_LEN	1024 /* 76, SAR GEW VLAN Counter Hash Table */
#define IPMUX_TAB_SAR_CNT_VLANIN_LEN		1024 /* 77, SAR GEW VLAN In Frame Statistics Group Table */
#define IPMUX_TAB_SAR_CNT_VLANOUT_LEN	1024 /* 78, SAR GEW VLAN Out Frame Statistics Group Table */
#define IPMUX_TAB_SAR_GIT_LEN				16 /* 79, SAR Gateway Information Table */
    
#define IPMUX_TAB_HOST_RX_LEN 		64 /* 80, HOST Rx BD table, DMA0 BD table */
#define IPMUX_TAB_HOST_TX_LEN 		64 /* 81, HOST Tx BD table, DMA0 BD table */
#define IPMUX_TAB_FE_RX_LEN 		64 /* 96, Fast Ethernet Rx BD table, DMA1 BD table */
#define IPMUX_TAB_FE_TX_LEN 		64 /* 97, Fast Ethernet Tx BD table, DMA1 BD table */
#define IPMUX_TAB_GEW0_TX_LEN		64 /* 113, GEW0 Tx BD table, DMA2 Tx BD table */
#define IPMUX_TAB_GEW1_TX_LEN		64 /* 129, GEW1 Tx BD table, DMA3 Tx BD table */
#define IPMUX_TAB_EXCEPTION_CONTEXT_LEN	128/* 144, MIPS Exception Context table */

#define IPMUX_TAB_ATB_CNT_RXCL_LEN	144 /* 112, Rx cells per Link,except asm */
#define IPMUX_TAB_ATB_CNT_RXCG_LEN	36 /* 113, Rx cells per Group,except asm */
#define IPMUX_TAB_ATB_CNT_RXAL_LEN	144 /* 114, Rx asm cells per Link */
#define IPMUX_TAB_ATB_CNT_TXCL_LEN	144 /* 115, Tx cells per Link,except asm */
#define IPMUX_TAB_ATB_CNT_TXCG_LEN	36 /* 116, Tx cells per Group,except asm */
#define IPMUX_TAB_ATB_CNT_TXAL_LEN	144 /* 117, Tx asm cells per Link */

	/* Indirect Table Access Related Register definitions */
#define REG_BIT_TBUSY_OFF							31
#define REG_BIT_TBUSY								(FIELD_BITS(1) << REG_BIT_TBUSY_OFF)
#define REG_BIT_TTYPE_OFF							30
#define REG_BIT_TTYPE								(FIELD_BITS(1) << REG_BIT_TTYPE_OFF)
#define REG_BIT_TID_OFF								16
#define REG_BIT_TID									(FIELD_BITS(7) << REG_BIT_TID_OFF)
#define REG_BIT_RECNO_OFF							0
#define REG_BIT_RECNO								(FIELD_BITS(16) << REG_BIT_RECNO_OFF)
#define IPMUX_TABLE_READ							0
#define IPMUX_TABLE_WRITE							REG_BIT_TTYPE
#define REG_BIT_SRS_OFF							4
#define REG_BIT_SRS									(FIELD_BITS(4) << REG_BIT_SRS_OFF)

#define NUM_OF_SET									10
#define MAX_LEN_OF_TAB_REC						19



#define IF_GET_MODE(phyNum)	(IPMUX_NUM_OF_PHY_SL == (phyNum) ? IPMUX_SINGLE_LATENCY : (IPMUX_NUM_OF_PHY_DL == (phyNum) ? IPMUX_DUAL_LATENCY : IPMUX_DUAL_LATENCY2))

typedef enum
	{
	IPMUX_SHAPER_trTCM,				/* 0 */
	IPMUX_SHAPER_srTCM,				/* 1 */
	SHAPER_MODE_END
}SHAPER_MODE_e;

typedef enum
	{
	IPMUX_YELLOW_SHAPER_BUFFER,	/* 0 */
	IPMUX_GREEN_SHAPER_BUFFER,		/* 1 */
	SHAPER_BUFFER_MODE_END
}SHAPER_BUFFER_MODE_e;


	/* struct for access to IPMux-CPU interface */
		/* for Tx */
#define REG_BIT_HOST_PTYPE_OFF		30
#define REG_BIT_HOST_PTYPE				(FIELD_BITS(2) << REG_BIT_HOST_PTYPE_OFF)
#define REG_BIT_HOST_FMIDPID_OFF		20
#define REG_BIT_HOST_FMIDPID			(FIELD_BITS(10) << REG_BIT_HOST_FMIDPID_OFF)
#define REG_BIT_HOST_FMSEL_OFF		16
#define REG_BIT_HOST_FMSEL				(FIELD_BITS(4) << REG_BIT_HOST_FMSEL_OFF)
#define REG_BIT_HOST_UC_OFF			15
#define REG_BIT_HOST_UC				(FIELD_BITS(1) << REG_BIT_HOST_UC_OFF)
#define REG_BIT_HOST_US_OFF			14
#define REG_BIT_HOST_US				(FIELD_BITS(1) << REG_BIT_HOST_US_OFF)
#define REG_BIT_HOST_CB_OFF			13
#define REG_BIT_HOST_CB				(FIELD_BITS(1) << REG_BIT_HOST_CB_OFF)
		/* for Rx */
#define REG_BIT_HOST_IPORT_OFF		22
#define REG_BIT_HOST_IPORT				(FIELD_BITS(10) << REG_BIT_HOST_IPORT_OFF)


/* HOST traffic definitions */

typedef enum IPMUX_TRAFFIC_TYPE_s
	{
	IPMUX_UNICAST,
	IPMUX_FLOODING,
	IPMUX_BROADCAST,
	IPMUX_MULTICAST,
	TRAFFIC_TYPE_END
}IPMUX_TRAFFIC_TYPE_e;

#define IPMUX_VLAN_UNTAGGED 	1
#define IPMUX_VLAN_TAGGED 		0
#define IPMUX_CLASSIFICATION_BYPASSED 	1
#define IPMUX_CLASSIFICATION_REQUIRED 	0
#define IPMUX_HOST_TRAFFIC_HEAD_SIZE		4
#define IPMUX_FLOW_SEARCH_BYPASSED 	1
#define IPMUX_FLOW_SEARCH_REQUIRED 	0
#define IPMUX_VLAN_TRANSLATION_BYPASSED 	1
#define IPMUX_VLAN_TRANSLATION_REQUIRED 	0
#define IPMUX_MAC_TRANSLATION_BYPASSED 	1
#define IPMUX_MAC_TRANSLATION_REQUIRED 	0

#ifdef IPMUX_LITTLE_ENDIAN
typedef struct IPMUX_HOST_GENERATED_s
	{
	UINT32 rsvd				:6;
	UINT32 mac_trans_bypass:1;
	UINT32 vlan_trans_bypass:1;
	UINT32 flow_serach_bypass:1;
	UINT32 classify_bypass	:1;
	UINT32 fmsel				:10;
	UINT32 fmid_pid			:10;
	UINT32 packet_type		:2;
	UINT8  payload[1];
}IPMUX_HOST_GENERATED_t;

#else
typedef struct IPMUX_HOST_GENERATED_s
	{
	UINT32 packet_type		:2;
	UINT32 fmid_pid				:10;
	UINT32 fmsel						:10;
	UINT32 classify_bypass	:1;
	UINT32 flow_serach_bypass:1;
	UINT32 vlan_trans_bypass:1;
	UINT32 mac_trans_bypass:1;
	UINT32 rsvd				:6;
	UINT8  payload[1];
}IPMUX_HOST_GENERATED_t;

#endif

typedef struct IPMUX_HOST_BOUND_s
	{
	UINT32 rsvd			:22;
	UINT32 ingress_pid	:10;
	UINT8  payload[1];
}IPMUX_HOST_BOUND_t;

/*****   Zhihua Lu add   *****/
typedef struct BRIDGE_ARL_CAP_s
{
	unsigned int uiCap1;
	unsigned int uiCap2;
	unsigned int uiCap3;
}BRIDGE_ARL_CAP_t;

typedef struct CLASSIFYSIP_s
{
	UINT8 ucValid;
	UINT32 ulSIP;
	UINT16 usSVid;
}CLASSIFYSIP_t;
/*****************************************************************************/
#define IPMUX_REG_ID2ADDR(regId) 	((UINT32)IPMUX_REG_BASE + (UINT32)(regId))
#define IPMUX_REG_ADDR2ID(addr) 	((UINT32)(addr) - (UINT32)IPMUX_REG_BASE)


#ifdef OPULAN_RTOS_LINUX
#ifdef __KERNEL__
static inline OPL_STATUS ipMuxRegRead(UINT32 regID, volatile UINT32 *pval)
{
	if (regID % 4 != 0)
		return OPL_ERROR;

	*(UINT32*)(pval) = *(volatile UINT32 *)IPMUX_REG_ID2ADDR(regID);

	return OPL_OK;
}

static inline OPL_STATUS ipMuxRegWrite(UINT32 regID, UINT32 val)
{
	if (regID % 4 != 0)
		return OPL_ERROR;

	*((volatile UINT32 *)IPMUX_REG_ID2ADDR(regID)) = (UINT32)(val);

	return OPL_OK;
}

/*#define ipMuxRegRead(regId, pval)	*(UINT32*)(pval) = *(volatile UINT32 *)IPMUX_REG_ID2ADDR(regId)*/
/*#define ipMuxRegWrite(regId, val)		*((volatile UINT32 *)IPMUX_REG_ID2ADDR(regId)) = (UINT32)(val)*/
#else
extern char *g_pbuf;
static inline OPL_STATUS ipMuxRegRead(UINT32 regID, volatile UINT32 *pval)
{
	if (regID % 4 != 0)
		return OPL_ERROR;

        *(UINT32*)(pval) = *(volatile UINT32 *)(g_pbuf + regID);
        return OPL_OK;
}

static inline OPL_STATUS ipMuxRegWrite(UINT32 regID, UINT32 val)
{
	if (regID % 4 != 0)
		return OPL_ERROR;

        *((volatile UINT32 *)(g_pbuf + regID)) = (UINT32)(val);
        return OPL_OK;
}
#endif

static inline int ipMuxRegBitTest (UINT32 regId, UINT32 bits)
{
        UINT32 nTemp = 0;
	if (regId % 4 != 0)
		return OPL_ERROR;

        ipMuxRegRead (regId, &nTemp);
        return (nTemp & bits);
}

static inline void ipMuxRegBitSet(UINT32 regId, UINT32 bits)	
{
	UINT32 nTemp = OPL_ZERO;
	if (regId % 4 != 0)
		return;

	ipMuxRegRead (regId, &nTemp);
	nTemp |= (bits);
	ipMuxRegWrite (regId, nTemp);
}

static inline void ipMuxRegBitClear(UINT32 regId, UINT32 bits)	
{
	UINT32 nTemp = OPL_ZERO;
	if (regId % 4 != 0)
		return;

	ipMuxRegRead (regId, &nTemp);
	nTemp &= (~(bits));
	ipMuxRegWrite (regId, nTemp);
}

static inline void ipMuxRegBitToggle(UINT32 regId, UINT32 bits)	
{
	UINT32 nTemp = OPL_ZERO;
	UINT32 nTemp1 = OPL_ZERO;
	if (regId % 4 != 0)
		return;

	ipMuxRegRead (regId, &nTemp);
	nTemp1 = nTemp;
	nTemp &= (bits);
	nTemp1 &= (~(bits));
	nTemp1 |= (~nTemp);
	ipMuxRegWrite (regId, nTemp1);
}

#else	/* for vxworks */
/*
extern int nHWDbgFd;
#define ipMuxRegRead(regId, pval)	*(UINT32*)(pval) = *(volatile UINT32 *)IPMUX_REG_ID2ADDR(regId); if (nHWDbgFd) printf ("Read register 0x%x, val=%#x.\n", (regId), *(UINT32*)(pval))
#define ipMuxRegWrite(regId, val)		*((volatile UINT32 *)IPMUX_REG_ID2ADDR(regId)) = (UINT32)(val); if (nHWDbgFd) printf ("Write register 0x%x, val=%#x.\n", (regId), (UINT32)(val))
*/

extern OPL_STATUS ipMuxRegRead(UINT32 regID,volatile UINT32 *pval);
extern OPL_STATUS ipMuxRegWrite(UINT32 regID,UINT32 val);

static int ipMuxRegBitTest (UINT32 regId, UINT32 bits)
{
	UINT32 nTemp = 0;

	ipMuxRegRead (regId, &nTemp);
	return (nTemp & bits);
}

#define ipMuxRegBitSet(regId, bits)	{\
									UINT32 nTemp = OPL_ZERO;\
									ipMuxRegRead (regId, &nTemp);\
									nTemp |= (bits);\
									ipMuxRegWrite (regId, nTemp);\
								}
#define ipMuxRegBitClear(regId, bits)	{\
									UINT32 nTemp = OPL_ZERO;\
									ipMuxRegRead (regId, &nTemp);\
									nTemp &= (~(bits));\
									ipMuxRegWrite (regId, nTemp);\
								}
#define ipMuxRegBitToggle(regId, bits)	{\
									UINT32 nTemp = OPL_ZERO;\
									UINT32 nTemp1 = OPL_ZERO;\
									ipMuxRegRead (regId, &nTemp);\
									nTemp1 = nTemp;\
									nTemp &= (bits);\
									nTemp1 &= (~(bits));\
									nTemp1 |= (~nTemp);\
									ipMuxRegWrite (regId, nTemp1);\
								}


#endif



extern OPL_STATUS ipmuxTableRead (IPMUX_TAB_e region, UINT16 offset, int nRecNum, volatile UINT32* pVal);
extern OPL_STATUS ipmuxTableWrite (IPMUX_TAB_e region, UINT16 offset, int nRecNum, volatile UINT32* pVal);
extern OPL_STATUS ipmuxTableShow ( int nMod );
extern OPL_STATUS ipmuxPhyNumGet (UINT16 *usPhyNum );
extern int ipMuxRegRead_multi (UINT32 regAddr, int num, UINT32 *pval);

#endif /* IPMUX_HW_INTERFACE_INCLUDE */	
