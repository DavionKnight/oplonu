#ifndef __DB_MNGT_H__
#define __DB_MNGT_H__
#include "cs_types.h"
#include "plat_common.h"
#ifdef HAVE_DB_MANAGMENT
typedef struct{
    cs_uint32           data_mutex;
    cs_uint8            *data_pool;
    cs_uint32           pool_size;
    cs_uint32           dirty_count;
    cs_int32            poll_timer;
    cs_boolean          create_flag;
    cs_boolean          encrypt;
    cs_boolean          auto_sync;
    cs_uint32           flash_offset;
    cs_uint32           flash_version; 
    cs_uint32           cur_version;
}db_core_info_t;


#define DB_CENTER_SYN_DATA_FORCE_MSG           0x20000000
#define DB_CENTER_SYN_DATA_PERIOD_MSG          0x20000001 

#define DB_CENTER_SYN_DATA_PERIOD_TIME   1000*3

#define DB_DIRTY_COUNT_MAX  8

#if 0
#define DB_LOG(arg,...) \
    cs_log_msg(IROS_LOG_LEVEL_MAJ,IROS_MID_APP,"%s(): "arg, __FUNCTION__,##__VA_ARGS__)

#define DB_DEBUG(arg,...) \
    cs_log_msg(IROS_LOG_LEVEL_DBG0,IROS_MID_APP,"%s(): "arg, __FUNCTION__,##__VA_ARGS__)


#define  DB_ASSERT_RET(x,ret)    \
    do{ \
        if (!(x))  { \
            cs_printf("%s() _%d_: assert fail\n", __FUNCTION__, __LINE__); \
            return (ret); \
        } \
    }while(0)

#define  DB_ASSERT(x)    \
    do{ \
        if (!(x))  { \
            cs_printf("%s() _%d_: assert fail\n", __FUNCTION__, __LINE__); \
            return ; \
        } \
    }while(0)                                                        
#endif


void db_encrypt(cs_uint8 *buf, cs_uint32 len);
void db_decrypt(cs_uint8 *buf, cs_uint32 len);
cs_status db_table_create();
void db_sync_check_tmfunc(void *data);
void db_send_msg(cs_uint32 msg);
void db_daemon_task_handler_init();
cs_status db_load_from_flash();

#endif
#endif



