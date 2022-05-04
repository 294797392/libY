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
#if (defined(Y_API_WIN32))
	HANDLE event;
#elif (defined(Y_API_UNIX))
	int signaled;                   // 是否收到了信号
	pthread_mutex_t mutex;
	pthread_cond_t cond;
#endif
};

Yevent *Y_create_event()
{
	Yevent *ye = (Yevent *)calloc(1, sizeof(Yevent));
#if (defined(Y_API_WIN32))
	ye->event = CreateEvent(NULL, TRUE, FALSE, NULL);
#elif (defined(Y_API_UNIX))
	pthread_mutex_init(&ye->mutex, NULL);
	pthread_cond_init(&ye->cond, NULL);
#endif
	return ye;
}

void Y_delete_event(Yevent *ye)
{
#if (defined(Y_API_WIN32))
	CloseHandle(ye->event);
#elif (defined(Y_API_UNIX))
	pthread_mutex_destroy(&ye->mutex);
	pthread_cond_destroy(&ye->cond);
#endif
	free(ye);
}

void Y_event_wait(Yevent *ye)
{
#if (defined(Y_API_WIN32))
	WaitForSingleObject(ye->event, INFINITE);
#elif (defined(Y_API_UNIX))
	pthread_mutex_lock(&ye->mutex);
	pthread_cond_wait(&ye->cond, &ye->mutex);
	pthread_mutex_unlock(&ye->mutex);
#endif
}

void Y_event_signal(Yevent *ye)
{
#if (defined(Y_API_WIN32))
	SetEvent(ye->event);
#elif (defined(Y_API_UNIX))
	pthread_mutex_lock(&ye->mutex);
	ye->signaled = 1;
	pthread_cond_signal(&ye->cond);
	pthread_mutex_unlock(&ye->mutex);
#endif
}
