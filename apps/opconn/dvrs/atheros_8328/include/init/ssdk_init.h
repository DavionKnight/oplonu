/*
 * Copyright (c) 2007-2008 Atheros Communications, Inc.
 * All rights reserved.
 *
 */

#ifndef _SSDK_INIT_H_
#define _SSDK_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"

typedef enum {
    HSL_MDIO = 1,
    HSL_HEADER,
} hsl_access_mode;

typedef enum {
    HSL_NO_CPU = 0,
    HSL_CPU_1,
    HSL_CPU_2,
    HSL_CPU_1_PLUS,
} hsl_init_mode;

typedef sw_error_t
    (*mdio_reg_set) (a_uint32_t phy_addr, a_uint32_t reg,
                    a_uint16_t data);

typedef sw_error_t
    (*mdio_reg_get) (a_uint32_t phy_addr, a_uint32_t reg,
                    a_uint16_t * data);

typedef sw_error_t
    (*hdr_reg_set) (a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t *reg_data, a_uint32_t len);

typedef sw_error_t
    (*hdr_reg_get) (a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t *reg_data, a_uint32_t len);

typedef struct {
    mdio_reg_set    mdio_set;
    mdio_reg_get    mdio_get;
    hdr_reg_set     header_reg_set;
    hdr_reg_get     header_reg_get;
} hsl_reg_func;

typedef struct {
    a_bool_t  mac0_rgmii;
    a_bool_t  mac5_rgmii;
    a_bool_t  rx_delay_s0;
    a_bool_t  rx_delay_s1;
    a_bool_t  tx_delay_s0;
    a_bool_t  tx_delay_s1;
    a_bool_t  rgmii_rxclk_delay;
    a_bool_t  rgmii_txclk_delay;
    a_bool_t  phy4_rx_delay;
    a_bool_t  phy4_tx_delay;
}garuda_init_spec_cfg;

typedef enum {
    CHIP_UNSPECIFIED = 0,
    CHIP_ATHENA,
    CHIP_GARUDA,
    CHIP_SHIVA,
    CHIP_HORUS,
    CHIP_ISIS,
}ssdk_chip_type;

typedef struct {
    hsl_init_mode   cpu_mode;
    hsl_access_mode reg_mode;
    hsl_reg_func    reg_func;

    ssdk_chip_type  chip_type;

    /* os specific parameter */
    /* netlink protocol type for linux user-kernel space communication */
    a_uint32_t      nl_prot;

    /* chip specific parameter */
    void *          chip_spec_cfg;
} ssdk_init_cfg;

#if defined ATHENA
#define def_init_cfg  {.reg_mode = HSL_MDIO, .cpu_mode = HSL_CPU_2};
#elif defined GARUDA

#define def_init_cfg_cpu2  {.reg_mode = HSL_MDIO, .cpu_mode = HSL_CPU_2,};

#define def_init_spec_cfg_cpu2 {.mac0_rgmii = A_TRUE, .mac5_rgmii = A_TRUE, \
                    .rx_delay_s0 = A_FALSE, .rx_delay_s1 = A_FALSE, \
                    .tx_delay_s0 = A_TRUE,  .tx_delay_s1 = A_FALSE,\
                    .rgmii_rxclk_delay = A_TRUE, .rgmii_txclk_delay = A_TRUE,\
                    .phy4_rx_delay = A_TRUE, .phy4_tx_delay = A_TRUE,}

#define def_init_cfg_cpu1  {.reg_mode = HSL_MDIO, .cpu_mode = HSL_CPU_1,};

#define def_init_spec_cfg_cpu1 {.mac0_rgmii = A_TRUE, .mac5_rgmii = A_FALSE, \
                    .rx_delay_s0 = A_FALSE, .rx_delay_s1 = A_FALSE, \
                    .tx_delay_s0 = A_TRUE,  .tx_delay_s1 = A_FALSE,\
                    .rgmii_rxclk_delay = A_TRUE, .rgmii_txclk_delay = A_TRUE, \
                    .phy4_rx_delay = A_TRUE, .phy4_tx_delay = A_TRUE,}

#define def_init_cfg_cpu1plus  {.reg_mode = HSL_MDIO, .cpu_mode = HSL_CPU_1_PLUS,};

#define def_init_spec_cfg_cpu1plus {.mac0_rgmii = A_TRUE, .mac5_rgmii = A_FALSE, \
                    .rx_delay_s0 = A_FALSE, .rx_delay_s1 = A_FALSE, \
                    .tx_delay_s0 = A_FALSE,  .tx_delay_s1 = A_FALSE,\
                    .rgmii_rxclk_delay = A_TRUE, .rgmii_txclk_delay = A_TRUE, \
                    .phy4_rx_delay = A_TRUE, .phy4_tx_delay = A_TRUE,}

#define def_init_cfg_nocpu  {.reg_mode = HSL_MDIO, .cpu_mode = HSL_NO_CPU,};

#define def_init_spec_cfg_nocpu { .mac0_rgmii = A_FALSE, .mac5_rgmii = A_FALSE, \
                    .rx_delay_s0 = A_FALSE, .rx_delay_s1 = A_FALSE, \
                    .tx_delay_s0 = A_FALSE,  .tx_delay_s1 = A_FALSE,\
                    .rgmii_rxclk_delay = A_TRUE, .rgmii_txclk_delay = A_TRUE, \
                    .phy4_rx_delay = A_TRUE, .phy4_tx_delay = A_TRUE,}

#define def_init_cfg_cpu1_gmii  {.reg_mode = HSL_MDIO, .cpu_mode = HSL_CPU_1,};

#define def_init_spec_cfg_cpu1_gmii {.mac0_rgmii = A_FALSE, .mac5_rgmii = A_FALSE, \
                    .rx_delay_s0 = A_FALSE, .rx_delay_s1 = A_FALSE, \
                    .tx_delay_s0 = A_TRUE,  .tx_delay_s1 = A_FALSE,\
                    .rgmii_rxclk_delay = A_TRUE, .rgmii_txclk_delay = A_TRUE, \
                    .phy4_rx_delay = A_TRUE, .phy4_tx_delay = A_TRUE,}

#define def_init_cfg def_init_cfg_cpu2
#define def_init_spec_cfg def_init_spec_cfg_cpu2

#elif defined SHIVA
#define def_init_cfg  {.reg_mode = HSL_MDIO, .cpu_mode = HSL_CPU_2};
#elif defined HORUS
#define def_init_cfg  {.reg_mode = HSL_MDIO, .cpu_mode = HSL_CPU_2};
#elif defined ISIS
#define def_init_cfg  {.reg_mode = HSL_MDIO, .cpu_mode = HSL_CPU_2};
#endif

typedef struct {
    a_bool_t in_acl;
    a_bool_t in_fdb;
    a_bool_t in_igmp;
    a_bool_t in_leaky;
    a_bool_t in_led;
    a_bool_t in_mib;
    a_bool_t in_mirror;
    a_bool_t in_misc;
    a_bool_t in_portcontrol;
    a_bool_t in_portvlan;
    a_bool_t in_qos;
    a_bool_t in_rate;
    a_bool_t in_stp;
    a_bool_t in_vlan;
    a_bool_t in_reduced_acl;
    a_bool_t in_ip;
    a_bool_t in_nat;
    a_bool_t in_cosmap;
    a_bool_t in_sec;
    a_bool_t in_trunk;
}ssdk_features;

#define CFG_STR_SIZE 20
typedef struct {
    a_uint8_t build_ver[CFG_STR_SIZE];
    a_uint8_t build_date[CFG_STR_SIZE];

    a_uint8_t chip_type[CFG_STR_SIZE]; //GARUDA
    a_uint8_t cpu_type[CFG_STR_SIZE];  //mips
    a_uint8_t os_info[CFG_STR_SIZE];   //OS=linux OS_VER=2_6

    a_bool_t  fal_mod;
    a_bool_t  kernel_mode;
    a_bool_t  uk_if;    

    ssdk_features features;
    ssdk_init_cfg init_cfg;
} ssdk_cfg_t;

sw_error_t
ssdk_init(a_uint32_t dev_id, ssdk_init_cfg *cfg);

sw_error_t
ssdk_reduced_init(a_uint32_t dev_id, hsl_init_mode cpu_mode,
                  hsl_access_mode reg_mode);

sw_error_t
ssdk_hsl_access_mode_set(a_uint32_t dev_id, hsl_access_mode reg_mode);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _SSDK_INIT_H */
