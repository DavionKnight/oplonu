#ifndef __CS_TYPES_H__
#define __CS_TYPES_H__

typedef unsigned long long      cs_uint64;
typedef long long               cs_int64;
typedef unsigned int            cs_uint32;
typedef int                     cs_int32;
typedef unsigned short          cs_uint16;
typedef short                   cs_int16;
typedef unsigned char           cs_uint8;
typedef char                    cs_int8;
typedef unsigned char           cs_boolean;

#define MAX_UART_NUM  			5

typedef enum {
    CS_PON_PORT_ID     = 0x00,
    CS_UNI_PORT_ID1    = 0x01,
    CS_UNI_PORT_ID2    = 0x02,
    CS_UNI_PORT_ID3    = 0x03,
    CS_UNI_PORT_ID4    = 0x04,
    CS_DOWNLINK_PORT   = 0x20,
    CS_UPLINK_PORT     = 0x30,
    CS_MGMT_PORT_ID    = 0x4F,
    CS_VOIP_PORT_ID    = 0x50,
    CS_ALL_UNI_PORT_ID = 0xFF,
} cs_port_id_t;

typedef enum {
    CS_DOWN_STREAM     = 0x00,
    CS_UP_STREAM       = 0x01,
} cs_direction_t;


typedef enum {
    CS_E_OK               = 0,
    CS_E_RESOURCE         = 1,
    CS_E_PARAM            = 2,
    CS_E_NOT_FOUND        = 3,
    CS_E_CONFLICT         = 4,
    CS_E_TIMEOUT          = 5,
    CS_E_NOT_SUPPORT      = 6,
    CS_E_ERROR            = 0xffffffff
} cs_status;


typedef int (*cs_funcptr)(void *arg);

typedef cs_uint16                cs_ipv6_t[8];

enum {
    CS_IPV4,
    CS_IPV6
};

typedef struct {
    cs_uint32   ipver;
    union {
        cs_uint32 ipv4;
        cs_ipv6_t ipv6;
    } addr;
} cs_ip_t;

typedef cs_uint16                cs_dev_id_t;
typedef cs_uint16                cs_llid_t;
typedef struct {
    cs_uint32    bmp[4];
}                                cs_llid_bmp_t;

typedef cs_uint16                cs_sub_id_t;


#define CS_MACADDR_LEN          6

typedef union {
    cs_uint8       addr[CS_MACADDR_LEN];
}__attribute__((packed)) cs_mac_t;


#define CS_OK               CS_E_OK
#define CS_ERROR            CS_E_ERROR

#ifndef TRUE
#define TRUE                       1
#endif

#ifndef FALSE
#define FALSE                  0
#endif

#ifndef NULL
#define NULL                   ((void *) 0)
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef CS_IN
#define CS_IN
#endif

#ifndef CS_OUT
#define CS_OUT
#endif

#ifndef CS_IN_OUT
#define CS_IN_OUT
#endif

#define _CS_INLINE_ __inline__ static

#ifdef __BIG_ENDIAN__

#ifndef ntohl
#define ntohl(x)        (x)
#endif

#ifndef ntohs
#define ntohs(x)        (x)
#endif

#ifndef htonl
#define htonl(x)        (x)
#endif

#ifndef htons
#define htons(x)        (x)
#endif

#ifndef ntohll
#define ntohll(x)       (x)
#endif


#ifndef htonll
#define htonll(x)       (x)
#endif

#elif defined(__LITTLE_ENDIAN__)

#ifndef ntohl
#define ntohl(x)        ((((x) & 0x000000ff) << 24) | \
                            (((x) & 0x0000ff00) <<  8) | \
                            (((x) & 0x00ff0000) >>  8) | \
                            (((x) & 0xff000000) >> 24))
#endif

#ifndef htonl
#define htonl(x)        ((((unsigned long)(x) & 0x000000ff) << 24) | \
                            (((unsigned long)(x) & 0x0000ff00) <<  8) | \
                            (((unsigned long)(x) & 0x00ff0000) >>  8) | \
                            (((unsigned long)(x) & 0xff000000) >> 24))
#endif


#ifndef ntohs
#define ntohs(x)        ((((x) & 0x00ff) << 8) | \
                            (((x) & 0xff00) >> 8))
#endif

#ifndef htons
#define htons(x)        ((((x) & 0x00ff) << 8) | \
                            (((x) & 0xff00) >> 8))

#endif


#ifndef ntohll
#define ntohll(x)        ((((cs_uint64)ntohl(x)) << 32) | \
                                       ntohl((x) >> 32))
#endif


#ifndef htonll
#define htonll(x)        ((((cs_uint64)htonl(x)) << 32) | \
                                 htonl((x) >> 32))
#endif

#else
#define errora Endianness not defined
#endif


/* range: should assume only two values: 0 or 1 */
typedef unsigned int                    epon_mask_t;
typedef unsigned char                   epon_macaddr_t[CS_MACADDR_LEN];
typedef unsigned char                   *epon_macaddr_pt;

typedef epon_macaddr_t                  epon_onu_device_id_t;
typedef cs_uint32                   epon_olt_device_id_t;

typedef cs_uint32                   epon_onu_version_t;
typedef cs_uint32                   epon_olt_version_t;


#define INVALID_PORT_ID 0xffffffff
#define ONU_LLIDPORT_FOR_API   (-1)
#define ONU_DEVICEID_FOR_API   (-1)

#define EPON_MAX_ONU_PON_PORTS          1
#define EPON_MAX_ONU_LLID_PORTS         (EPON_MAX_ONU_PON_PORTS * EPON_MAX_LLIDS_PER_ONU_PON_PORT)

/* number of LLIDs assigned on an ONU PON port */
#define EPON_MAX_LLIDS_PER_ONU_PON_PORT 1

#define USER_NAME_LEN               32
#define PASSWORD_LEN                32


/* asynchronous API result code */
typedef enum {
    EPON_RESULT_SUCCESS = 0,
    EPON_RESULT_FAIL,
    EPON_RESULT_NO_REQ_RECORD,
    EPON_RESULT_NO_OLT_MAC,
    EPON_RESULT_NO_MEMORY,
    EPON_RESULT_WRONG_EVENT,
    EPON_RESULT_NULL_HANDLER,
    EPON_RESULT_REQ_TIMEOUT,
    EPON_RESULT_NO_ONU_MAC,
    EPON_RESULT_PING_TOO_LONG,
    EPON_RESULT_CONTROL_IF_DOWN,
    EPON_RESULT_INV_PARAM,
    EPON_RESULT_INV_CHIP_VERSION,
    EPON_RESULT_DEVICE_NOT_SUPPORTED
} epon_result_code_t;


typedef enum {
    EPON_RETURN_SUCCESS = 0,
    EPON_RETURN_FAIL,
    EPON_RETURN_REINIT,
    EPON_RETURN_INV_CHIP_VERSION,
    EPON_RETURN_INV_STATE,
    EPON_RETURN_INV_MAC,
    EPON_RETURN_INV_PARAM,
    EPON_RETURN_NOT_SUPPORT,
    EPON_RETURN_NOT_FOUND,
    EPON_RETURN_NO_RESOURCE
} epon_return_code_t;


/*default CPU pkt priority */
#define ONU_CPU_PKT_PRIO        7
#define ONU_CPU_PKT_PRIO_6      6

/* boolean values */
#define EPON_TRUE                       1
#define EPON_FALSE                      0


typedef struct{
    cs_uint8    init_flag; 
    cs_uint8    uart_id;        /* UART number 0,1,2,3,4*/
    cs_uint8    data_bits;      /* 7/8  */
    cs_uint8    stop_bits;      /* 1/2 */
    cs_uint8    flow_control;   /* None/XONXOFF/HardWare */
    cs_uint8    parity;         /* None/Odd/Even */
    cs_uint8    duplex;         /* full-duplex or half-duplex*/
    cs_uint8    mode;           /* Polling or Interrupt */    
    cs_uint32   baud_rate;          
    cs_uint32   proto_type;     /* protocol type TCP 6/UDP 1*/
    cs_uint32   proto_port;     /* L4 protocol port 1~65535 */
    cs_uint32   client_timeout; /* remote client timeout 0:never thoer:1~65535s*/
    cs_uint32   max_payload;    /* 512 ~1400 bytes */
    cs_uint32   min_payload;    /* 40 ~ max_payload */
    cs_uint32   max_res_time;   /* max response time 25~1000ms */
}db_ts_cfg_t;

#endif /* _CS_TYPES_H_ */


