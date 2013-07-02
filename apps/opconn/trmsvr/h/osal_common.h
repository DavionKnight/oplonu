#ifndef __OSAL_COMMON__
#define __OSAL_COMMON__

/*
** Common include files
*/
#include "iros_config.h"
#ifdef printf
#undef printf
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#include "stdlib.h"
#include "string.h"

#include "cs_types.h"
//#include "os_core.h"
#include "osal_config.h"
#include "osal_api_core.h"
#include "util_list.h"
#include "cs_module.h"
#include "mempool.h"


/* Utility for return codes */
#define CS_ERR_DEFINE(mod, sub_mod,code)   (((mod)<<24)|((sub_mod)<<16)|(code))
#define CS_ERR_DEFINE_OSAL(code)            (  CS_ERR_DEFINE(IROS_MID_OSAL, 0, code) )

#define CS_E_OSAL_OK                    CS_E_OK
#define CS_E_OSAL_ERR                   CS_E_ERROR
#define CS_E_OSAL_INVALID_POINTER       CS_ERR_DEFINE_OSAL(0x1)
#define CS_E_OSAL_ERR_ADDR_MISALIGNED   CS_ERR_DEFINE_OSAL(0x2)
#define CS_E_OSAL_ERR_TIMEOUT           CS_ERR_DEFINE_OSAL(0x3)
#define CS_E_OSAL_INVALID_INT_NUM       CS_ERR_DEFINE_OSAL(0x4)
#define CS_E_OSAL_SEM_FAILURE           CS_ERR_DEFINE_OSAL(0x5)
#define CS_E_OSAL_SEM_TIMEOUT           CS_ERR_DEFINE_OSAL(0x6)
#define CS_E_OSAL_QUEUE_EMPTY           CS_ERR_DEFINE_OSAL(0x7)
#define CS_E_OSAL_QUEUE_FULL            CS_ERR_DEFINE_OSAL(0x8)
#define CS_E_OSAL_QUEUE_TIMEOUT         CS_ERR_DEFINE_OSAL(0x9)
#define CS_E_OSAL_QUEUE_INVALID_SIZE    CS_ERR_DEFINE_OSAL(0xA)
#define CS_E_OSAL_QUEUE_ID_ERROR        CS_ERR_DEFINE_OSAL(0xB)
#define CS_E_OSAL_ERR_NAME_TOO_LONG     CS_ERR_DEFINE_OSAL(0xC)
#define CS_E_OSAL_ERR_NO_FREE_IDS       CS_ERR_DEFINE_OSAL(0xD)
#define CS_E_OSAL_ERR_NAME_TAKEN        CS_ERR_DEFINE_OSAL(0xE)
#define CS_E_OSAL_ERR_INVALID_ID        CS_ERR_DEFINE_OSAL(0xF)
#define CS_E_OSAL_ERR_NAME_NOT_FOUND    CS_ERR_DEFINE_OSAL(0x10)
#define CS_E_OSAL_ERR_SEM_NOT_FULL      CS_ERR_DEFINE_OSAL(0x11)
#define CS_E_OSAL_ERR_INVALID_PRIORITY  CS_ERR_DEFINE_OSAL(0x12)

typedef void (*cs_io_hdl_t)(cs_uint8*, cs_uint32);

extern void cs_sys_capability_show();
extern void cs_bit_set(cs_int8 *pbuf , cs_int32 bit_pos);
extern void cs_bit_clr(cs_int8 *pbuf , cs_int32 bit_pos);
extern cs_int32 cs_bit_tst(cs_int8 *pbuf , cs_int32 bit_pos);
extern cs_int32 cs_bit_get_0(cs_int8 *pbuf , cs_int32 total_bit);
extern cs_int32 cs_bit_get_1(cs_int8 *pbuf , cs_int32 total_bit);
extern cs_int32 cs_bit_tst_all(cs_int8 *pbuf , cs_int32 total_bit);


#endif

