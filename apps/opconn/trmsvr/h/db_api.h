#ifndef __DB_API_H__
#define __DB_API_H__
#include "iros_config.h"
#include "cs_types.h"
#ifdef HAVE_DB_MANAGMENT
typedef enum
{
    DB_TYPE_UINT8 = 1,
    DB_TYPE_UINT16,
    DB_TYPE_UINT32,
    DB_TYPE_BITS,
    DB_TYPE_BYTES,
    DB_TYPE_MAX
}db_data_type_t;

typedef struct
{
    cs_uint8*       name;
    cs_uint16       id;
    db_data_type_t  type;
    cs_uint32       max_len;
    cs_uint8        dirty;
} db_data_confg_t;

typedef cs_status(* db_table_def_init_handle)();
typedef cs_status(* db_table_upgrade_handle)(cs_uint32 version);

cs_status db_read(cs_uint32  id, cs_uint8 *buf, cs_uint32 len);
cs_status db_write(cs_uint32 id, cs_uint8 *buf, cs_uint32 len);

#define db_common_read     db_read
#define db_common_write    db_write


cs_status  db_table_init(
        db_data_confg_t * dbDesc,
        cs_uint32 size,
        cs_uint32 version,
        db_table_def_init_handle defHandler,
        db_table_upgrade_handle upgradeHandler);

cs_status db_save_to_file(cs_int8 * filename);

cs_status db_save_to_flash();

cs_status db_reset_to_factory();

cs_status db_load_from_flash();

void db_core_init(
        cs_uint32 flash_offset, 
        cs_boolean encrypt,
        cs_boolean auto_sync);

void db_encrypt_set(cs_boolean enable);

void db_auto_sync_flash_set(cs_boolean enable);

void db_config_dump();
#endif
#endif

