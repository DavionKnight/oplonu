#ifndef __MEMPOOL_H__
#define __MEMPOOL_H__

#include "cs_types.h"
#include "terminal_server.h"

//#define MEMPOOL_EX
#define MAX_SYS_MODULE_NUMBER 32
#define CS_INVALID_MODULE_ID 0xFFFF

#define MAX_PKT_LEN     1600
#define MAX_PKT_NUM     80

#ifdef MEMPOOL_EX
#define MAX_MODULE_NAME_LEN  32

typedef struct {
    cs_uint16 pool_id;
    cs_uint16 u2Rsvd;
    cs_uint32 count;
} pool_counter_t;

typedef struct {
    cs_uint32  module_id;
    pool_counter_t  *pool_counter;
    cs_uint32  count_64bytes;
    cs_uint32  count_65_128bytes;
    cs_uint32  count_129_256bytes;
    cs_uint32  count_257_512bytes;
    cs_uint32  count_513_1024bytes;
    cs_uint32  count_1k_2kbytes;
    cs_uint32  count_2k_4kbytes;
    cs_uint32  count_4k_8kbytes;
    cs_uint32  count_8kbytes; /*greater than 8k*/
} sys_module_desc_t;

typedef struct {
    cs_int32 elem_size;
    cs_int32 elem_num;
}sys_pool_cfg_t;

#ifdef HAVE_MEM_TRACE
#define MAX_SYS_MEM_STAT 256
typedef struct {
    cs_uint32 free;
    cs_uint16 module_id;
    cs_uint16 alloc_line;
    cs_int8     *func_name;
    cs_uint32 count;
} sys_mem_stat_t;
#endif

extern cs_uint8 *cs_sys_pool_malloc(cs_uint32 module_id , cs_int32 size , cs_int8 *func_name , cs_int32 line,cs_int32 trace_flag);
extern void cs_sys_pool_free(cs_uint8 *ptr , cs_int8 *func_name , cs_int32 line,cs_int32 trace_flag);
extern void cs_sys_pool_show(cs_uint32 pool_id);
extern void cs_sys_module_show(cs_uint32 module_id);

#define iros_malloc(module_id , size) (void *)cs_sys_pool_malloc(module_id , size , (cs_int8 *)__FUNCTION__ , __LINE__,1)
#define iros_free(ptr) cs_sys_pool_free((cs_uint8 *)ptr , (cs_int8 *)__FUNCTION__ , __LINE__,1)

#define iros_malloc_no_trace(module_id , size) (void *)cs_sys_pool_malloc(module_id , size , (cs_int8 *)__FUNCTION__ , __LINE__,0)
#define iros_free_no_trace(ptr) cs_sys_pool_free((cs_uint8 *)ptr , (cs_int8 *)__FUNCTION__ , __LINE__,0)

#else
#define iros_malloc(module_id , size) malloc(size)
#define iros_free(ptr) free(ptr)
#endif

#define MAX_MEMPOOL_NAME_LEN 16

typedef struct {
    cs_uint16  seq_id;
    cs_uint16 alloc_line;
    cs_uint16  module_id;
    cs_uint16 mem_pool_id;
    cs_uint32 mem_size;
    cs_int8    *func_name;
} cs_mem_rsvd_header;

typedef struct {
    cs_uint16 pool_id;
    cs_uint16 free;
    cs_uint32 element_num;
    cs_uint32 element_size;
    cs_uint32 real_element_size;
    cs_uint32 free_elem_num;
    cs_uint32 peek_value;
    cs_uint32 mutex_id;
    cs_int8     pool_name[MAX_MEMPOOL_NAME_LEN];
    cs_uint32   *buf_ptr_list;
    cs_uint8   *mem_bit_map;
} cs_mempool_t;


#define MEM_IS_FREE 0
#define MEM_IS_BUSY 1

#define CS_INVALID_POOL_ID 0

//#define iros_mempool_log if(mempool_debug) cs_printf

extern cs_int32 cs_mempool_create(cs_uint32 *pool_id ,cs_int8 *pool_name , cs_int32 elem_size ,cs_int32 elem_num);
extern cs_uint8 *cs_mem_malloc(cs_uint32 pool_id);
extern void cs_mem_free(cs_uint8 *pbuf);
extern cs_int32 cs_mempool_destroy(cs_uint32 pool_id);
extern void cs_pool_show(cs_uint32 pool_id);
extern cs_uint32 cs_pool_element_size_get(cs_uint32 pool_id);
extern cs_int8 *cs_pool_name_get(cs_uint32 pool_id);
extern cs_uint32 cs_pool_number();
extern void cs_updata_mem_stat(cs_mem_rsvd_header *pHeader ,cs_int32 flag);
extern cs_int32 cs_mempool_init();
#endif
