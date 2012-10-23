/*
 * Copyright (c) 2007-2008 Atheros Communications, Inc.
 * All rights reserved.
 *
 */

#ifndef _SW_API_H
#define _SW_API_H

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "sw_ioctl.h"

#define SW_API_MAX             0xffff
#define SW_MAX_API_BUF         1024
#define SW_MAX_API_PARAM       12 /* cmd type + return value + ten parameters */
#define SW_PARAM_IN            0x1
#define SW_PARAM_OUT           0x2
#define SW_PARAM_PTR           0x4

#define SW_API_DEF(ioctl, nr, name) {ioctl, nr, name}
#ifdef SHELL_MODE
#define SW_PARAM_DEF(ioctl, data, size, type, name) \
    {ioctl, data, size, type, name}
#else
#define SW_PARAM_DEF(ioctl, data, size, type, name) {ioctl, data, size, type}
#endif

typedef enum {
    SW_UINT8 = 1,
    SW_INT8,
    SW_UINT16,
    SW_INT16,
    SW_UINT32,
    SW_INT32,
    SW_UINT64,
    SW_INT64,
    SW_ENABLE,
    SW_SPEED,
    SW_DUPLEX,
    SW_1QMODE,
    SW_EGMODE,
    SW_CAP,
    SW_VLAN,
    SW_PBMP,
    SW_MIB,
    SW_MACADDR,
    SW_FDBENTRY,
    SW_SCH,
    SW_QOS,
    SW_STORM,
    SW_STP,
    SW_LEAKY,
    SW_MACCMD,
    SW_UINT_A,
    SW_ACLRULE,
    SW_LEDPATTERN,
    SW_INVLAN,
    SW_VLANPROPAGATION,
    SW_VLANTRANSLATION,    
    SW_QINQMODE,
    SW_QINQROLE,   
    SW_DATA_MAX,
} sw_data_type_e;

typedef struct {
    a_uint32_t func_type;
    a_uint8_t nr_param;
    sw_error_t (*func) ();
} sw_api_table_t;

typedef struct {
    a_uint32_t func_type;
    sw_data_type_e data_type;
    a_uint32_t data_size;
    a_uint8_t param_type;
#ifdef SHELL_MODE
    a_uint8_t param_name[20];
#endif
} sw_param_t;

#ifdef __cplusplus
}
#endif                          /* __cplusplus */

#endif                          /* _SW_API_H */
