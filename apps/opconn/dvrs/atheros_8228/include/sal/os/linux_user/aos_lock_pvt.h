#ifndef _AOS_LOCK_PVT_H
#define _AOS_LOCK_PVT_H

#include <pthread.h>
#include "sal/os/aos_types.h"


typedef pthread_mutex_t aos_lock_pvt_t;


#define __aos_lock_init(lock) \
{ \
    pthread_mutexattr_t attr; \
    pthread_mutexattr_init(&attr); \
    attr.__mutexkind = PTHREAD_MUTEX_RECURSIVE_NP; \
    pthread_mutex_init(lock, &attr); \
}



#if 1
#define __aos_lock(lock) pthread_mutex_lock(lock)
#else
#define __aos_lock(lock) { \
    int ii = 0; \
    while (1) {\
        if (pthread_mutex_trylock(lock)) { \
            printf("\n %s[%d] try get locker....%d", __FUNCTION__, __LINE__, ii); \
            sleep(1); \
            ii++; \
        } else { \
            printf("\n %s[%d] get locker....ok! %d", __FUNCTION__, __LINE__, ii); \
            break; \
        } \
    } \
}
#endif


#if 1
#define __aos_unlock(lock) pthread_mutex_unlock(lock)
#else
#define __aos_unlock(lock) \
    { \
      pthread_mutex_unlock(lock); \
      printf("\n %s[%d] release locker....ok!", __FUNCTION__, __LINE__); \
    }
#endif

 


#define __aos_irq_save(flags)


#define __aos_irq_restore(flags)


#define __aos_default_unlock PTHREAD_MUTEX_INITIALIZER


#endif /*_AOS_LOCK_PVT_H*/

