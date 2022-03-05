#include "Yfirstinclude.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if (defined(Y_API_WIN32))
#include <Windows.h>
#elif (defined(Y_API_UNIX))
#include <pthread.h>
#endif

#include "Yevent.h"

struct Yevent_s
{
    int signaled;                   // 是否收到了信号

#if (defined(Y_API_WIN32))
#elif (defined(Y_API_UNIX))
    pthread_mutex_t mutex;
    pthread_cond_t cond;
#endif
};

Yevent *Y_create_event()
{
    Yevent *ye = (Yevent*)calloc(1, sizeof(Yevent));
#if (defined(Y_API_WIN32))
#elif (defined(Y_API_UNIX))
    pthread_mutex_init(&ye->mutex, NULL);
    pthread_cond_init(&ye->cond, NULL);
#endif
    return ye;
}

void Y_delete_event(Yevent *ye)
{
#if (defined(Y_API_WIN32))
#elif (defined(Y_API_UNIX))
    pthread_mutex_destroy(&ye->mutex);
    pthread_cond_destroy(&ye->cond);
#endif
    free(ye);
}

void Y_event_wait(Yevent *ye)
{
#if (defined(Y_API_WIN32))
#elif (defined(Y_API_UNIX))
    pthread_mutex_lock(&ye->mutex);
    // 这里判断的目的是当有多个线程等待信号的时候, 防止当信号已经触发之后, 还在等待空信号的问题
    // 参考：https://blog.csdn.net/zzran/article/details/8830213
    if(!ye->signaled)
    {
        pthread_cond_wait(&ye->cond, &ye->mutex);
    }
    pthread_mutex_unlock(&ye->mutex);
#endif
}

void Y_event_signal(Yevent *ye)
{
#if (defined(Y_API_WIN32))
#elif (defined(Y_API_UNIX))
    pthread_mutex_lock(&ye->mutex);
    ye->signaled = 1;
    pthread_cond_signal(&ye->cond);
    pthread_mutex_unlock(&ye->mutex);
#endif
}
