/*
 * Copyright (c) 2007-2009 Atheros Communications, Inc.
 * All rights reserved.
 *
 */
/**
 * @defgroup shiva_port_ctrl SHIVA_PORT_CONTROL
 * @{
 */
#ifndef _SHIVA_PORT_CTRL_H_
#define _SHIVA_PORT_CTRL_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "fal_port_ctrl.h"

sw_error_t shiva_port_ctrl_init(a_uint32_t dev_id);

#ifdef IN_PORTCONTROL
    #define SHIVA_PORT_CTRL_INIT(rv, dev_id) \
    { \
        rv = shiva_port_ctrl_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
    #define SHIVA_PORT_CTRL_INIT(rv, dev_id)
#endif

#ifdef HSL_STANDALONG


HSL_LOCAL sw_error_t
shiva_port_duplex_set(a_uint32_t dev_id, fal_port_t port_id,
                       fal_port_duplex_t duplex);


HSL_LOCAL sw_error_t
shiva_port_duplex_get(a_uint32_t dev_id, fal_port_t port_id,
                       fal_port_duplex_t * pduplex);


HSL_LOCAL sw_error_t
shiva_port_speed_set(a_uint32_t dev_id, fal_port_t port_id,
                      fal_port_speed_t speed);


HSL_LOCAL sw_error_t
shiva_port_speed_get(a_uint32_t dev_id, fal_port_t port_id,
                      fal_port_speed_t * pspeed);


HSL_LOCAL sw_error_t
shiva_port_autoneg_status_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t * status);


HSL_LOCAL sw_error_t
shiva_port_autoneg_enable(a_uint32_t dev_id, fal_port_t port_id);


HSL_LOCAL sw_error_t
shiva_port_autoneg_restart(a_uint32_t dev_id, fal_port_t port_id);


HSL_LOCAL sw_error_t
shiva_port_autoneg_adv_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t autoadv);


HSL_LOCAL sw_error_t
shiva_port_autoneg_adv_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t * autoadv);


HSL_LOCAL sw_error_t
shiva_port_hdr_status_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t enable);


HSL_LOCAL sw_error_t
shiva_port_hdr_status_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t * enable);


HSL_LOCAL sw_error_t
shiva_port_flowctrl_set(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t enable);


HSL_LOCAL sw_error_t
shiva_port_flowctrl_get(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t * enable);


HSL_LOCAL sw_error_t
shiva_port_flowctrl_forcemode_set(a_uint32_t dev_id,
                                   fal_port_t port_id, a_bool_t enable);


HSL_LOCAL sw_error_t
shiva_port_flowctrl_forcemode_get(a_uint32_t dev_id,
                                   fal_port_t port_id,
                                   a_bool_t * enable);


HSL_LOCAL sw_error_t
shiva_port_powersave_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


HSL_LOCAL sw_error_t
shiva_port_powersave_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);


HSL_LOCAL sw_error_t
shiva_port_hibernate_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


HSL_LOCAL sw_error_t
shiva_port_hibernate_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);


HSL_LOCAL sw_error_t
shiva_port_cdt(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t mdi_pair,
                fal_cable_status_t *cable_status, a_uint32_t *cable_len);


HSL_LOCAL sw_error_t
shiva_port_admin_status_get(a_uint32_t dev_id, fal_port_t port_id,a_bool_t *up);

HSL_LOCAL sw_error_t
shiva_phy_autoneg_done_read (a_uint32_t dev_id, fal_port_t port_id, a_bool_t * status);

#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _SHIVA_PORT_CTRL_H_ */
/**
 * @}
 */
