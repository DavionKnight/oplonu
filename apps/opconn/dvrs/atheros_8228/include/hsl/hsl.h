/*
 * Copyright (c) 2007-2008 Atheros Communications, Inc.
 * All rights reserved.
 *
 */

#ifndef _HSL_H
#define _HSL_H

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "init/ssdk_init.h"
#include "hsl/shiva_reg_access.h"

typedef sw_error_t
    (*hsl_reg_access_mode_set) (a_uint32_t dev_id, hsl_access_mode mode);

typedef sw_error_t
    (*hsl_acl_rule_copy) (a_uint32_t dev_id, a_uint32_t src_addr,
                                a_uint32_t dest_addr, a_uint32_t size);

typedef sw_error_t
    (*hsl_acl_rule_invalid) (a_uint32_t dev_id, a_uint32_t addr,
                                    a_uint32_t size);

typedef sw_error_t
    (*hsl_acl_addr_update) (a_uint32_t dev_id, a_uint32_t old_addr,
                                a_uint32_t new_addr, a_uint32_t info);

typedef struct {
    hsl_acl_rule_copy     acl_rule_copy;
    hsl_acl_rule_invalid  acl_rule_invalid;
    hsl_acl_addr_update   acl_addr_update;
} hsl_acl_func_t;

#if defined ATHENA
    #define _hsl_reg_entry_get     athena_reg_get
    #define _hsl_reg_entry_set     athena_reg_set
    #define _hsl_reg_field_get     athena_reg_field_get
    #define _hsl_reg_field_set     athena_reg_field_set
    #define _hsl_reg_entry_gen_get athena_reg_get
    #define _hsl_reg_entry_gen_set athena_reg_set
    #define _hsl_reg_field_gen_get athena_reg_field_get
    #define _hsl_reg_field_gen_set athena_reg_field_set
    #define _hsl_phy_get           athena_phy_get
    #define _hsl_phy_set           athena_phy_set
#elif defined GARUDA
    #define _hsl_reg_entry_get     garuda_reg_get
    #define _hsl_reg_entry_set     garuda_reg_set
    #define _hsl_reg_field_get     garuda_reg_field_get
    #define _hsl_reg_field_set     garuda_reg_field_set
    #define _hsl_reg_entry_gen_get garuda_reg_get
    #define _hsl_reg_entry_gen_set garuda_reg_set
    #define _hsl_reg_field_gen_get garuda_reg_field_get
    #define _hsl_reg_field_gen_set garuda_reg_field_set
    #define _hsl_phy_get           garuda_phy_get
    #define _hsl_phy_set           garuda_phy_set
#elif defined SHIVA
    #define _hsl_reg_entry_get     shiva_reg_get
    #define _hsl_reg_entry_set     shiva_reg_set
    #define _hsl_reg_field_get     shiva_reg_field_get
    #define _hsl_reg_field_set     shiva_reg_field_set
    #define _hsl_reg_entry_gen_get shiva_reg_get
    #define _hsl_reg_entry_gen_set shiva_reg_set
    #define _hsl_reg_field_gen_get shiva_reg_field_get
    #define _hsl_reg_field_gen_set shiva_reg_field_set
    #define _hsl_phy_get           shiva_phy_get
    #define _hsl_phy_set           shiva_phy_set
#endif

#define HSL_REG_ENTRY_GET(rv, dev, reg, index, value, val_len) \
    rv = _hsl_reg_entry_get(dev, reg##_OFFSET + ((a_uint32_t)index) * reg##_E_OFFSET,\
                               (a_uint8_t*)value, (a_uint8_t)val_len);

#define HSL_REG_ENTRY_SET(rv, dev, reg, index, value, val_len) \
    rv = _hsl_reg_entry_set (dev, reg##_OFFSET + ((a_uint32_t)index) * reg##_E_OFFSET,\
                               (a_uint8_t*)value, (a_uint8_t)val_len);

#define HSL_REG_FIELD_GET(rv, dev, reg, index, field, value, val_len) \
    rv = _hsl_reg_field_get(dev, reg##_OFFSET + ((a_uint32_t)index) * reg##_E_OFFSET,\
                              reg##_##field##_BOFFSET, \
                              reg##_##field##_BLEN, (a_uint8_t*)value, val_len);

#define HSL_REG_FIELD_SET(rv, dev, reg, index, field, value, val_len) \
    rv = _hsl_reg_field_set(dev, reg##_OFFSET + ((a_uint32_t)index) * reg##_E_OFFSET,\
                              reg##_##field##_BOFFSET, \
                              reg##_##field##_BLEN, (a_uint8_t*)value, val_len);

#define HSL_REG_ENTRY_GEN_GET(rv, dev, addr, reg_len, value, val_len) \
    rv = _hsl_reg_entry_gen_get (dev, addr, (a_uint8_t*)value, val_len);

#define HSL_REG_ENTRY_GEN_SET(rv, dev, addr, reg_len, value, val_len) \
    rv = _hsl_reg_entry_gen_set (dev, addr, (a_uint8_t*)value, val_len);

#define HSL_REG_FIELD_GEN_GET(rv, dev, regaddr, bitlength, bitoffset, value, val_len) \
    rv = _hsl_reg_field_gen_get(dev, regaddr, bitoffset, bitlength, \
                               (a_uint8_t *) value, val_len);

#define HSL_REG_FIELD_GEN_SET(rv, dev, regaddr, bitlength, bitoffset, value, val_len) \
    rv = _hsl_reg_field_gen_set(dev, regaddr, bitoffset, bitlength, \
                               (a_uint8_t *) value, val_len);

#define HSL_PHY_GET(rv, dev, phy_addr, reg, value) \
    rv = _hsl_phy_get(dev, phy_addr, reg, value);

#define HSL_PHY_SET(rv, dev, phy_addr, reg, value) \
    rv = _hsl_phy_set(dev, phy_addr, reg, value);

/* added by zran */
#define API_LOCK

//#if (defined(API_LOCK) \
//&& (defined(HSL_STANDALONG) || (defined(KERNEL_MODULE) && defined(USER_MODE))))
extern  aos_lock_t sw_hsl_api_lock;
#define HSL_API_LOCK    aos_lock(&sw_hsl_api_lock)
#define HSL_API_UNLOCK  aos_unlock(&sw_hsl_api_lock)
//#else
//#define HSL_API_LOCK
//#define HSL_API_UNLOCK
//#endif

#ifdef __cplusplus
    }
#endif                          /* __cplusplus */
#endif                          /* _HSL_H */
