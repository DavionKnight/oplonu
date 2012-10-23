/*
 * Copyright (c) 2007-2008 Atheros Communications, Inc.
 * All rights reserved.
 *
 */
/**
 * @defgroup fal_acl FAL_ACL
 * @{
 */ 
#ifndef _FAL_ACL_H_
#define _FAL_ACL_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal_type.h"


/**
@brief This enum defines the ACL rule type.
*/
typedef enum {
    FAL_ACL_RULE_MAC = 0,   /**< include MAC, udf fields*/
    FAL_ACL_RULE_IP4, 		/**< include MAC, IP4 and Tcp/Udp udf fields*/
    FAL_ACL_RULE_IP6, 		/**< include MAC, IP6 and Tcp/Udp udf fields*/
    FAL_ACL_RULE_UDF, 		/**< only include user defined fields*/
    FAL_ACL_RULE_BUTT, 		
} fal_acl_rule_type_t;


/**
@brief This enum defines the ACL field operation type.
*/
typedef enum {
    FAL_ACL_FIELD_MASK = 0, /**< match operation is mask*/
    FAL_ACL_FIELD_RANGE,	/**< match operation is range*/
    FAL_ACL_FIELD_LE,		/**< match operation is less and equal*/
    FAL_ACL_FIELD_GE,		/**< match operation is great and equal*/
    FAL_ACL_FIELD_NE,		/**<- match operation is not equal*/
    FAL_ACL_FIELD_OP_BUTT,	
} fal_acl_field_op_t;

/**
@brief This enum defines the ACL field operation type.
*/
typedef enum {
    FAL_ACL_UDF_TYPE_L2 = 0, /**< match operation is mask*/
    FAL_ACL_UDF_TYPE_L3,	 /**< match operation is range*/
    FAL_ACL_UDF_TYPE_BUTT,	
} fal_acl_udf_type_t;

#define    FAL_ACL_FIELD_MAC_DA         0
#define    FAL_ACL_FIELD_MAC_SA         1 
#define    FAL_ACL_FIELD_MAC_ETHTYPE    2
#define    FAL_ACL_FIELD_MAC_TAGGED     3
#define    FAL_ACL_FIELD_MAC_UP         4
#define    FAL_ACL_FIELD_MAC_VID        5
#define    FAL_ACL_FIELD_IP4_SIP        6
#define    FAL_ACL_FIELD_IP4_DIP        7
#define    FAL_ACL_FIELD_IP6_LABEL      8
#define    FAL_ACL_FIELD_IP6_SIP        9 
#define    FAL_ACL_FIELD_IP6_DIP        10
#define    FAL_ACL_FIELD_IP_PROTO       11 
#define    FAL_ACL_FIELD_IP_DSCP        12  
#define    FAL_ACL_FIELD_L4_SPORT       13
#define    FAL_ACL_FIELD_L4_DPORT       14
#define    FAL_ACL_FIELD_UDF            15
#define    FAL_ACL_FIELD_MAC_CFI        16

#define    FAL_ACL_ACTION_PERMIT        0
#define    FAL_ACL_ACTION_DENY          1
#define    FAL_ACL_ACTION_REDPT         2
#define    FAL_ACL_ACTION_RDTCPU        3
#define    FAL_ACL_ACTION_CPYCPU        4
#define    FAL_ACL_ACTION_MIRROR        5
#define    FAL_ACL_ACTION_MODIFY_VLAN   6
#define    FAL_ACL_ACTION_NEST_VLAN     7
#define    FAL_ACL_ACTION_REMARK_UP     8
#define    FAL_ACL_ACTION_REMARK_QUEUE  9
#define    FAL_ACL_ACTION_VID_MEM_DIS	10


/**
  * @brief This type defines the action in Acl rule.
  *   @details  Comments:
  *  It's a bit map type, we can access it through macro FAL_ACTION_FLG_SET,
  * FAL_ACTION_FLG_CLR and FAL_ACTION_FLG_TST.
*/
typedef a_uint32_t fal_acl_action_map_t;

#define FAL_ACTION_FLG_SET(flag, action) \
    (flag) |= (0x1UL << (action))

#define FAL_ACTION_FLG_CLR(flag, action) \
    (flag) &= (~(0x1UL << (action)))

#define FAL_ACTION_FLG_TST(flag, action) \
    ((flag) & (0x1UL << (action))) ? 1 : 0


/**
  * @brief This type defines the field in Acl rule.
  *   @details   Comments:
  *   It's a bit map type, we can access it through macro FAL_FIELD_FLG_SET,
  *   FAL_FIELD_FLG_CLR and FAL_FIELD_FLG_TST.
*/
typedef a_uint32_t fal_acl_field_map_t[2];

#define FAL_FIELD_FLG_SET(flag, field) \
    (flag[(field) / 32]) |= (0x1UL << ((field) % 32))

#define FAL_FIELD_FLG_CLR(flag, field) \
    (flag[(field) / 32]) &= (~(0x1UL << ((field) % 32)))

#define FAL_FIELD_FLG_TST(flag, field) \
    ((flag[(field) / 32]) & (0x1UL << ((field) % 32))) ? 1 : 0

/**
 * @brief This structure defines the Acl rule.
 *   @details  Fields description:
 *    
 *   
 *    vid_val - If vid_op equals FAL_ACL_FIELD_MASK it's vlan id field value.
 *     If vid_op equals FAL_ACL_FIELD_RANGE it's vlan id field low value. If
 *     vid_op equals other value it's the compared value.
 * 
 *     vid_mask - If vid_op equals FAL_ACL_FIELD_MASK it's vlan id field mask.
 *     If vid_op equals FAL_ACL_FIELD_RANGE it's vlan id field high value. If vid_op
 *     equals other value it's meaningless.
 *    
 *  
 *     ip_dscp_val - It's eight bits field we can set any value between 0 - 255.
 *     ip_dscp_mask - It's eight bits field we can set any value between 0 - 255.
 *    
 *  
 *     src_l4port_val - If src_l4port_op equals FAL_ACL_FIELD_MASK it's layer four
 *     source port field value. If src_l4port_op equals FAL_ACL_FIELD_RANGE it's
 *     layer four source port field low value. If src_l4port_op equals other value
 *     it's the compared value.
 *    
 *  
 *     src_l4port_mask - If src_l4port_op equals FAL_ACL_FIELD_MASK it's layer four
 *     source port field mask. If src_l4port_op equals FAL_ACL_FIELD_RANGE it's
 *     layer four source port field high value. If src_l4port_op equals other value
 *     it's meaningless.
 *    
 *  
 *     dest_l4port_val - If dest_l4port_op equals FAL_ACL_FIELD_MASK it's layer four
 *     destination port field value. If dest_l4port_op equals FAL_ACL_FIELD_RANGE it's
 *     layer four source port field low value. If dest_l4port_op equals other value
 *     it's the compared value.
 *    
 *  
 *     dest_l4port_mask - If dest_l4port_op equals FAL_ACL_FIELD_MASK it's layer four
 *     source port field mask. If dest_l4port_op equals FAL_ACL_FIELD_RANGE it's
 *     layer four source port field high value. If dest_l4port_op equals other value
 *     it's meaningless.
 *    
 *  
 *     ports - If FAL_ACL_ACTION_REDPT bit is setted in action_flg it's redirect
 *     destination ports.
 *    
 *  
 *     dot1p - If FAL_ACL_ACTION_REMARK_DOT1P bit is setted in action_flg it's
 *     the expected dot1p value.
 *    
 *  
 *     queue - If FAL_ACL_ACTION_REMARK_QUEUE bit is setted in action_flg it's
 *     the expected queue value.
 *    
 *  
 *     vid - If FAL_ACL_ACTION_MODIFY_VLAN or FAL_ACL_ACTION_NEST_VLAN bit is
 *     setted in action_flg it's the expected vlan id value.
 */
typedef struct {
    fal_acl_rule_type_t rule_type;
    fal_acl_field_map_t field_flg;

    /* fields of mac rule */
    fal_mac_addr_t     src_mac_val;
    fal_mac_addr_t     src_mac_mask;
    fal_mac_addr_t     dest_mac_val;
    fal_mac_addr_t     dest_mac_mask;
    a_uint16_t         ethtype_val;
    a_uint16_t         ethtype_mask;
    a_uint16_t         vid_val;
    a_uint16_t         vid_mask;
    fal_acl_field_op_t vid_op;
    a_uint8_t          tagged_val;
    a_uint8_t          tagged_mask;
    a_uint8_t          up_val;
    a_uint8_t          up_mask;
    a_uint8_t          cfi_val;
    a_uint8_t          cfi_mask;
    a_uint16_t         resv0;

    /* fields of ip4 rule */
    fal_ip4_addr_t      src_ip4_val;
    fal_ip4_addr_t      src_ip4_mask;
    fal_ip4_addr_t      dest_ip4_val;
    fal_ip4_addr_t      dest_ip4_mask;

    /* fields of ip6 rule */
    a_uint32_t         ip6_lable_val;
    a_uint32_t         ip6_lable_mask;
    fal_ip6_addr_t      src_ip6_val;
    fal_ip6_addr_t      src_ip6_mask;
    fal_ip6_addr_t      dest_ip6_val;
    fal_ip6_addr_t      dest_ip6_mask;

    /* fields of ip rule */
    a_uint8_t          ip_proto_val;
    a_uint8_t          ip_proto_mask;
    a_uint8_t          ip_dscp_val;
    a_uint8_t          ip_dscp_mask;

    /* fields of layer four */
    a_uint16_t         src_l4port_val;
    a_uint16_t         src_l4port_mask;
    fal_acl_field_op_t src_l4port_op;
    a_uint16_t         dest_l4port_val;
    a_uint16_t         dest_l4port_mask;
    fal_acl_field_op_t dest_l4port_op;

    /* user defined fields */
    fal_acl_udf_type_t udf_type;
    a_uint8_t udf_offset;
    a_uint8_t udf_len;    
    a_uint8_t udf_val[16];
    a_uint8_t udf_mask[16];    

    /* fields of action */
    fal_acl_action_map_t  action_flg;
    fal_pbmp_t            ports;
    a_uint32_t            match_cnt;
    a_uint8_t             up;
    a_uint8_t             queue; 
    a_uint16_t            vid;
} fal_acl_rule_t;


/**
@brief This enum defines the ACL will work on which derection traffic.
*/
typedef enum {
    FAL_ACL_DIREC_IN = 0, 	/**<   Acl will work on ingressive traffic */
    FAL_ACL_DIREC_EG,		/**<   Acl will work on egressive traffic */
    FAL_ACL_DIREC_BOTH,		/**<    Acl will work on both ingressive and egressive traffic*/
} fal_acl_direc_t;


/**
@brief This enum defines the ACL will work on which partiualr object.
*/
typedef enum {
    FAL_ACL_BIND_PORT = 0,	/**<   Acl wil work on particular port */
} fal_acl_bind_obj_t;



sw_error_t
fal_acl_list_creat(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t list_pri);



sw_error_t
fal_acl_list_destroy(a_uint32_t dev_id, a_uint32_t list_id);



sw_error_t
fal_acl_rule_add(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t rule_id,
                    a_uint32_t rule_nr, fal_acl_rule_t * rule);



sw_error_t
fal_acl_rule_delete(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t rule_id,
                    a_uint32_t rule_nr);



sw_error_t
fal_acl_rule_query(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t rule_id,
                    fal_acl_rule_t * rule);



sw_error_t
fal_acl_list_bind(a_uint32_t dev_id, a_uint32_t list_id,
                     fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t,
                     a_uint32_t obj_idx);



sw_error_t
fal_acl_list_unbind(a_uint32_t dev_id, a_uint32_t list_id,
                       fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t,
                       a_uint32_t obj_idx);



sw_error_t
fal_acl_status_set(a_uint32_t dev_id, a_bool_t enable);



sw_error_t
fal_acl_status_get(a_uint32_t dev_id, a_bool_t * enable);

sw_error_t
fal_acl_list_dump(a_uint32_t dev_id);

sw_error_t
fal_acl_rule_dump(a_uint32_t dev_id);


#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_ACL_H_ */
/**
 * @}
 */
