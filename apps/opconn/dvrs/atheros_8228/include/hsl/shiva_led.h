/*
 * Copyright (c) 2007-2009 Atheros Communications, Inc.
 * All rights reserved.
 *
 */
/**
 * @defgroup shiva_fdb SHIVA_FDB
 * @{
 */  
#ifndef _SHIVA_LED_H_
#define _SHIVA_LED_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "fal/fal_led.h"

sw_error_t
shiva_led_init(a_uint32_t dev_id);

#ifdef IN_LED
    #define SHIVA_LED_INIT(rv, dev_id) \
    { \
        rv = shiva_led_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
    #define SHIVA_LED_INIT(rv, dev_id)
#endif

#ifdef HSL_STANDALONG

/**
* @brief Set led control pattern on a particular device.
* @param[in] dev_id device id
* @param[in] group  pattern group, lan or wan
* @param[in] id pattern id
* @param[in] pattern led control pattern
* @return SW_OK or error code
*/
HSL_LOCAL sw_error_t
shiva_led_ctrl_pattern_set(a_uint32_t dev_id, led_pattern_group_t group,
                      led_pattern_id_t id, led_ctrl_pattern_t * pattern);


/**
* @brief Get led control pattern on a particular device.
* @param[in] dev_id device id
* @param[in] group  pattern group, lan or wan
* @param[in] id pattern id
* @param[out] pattern led control pattern
* @return SW_OK or error code
*/
HSL_LOCAL sw_error_t
shiva_led_ctrl_pattern_get(a_uint32_t dev_id, led_pattern_group_t group,
                      led_pattern_id_t id, led_ctrl_pattern_t * pattern);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SHIVA_FDB_H_ */
/**
 * @}
 */

