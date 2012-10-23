/*
 * Copyright (c) 2007-2008 Atheros Communications, Inc.
 * All rights reserved.
 *
 */
/**
 * @defgroup fal_rate FAL_RATE
 * @{
 */ 
#ifndef _FAL_RATE_H_
#define _FAL_RATE_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"

/**
@brief This enum defines storm type
*/
typedef enum {
    FAL_UNICAST_STORM = 0,		/**<  storm caused by unknown unicast packets */
    FAL_MULTICAST_STORM,		/**<  storm caused by unknown multicast packets */
    FAL_BROADCAST_STORM,   		/**<  storm caused by broadcast packets */ 
    FAL_STORM_TYPE_BUTT
} fal_storm_type_t;



sw_error_t
fal_rate_queue_egrl_set(a_uint32_t dev_id, fal_port_t port_id,
                       fal_queue_t queue_id, a_uint32_t * speed,
                       a_bool_t enable);



sw_error_t
fal_rate_queue_egrl_get(a_uint32_t dev_id, fal_port_t port_id,
                       fal_queue_t queue_id, a_uint32_t * speed,
                       a_bool_t * enable);



sw_error_t
fal_rate_port_egrl_set(a_uint32_t dev_id, fal_port_t port_id,
                      a_uint32_t * speed, a_bool_t enable);



sw_error_t
fal_rate_port_egrl_get(a_uint32_t dev_id, fal_port_t port_id,
                      a_uint32_t * speed, a_bool_t * enable);



sw_error_t
fal_rate_port_inrl_set(a_uint32_t dev_id, fal_port_t port_id,
                      a_uint32_t * speed, a_bool_t enable);



sw_error_t
fal_rate_port_inrl_get(a_uint32_t dev_id, fal_port_t port_id,
                      a_uint32_t * speed, a_bool_t * enable);



sw_error_t
fal_storm_ctrl_frame_set(a_uint32_t dev_id, fal_port_t port_id,
                         fal_storm_type_t frame_type, a_bool_t enable);



sw_error_t
fal_storm_ctrl_frame_get(a_uint32_t dev_id, fal_port_t port_id,
                         fal_storm_type_t frame_type,
                         a_bool_t * enable);



sw_error_t
fal_storm_ctrl_rate_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t * rate);



sw_error_t
fal_storm_ctrl_rate_get(a_uint32_t dev_id, fal_port_t port_id,
                    a_uint32_t * rate);


#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_RATE_H_ */
/**
 * @}
 */
