#include <stdio.h>
#include "mempool.h"
//#include "cs_types.h"
#include "mempool.h"
#include "osal_common.h"
#include "vos_sync.h"
//#include "terminal_server.h"
#if 0
cs_mempool_t *sys_mem_pool = NULL;
cs_uint32 sys_mempool_num = 0;   //changed from cs_int32
cs_uint32 sys_mempool_mutex = 0;
cs_int32 mempool_debug=0;
cs_uint32 pkt_pool_id = CS_INVALID_POOL_ID;

#ifdef MEMPOOL_EX
sys_pool_cfg_t sys_pool_cfg[] = {
    {64,            800},
    {128,           800},
    {256,           200},
    {512,           100},
    {1024,         200},
    {1600,         200},
    {4096,         4 },
    {8192,         2 },
    {16384,        2 },
};

cs_uint32 sys_module_cfg_mutex = 0;
sys_module_desc_t sys_module_cfg[IROS_MID_MAX];

cs_uint32 *sys_pool_ex_list = NULL;
cs_uint32 sys_pool_ex_num = 0;
cs_boolean sys_pool_ex_more_flag = TRUE;
cs_int32 sys_pool_ex_init(cs_boolean more_mem_flag);
sys_module_desc_t *cs_module_get_by_id(cs_uint32 module_id);

//#ifdef HAVE_MEM_TRACE
cs_uint32 mem_stat_mutex = 0;
sys_mem_stat_t mem_stat[MAX_SYS_MEM_STAT];
//#endif

#endif

pthread_mutex_t updata_module_cfg;



cs_int32 cs_mempool_init()
{
	vosMutexCreate(&updata_module_cfg);
	return OK;

}




void cs_updata_module_cfg(cs_mem_rsvd_header *pHeader ,cs_int32 flag)
{
    cs_uint32 i;
    sys_module_desc_t *pModule = NULL;
    cs_uint32 module_id;
    cs_uint32 pool_id;
    cs_uint32 size;

    if(pHeader == NULL) {
        return;
    }

    module_id = pHeader->module_id;
    pool_id = pHeader->mem_pool_id;
    size = pHeader->mem_size;
    if(module_id >= IROS_MID_MAX) {
        printf("\r\n can not update an invalid module's counter\n");
        return ;
    }

    vosMutexTake(&updata_module_cfg);
    pModule = cs_module_get_by_id(module_id);
    if(pModule == NULL) {
        vosMutexGive(&updata_module_cfg);
        return ;
    }

    if(size <= 64) {
        if(flag)
            pModule->count_64bytes++;
        else
            pModule->count_64bytes--;
    }
    else if(size > 64 && size <= 128){
        if(flag)
            pModule->count_65_128bytes++;
        else
            pModule->count_65_128bytes--;
    }
     else if(size > 128 && size <= 256){
        if(flag)
            pModule->count_129_256bytes++;
        else
            pModule->count_129_256bytes--;
    }
     else if(size > 256 && size <= 512){
        if(flag)
            pModule->count_257_512bytes++;
        else
            pModule->count_257_512bytes--;
    }
     else if(size > 512 && size <= 1024){
        if(flag)
            pModule->count_513_1024bytes++;
        else
            pModule->count_513_1024bytes--;
    }
     else if(size > 1024 && size <= 2048){
        if(flag)
            pModule->count_1k_2kbytes++;
        else
            pModule->count_1k_2kbytes--;
    }
     else if(size > 2048 && size <= 4096){
        if(flag)
            pModule->count_2k_4kbytes++;
        else
            pModule->count_2k_4kbytes--;
    }
     else if(size > 4096 && size <= 8192){
        if(flag)
            pModule->count_4k_8kbytes++;
        else
            pModule->count_4k_8kbytes--;
    }
     else{
        if(flag)
            pModule->count_8kbytes++;
        else
            pModule->count_8kbytes--;
    }

#ifdef HAVE_MEM_TRACE
    cs_updata_mem_stat(pHeader, flag);
#endif

    if(pool_id == CS_INVALID_POOL_ID || pModule->pool_counter == NULL) {
        vosMutexGive(&updata_module_cfg);
        return;
    }

    for(i = 0 ; i < sys_pool_ex_num ; i++)
    {
        if(pModule->pool_counter[i].pool_id != (cs_uint16)pool_id) {
            continue;
        }

        if(flag) {
            pModule->pool_counter[i].count++;
        }
        else {
            pModule->pool_counter[i].count--;
        }

        vosMutexGive(&updata_module_cfg);
        return;
    }
    vosMutexGive(&updata_module_cfg);

    return;
}


cs_uint8 *cs_mem_malloc(cs_uint32 pool_id)
{
    cs_mempool_t *pool = NULL;
    cs_mem_rsvd_header *rsvd_header = NULL;
    cs_int32 i;

    if (pool_id > sys_mempool_num || pool_id == CS_INVALID_POOL_ID) {
        printf("\r\n mem alloc failed, cause pool_id is invalid(%d)",pool_id);
        return (cs_uint8 *)NULL;
    }

    pool = (cs_mempool_t *) & sys_mem_pool[pool_id-1];
    if (pool->free == MEM_IS_FREE) {
        printf("\r\n memory pool which id is pool_id is free");
        return (cs_uint8 *)NULL;
    }

    cs_mutex_lock(pool->mutex_id);
    if (pool->free_elem_num <= 0) {
        printf("\r\n no memory is free in the pool(%d)",pool_id);
        cs_mutex_unlock(pool->mutex_id);
        return (cs_uint8 *)NULL;
    }

#if 0
    for (i = 0 ; i < pool->element_num ; i++) {
        rsvd_header = (cs_mem_rsvd_header *)pool->buf_ptr_list[i];
        if (rsvd_header->free == MEM_IS_FREE) {
            break;
        }
    }
#endif

    if((i = cs_bit_get_1((cs_int8 *)pool->mem_bit_map , pool->element_num)) == -1)
    {
        printf("\r\n no memory is free in the pool(%d).",pool_id);
        cs_mutex_unlock(pool->mutex_id);
        return (cs_uint8 *)0;
    }
    cs_bit_clr((cs_int8 *)pool->mem_bit_map, i);

    rsvd_header = (cs_mem_rsvd_header *)pool->buf_ptr_list[i];

    /*rsvd_header->free = MEM_IS_BUSY;*/
    rsvd_header->module_id = CS_INVALID_MODULE_ID;
    pool->free_elem_num--;
    if(pool->element_num - pool->free_elem_num > pool->peek_value) {
        pool->peek_value = pool->element_num - pool->free_elem_num;
    }
    cs_mutex_unlock(pool->mutex_id);

    return (cs_uint8 *)&rsvd_header[1];
}

/***********************************************************************
* NAME : cs_mem_free
* DESC :
*             Free the memory which you allocated.
* ARGUMENT:
*             pbuf - pointer of the line buffer
* RETURN :
*             N/A
***********************************************************************/

void cs_mem_free(cs_uint8 *pbuf)
{
    cs_mempool_t *pool = NULL;
    cs_mem_rsvd_header *rsvd_header = NULL;
    cs_int32 pool_id;

    if (pbuf == NULL) {
        printf("\r\n can not free a NULL pointer");
        return;
    }

    rsvd_header = (cs_mem_rsvd_header *)(pbuf - sizeof(cs_mem_rsvd_header));
    pool_id = rsvd_header->mem_pool_id;
    if (pool_id > sys_mempool_num || pool_id == CS_INVALID_POOL_ID) {
        printf("\r\n free a invalid pool(%d)",pool_id);
        return ;
    }

    pool = (cs_mempool_t *) & sys_mem_pool[pool_id-1];
    if (pool->free == MEM_IS_FREE) {
        printf("\r\n return the free memory to an invalid pool");
        return ;
    }

    /*rsvd_header->free = MEM_IS_FREE;*/
    cs_mutex_lock(pool->mutex_id);
    pool->free_elem_num++;
    cs_bit_set((cs_int8 *)pool->mem_bit_map , rsvd_header->seq_id);
    cs_mutex_unlock(pool->mutex_id);

    return ;
}


cs_uint32 cs_pool_element_size_get(cs_uint32 pool_id)
{
    cs_mempool_t *pool = NULL;

    if (pool_id >  sys_mempool_num || pool_id == CS_INVALID_POOL_ID) {
        return 0;
    }

    pool = &sys_mem_pool[pool_id-1];
    if (pool->free == MEM_IS_FREE) {
        return 0;
    }

    return pool->element_size;
}


cs_uint8 *cs_sys_pool_malloc(cs_uint32 module_id , cs_int32 size , cs_int8 *func_name , cs_int32 line ,cs_int32 trace_flag)
{
    cs_uint32 i;
    cs_uint8 *pBuf = NULL;
    cs_mem_rsvd_header *pHeader = NULL;
    cs_int32 real_size = 0;

    if(size <= 0 || module_id >= IROS_MID_MAX)
    {
        printf("\r\n cs_sys_pool_malloc failed(%d , %d)",module_id , size);
        return NULL;
    }

#ifdef HAVE_SYS_MEMPOOL
    if(module_id == IROS_MID_PACKET)
        pBuf = (cs_uint8 *)cs_mem_malloc(pkt_pool_id);
    else
        goto ALLOCATE_FOR_ECOS;

    if(pBuf == NULL)
        return NULL;

    pHeader = (cs_mem_rsvd_header *)(pBuf - sizeof(cs_mem_rsvd_header));
    pHeader->alloc_line = line;
    pHeader->module_id = (cs_uint16)module_id;
    pHeader->func_name = func_name;
    pHeader->mem_size = size;
    if(trace_flag)
        cs_updata_module_cfg(pHeader,1);
    return (cs_uint8 *)pBuf;
#endif

    for(i = 0 ; i < sys_pool_ex_num ; i++)
    {
        if(size > cs_pool_element_size_get(sys_pool_ex_list[i])) {
            continue;
        }

        pBuf = (cs_uint8 *)cs_mem_malloc(sys_pool_ex_list[i]);
        if(pBuf == NULL)
        {
            continue;
        }

        pHeader = (cs_mem_rsvd_header *)(pBuf - sizeof(cs_mem_rsvd_header));
        pHeader->alloc_line = line;
        pHeader->module_id = (cs_uint16)module_id;
        pHeader->func_name = func_name;
        pHeader->mem_size = size;
        if(trace_flag)
            cs_updata_module_cfg(pHeader,1);
        return (cs_uint8 *)pBuf;
    }

    if(!sys_pool_ex_more_flag) {
        /*need add log information*/
        printf("\r\n sys_pool_ex_more_flag is %d\n",sys_pool_ex_more_flag);
        return NULL;
    }

#ifdef HAVE_SYS_MEMPOOL
ALLOCATE_FOR_ECOS:
#endif
    real_size = size + sizeof(cs_mem_rsvd_header);
    pBuf = (cs_uint8 *)malloc(real_size);
    if(pBuf != NULL)
    {
        pHeader = (cs_mem_rsvd_header *)pBuf;
        pHeader->alloc_line = line;
        pHeader->func_name = func_name;
        pHeader->mem_size = size;
        pHeader->module_id = (cs_uint16)module_id;
        pHeader->mem_pool_id = CS_INVALID_POOL_ID;
        /*pHeader->free = MEM_IS_BUSY;*/
        pBuf += sizeof(cs_mem_rsvd_header);
        if(trace_flag)
            cs_updata_module_cfg(pHeader , 1);
    }

    return (cs_uint8 *)pBuf;
}
#if 1
void cs_sys_pool_free(cs_uint8 *ptr , cs_int8 *func_name , cs_int32 line , cs_int32 trace_flag)
{
    cs_mem_rsvd_header *pHeader = NULL;

    if(ptr == NULL) {
        return ;
    }

    pHeader = (cs_mem_rsvd_header *)(ptr-sizeof(cs_mem_rsvd_header));
    if(pHeader->mem_pool_id == CS_INVALID_POOL_ID) {
        /*need add log infromation*/
        if(trace_flag)
            cs_updata_module_cfg(pHeader,0);
        free((void *)pHeader);
        return;
    }

    if(trace_flag)
        cs_updata_module_cfg(pHeader,0);
    cs_mem_free(ptr);
    return;
}
#endif
sys_module_desc_t *cs_module_get_by_id(cs_uint32 module_id)
{
    cs_int32 i;

    for(i = 0 ; i < IROS_MID_MAX ; i++)
    {
        if(sys_module_cfg[i].module_id == module_id) {
            return (sys_module_desc_t *)&sys_module_cfg[i];
        }
    }

    return (sys_module_desc_t *)NULL;
}


void cs_updata_mem_stat(cs_mem_rsvd_header *pHeader ,cs_int32 flag)
{
    cs_int32 i;
    cs_int32 free_idx = -1;

    /*cs_mutex_lock(mem_stat_mutex);*/
    for (i = 0 ; i < MAX_SYS_MEM_STAT ; i++) {
        if(free_idx == -1 && mem_stat[i].free == MEM_IS_FREE) {
            free_idx = i;
        }

        if(mem_stat[i].module_id == pHeader->module_id &&
            mem_stat[i].func_name == pHeader->func_name &&
            mem_stat[i].alloc_line == pHeader->alloc_line &&
            mem_stat[i].free == MEM_IS_BUSY) {
            if(flag)
                mem_stat[i].count++;
            else {
                if(mem_stat[i].count)
                    mem_stat[i].count--;

                if(mem_stat[i].count == 0) {
                    memset(&mem_stat[i] , 0 , sizeof(sys_mem_stat_t));
                    mem_stat[i].free = MEM_IS_FREE;
                    mem_stat[i].module_id = CS_INVALID_MODULE_ID;
                }
            }

            /*cs_mutex_unlock(mem_stat_mutex);*/
            return;
        }
    }

    if(free_idx == -1) {
        printf("\r\n no free mem_stat slot\n");
        /*cs_mutex_unlock(mem_stat_mutex);*/
        return;
    }

    if(!flag) {
        printf("\r\n no alloc , but you try to free\n");
        /*cs_mutex_unlock(mem_stat_mutex);*/
        return;
    }
    mem_stat[free_idx].free = MEM_IS_BUSY;
    mem_stat[free_idx].module_id = pHeader->module_id;
    mem_stat[free_idx].alloc_line = pHeader->alloc_line;
    mem_stat[free_idx].func_name = pHeader->func_name;
    mem_stat[free_idx].count = 1;

    /*cs_mutex_unlock(mem_stat_mutex);*/

    return;
}

void cs_module_mem_stat_show(cs_uint16 module_id)
{
    cs_int32 i;

    if(module_id >= IROS_MID_MAX)
        return;

    /*cs_mutex_lock(mem_stat_mutex);*/
    for(i = 0 ; i < MAX_SYS_MEM_STAT ; i++) {
        if(mem_stat[i].free == MEM_IS_FREE)
            continue;

        if(mem_stat[i].module_id != module_id)
            continue;

        printf("\r\n   %32s(%-5d) : %-8d",mem_stat[i].func_name,mem_stat[i].alloc_line,mem_stat[i].count);
    }
    /*cs_mutex_unlock(mem_stat_mutex);*/
}
#endif




