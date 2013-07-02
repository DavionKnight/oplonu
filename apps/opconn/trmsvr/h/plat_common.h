#ifndef _PLAT_COMMON_H_
#define _PLAT_COMMON_H_

#include "iros_config.h"

#include "cs_types.h"
/*#include "plat_config.h"
#include "osal_common.h"
#include "util_list.h"
#include "tlv_parser.h"
#include "interrupt.h"
#include "cs_timer.h"
#include "startup_cfg.h"
#include "mempool.h"
#include "cache.h"
#include "image.h"
#include "iros_flash_oper.h"
#include "cpu_load.h"
#include "cs_module.h"
#include "log.h"
#include "exception.h"
#include "led.h"
#include "glb_reg_api.h"
#include "crc32.h"
#include "cs_cmn.h"
#include "cs_utils.h"
//#include "stdio.h"
#include "sdl_plat.h"
#include "iros_version.h"
#include "app_gpio.h"*/

/**********************************************************
 *         Register Access Methods                        *
 **********************************************************/
#ifndef __CS_REG_ACCESS_DEFINED__
#define __CS_REG_ACCESS_DEFINED__
/* Register Access macros - customize as needed */
#define CS_REG_READ(REG_ADDR, REG_DATA)     \
        HAL_READ_UINT32(REG_ADDR, REG_DATA)

#define CS_REG_WRITE(REG_ADDR, REG_DATA)    \
        HAL_WRITE_UINT32(REG_ADDR, REG_DATA)
#endif /* __CS_REG_ACCESS_DEFINED__ */


#define CS_OFFSET(_type, _member) (size_t)( &(((_type *)0)->_member) )


typedef enum
{
    RESET_TYPE_RESERVED = 0
    ,RESET_TYPE_FORCE
    ,RESET_TYPE_AUTO_ENABLE
    ,RESET_TYPE_AUTO_CANCEL
    ,RESET_TYPE_MAX
} RESET_TYPE_E;

extern void iros_system_reset(RESET_TYPE_E reset_type);
extern void cs_polling_init();

#if 1 /*Remove immenstar.h*/
extern void data_dump(char *ptext, int len, char *show_addr);
extern void data_dump_w(char *ptext, int len, char *show_addr);
extern int cmd_cmp(char *str, const char *mode);

#define IROS_MSG_UNDEF        0
#define IROS_MSG_PKT    1 
#define IROS_MSG_AAL_ALARM    2
#define IROS_MSG_IFM          3
#define IROS_MSG_GPIO         4
#define IROS_MSG_TIME_OUT     5
#define IROS_MSG_MAC_FILTER_AGED    6
#define IROS_MSG_EVENT          7
#define IROS_MSG_TERM_SERVER          8

extern cs_uint32  app_msg_q;
#define DUMPPKT(a, b, c)        if(pktTrace) cs_buffer_dump(a, b, c)
#define DUMPDOT1XPKT(a, b, c)        if(pktDot1xTrace) cs_buffer_dump(a, b, c)

typedef struct {
    cs_uint8 type;
    cs_uint8 u1Reserved;
    cs_uint16 u2Reserved;
} iros_msg_t;

extern unsigned char pktTrace;
extern unsigned char pktDot1xTrace;
extern unsigned char pktRstpTrace;

#endif

#endif
