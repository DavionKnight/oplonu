/*
 * Copyright (c) 2007-2009 Atheros Communications, Inc.
 * All rights reserved.
 *
 */
/**
 * @defgroup shiva_fdb SHIVA_FDB
 * @{
 */
#ifndef _SHIVA_FDB_H_
#define _SHIVA_FDB_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "fal_fdb.h"

#define ARL_FLUSH_ALL             1
#define ARL_LOAD_ENTRY            2
#define ARL_PURGE_ENTRY           3
#define ARL_FLUSH_ALL_UNLOCK      4
#define ARL_FLUSH_PORT_UNICAST    5
#define ARL_NEXT_ENTRY            6
#define ARL_FIND_ENTRY            7

#define ARL_FIRST_ENTRY           1001
#define ARL_FLUSH_PORT_NO_STATIC  1002
#define ARL_FLUSH_PORT_AND_STATIC 1003

sw_error_t
shiva_fdb_init(a_uint32_t dev_id);

#ifdef IN_FDB
    #define SHIVA_FDB_INIT(rv, dev_id) \
    { \
        rv = shiva_fdb_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
    #define SHIVA_FDB_INIT(rv, dev_id)
#endif

#ifdef HSL_STANDALONG


HSL_LOCAL sw_error_t
shiva_fdb_add(a_uint32_t dev_id, const fal_fdb_entry_t * entry);


HSL_LOCAL sw_error_t
shiva_fdb_del_all(a_uint32_t dev_id, a_uint32_t flag);


HSL_LOCAL sw_error_t
shiva_fdb_del_by_port(a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t flag);


HSL_LOCAL sw_error_t
shiva_fdb_del_by_mac(a_uint32_t dev_id, 
                                const fal_fdb_entry_t *entry);


HSL_LOCAL sw_error_t
shiva_fdb_first(a_uint32_t dev_id, fal_fdb_entry_t * entry);


HSL_LOCAL sw_error_t
shiva_fdb_find(a_uint32_t dev_id, fal_fdb_entry_t * entry);


HSL_LOCAL sw_error_t
shiva_fdb_port_learn_set(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t enable);


HSL_LOCAL sw_error_t
shiva_fdb_port_learn_get(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t *enable);


HSL_LOCAL sw_error_t
shiva_fdb_age_ctrl_set(a_uint32_t dev_id, a_bool_t enable);


HSL_LOCAL sw_error_t
shiva_fdb_age_ctrl_get(a_uint32_t dev_id, a_bool_t *enable);


HSL_LOCAL sw_error_t
shiva_fdb_age_time_set(a_uint32_t dev_id, a_uint32_t * time);


HSL_LOCAL sw_error_t
shiva_fdb_age_time_get(a_uint32_t dev_id, a_uint32_t * time);


HSL_LOCAL sw_error_t
shiva_fdb_iterate(a_uint32_t dev_id, a_uint32_t * iterator, fal_fdb_entry_t * entry);


#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SHIVA_FDB_H_ */
/**
 * @}
 */
