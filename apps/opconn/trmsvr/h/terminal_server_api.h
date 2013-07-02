#ifndef __TERMINAL_SERVER_API_H
#define __TERMINAL_SERVER_API_H
#include "terminal_server.h"
typedef enum
{
    TS_CFG_UART_DATA_BITS   = 0x00000001,
    TS_CFG_UART_STOP_BITS   = 0x00000002,
    TS_CFG_UART_FLOW_CTRL   = 0x00000004,
    TS_CFG_UART_PARITY      = 0x00000008,
    TS_CFG_UART_DUPLEX      = 0x00000010,
    TS_CFG_UART_MODE        = 0x00000020,
    TS_CFG_UART_BAUD_RATE   = 0x00000040,
    TS_CFG_PROTO_TYPE       = 0x00000080,
    TS_CFG_PROTO_PORT       = 0x00000100,
    TS_CFG_CLIENT_TIMEOUT   = 0x00000200,
    TS_CFG_MAX_PAYLOAD      = 0x00000400,
    TS_CFG_MIN_PAYLOAD      = 0x00000800,
    TS_CFG_MAX_RES_TIME     = 0x00001000,
}ts_cfg_enum_t;

cs_status epon_request_onu_ts_enable(
     cs_int32              device,
     cs_int32              llidport,
     cs_uint8              uart_id,
     term_server_config_t  *cfg
    );
cs_status epon_request_onu_ts_disable(
    CS_IN cs_int32              device,
    CS_IN cs_int32              llidport,
    CS_IN cs_uint8              uart_id
    );
cs_status epon_request_onu_ts_cfg_set(
    CS_IN cs_int32              device,
    CS_IN cs_int32              llidport,
    CS_IN cs_uint32             mask,
    CS_IN cs_uint8              uart_id,
    CS_IN cs_uint32             value
    );
cs_status epon_request_onu_ts_cfg_get(
    CS_IN cs_int32              device,
    CS_IN cs_int32              llidport,
    CS_IN cs_uint32             mask,
    CS_IN cs_uint8              uart_id,
    CS_IN cs_uint32             *value
    );
cs_status epon_request_onu_ts_cfg_save(
    CS_IN cs_int32              device,
    CS_IN cs_int32              llidport,
    CS_IN cs_uint8              uart_id
    );
cs_status epon_request_onu_ts_cfg_clear(
    CS_IN cs_int32              device,
    CS_IN cs_int32              llidport,
    CS_IN cs_uint8              uart_id
    );

#endif



