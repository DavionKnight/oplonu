#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cs_types.h"
#include "osal_api_core.h"
#include "plat_common.h"
#include "tlv_parser.h"
//#include "app_daemon.h"
#include "db_mngt.h"
#include "db_api.h"
#ifdef HAVE_DB_MANAGMENT

static db_data_confg_t  *g_DbCfgTable = NULL;
static cs_uint32         g_DbCfgTableSize = 0;
static db_table_def_init_handle g_DbSetDefault = NULL;
static db_table_upgrade_handle  g_DbUpgrade = NULL; 
static db_core_info_t    g_db_info;
static cs_uint32 gUsrDataInst = 0;
static cs_uint16 g_DbTimerInterval = DB_CENTER_SYN_DATA_PERIOD_TIME;

#if 0
cs_status db_write(
        cs_uint32 id, 
        cs_uint8 *buf, 
        cs_uint32 len)
{
    cs_status result = CS_E_OK;
    cs_int32 ret = CS_E_OK;
    cs_uint16 tmpShort = 0;
    cs_uint32 tmpInt = 0;
    cs_uint32 i = 0;
    
 //   DB_ASSERT_RET(buf != NULL,CS_E_PARAM);

    cs_mutex_lock(g_db_info.data_mutex);
    
    if(!g_db_info.create_flag){
        DB_LOG("the db table has not created \n");
        result = CS_E_ERROR;
        goto END;
    }

    if(len <= 0){
        DB_LOG("invalid len = %d \n",len);
        result = CS_E_PARAM;
        goto END;
    }


    for(i = 0; i < g_DbCfgTableSize; i++){
        if(g_DbCfgTable[i].id == id){
            break;
        }
    }

    if(i == g_DbCfgTableSize){
        DB_LOG("can not find the id = %d \n",id);
        result = CS_E_ERROR;
        goto END;
    }

    switch(g_DbCfgTable[i].type)
    {
        case DB_TYPE_UINT8:
            ret = tlv_write_char(gUsrDataInst, (cs_uint16)id, buf[0]);
            break;
        case DB_TYPE_UINT16:
            memcpy(&tmpShort, buf, sizeof(tmpShort));
            ret = tlv_write_short(gUsrDataInst, (cs_uint16)id, tmpShort);
            break;
        case DB_TYPE_UINT32:
            memcpy(&tmpInt, buf, sizeof(tmpInt));
            ret = tlv_write_int(gUsrDataInst, (cs_uint16)id, tmpInt);
            break;
        case DB_TYPE_BYTES:
            ret = tlv_write_buffer(gUsrDataInst, (cs_uint16)id, g_DbCfgTable[i].max_len, len , buf);
            break;
        default:
            ret = TLV_PARSER_ERROR;
            break;
    }
    
    if(ret != TLV_PARSER_OK){
        DB_LOG("write the db failed, id = %d ,ret = %d \n",i,ret);
        goto END;
    }

    if(g_db_info.auto_sync){
        g_db_info.dirty_count++;
        if(DB_DIRTY_COUNT_MAX == g_db_info.dirty_count)
        {
            db_send_msg(DB_CENTER_SYN_DATA_FORCE_MSG);
        }
    }

END:
    cs_mutex_unlock(g_db_info.data_mutex);
    return result;

}


cs_status db_read(
        cs_uint32 id, 
        cs_uint8 *buf,
        cs_uint32 len
        )
{
    cs_status result = CS_E_OK;
    cs_int32 ret = CS_E_OK;
    cs_uint32 i = 0;
    
  //  DB_ASSERT_RET(buf != NULL,CS_E_PARAM);
    cs_mutex_lock(g_db_info.data_mutex);
    
    if(!g_db_info.create_flag){
 //       DB_LOG("the db table has not created \n");
        result = CS_E_ERROR;
        goto END;
    }

    for(i = 0; i < g_DbCfgTableSize; i++){
        if(g_DbCfgTable[i].id == id){
            break;
        }
    }

    if(i == g_DbCfgTableSize){
 //       DB_LOG("can not find the id = %d \n",id);
        result = CS_E_ERROR;
        goto END;
    }

    switch(g_DbCfgTable[i].type)
    {
        case DB_TYPE_UINT8:
            ret = tlv_read_char(gUsrDataInst, (cs_uint16)id, buf);
            break;
        case DB_TYPE_UINT16:
            ret = tlv_read_short(gUsrDataInst, (cs_uint16)id, (cs_uint16 *)buf);
            break;
        case DB_TYPE_UINT32:
            ret = tlv_read_int(gUsrDataInst, (cs_uint16)id, (cs_uint32 *)buf);
            break;
        case DB_TYPE_BYTES:
            ret = tlv_read_buffer(gUsrDataInst, (cs_uint16)id, len , buf);
            if(ret == 0)
                ret = TLV_PARSER_ERROR;
            break;
        default:
            ret = TLV_PARSER_ERROR;
            break;
    }

    if(ret == TLV_PARSER_ERROR){
        result =  CS_ERROR;
    }
END:
    cs_mutex_unlock(g_db_info.data_mutex);
    return result;
}
#endif
#endif



