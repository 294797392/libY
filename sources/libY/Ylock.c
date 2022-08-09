#if defined(Y_WIN32)
#include <Windows.h>
#elif defined(Y_UNIX)
#endif

#include "Ybase.h"
#include "Ylock.h"

// struct Ylock_s
// {
// #if defined(Y_WIN32)
//     CRITICAL_SECTION lock;
// #elif defined(Y_UNIX)
//     pthread_mutex_t lock;
// #endif
// };

// Ylock *Y_create_lock()
// {
//     Ylock *yl = (Ylock*)calloc(1, sizeof(Ylock));
// #if defined(Y_WIN32)
//     InitializeCriticalSection(&yl->lock);
// #elif defined(Y_UNIX)
// 	pthread_mutex_init(&yl->lock, NULL);
// #endif
//     return yl;
// }

// void Y_delete_lock(Ylock *yl)
// {
// #if defined(Y_WIN32)
//     DeleteCriticalSection(&yl->lock);
// #elif defined(Y_UNIX)
//     pthread_mutex_destroy(&yl->lock);
// #endif
//     free(yl);
// }

// void Y_lock_lock(Ylock *yl)
// {
// #if defined(Y_WIN32)
//     EnterCriticalSection(&yl->lock);
// #elif defined(Y_UNIX)
//     pthread_mutex_lock(&yl->lock);
// #endif
// }

// void Y_lock_unlock(Ylock *yl)
// {
// #if defined(Y_WIN32)
//     LeaveCriticalSection(&yl->lock);
// #elif defined(Y_UNIX)
//     pthread_mutex_unlock(&yl->lock);
// #endif
// }


