/*
=============================================================================
     Header Name: ipmux_misc.h

     General Description:
chip-level configuration, user interface (UTOPIA/POSPHY) driver APIs head file.
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 yzhang            09/01/05      Initial Version
----------------   ------------  ----------------------------------------------
*/
#ifndef MISC_API_DRIVER_INCLUDE
#define	MISC_API_DRIVER_INCLUDE 

/* includes */
#include "opl_driver.h"

/* definition */

/*******************************************************************************
 **
 **			Section for System Configuration
 **
 *******************************************************************************/
		/* System Reset and Device Identifier */
#define REG_SYSRDI						(IPMUX_REG_BASE_GLOBAL + 0x0)
#define REG_BIT_SYSRDI_RSTF_OFF		31
#define REG_BIT_SYSRDI_RSTF			(FIELD_BITS(1) << REG_BIT_SYSRDI_RSTF_OFF)
#define REG_BIT_SYSRDI_PROID_OFF		4
#define REG_BIT_SYSRDI_PROID			(FIELD_BITS(16) << REG_BIT_SYSRDI_PROID_OFF)
#define REG_BIT_SYSRDI_PRORV_OFF		0
#define REG_BIT_SYSRDI_PRORV			(FIELD_BITS(4) << REG_BIT_SYSRDI_PRORV_OFF)

		/* System Configuration 1 */
#define REG_SYSCFG1						(IPMUX_REG_BASE_GLOBAL + 0x8)
#if 0
#define REG_BIT_SYSCFG1_RSTC_OFF		31
#define REG_BIT_SYSCFG1_RSTC			(FIELD_BITS(1) << REG_BIT_SYSCFG1_RSTC_OFF)
#endif
#if 1
#define REG_BIT_SYSCFG1_MINTE_OFF		0
#define REG_BIT_SYSCFG1_MINTE			(FIELD_BITS(1) << REG_BIT_SYSCFG1_MINTE_OFF)
#endif

		/* System Configuration 2 */
#define REG_SYSCFG2						/*(IPMUX_REG_BASE_GLOBAL + 0x8)*/REG_SYSCFG1
#define REG_BIT_SYSCFG2_MODEML_OFF	2
#define REG_BIT_SYSCFG2_MODEML		(FIELD_BITS(2) << REG_BIT_SYSCFG2_MODEML_OFF)
#define REG_BIT_SYSCFG2_TPM_OFF		1
#define REG_BIT_SYSCFG2_TPM			(FIELD_BITS(1) << REG_BIT_SYSCFG2_TPM_OFF)
#define REG_BIT_SYSCFG2_UEN_OFF		0
#define REG_BIT_SYSCFG2_UEN			(FIELD_BITS(1) << REG_BIT_SYSCFG2_UEN_OFF)

		/* Chip Link List Enable */
#define REG_LLE							(IPMUX_REG_BASE_GLOBAL + 0xC)
#define REG_BIT_SYSCFG1_RSTC_OFF		1
#define REG_BIT_SYSCFG1_RSTC			(FIELD_BITS(1) << REG_BIT_SYSCFG1_RSTC_OFF)
#define REG_BIT_LLE_E_OFF				0
#define REG_BIT_LLE_E					(FIELD_BITS(1) << REG_BIT_LLE_E_OFF)

		/* Chip DRAM Size Configuration */
#define REG_DRAMCFG					(IPMUX_REG_BASE_GLOBAL + 0x10)
#define REG_BIT_DRAMCFG_UPRAM_OFF	8
#define REG_BIT_DRAMCFG_UPRAM		(FIELD_BITS(8) << REG_BIT_DRAMCFG_UPRAM_OFF)
#define REG_BIT_DRAMCFG_DNRAM_OFF	0
#define REG_BIT_DRAMCFG_DNRAM		(FIELD_BITS(8) << REG_BIT_DRAMCFG_DNRAM_OFF)


/*******************************************************************************
 **
 **			Section for POSPHY and UTOPIA 
 **
 *******************************************************************************/
		/* POSPHY/UTOPIA phy enable */
#define IPMUX_NUM_OF_PHY_PER_GROUP		18
#define IPMUX_NUM_OF_PHY_GROUP_SL		(IPMUX_NUM_OF_PHY_SL / IPMUX_NUM_OF_PHY_PER_GROUP)
#define IPMUX_NUM_OF_PHY_GROUP_DL		(IPMUX_NUM_OF_PHY_DL / IPMUX_NUM_OF_PHY_PER_GROUP)
#define IPMUX_NUM_OF_PHY_GROUP			IPMUX_NUM_OF_PHY_GROUP_DL

#define REG_PHYCTRL0						(IPMUX_REG_BASE_UTOPIA + 0x0)
#if 0
#define REG_PHYCTRLx(phyid)					(REG_PHYCTRL0 + (phyid) / IPMUX_NUM_OF_PHY_PER_GROUP * REG_ADDR_ALLIGNED)
#endif

	/* symmetric, phy enable register address */
/*#define REG_PHYCTRLx2(mod, phyid)			(REG_PHYCTRL0 + (phyid) / IPMUX_NUM_OF_PHY_PER_GROUP * REG_ADDR_ALLIGNED + (IPMUX_SINGLE_LATENCY==(mod) ? (phyid) / (2 * IPMUX_NUM_OF_PHY_PER_GROUP) * 8 : 0))*/
#define REG_PHYCTRLx_ADJUST_FOR_SL_DL2(mod, phyid) ((IPMUX_SINGLE_LATENCY==(mod)) ? ((phyid) / (2 * IPMUX_NUM_OF_PHY_PER_GROUP) * 8) : ((phyid) / (3 * IPMUX_NUM_OF_PHY_PER_GROUP) * 4))
#define REG_PHYCTRLx2(mod, phyid)			(REG_PHYCTRL0 + (phyid) / IPMUX_NUM_OF_PHY_PER_GROUP * REG_ADDR_ALLIGNED + (IPMUX_DUAL_LATENCY ==(mod) ? 0: REG_PHYCTRLx_ADJUST_FOR_SL_DL2(mod, phyid)))
	/* asymmetric, phy enable register address */
#define REG_PHYCTRLxa(mod, phyid)			(REG_PHYCTRL0 + (IPMUX_SINGLE_LATENCY==(mod) ? ((phyid) > 39 ? (phyid) - 4 : (phyid)) : ((phyid) >71 ? (phyid) - 8 : (phyid))) / IPMUX_NUM_OF_PHY_PER_GROUP * REG_ADDR_ALLIGNED + (IPMUX_DUAL_LATENCY==(mod) ||IPMUX_DUAL_LATENCY2==(mod) || ((phyid)<=39/* && (phyid) > 35*/) ? 0 : ((phyid) > 39 ? (phyid) - 4 : (phyid)) / (2 * IPMUX_NUM_OF_PHY_PER_GROUP) * REG_ADDR_ALLIGNED * 2))

	/* phy enable register address */
#define REG_PHYCTRLx(grpMd, mod, phyid)		(ASYMMETRIC_MODE == (grpMd) ? REG_PHYCTRLxa(mod, phyid) : REG_PHYCTRLx2(mod, phyid))

	/* symmetric, phy enable bit */
#define REG_BIT_PHYCTRL_PHYEx_OFF(phyid)	((phyid) % IPMUX_NUM_OF_PHY_PER_GROUP)
#define REG_BIT_PHYCTRL_PHYEx(phyid)		(FIELD_BITS(1) << REG_BIT_PHYCTRL_PHYEx_OFF(phyid))

	/* asymmetric, dual mode, phy enable bit */
#define REG_BIT_PHYCTRL_PHYExad_OFF(phyid)	(((phyid) >= 80 ? (phyid) - 8 : (phyid)) % ((phyid) > 53 && (phyid) < 80 ? 54 : IPMUX_NUM_OF_PHY_PER_GROUP))
	/* asymmetric, single mode, phy enable bit */
#define REG_BIT_PHYCTRL_PHYExas_OFF(phyid)	(((phyid) >= 40 ? (phyid) - 4 : (phyid)) % ((phyid) > 35 && (phyid) < 40 ? 36 : IPMUX_NUM_OF_PHY_PER_GROUP))
	/* asymmetric, phy enable bit */
#define REG_BIT_PHYCTRL_PHYExa_OFF(mod, phyid)	(IPMUX_SINGLE_LATENCY==(mod) ? REG_BIT_PHYCTRL_PHYExas_OFF(phyid) : REG_BIT_PHYCTRL_PHYExad_OFF(phyid))

	/* phy enable bit */
#define REG_BIT_PHYCTRL_PHYEx2(grpMd, mod, phyid)	(FIELD_BITS(1) << (ASYMMETRIC_MODE == (grpMd) ? REG_BIT_PHYCTRL_PHYExa_OFF(mod, phyid) : REG_BIT_PHYCTRL_PHYEx_OFF(phyid)))


		/* POSPHY/UTOPIA interface Configuration */
#define REG_IFCFG						(IPMUX_REG_BASE_UTOPIA + 0x20)
#define REG_BIT_IFCFG_E1_OFF			31
#define REG_BIT_IFCFG_E1				(FIELD_BITS(1) << REG_BIT_IFCFG_E1_OFF)

/* Only support in IPMux-E2 */
#define REG_BIT_IFCFG_RX_INTLVDM1_OFF  30
#define REG_BIT_IFCFG_RX_INTLVDM1				(FIELD_BITS(1) << REG_BIT_IFCFG_RX_INTLVDM1_OFF)

/* Only support in IPMux-E2 */
#define REG_BIT_IFCFG_RX_SHAREM1_OFF  29
#define REG_BIT_IFCFG_RX_SHAREM1				(FIELD_BITS(1) << REG_BIT_IFCFG_RX_SHAREM1_OFF)


#define REG_BIT_IFCFG_ETHRSHLD1_OFF	21
#define REG_BIT_IFCFG_ETHRSHLD1		(FIELD_BITS(8) << REG_BIT_IFCFG_ETHRSHLD1_OFF)
#define REG_BIT_IFCFG_INTLVDM1_OFF	20
#define REG_BIT_IFCFG_INTLVDM1			(FIELD_BITS(1) << REG_BIT_IFCFG_INTLVDM1_OFF)
#define REG_BIT_IFCFG_P1_OFF			19
#define REG_BIT_IFCFG_P1				(FIELD_BITS(1) << REG_BIT_IFCFG_P1_OFF)
#define REG_BIT_IFCFG_U1IBUS8_OFF		18
#define REG_BIT_IFCFG_U1IBUS8			(FIELD_BITS(1) << REG_BIT_IFCFG_U1IBUS8_OFF)
#define REG_BIT_IFCFG_U1EBUS8_OFF		17
#define REG_BIT_IFCFG_U1EBUS8			(FIELD_BITS(1) << REG_BIT_IFCFG_U1EBUS8_OFF)
#define REG_BIT_IFCFG_U1ICG_OFF		16
#define REG_BIT_IFCFG_U1ICG			(FIELD_BITS(1) << REG_BIT_IFCFG_U1ICG_OFF)

#define REG_BIT_IFCFG_E0_OFF			15
#define REG_BIT_IFCFG_E0				(FIELD_BITS(1) << REG_BIT_IFCFG_E0_OFF)

/* Only support in IPMux-E2 */
#define REG_BIT_IFCFG_RX_INTLVDM0_OFF	14
#define REG_BIT_IFCFG_RX_INTLVDM0			(FIELD_BITS(1) << REG_BIT_IFCFG_RX_INTLVDM0_OFF)

/* Only support in IPMux-E2 */
#define REG_BIT_IFCFG_RX_SHAREM0_OFF         13
#define REG_BIT_IFCFG_RX_SHAREM0				(FIELD_BITS(1) << REG_BIT_IFCFG_RX_SHAREM0_OFF)


#define REG_BIT_IFCFG_ETHRSHLD0_OFF	5
#define REG_BIT_IFCFG_ETHRSHLD0		(FIELD_BITS(8) << REG_BIT_IFCFG_ETHRSHLD0_OFF)
#define REG_BIT_IFCFG_INTLVDM0_OFF	4
#define REG_BIT_IFCFG_INTLVDM0			(FIELD_BITS(1) << REG_BIT_IFCFG_INTLVDM0_OFF)
#define REG_BIT_IFCFG_P0_OFF			3
#define REG_BIT_IFCFG_P0				(FIELD_BITS(1) << REG_BIT_IFCFG_P0_OFF)
#define REG_BIT_IFCFG_U0IBUS8_OFF		2
#define REG_BIT_IFCFG_U0IBUS8			(FIELD_BITS(1) << REG_BIT_IFCFG_U0IBUS8_OFF)
#define REG_BIT_IFCFG_U0EBUS8_OFF		1
#define REG_BIT_IFCFG_U0EBUS8			(FIELD_BITS(1) << REG_BIT_IFCFG_U0EBUS8_OFF)
#define REG_BIT_IFCFG_U0ICG_OFF		0
#define REG_BIT_IFCFG_U0ICG			(FIELD_BITS(1) << REG_BIT_IFCFG_U0ICG_OFF)

		/* POSPHY/UTOPIA interface loopback control */
#define REG_IFLP						(IPMUX_REG_BASE_UTOPIA + 0x24)
#define REG_BIT_IFLP_LP_OFF		0
#define REG_BIT_IFLP_LP				(FIELD_BITS(1) << REG_BIT_IFLP_LP_OFF)

		/* UTOPIA HEC Check Control */
#define REG_IFHECC						(IPMUX_REG_BASE_UTOPIA + 0x28)
#define REG_BIT_IFHECC_HCE1_OFF		16
#define REG_BIT_IFHECC_HCE1			(FIELD_BITS(1) << REG_BIT_IFHECC_HCE1_OFF)
#define REG_BIT_IFHECC_HCE0_OFF		0
#define REG_BIT_IFHECC_HCE0			(FIELD_BITS(1) << REG_BIT_IFHECC_HCE0_OFF)

		/* UTOPIA OAM Cell Check Control */
#define REG_IFOAMC						(IPMUX_REG_BASE_UTOPIA + 0x2C)
#define REG_BIT_IFOAMC_OCCE1_OFF		16
#define REG_BIT_IFOAMC_OCCE1			(FIELD_BITS(1) << REG_BIT_IFOAMC_OCCE1_OFF)
#define REG_BIT_IFOAMC_OCCE0_OFF		0
#define REG_BIT_IFOAMC_OCCE0			(FIELD_BITS(1) << REG_BIT_IFOAMC_OCCE0_OFF)

		/* UTOPIA/POSPHY Work Mode Configure */
#define REG_IFGRPMD						(IPMUX_REG_BASE_UTOPIA + 0x30)
#define REG_BIT_IFGRPMD_GRPMD_OFF	0
#define REG_BIT_IFGRPMD_GRPMD			(FIELD_BITS(2) << REG_BIT_IFGRPMD_GRPMD_OFF)

/* Only support in IPMux-E2 */
		/* POSPHY interface mode configure */
#define REG_IFMODCFG					(IPMUX_REG_BASE_UTOPIA + 0x34)
#define REG_BIT_IFMODCFG_IFM1_OFF	1
#define REG_BIT_IFMODCFG_IFM1			(FIELD_BITS(1) << REG_BIT_IFMODCFG_IFM1_OFF)
#define REG_BIT_IFMODCFG_IFM0_OFF	0
#define REG_BIT_IFMODCFG_IFM0			(FIELD_BITS(1) << REG_BIT_IFMODCFG_IFM0_OFF)



		/*POSPHY PTM Mode MAX ,Min Packet Length,add by Wu xiaosong 2007/08/14*/
#define REG_PHYPACKETLENMAX			(IPMUX_REG_BASE_UTOPIA + 0x58)
#define REG_BIT_DDR_MAX_LEN_OFF		0
#define REG_BIT_DDR_MAX_LEN			(FIELD_BITS(9) << REG_BIT_DDR_MAX_LEN_OFF)

#define REG_PHYPACKETLENMIN			(IPMUX_REG_BASE_UTOPIA + 0x5C)
#define REG_BIT_DDR_MIN_LEN_OFF		0
#define REG_BIT_DDR_MIN_LEN			(FIELD_BITS(11) << REG_BIT_DDR_MIN_LEN_OFF)

		/* POSPHY Packet type, yzhang add 20061114 for IPMux-e */
#define REG_PHYPTME					(IPMUX_REG_BASE_UTOPIA + 0x68)
#define REG_PHYFCSE						(IPMUX_REG_BASE_UTOPIA + 0x8C)

		/* ASM configuration */
#define REG_DDRADDR					0x1040
#define REG_BIT_DDRADDR_AM_OFF		16
#define REG_BIT_DDRADDR_AM			(FIELD_BITS(12) << REG_BIT_DDRADDR_AM_OFF)

		/* Link aggregation configuration */
#define REG_LINKCTRL					0x800C
#define REG_BIT_LINKCTRL_GUE1_OFF		9
#define REG_BIT_LINKCTRL_GUE1			(FIELD_BITS(1) << REG_BIT_LINKCTRL_GUE1_OFF)
#define REG_BIT_LINKCTRL_GUE0_OFF		8
#define REG_BIT_LINKCTRL_GUE0			(FIELD_BITS(1) << REG_BIT_LINKCTRL_GUE0_OFF)
#define REG_BIT_LINKCTRL_DCW_OFF		5
#define REG_BIT_LINKCTRL_DCW			(FIELD_BITS(3) << REG_BIT_LINKCTRL_DCW_OFF)
#define REG_BIT_LINKCTRL_MODE_OFF		3
#define REG_BIT_LINKCTRL_MODE			(FIELD_BITS(2) << REG_BIT_LINKCTRL_MODE_OFF)
#define REG_BIT_LINKCTRL_LCTL_OFF		1
#define REG_BIT_LINKCTRL_LCTL			(FIELD_BITS(2) << REG_BIT_LINKCTRL_LCTL_OFF)
#define REG_BIT_LINKCTRL_LADA_OFF		0
#define REG_BIT_LINKCTRL_LADA			(FIELD_BITS(1) << REG_BIT_LINKCTRL_LADA_OFF)

		/* ASM configuration */
#define REG_ASM_ABSIA					0x13c00
#define REG_BIT_ASM_ABSIA_ARBE_OFF	31
#define REG_BIT_ASM_ABSIA_ARBE		(FIELD_BITS(1) << REG_BIT_ASM_ABSIA_ARBE_OFF)
#define REG_BIT_ASM_ABSIA_MADDR_OFF	0
#define REG_BIT_ASM_ABSIA_MADDR		(FIELD_BITS(22) << REG_BIT_ASM_ABSIA_MADDR_OFF)


		/* errno */

		/* POS-PHY            -0x1500--0x15FF */
#define ERRNO_MISC_SYS_FLOW_FULL		(OPL_ERROR - 0x1500)
#define ERRNO_MISC_PORT_FLOW_FULL	(OPL_ERROR - 0x1501)
#define ERRNO_MISC_PORT_SM_REG		(OPL_ERROR - 0x1502)

/* typedef  */

	/* system global definitions */
typedef enum IPMUX_RESET_TYPE_s
	{
	SOFT_RESET=0x1234,
	INTERNAL_RESET=0xabcd, /* magic number in case of mistake */
	RESET_TYPE_END
}IPMUX_RESET_TYPE_e;

typedef enum IPMUX_IF_TYPE_s
	{
	IPMUX_UTOPIA,
	IPMUX_POSPHY,
	TYPE_END
}IPMUX_IF_TYPE_e;

typedef enum IPMUX_IF_PARITY_s
	{
	PARITY_ODD,
	PARITY_EVEN,
	PARITY_END
}IPMUX_IF_PARITY_e;

typedef enum IPMUX_IF_WIDTH_s
	{
	WIDTH_16BIT,
	WIDTH_8BIT,
	WIDTH_END
}IPMUX_IF_WIDTH_e;

typedef enum IPMUX_PL2_MODE_s
	{
	PL2_INTERLEAVED,
	PL2_RESELECTION,
	PL2_PACKET,
	PL2_MODE_END
}IPMUX_PL2_MODE_e;

typedef enum IPMUX_UL2_ICG_s
	{
	ICG_ONE_CLOCK,
	ICG_TWO_CLOCKS,
	UL2_ICG_END
}IPMUX_UL2_ICG_t;

typedef enum IPMUX_IF_LP_s
	{
	IF_NORMAL,
	IF_LOOPBACK,
	IF_LP_END
}IPMUX_IF_LP_e;

typedef enum IPMUX_IF_GRPMD_s
	{
	SINGLE_GROUP,
	FOUR_GROUP,
	THREE_GROUP,
	ASYMMETRIC_MODE,
	IF_GRPMD_END
}IPMUX_IF_GRPMD_e;

/* Only support in IPMux-E2 */
typedef enum IPMUX_POSPHY_IFMODE_s
{
	NORMAL_POSPHY_LEVEL2,
	UTOPIA_LEVEL2,
	POSPHY_IFMOD_END
}IPMUX_POSPHY_IFMODE_e;


extern OPL_STATUS ipmuxPhyNumGet (UINT16 *usPhyNum );
extern void ipmuxChipInternalReset ( void );
extern OPL_STATUS ipmuxLatencyModeSet (UINT32 u32Latency);
extern OPL_STATUS ipmuxInterfaceModeSet (UINT32 u32Tpm);
extern OPL_STATUS ipmuxInterfaceGroupModeSet (UINT32 u32GrpMd);
extern OPL_STATUS ipmuxMultiPhyEnable (char * pPhyids);
extern OPL_STATUS ipmuxMultiPhyDisable (char * pPhyids);
extern OPL_STATUS ipmuxPhyEnable (UINT16 u16Phyid);
extern OPL_STATUS ipmuxUTOPIAWidthSet (UINT32 u8Mid, UINT32 u8IWidth, UINT32 u8EWidth);
extern OPL_STATUS ipmuxInterfaceParitySet (UINT32 u8Mid, UINT32 u8Type);
extern OPL_STATUS ipmuxUTOPIAInterCellClockSet (UINT32 u32Mid, UINT32 u32State);
extern OPL_STATUS ipmuxUTOPIAHecCheckSet (UINT32 u32Mid, UINT32 u32State);
extern OPL_STATUS ipmuxUTOPIAOamCheckSet (UINT32 u32Mid, UINT32 u32State);
extern OPL_STATUS ipmuxPOSPHYTxModeSet (UINT32 u32Mid, UINT32 u32Mod);
/* Only support in IPMux-E2 */
extern OPL_STATUS ipmuxPOSPHYRxModeSet (UINT32 u32Mid, UINT32 u32Mod);

extern OPL_STATUS ipmuxPOSPHYTxThresholdSet (UINT32 u8Mid, UINT32 u8Thrshold);
extern OPL_STATUS ipmuxInterfaceEnable (UINT32 u8Mid);
extern OPL_STATUS ipmuxInterfaceDisable (UINT32 u8Mid);
#endif /* MISC_API_DRIVER_INCLUDE */	
