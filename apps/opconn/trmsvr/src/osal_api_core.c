/***********************************************************************/
/* This file contains unpublished documentation and software                                            */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure,                              */
/* in whole or in part, of the information in this file without a                                              */
/* written consent of an officer of Cortina Systems Incorporated is                                     */
/* strictly prohibited.                                                                                                             */
/* Copyright (c) 2002-2010 by Cortina Systems Incorporated.                                            */
/***********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <mqueue.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include "semaphore.h"
#include <mqueue.h>
#include "osal_common.h"
#include "osal_api_core.h"
#include "vos_sync.h"
//#include "gw_os_api_core.h"

#define CS_OSAL_MAX_PRI         31
#define CS_OSAL_UNINIT          0
#define OK	                    0


/*
void usleep(unsigned int usecs)
{
	unsigned int msecs = 0;
	unsigned int ticks = 0;

	msecs = usecs/1000;
	ticks = msecs/10;
	if(ticks == 0)
		ticks = 1;

	cyg_thread_delay(ticks);
	return;
}*/

/*  tables for the properties of objects */

/* threads */
#if 1
typedef struct {
    cs_int32 free;
    pthread_t id;
    cs_int8 name [CS_OSAL_MAX_API_NAME];
//    pthread_t creator;
//    cs_uint32 stack_size;
//    cs_uint32 priority;
//    cyg_thread thread_ctrl;
//    cs_uint8 *stack_buf;
}osal_thread_record_t;
#else
typedef struct {
    int 			free;
    pthread_t 		id;
    char 			name[CS_OSAL_MAX_API_NAME];
    int 			creator;
    unsigned int 	stack_size;
    unsigned int 	priority;
}osal_thread_record_t;
#endif

/* Counting Semaphores */
#if 0
typedef struct {
    cs_int32 free;
    cyg_sem_t  id;                       /* a pointer to the id */
    cs_int8 name [CS_OSAL_MAX_API_NAME];
    cyg_handle_t creator;
}osal_count_sem_record_t;
#else
typedef struct {
    int 	free;
    sem_t 	id;
    char 	name[CS_OSAL_MAX_API_NAME];
    int 	creator;
}osal_count_sem_record_t;
#endif

/* Mutexes */
#if 0
typedef struct {
    cs_int32 free;
    cyg_mutex_t id;
    cs_int8 name [CS_OSAL_MAX_API_NAME];
    cyg_handle_t creator;
}osal_mut_record_t;
#else
typedef struct {
    int 			free;
    pthread_mutex_t id;
    char			name[CS_OSAL_MAX_API_NAME];
    int 			creator;
}osal_mut_record_t;
#endif

/* queues */
#if 0
typedef struct {
    cs_int32 free;
    cyg_handle_t id;
    cs_int8 name [CS_OSAL_MAX_API_NAME];
    cyg_handle_t creator;
    cyg_mbox mbox;
    cs_uint32 mempool_id;
    cs_int32 depth;
    cs_int32 queue_size;
    cs_int32 queue_type;
    cs_int32    pri_num;
    cs_uint32   cnt_sem_id;
    cs_list         queue_list[CS_OSAL_MAX_QUEUE_PRI];
    cs_uint32 queue_mutex[CS_OSAL_MAX_QUEUE_PRI];
    cs_uint32   pri_queue_peek[CS_OSAL_MAX_QUEUE_PRI];

    cs_uint32 peek_value;
    cs_uint32 put_error;
    cs_uint32 put_full_error;
    cs_uint32 put_timeout_error;
    cs_uint32 put_data_error;
    cs_uint32 put_data_long;
    cs_uint32 get_error;
    cs_uint32 get_timeout_error;
    cs_uint32 get_data_error;
    cs_uint32 get_data_long;
}osal_queue_record_t;
#else
typedef struct {
    int 			free;
    mqd_t 			id;
    char 			name [CS_OSAL_MAX_API_NAME];
    int 			creator;
    unsigned int 	mempool_id;
    unsigned int	queue_size;
    int				depth;
    int				queue_type;
    int    			pri_num;
    unsigned int   	cnt_sem_id;
    cs_list     	queue_list[CS_OSAL_MAX_QUEUE_PRI];
    unsigned int 	queue_mutex[CS_OSAL_MAX_QUEUE_PRI];
    unsigned int	pri_queue_peek[CS_OSAL_MAX_QUEUE_PRI];
    unsigned int 	cur_num;
    unsigned int 	peek_value;
    unsigned int 	put_error;
    unsigned int 	put_full_error;
    unsigned int 	put_timeout_error;
    unsigned int 	put_data_error;
    unsigned int 	put_data_long;
    unsigned int 	get_error;
    unsigned int 	get_timeout_error;
    unsigned int 	get_data_error;
    unsigned int 	get_data_long;
}osal_queue_record_t;

#endif

/* Tables where the OS object information is stored */
osal_thread_record_t    osal_thread_table      [CS_OSAL_MAX_THREAD];
osal_count_sem_record_t osal_count_sem_table   [CS_OSAL_MAX_COUNT_SEM];
osal_mut_record_t       osal_mut_table         [CS_OSAL_MAX_MUTEX];
osal_queue_record_t     osal_queue_table       [CS_OSAL_MAX_QUEUE];

#if 1
VOS_MUTEX_t  osal_task_table_mutex;
#else
pthread_mutex_t osal_thread_table_mut;
#endif
#if 0
cyg_mutex_t osal_count_sem_table_mutex;
#else
VOS_MUTEX_t  osal_count_sem_table_mut;
#endif
#if 0
cyg_mutex_t osal_mut_table_mutex;
#else
VOS_MUTEX_t  osal_mut_table_mut;
#endif
#if 0
cyg_mutex_t osal_queue_table_mutex;
#else
VOS_MUTEX_t  osal_queue_table_mut;
#endif
// cs_int32 osal_debug=0;



void cs_osal_core_init(void)
{
    cs_int32 i;
    /* Initialize Task Table */
	#if 0
    for (i = 0; i < CS_OSAL_MAX_THREAD; i++) {
        memset(&osal_thread_table[i] , 0 , sizeof(osal_thread_record_t));
        osal_thread_table[i].free        = TRUE;
        osal_thread_table[i].id          = CS_OSAL_UNINIT;
        osal_thread_table[i].creator     = CS_OSAL_UNINIT;
        strcpy(osal_thread_table[i].name, "");
    }
	#else
	for (i = 0; i < CS_OSAL_MAX_THREAD; i++) {
        memset(&osal_thread_table[i] , 0 , sizeof(osal_thread_record_t));
        osal_thread_table[i].free = TRUE;
    }
	#endif

    /* Initialize Counting Semaphore Table */
	#if 0
    for (i = 0; i < CS_OSAL_MAX_COUNT_SEM; i++) {
        osal_count_sem_table[i].free        = TRUE;
        memset(&osal_count_sem_table[i].id , 0 , sizeof(cyg_sem_t));
        osal_count_sem_table[i].creator     = CS_OSAL_UNINIT;
        strcpy(osal_count_sem_table[i].name, "");
    }
	#else
	for (i = 0; i < CS_OSAL_MAX_COUNT_SEM; i++) {
        memset(&osal_count_sem_table[i] , 0 , sizeof(osal_count_sem_record_t));
        osal_count_sem_table[i].free = TRUE;
    }
	#endif


    /* Initialize Mutex Semaphore Table */
	#if 0
    for (i = 0; i < CS_OSAL_MAX_MUTEX; i++) {
        osal_mut_table[i].free        = TRUE;
        memset(&osal_mut_table[i].id , 0 , sizeof(cyg_mutex_t));
        osal_mut_table[i].creator     = CS_OSAL_UNINIT;
        strcpy(osal_mut_table[i].name, "");
    }
	#else
	for (i = 0; i < CS_OSAL_MAX_MUTEX; i++) {
        memset(&osal_mut_table[i] , 0 , sizeof(osal_mut_record_t));
        osal_mut_table[i].free = TRUE;
    }
	#endif

    /* Initialize message queue Table */
	#if 0
    for (i = 0; i < CS_OSAL_MAX_QUEUE ; i++) {
        memset(&osal_queue_table[i] , 0 , sizeof(osal_queue_record_t));
        osal_queue_table[i].free        = TRUE;
        osal_queue_table[i].id          = 0;
        osal_queue_table[i].creator     = CS_OSAL_UNINIT;
        strcpy(osal_queue_table[i].name, "");
    }
	#else
	for (i = 0; i < CS_OSAL_MAX_QUEUE; i++) {
        memset(&osal_queue_table[i] , 0 , sizeof(osal_queue_record_t));
        osal_queue_table[i].free = TRUE;
    }
	#endif

	#if 1
    vosMutexCreate(&osal_task_table_mutex);
	#else
	pthread_mutex_init((pthread_mutex_t *)&osal_thread_table_mut, NULL);
	#endif
	#if 0
    cyg_mutex_init(&osal_count_sem_table_mutex);
	#else
	vosMutexCreate(&osal_count_sem_table_mut);
	#endif
	#if 0
    cyg_mutex_init(&osal_mut_table_mutex);
	#else
	vosMutexCreate(&osal_mut_table_mut);
	#endif
	#if 0
    cyg_mutex_init(&osal_queue_table_mutex);
	#else
	vosMutexCreate(&osal_queue_table_mut);
	#endif

    cs_sys_mod_init();
    return;

}


#if 0
cs_int32 cs_thread_create(cs_uint32 *thread_id,  const cs_int8 *thread_name,
                              const void *function_pointer, void *param ,cs_uint32 stack_size, cs_uint32 priority, cs_uint32 flags)
{
    cs_uint32 possible_taskid;
    cs_uint8 *stack_buf = NULL;

    /* we don't want to allow names too long*/
    /* if truncated, two names might be the same */

    /* Check for NULL pointers */

    if ((thread_name == NULL) || (function_pointer == NULL) || (thread_id == NULL)) {
        printf("\r\n thread create failed , cause some parameter is NULL\n");
        return CS_E_OSAL_INVALID_POINTER;
    }

    if (strlen(thread_name) >= CS_OSAL_MAX_API_NAME) {
        printf("\r\n thread name is too long");
        return CS_E_OSAL_ERR_NAME_TOO_LONG;
    }


    pthread_mutex_lock(&osal_task_table_mutex);
    for (possible_taskid = 0; possible_taskid < CS_OSAL_MAX_THREAD; possible_taskid++) {
        if (osal_thread_table[possible_taskid].free  == TRUE) {
            break;
        }
    }

    /* Check to see if the id is out of bounds */
    if (possible_taskid >= CS_OSAL_MAX_THREAD || osal_thread_table[possible_taskid].free != TRUE) {
        pthread_mutex_unlock(&osal_task_table_mutex);
        iros_free(stack_buf);
        printf("\r\n no free thread can be allocate\n");
        return CS_E_OSAL_ERR_NO_FREE_IDS;
    }

#if 0 /*Remove it for new requirement 2009-12-28*/
    /* Check to see if the name is already taken */
    for (i = 0; i < CS_OSAL_MAX_THREAD; i++) {
        if ((osal_thread_table[i].free == FALSE) &&
                (strcmp(thread_name, osal_thread_table[i].name) == 0)) {
            cyg_mutex_unlock(&osal_task_table_mutex);
            iros_free(stack_buf);
            return CS_E_OSAL_ERR_NAME_TAKEN;
        }
    }
#endif

    osal_thread_table[possible_taskid].free = FALSE;

    pthread_mutex_unlock(&osal_task_table_mutex);
    /* Create VxWorks Task */

    pthread_create(&osal_thread_table[possible_taskid].id,NULL,
                      function_pointer,
                      param);
 //   cyg_thread_resume(osal_thread_table[possible_taskid].id);

    *thread_id = possible_taskid;

    strcpy(osal_thread_table[*thread_id].name, thread_name);

    /* this Id no longer free */
    pthread_mutex_lock(&osal_task_table_mutex);
    osal_thread_table[*thread_id].free = FALSE;
 //  osal_thread_table[*thread_id].creator = (pthread_t)cs_creator_find();
 //  osal_thread_table[*thread_id].stack_size = stack_size;
 //  osal_thread_table[*thread_id].priority = priority;
 //  osal_thread_table[*thread_id].stack_buf = stack_buf;
    pthread_mutex_unlock(&osal_task_table_mutex);
    return CS_E_OSAL_OK;
}
#endif

int cs_mutex_unlock(unsigned int mut_id)
{
    int ret_val ;

    if (mut_id >= CS_OSAL_MAX_MUTEX ) {
        printf("\r\n unlock an invalid mutex");
        return CS_E_OSAL_ERR_INVALID_ID;
    }

    if (pthread_mutex_unlock(&(osal_mut_table[mut_id].id))) {
        ret_val = CS_E_OSAL_SEM_FAILURE ;
    } else {
        ret_val = CS_E_OSAL_OK;
    }

    return ret_val;
}

int cs_mutex_lock(unsigned int mut_id)
{
    int ret_val;

    if (mut_id >= CS_OSAL_MAX_MUTEX ) {
        printf("\r\n lock an invalid mutex\n");
        return CS_E_OSAL_ERR_INVALID_ID;
    }

    if (pthread_mutex_lock(&(osal_mut_table[mut_id].id))) {
        ret_val = CS_E_OSAL_SEM_FAILURE;
    } else {
        ret_val = CS_E_OSAL_OK;
    }

    return ret_val;
}
#if 0
int cs_queue_create
	(
    unsigned int 	*queue_id,
    const char		*queue_name,
    unsigned int 	queue_depth,
    unsigned int 	data_size,
    unsigned int 	flags
	)
{
    unsigned int possible_qid;
    struct mq_attr queue_attr;
    int i;

    if (queue_id == NULL || queue_name == NULL) {
        printf("\r\n Create queue failed cause some parameter is NULL");
        return CS_E_OSAL_INVALID_POINTER;
    }

    if (queue_depth == 0 || data_size == 0) {
        printf("\r\n Create queue with queue_depth %d and data_size %d", queue_depth, data_size);
        return CS_E_OSAL_ERR;
    }

    if (strlen(queue_name) >= CS_OSAL_MAX_API_NAME) {
        printf("\r\n queue name is too long\n");
        return CS_E_OSAL_ERR_NAME_TOO_LONG;
    }

    vosMutexTake(&osal_queue_table_mut);
    for (possible_qid = 0; possible_qid < CS_OSAL_MAX_QUEUE; possible_qid++) {
        if (osal_queue_table[possible_qid].free == TRUE)
            break;
    }

    if (possible_qid >= CS_OSAL_MAX_QUEUE) {
        printf("\r\n no free queue slot");
        vosMutexGive(&osal_queue_table_mut);
        return CS_E_OSAL_ERR_NO_FREE_IDS;
    }

    for (i = 0; i < CS_OSAL_MAX_QUEUE; i++) {
        if ((osal_queue_table[i].free == FALSE) &&
                strcmp((char*)queue_name, osal_queue_table[i].name) == 0) {
            pthread_mutex_unlock(&osal_queue_table_mut);
            return CS_E_OSAL_ERR_NAME_TAKEN;
        }
    }

    osal_queue_table[possible_qid].free = FALSE;

	memset(&queue_attr , 0 ,sizeof(struct mq_attr));
    queue_attr.mq_maxmsg = queue_depth;
    queue_attr.mq_msgsize = data_size;
    osal_queue_table[possible_qid].id =sys_mq_open(queue_name , O_CREAT | O_RDWR , 0777 , &queue_attr);
    if (osal_queue_table[possible_qid].id == (mqd_t)-1) {
        memset(&osal_queue_table[possible_qid] , 0 , sizeof(osal_queue_record_t));
        osal_queue_table[possible_qid].free = TRUE;
        pthread_mutex_unlock(&osal_queue_table_mut);
        return CS_E_OSAL_ERR;
    }
    *queue_id = possible_qid;

    osal_queue_table[*queue_id].free = FALSE;
    strcpy(osal_queue_table[*queue_id].name, (char*) queue_name);
    osal_queue_table[*queue_id].depth = queue_depth;
    osal_queue_table[*queue_id].queue_size = data_size;
	osal_queue_table[*queue_id].queue_type = CS_NORMAL_QUEUE_TYPE;
    pthread_mutex_unlock(&osal_queue_table_mut);
    return CS_E_OSAL_OK;
}

int cs_queue_delete(unsigned int queue_id)
{
    if (queue_id >= CS_OSAL_MAX_QUEUE || osal_queue_table[queue_id].free == TRUE) {
        cs_printf("\r\n can not delete an invalid queue");
        return CS_E_OSAL_ERR_INVALID_ID;
    }

    pthread_mutex_lock(&osal_queue_table_mut);

    sys_mq_unlink(osal_queue_table[queue_id].name);
    memset(&osal_queue_table[queue_id] , 0 , sizeof(osal_queue_record_t));
    osal_queue_table[queue_id].free = TRUE;
    pthread_mutex_unlock(&osal_queue_table_mut);

    return CS_E_OSAL_OK;
}

int cs_queue_get
	(
    unsigned int 	queue_id,
    void 			*data,
    unsigned int 	size,
    unsigned int 	*size_copied,
    int 			timeout
	)
{
    unsigned int msg_pri = 0;
    ssize_t recv_len = -1;
    struct timespec tv,tv1;

    if (queue_id >= CS_OSAL_MAX_QUEUE || osal_queue_table[queue_id].free == TRUE) {
        printf("\r\n can not get msg from an invalid queue");
        return CS_E_OSAL_ERR_INVALID_ID;
    } else {
        if ((data == NULL) || (size_copied == NULL)) {
            printf("\r\n get msg failed cause some parameter is NULL");
            osal_queue_table[queue_id].get_error++;
            osal_queue_table[queue_id].get_data_error++;
            return CS_E_OSAL_INVALID_POINTER;
        }
    }

    if(timeout != -1)
    {
        cs_uint64 tim = 0;
        tim=cs_current_time();
        tim += timeout/10;
        tv.tv_sec = tim/100;
        tv.tv_nsec = (tim%100)*10*1000*1000;
    }

	tv1.tv_sec=1000000000;
	tv1.tv_nsec=1000000000;
	
    if(timeout == CS_OSAL_WAIT_FOREVER)
        recv_len = sys_mq_timedreceive(osal_queue_table[queue_id].id , (char *)data , size , &msg_pri,&tv1);
    else if(timeout == CS_OSAL_NO_WAIT)
        recv_len = sys_mq_timedreceive(osal_queue_table[queue_id].id , (char *)data , size , &msg_pri, NULL);
    else
        recv_len = sys_mq_timedreceive(osal_queue_table[queue_id].id , (char *)data , size , &msg_pri,&tv);
    
    if (recv_len == -1) {
        osal_queue_table[queue_id].get_error++;
        osal_queue_table[queue_id].get_data_error++;
        return CS_E_OSAL_ERR;
    }
	
    *size_copied = (unsigned int)recv_len;
	if(osal_queue_table[queue_id].cur_num > 0)
		osal_queue_table[queue_id].cur_num--;

    return CS_E_OSAL_OK;
}

int cs_queue_put
	(
    unsigned int	queue_id,
    void 			*data,
    unsigned int 	size,
    int 			timeout,
    unsigned int 	priority
	)
{
    int ret = -1;

    if (queue_id >= CS_OSAL_MAX_QUEUE || osal_queue_table[queue_id].free == TRUE) {
        printf("\r\n can not put msg into an invalid queue\n");
        return CS_E_OSAL_ERR_INVALID_ID;
    }

    if (data == NULL) {
        printf("\r\n can not put NULL msg into the queue\n");
        osal_queue_table[queue_id].put_error++;
        osal_queue_table[queue_id].put_data_error++;
        return CS_E_OSAL_INVALID_POINTER;
    }

    if (size > osal_queue_table[queue_id].queue_size) {
        printf("\r\n data size is greater than the queue's size\n");
        osal_queue_table[queue_id].put_error++;
        osal_queue_table[queue_id].put_data_long++;
        return CS_E_OSAL_ERR;
    }

    #if 0
    ret = mq_send(osal_queue_table[queue_id].id , (char *)data , size , priority);
    #else
    ret = sys_mq_timedsend(osal_queue_table[queue_id].id , (char *)data , size , priority, NULL);
    #endif
    if (ret == -1) {
		osal_queue_table[queue_id].put_full_error++;
        return CS_E_OSAL_ERR;
    }

	osal_queue_table[queue_id].cur_num++;
	if(osal_queue_table[queue_id].peek_value <= osal_queue_table[queue_id].cur_num)
        osal_queue_table[queue_id].peek_value = osal_queue_table[queue_id].cur_num;
	
    return CS_E_OSAL_OK;
}

#endif
/*---------------------------------------------------------------------------------------
   Name: cs_current_time

   Purpose: This function returns the system time in tick.
---------------------------------------------------------------------------------------*/

cs_uint64 cs_current_time(void)
{
	struct timeval tp;

	if(!gettimeofday(&tp,NULL))
		return tp.tv_sec*100+tp.tv_usec/10000;
	else 
		return -1;
}




/*---------------------------------------------------------------------------------------
 * cs_creator_find()
 *  This function will return the OSAL ID of the task that created the calling function;
 *  This is an internal call, not to be used by the user.
---------------------------------------------------------------------------------------*/
cs_uint32 cs_creator_find()
{
    pthread_t thread_id;
    cs_int32 i;
    thread_id = pthread_self();

    for (i = 0; i < CS_OSAL_MAX_THREAD; i++) {
        if (thread_id == osal_thread_table[i].id)
            break;
    }

    return i;
}







