/*
 * Copyright (c) 2007-2008 Atheros Communications, Inc.
 * All rights reserved.
 *
 */

#ifndef _SD_H_
#define _SD_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

extern mdio_reg_set phy_reg_write;
extern mdio_reg_get phy_reg_read;
extern hdr_reg_set  athr_header_write_reg;
extern hdr_reg_get  athr_header_read_reg;

sw_error_t
sd_reg_mdio_set(a_uint32_t phy, a_uint32_t reg, a_uint16_t data);

sw_error_t
sd_reg_mdio_get(a_uint32_t phy, a_uint32_t reg, a_uint16_t * value);

sw_error_t
sd_reg_hdr_set(a_uint32_t dev_id, a_uint32_t reg_addr,
               a_uint8_t * reg_data, a_uint8_t len);

sw_error_t
sd_reg_hdr_get(a_uint32_t dev_id, a_uint32_t reg_addr,
               a_uint8_t * reg_data, a_uint8_t len);

sw_error_t sd_init(a_uint32_t dev_id, ssdk_init_cfg * cfg);

a_bool_t sd_mdio_set_is_inited(void);

a_bool_t sd_mdio_get_is_inited(void);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _SD_H_ */
