/*
=============================================================================
     Header Name: ipmux_gmac.h

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
----------------   ------------  ----------------------------------------------
*/


#ifndef IPMUX_GMAC_INCLUDE
#define	IPMUX_GMAC_INCLUDE 

/* includes */
#include "opl_driver.h"

/* definitions */

	/* since this module can be compiled independently, below macroes are useful */
#if !defined (MODULE) && !defined (__KERNEL__)
#ifndef u32
#define u32 	UINT32
#endif

#ifndef u16
#define u16 	UINT16
#endif

#ifndef ulong
#define ulong 	unsigned long
#endif

#else
#include <asm/types.h>
#endif

	/* register base addresses */
#define OP_IPMUX_GEW0_ADDR	0xBF008000
#define OP_IPMUX_GEW1_ADDR	0xBF00A000
#define OP_IPMUX_GEWL_ADDR	0xBF00C000

     /* Bridge GEW Default VLAN ID and S-COS Config register, Only used in IPMux-E2 */
#define OP_IPMUX_REG_GEW_DEFAULT_VLAN_SCOS  0xBF020128

	/*GE0,1,2 Loopback config Register Address,Add by xwu 2007/08/14*/
#define OP_IPMUX_GEW0_LPBKCFG			0x8000
#define OP_IPMUX_GEW1_LPBKCFG			0xA000
#define OP_IPMUX_GEW2_LPBKCFG			0xC000

	/*GE0,1,2 Operation Mode Configuration,Add by xwu 2007/08/14*/
#define REG_GMAC_MOD_CFG				0x0004
#define OP_IPMUX_GEW0_OPMODE_CFG       (OP_IPMUX_GEW0_LPBKCFG + REG_GMAC_MOD_CFG)
#define OP_IPMUX_GEW1_OPMODE_CFG       (OP_IPMUX_GEW1_LPBKCFG + REG_GMAC_MOD_CFG)
#define OP_IPMUX_GEW2_OPMODE_CFG       (OP_IPMUX_GEW2_LPBKCFG + REG_GMAC_MOD_CFG)

	/*GE0,1 TBI Configure 0 Register,Add by xwu 2008/08/14*/
#define REG_GEW_CFG0					0x003C
#define OP_IPMUX_GEW0_CFG0			(OP_IPMUX_GEW0_LPBKCFG + REG_GEW_CFG0)
#define OP_IPMUX_GEW_CFG0_LP_BASE	16
#define IPMUX_GEW_CFG0_LP				(FIELD_BITS(16))
#define IPMUX_GEW_CFG0_ADV			(FIELD_BITS(16))
#define OP_IPMUX_GEW1_CFG0			(OP_IPMUX_GEW1_LPBKCFG + REG_GEW_CFG0)

	/*GE0,1 TBI Configure 1 Register,Add by xwu 2008/08/14*/
#define REG_GEW_CFG1					0x0040
#define OP_IPMUX_GEW0_CFG1			(OP_IPMUX_GEW0_LPBKCFG + REG_GEW_CFG1)
#define OP_IPMUX_GEW1_CFG1			(OP_IPMUX_GEW1_LPBKCFG + REG_GEW_CFG1)
#define IPMUX_GEW_CFG1_TX				(FIELD_BITS(16))

	/*GE0,1 TBI Configure 2 Register,Add by xwu 2008/08/14*/
#define REG_GEW_CFG2					0x0044
#define OP_IPMUX_GEW0_CFG2			(OP_IPMUX_GEW0_LPBKCFG + REG_GEW_CFG2)

#define OP_IPMUX_GEW1_CFG2			(OP_IPMUX_GEW1_LPBKCFG + REG_GEW_CFG2)
#define IPMUX_GEW_CFG2_TX				(FIELD_BITS(10))


	/* register 0x8200, 0xA200, 0xC200 GMAC Configuration 1 */
#define REG_GMAC_CFG1			0x200

#define OP_IPMUX_GE0_ADDR		(OP_IPMUX_GEW0_LPBKCFG + REG_GMAC_CFG1) /* the start address of GMAC control regs */
#define OP_IPMUX_GE1_ADDR		(OP_IPMUX_GEW1_LPBKCFG + REG_GMAC_CFG1)
#define OP_IPMUX_GEL_ADDR		(OP_IPMUX_GEWL_ADDR + REG_GMAC_CFG1)

#define GMAC_CONFIG1_SOFT_RESET_SHIFT		31
#define GMAC_CONFIG1_SIM_RESET_SHIFT			30
#define GMAC_CONFIG1_RESET_RX_CNTL_SHIFT	19
#define GMAC_CONFIG1_RESET_TX_CNTL_SHIFT	18
#define GMAC_CONFIG1_RESET_RX_FUNC_SHIFT	17
#define GMAC_CONFIG1_RESET_TX_FUNC_SHIFT	16
#define GMAC_CONFIG1_LOOPBACK_SHIFT			8
#define GMAC_CONFIG1_RX_FLOW_CNTL_SHIFT		5
#define GMAC_CONFIG1_TX_FLOW_CNTL_SHIFT		4
#define GMAC_RXRD_ENABLE_SHIFT				3
#define GMAC_RX_ENABLE_SHIFT					2
#define GMAC_TXRD_ENABLE_SHIFT				1
#define GMAC_TX_ENABLE_SHIFT					0

#define GMAC_RX_ENABLE						0x4
#define GMAC_TX_ENABLE						0x1

#define GMAC_CONFIG_1(soft_reset,      \
			 sim_reset,       \
			 reset_rx_cntl,   \
			 reset_tx_cntl,   \
                         reset_rx_func,   \
                         reset_tx_func,   \
                         loopback,        \
                         rx_flow_cntl,    \
                         tx_flow_cntl,    \
                         rx_enable,       \
                         tx_enable)       \
 ((soft_reset << GMAC_CONFIG1_SOFT_RESET_SHIFT) | \
  (sim_reset << GMAC_CONFIG1_SIM_RESET_SHIFT) | \
  (reset_rx_cntl << GMAC_CONFIG1_RESET_RX_CNTL_SHIFT) | \
  (reset_tx_cntl << GMAC_CONFIG1_RESET_TX_CNTL_SHIFT) | \
  (reset_rx_func << GMAC_CONFIG1_RESET_RX_FUNC_SHIFT) | \
  (reset_tx_func << GMAC_CONFIG1_RESET_TX_FUNC_SHIFT) | \
  (loopback << GMAC_CONFIG1_LOOPBACK_SHIFT) | \
  (rx_flow_cntl << GMAC_CONFIG1_RX_FLOW_CNTL_SHIFT) | \
  (tx_flow_cntl << GMAC_CONFIG1_TX_FLOW_CNTL_SHIFT) | \
  (rx_enable << GMAC_RX_ENABLE_SHIFT) | \
  (tx_enable << GMAC_TX_ENABLE_SHIFT))


	/* register 0x8204, 0xA204, 0xC204 GMAC Configuration 2 */
#define REG_GMAC_CFG2			0x204
#define GMAC_CONFIG2_FULL_DUPLEX_SHIFT			0
#define GMAC_CONFIG2_CRC_EN_SHIFT				1
#define GMAC_CONFIG2_PAD_CRC_EN_SHIFT			2
#define GMAC_CONFIG2_LEN_CHECK_EN_SHIFT			4
#define GMAC_CONFIG2_HUGE_FRAME_EN_SHIFT		5
#define GMAC_CONFIG2_INTERFACE_MODE_SHIFT		8
#define GMAC_CONFIG2_PREAMBLE_LEN_SHIFT			12 

#define GMAC_CONFIG_2(full_duplex,  \
			 crc_en,                   \
			 pad_and_crc_en,           \
			 len_field_check_en,       \
			 huge_frame_en,            \
			 interface_mode,           \
			 preamble_length)          \
( ((full_duplex) << GMAC_CONFIG2_FULL_DUPLEX_SHIFT) | \
  ((crc_en)      << GMAC_CONFIG2_CRC_EN_SHIFT)      | \
  ((pad_and_crc_en) << GMAC_CONFIG2_PAD_CRC_EN_SHIFT) | \
  ((len_field_check_en) << GMAC_CONFIG2_LEN_CHECK_EN_SHIFT) | \
  ((huge_frame_en) << GMAC_CONFIG2_HUGE_FRAME_EN_SHIFT) | \
  ((interface_mode) << GMAC_CONFIG2_INTERFACE_MODE_SHIFT) | \
  ((preamble_length) << GMAC_CONFIG2_PREAMBLE_LEN_SHIFT))

	/* register 0x8208, 0xA208, 0xC208 GMAC IPG */
#define REG_GMAC_IPG			0x208
#define GMAC_NON_B2B_IPG1_SHIFT			24
#define GMAC_NON_B2B_IPG2_SHIFT			16
#define GMAC_MIN_IFG_SHIFT				8
#define GMAC_B2B_IPG_SHIFT				0

#define DEFAULT_IPG_IFG					0x4060_5060

#define GMAC_IPG_IFG(b2b_ipg, \
			min_ifg,          \
			non_b2b_ipg2,     \
			non_b2b_ipg1)     \
 ((b2b_ipg << GMAC_B2B_IPG_SHIFT) |    \
  (min_ifg << GMAC_MIN_IFG_SHIFT) |    \
  (non_b2b_ipg2 << GMAC_NON_B2B_IPG2_SHIFT) | \
  (non_b2b_ipg1 << GMAC_NON_B2B_IPG1_SHIFT))


	/* register 0x821C, 0xA21C, 0xC21C GMAC test register */
#define REG_GMAC_TEST			0x21C
#define TEST_MAXIMUM_BACKOFF_SHIFT			3
#define TEST_REGISTER_TX_FLOW_EN_SHIFT		2
#define TEST_PAUSE_SHIFT					1
#define TEST_SHORTCUT_SLOT_TIME_SHIFT		0

#define	IPMUX_GEW0 	 1
#define	IPMUX_GEW1 	 2
#define	IPMUX_GEWL 	 4

#define IPMUX_MAC_FE	IPMUX_GEWL /* alias */

/* typedefs */

	/* all GMAC Control Registers Map */
typedef struct gmac_ctrl_registers
{
	volatile u32  mac_config1;		/* 0x00 */
	volatile u32  mac_config2;		/* 0x04 */
	volatile u32  ipg_ifg;			/* 0x08 */
	volatile u32  half_duplex;		/* 0x0c */
	volatile u32  maxframelen;		/* 0x10 */
	volatile u32  reserved1[2];		/* 0x14-0x18 */
	volatile u32  test;				/* 0x1c */
	volatile u32  mii_config;		/* 0x20 */
	volatile u32  mii_command;	/* 0x24 */
	volatile u32  mii_addr;			/* 0x28 */
	volatile u32  mii_ctrl;			/* 0x2c */
	volatile u32  mii_stat;			/* 0x30 */
	volatile u32  mii_indicator;		/* 0x34 */
	volatile u32  interface_ctrl;	/* 0x38 */
	volatile u32  interface_stat;	/* 0x3c */
	volatile u32  sta_addr0;		/* 0x40 */
	volatile u32  sta_addr1;		/* 0x44 */
	volatile u32  reserved2[2];		/* 0x48-0x4c */
} gmac_ctrl_reg;

/* function forward declaration */
#ifdef OPULAN_RTOS_LINUX
#endif
/*
#ifdef OPL_IPMUX_E2
*/

#define	IPMUX_GE_LINK_MODE_REDUNDANCY		0
#define	IPMUX_GE_LINK_MODE_AGGREGATION		1
#define	IPMUX_GE_LINK_MODE_DAISYCHAIN		2
#define	IPMUX_GE_LINK_MODE_RESERVED			3

typedef enum IPMUX_GE_LINK_CTL_s
{
    IPMUX_GE_LINK_LADA = 1,
    IPMUX_GE_LINK_LCTL,
    IPMUX_GE_LINK_MODE,
    IPMUX_GE_LINK_DCW,
    IPMUX_GE_LINK_GUE0 = 5,
    IPMUX_GE_LINK_GUE1,
    IPMUX_GE_LINK_GLD0,
    IPMUX_GE_LINK_GLD1,
    IPMUX_GE_LINK_GDE0,
    IPMUX_GE_LINK_GDE1 = 10,
    IPMUX_GE_LINK_GRE0,
    IPMUX_GE_LINK_GRE1,
    IPMUX_GE_LINK_END
}IPMUX_GE_LINK_CTL_e;

typedef struct IPMUX_GE_LINK_CFG_s	/* for GE link configuration register */
{
#ifdef IPMUX_BIG_ENDIAN
	UINT32 	:16;
	UINT32	Gre1:1;
	UINT32	Gre0:1;	/* GE port 0 reserved mac enable */
	UINT32 	Gde1:1;
	UINT32 	Gde0:1;	/* GE port 0 daisy path enable */
	UINT32	Gld1:1;
	UINT32	Gld0:1;	/* GE port 0 link disable */
	UINT32 	Gue1:1;
	UINT32 	Gue0:1;	/* GE port 0 User traffic Enable */
	UINT32	Dcw:3;	/* daisy chain weight */
	UINT32	Mode:2;	/* linkd mode */
	UINT32	Lctl:2;	/* link control bits */
	UINT32 	Lada:1;	/* link aggregation distribution algorithm */
#else
	UINT32 	Lada:1;	/* link aggregation distribution algorithm */
	UINT32	Lctl:2;	/* link control bits */
	UINT32	Mode:2;	/* linkd mode */
	UINT32	Dcw:3;	/* daisy chain weight */
	UINT32 	Gue0:1;	/* GE port 0 User traffic Enable */
	UINT32 	Gue1:1;
	UINT32	Gld0:1;	/* GE port 0 link disable */
	UINT32	Gld1:1;
	UINT32 	Gde0:1;	/* GE port 0 daisy path enable */
	UINT32 	Gde1:1;
	UINT32	Gre0:1;	/* GE port 0 reserved mac enable */
	UINT32	Gre1:1;
	UINT32 	:16;
#endif
}IPMUX_GE_LINK_CFG_t;


typedef enum IPMUX_GE_RSVD_MAC_RCV_CTL_s
{
    IPMUX_GE_RSVD_MAC_RCV_GE1 = 1,
    IPMUX_GE_RSVD_MAC_RCV_GE0,
    IPMUX_GE_RSVD_MAC_RCV_END
}IPMUX_GE_RSVD_MAC_RCV_CTL_e;

typedef struct IPMUX_GE_RSVD_MAC_RCV_CFG_s	/* for GE  Reserved Mac Receive register */
{
#ifdef IPMUX_BIG_ENDIAN
	UINT32 	:30;
	UINT32 	Ge0:1;	
	UINT32 	Ge1:1;
#else
	UINT32 	Ge1:1;
	UINT32 	Ge0:1;	
	UINT32 	:30;
#endif
}IPMUX_GE_RSVD_MAC_RCV_CFG_t;


typedef enum IPMUX_BRIDGE_GE_CTL_BIT_s
{
	IPMUX_BRIDGE_GE_TRANSMIT = 1,
	IPMUX_BRIDGE_GE_LEARN,
	IPMUX_BRIDGE_GE_FLOOD,
	IPMUX_BRIDGE_GE_TWOTAG,
	IPMUX_BRIDGE_GE_ONETAG = 5,
	IPMUX_BRIDGE_GE_RECEIVE,
	IPMUX_BRIDGE_GE_FORWARD,
	IPMUX_BRIDGE_GE_UNTAG,
    IPMUX_BRIDGE_GE_END
}IPMUX_BRIDGE_GE_CTL_BIT_e;

typedef enum IPMUX_BRIDGE_GE_PF_BIT_s	/* pause frame type */
{
	IPMUX_BRIDGE_GE_PF_MIN = 1,
	IPMUX_BRIDGE_GE_PF_MAX,
    IPMUX_BRIDGE_GE_PF_END
}IPMUX_BRIDGE_GE_PF_BIT_e;

typedef struct IPMUX_BRIDGE_GE_CTL_REG_s		/* for GE Port Control Register */
{
#ifdef IPMUX_BIG_ENDIAN
	UINT32	:24;
	UINT32	Untag:1;  		/* receive untag packet */
	UINT32 	Forward:1;		/* forward enable */
	UINT32	Receive:1;		/* receive enable */
	UINT32	Onetag:1;  		/* receive single tag packet */
	UINT32	Twotag:1;  		/* receive double tag packet */
	UINT32 	Flood:1;		/* flooding enable */
	UINT32 	Learn:1;        /* source MAC leanring enable */ 
	UINT32 	Transmit:1;		/* transmit enable */
#else
	UINT32 	Transmit:1;
	UINT32 	Learn:1;	
	UINT32 	Flood:1;	
	UINT32	Twotag:1;  
	UINT32	Onetag:1;  
	UINT32	Receive:1;
	UINT32 	Forward:1;	
	UINT32	Untag:1;  		/* receive untag packet */
	UINT32	:24;
#endif
}IPMUX_BRIDGE_GE_CTL_REG_t;



typedef struct IPMUX_BRIDGE_GE_EPD_REG_s		/* for GE Port daisy buffer EPD Control Register */
{
#ifdef IPMUX_BIG_ENDIAN
	UINT32	:20;
	UINT32 	Epdt:12;		/* EPD Threshold */
#else
	UINT32 	Epdt:12;
	UINT32	:20;
#endif
}IPMUX_BRIDGE_GE_EPD_REG_t;



typedef struct IPMUX_BRIDGE_GE_PF_REG_s		/* for GE Port daisy buffer pause frame */
{
#ifdef IPMUX_BIG_ENDIAN
	UINT32	:8;
	UINT32 	Max:12;		/* Max Threshold */
	UINT32 	Min:12;		/* Min Threshold */
#else
	UINT32 	Min:12;		/* Min Threshold */
	UINT32 	Max:12;		/* Max Threshold */
	UINT32	:8;
#endif
}IPMUX_BRIDGE_GE_PF_REG_t;


typedef enum IPMUX_BRIDGE_GE_MAX_PKTG_s
{
    IPMUX_GE_PKT_MAX = 1,
    IPMUX_GE_PKT_PTO,
    IPMUX_GE_PKT_END
}IPMUX_BRIDGE_GE_MAX_PKTG_e;


typedef struct IPMUX_BRIDGE_GE_MAX_PKTG_REG_s		/* for GE Port daisy Max Packet Length */
{
#ifdef IPMUX_BIG_ENDIAN
	UINT32	PTO:1;	/* Pause Frame Timeout Counter Enable. */
	UINT32	:21;
	UINT32 	MaxLen:10;		/* Max packet length */
#else
	UINT32 	MaxLen:10;		/* Max packet length */
	UINT32	:21;
	UINT32	PTO:1;	/* Pause Frame Timeout Counter Enable. */
#endif
}IPMUX_BRIDGE_GE_MAX_PKTG_REG_t;


/* Only used in IPMux-E2 */
	
typedef struct IPMUX_BRIDGE_GE_DEFAULT_VLAN_SCOS_REG_s		/* For GE default VLAN and SCOS  */
{
#ifdef IPMUX_BIG_ENDIAN
	UINT32	reserved:17;	 /* Reserved */
	UINT32	SCOS:3;         /* SCOS */
	UINT32 	VID:12;		/* VLAN ID */
#else
	UINT32 	VID:12;		 /* VLAN ID*/
	UINT32	SCOS:3;         /* S-COS */
	UINT32	reserved:17;	/* Reserved  */
#endif
}IPMUX_BRIDGE_GE_DEFAULT_VLAN_SCOS_REG_t;
/*
#endif
*/
extern void ipmux_gmac_reset(u32 gmacs);
extern int ipmux_gmac_init(u32 gmacs);
extern int ipmux_gmac_set_speed(u32 gmacs, u16 speed);
extern int ipmux_gmac_set_duplex(u32 gmacs, u16 duplex);

extern OPL_STATUS ipmuxBrgGeDefaultVIDSet(UINT16 Vid);
extern OPL_STATUS ipmuxBrgGeDefaultVIDGet(UINT16 *Vid);

extern OPL_STATUS ipmuxBrgGeDefaultSCOSSet(UINT8 Scos);
extern OPL_STATUS ipmuxBrgGeDefaultSCOSGet(UINT8 *Scos);


#endif /* IPMUX_GMAC_INCLUDE */	
