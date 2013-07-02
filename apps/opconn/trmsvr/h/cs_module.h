#ifndef __CS_MODULE_H__
#define __CS_MODULE_H__

#include "cs_types.h"

typedef enum
{
    IROS_MID_OSAL             = 0,
    IROS_MID_TIMER            = 1,
    IROS_MID_INTR             = 2,
    IROS_MID_CPULOAD          = 3,
    IROS_MID_MALLOC           = 4,
    IROS_MID_IFETH            = 5,
    IROS_MID_EXCEPT           = 6,
    IROS_MID_LOGGER           = 7,
    IROS_MID_AAL              = 8,
    IROS_MID_APP              = 9,
    IROS_MID_CLI              = 10,
    IROS_MID_SC               = 11,
    IROS_MID_OAM              = 12,
    IROS_MID_1X               = 13,
    IROS_MID_HELLO            = 14,
    IROS_MID_IGMP             = 15,
    IROS_MID_OOBCMD           = 16,
    IROS_MID_L2FTP            = 17,
    IROS_MID_EVENT            = 18,
    IROS_MID_ETHDRV           = 19,
    IROS_MID_STP              = 20,
    IROS_MID_SNMP             = 21,
    IROS_MID_UPGRADE          = 22,
    IROS_MID_PKT_HDR          = 23,
    IROS_MID_PACKET           = 24,
    IROS_MID_SYS              = 25,
    IROS_MID_SAL              = 26,
    IROS_MID_WEB              = 27,
    IROS_MID_SLIC             = 28,
    IROS_MID_DMA              = 29,
    IROS_MID_ESDMA            = 30,
    IROS_MID_VOIP             = 31,
    IROS_MID_ORG_OAM          = 32,
    IROS_MID_DHCP             = 33,
    IROS_MID_STACK_RSTP             = 34,
    IROS_MID_PPPOE             = 35,
    IROS_MID_VLAN             = 36,
    IROS_MID_PWRS             = 37,
    IROS_MID_PTP                = 38,
    IROS_MID_PPP             = 39,
    IROS_MID_MAX
} IROS_MID_E;


typedef struct
{
    cs_int32 mod_id;
    cs_int8  mod_name[8];
}sys_mod_desc_t;

extern sys_mod_desc_t sys_mod_desc[];
extern cs_int32 sys_mod_number;

extern cs_int32 cs_sys_mod_init();
extern cs_int8 *cs_sys_mod_desc_get(cs_int32 mod_id);

#endif


