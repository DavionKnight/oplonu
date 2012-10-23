/*
 * Copyright (c) 2007-2008 Atheros Communications, Inc.
 * All rights reserved.
 *
 */
#include "sw.h"
#include "ssdk_init.h"
#include "sd.h"
#include "opl_driver.h"

extern OPL_STATUS mdioRegisterWrite(UINT8 deviceAddr,UINT8 regAddr,UINT16 data0);
extern OPL_STATUS mdioRegisterRead(UINT8 deviceAddr,UINT8 regAddr,UINT16 *data0);

mdio_reg_set ssdk_mdio_set    = mdioRegisterWrite;
mdio_reg_get ssdk_mdio_get    = mdioRegisterRead;
hdr_reg_set  ssdk_hdr_reg_set = NULL;
hdr_reg_get  ssdk_hdr_reg_get = NULL;

sw_error_t
sd_reg_mdio_set(a_uint32_t phy, a_uint32_t reg, a_uint16_t data)
{
    sw_error_t rv = SW_NOT_SUPPORTED;

    if (NULL != ssdk_mdio_set) {
        rv = ssdk_mdio_set(phy, reg, data);
    } else {
        return SW_NOT_SUPPORTED;
    }

    if (SW_OK != rv) {
        rv = SW_FAIL;
    }

    return rv;
}

sw_error_t
sd_reg_mdio_get(a_uint32_t phy, a_uint32_t reg, a_uint16_t * value)
{
    sw_error_t rv = SW_NOT_SUPPORTED;

    if (NULL != ssdk_mdio_get) {
        rv = ssdk_mdio_get(phy, reg, value);   
    } else {
        return SW_NOT_SUPPORTED;
    }

    return rv;
}

sw_error_t
sd_reg_hdr_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t * reg_data, a_uint8_t len)
{
    sw_error_t rv = SW_NOT_SUPPORTED;

    if (NULL != ssdk_hdr_reg_set) {
        rv = ssdk_hdr_reg_set(dev_id, reg_addr, reg_data, len);
    } else {
        return SW_NOT_SUPPORTED;
    }

    if (SW_OK != rv) {
            rv = SW_FAIL;
    }

    return rv;
}

sw_error_t
sd_reg_hdr_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t * reg_data, a_uint8_t len)
{
    sw_error_t rv = SW_NOT_SUPPORTED;

    if (NULL != ssdk_hdr_reg_get) {
        rv = ssdk_hdr_reg_get(dev_id, reg_addr, reg_data, len);
    } else {
        return SW_NOT_SUPPORTED;
    }

    if (SW_OK != rv) {
            rv = SW_FAIL;
    }

    return rv;
}

sw_error_t
sd_init(a_uint32_t dev_id, ssdk_init_cfg * cfg)
{
    if (NULL != cfg->reg_func.mdio_set) {
        ssdk_mdio_set = cfg->reg_func.mdio_set;
    }

    if (NULL != cfg->reg_func.mdio_get) {
        ssdk_mdio_get = cfg->reg_func.mdio_get;
    }

    if (NULL != cfg->reg_func.header_reg_set) {
        ssdk_hdr_reg_set = cfg->reg_func.header_reg_set;
    }

    if (NULL != cfg->reg_func.header_reg_get) {
        ssdk_hdr_reg_get = cfg->reg_func.header_reg_get;
    }

    return SW_OK;
}

a_bool_t
sd_mdio_set_is_inited(void)
{
    if (ssdk_mdio_set) {
        return A_TRUE;
    } else {
        return A_FALSE;
    }
}

a_bool_t
sd_mdio_get_is_inited(void)
{
    if (ssdk_mdio_get) {
        return A_TRUE;
    } else {
        return A_FALSE;
    }
}

