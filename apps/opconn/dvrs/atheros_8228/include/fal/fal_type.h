/*
 * Copyright (c) 2007-2008 Atheros Communications, Inc.
 * All rights reserved.
 *
 */
/**
 * @defgroup fal_type FAL_TYPE
 * @{
 */  
#ifndef _FAL_TYPE_H_
#define _FAL_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#define IN_PORTCONTROL
#define IN_VLAN
#define IN_PORTVLAN
#define IN_FDB
#define IN_ACL
#define IN_QOS
#define IN_IGMP
#define IN_LEAKY
#define IN_MIRROR
#define IN_RATE
#define IN_STP
#define IN_MIB
#define IN_MISC
#define IN_LED

typedef a_uint32_t fal_port_t;

#if (SW_MAX_NR_PORT <= 32)
    typedef a_uint32_t fal_pbmp_t;
#else
    typedef a_uint64_t fal_pbmp_t;
#endif

typedef struct {
    a_uint8_t uc[6];
} fal_mac_addr_t;

typedef a_uint32_t fal_ip4_addr_t;

typedef struct {
    a_uint32_t ul[4];
} fal_ip6_addr_t;

/**
@brief This enum defines several forwarding command type.
* Field description:
    FAL_MAC_FRWRD      - packets are normally forwarded
    FAL_MAC_DROP       - packets are dropped
    FAL_MAC_CPY_TO_CPU - packets are copyed to cpu
    FAL_MAC_RDT_TO_CPU - packets are redirected to cpu
*/
typedef enum {
    FAL_MAC_FRWRD = 0,		/**<   packets are normally forwarded */
    FAL_MAC_DROP,			/**<   packets are dropped */
    FAL_MAC_CPY_TO_CPU,		/**<   packets are copyed to cpu */
    FAL_MAC_RDT_TO_CPU		/**<   packets are redirected to cpu */
} fal_fwd_cmd_t;

typedef enum {
    FAL_FRAME_BASED = 1,
    FAL_BYTE_BASED,
    FAL_RATE_MODE_BUTT
} fal_traffic_unit_t;

typedef a_uint32_t fal_queue_t;

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_TYPE_H_ */
/**
 * @}
 */
