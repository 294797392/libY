#include "Yfirstinclude.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef Y_API_WIN32
#include <Windows.h>
#elif Y_API_UNIX
#include <pthread.h>
#include <semaphore.h>
#endif

#include "Ybase.h"
#include "Ylog.h"
#include "Yqueue.h"
#include "Ythread.h"

#define MAX_QUEUE_SIZE 4096

struct Yqueue_s
{
	Yqueue_full_callback full_callback;

	Yqueue_callback callback;
	Yqueue_state state;
	void *userdata;
	void *elements[MAX_QUEUE_SIZE];
	int enqueue_index;  /* 当前要入队的索引 */
	int dequeue_index;  /* 当前要出队的索引 */

	// 队列里每个元素的大小
	int itemsize;

	// 当前队列里元素的总数量
	int count;

	// 消费者队列
	Ythread *consume_thread;

#ifdef Y_API_WIN32
	HANDLE sem;				// 信号量对象
	CRITICAL_SECTION cs;	// 临界区对象
#elif Y_API_UNIX
	sem_t sem;
	pthread_mutex_t queue_mutex;
#endif
};

static void dequeue_thread_process(void *state)
{
	Yqueue *q = (Yqueue *)state;
	while (q->state == YQUEUE_STATE_RUNNING)
	{
		/* 取下一个元素 */
		void *element = Y_queue_dequeue(q);

		/* 有信号量了，回调 */
		if (q->callback)
		{
			q->callback(q->userdata, element);
		}
	}
}

Yqueue *Y_create_queue(void *userdata)
{
	Yqueue *queue = (Yqueue *)calloc(1, sizeof(Yqueue));
	if (!queue)
	{
		//YLOGE(YTEXT("create Yqueue instance failed"));
		return NULL;
	}

#ifdef Y_API_WIN32
	if (!(queue->sem = CreateSemaphoreW(NULL, 0, MAX_QUEUE_SIZE, YTEXT("Yqueue"))))
	{
		//YLOGE(YTEXT("CreateSemaphore failed, %d"), GetLastError());
		free(queue);
		return NULL;
	}
#elif Y_API_UNIX
	if (sem_init(&queue->sem, 0, 0) < 0)
	{
		//YLOGE(YTEXT("create sem failed, %d"), errno);
		free(queue);
		return NULL;
	}
#endif

	queue->state = YQUEUE_STATE_IDLE;
	queue->userdata = userdata;
#ifdef Y_API_WIN32
	InitializeCriticalSection(&queue->cs);
#elif Y_API_UNIX
	pthread_mutex_init(&queue->queue_mutex, NULL);
#endif
	return queue;
}

void Y_delete_queue(Yqueue *yq)
{
	yq->state = YQUEUE_STATE_IDLE;
	yq->callback = NULL;
	yq->full_callback = NULL;

#ifdef Y_API_WIN32
	ReleaseSemaphore(yq->sem, yq->count, NULL);
	CloseHandle(yq->sem);
	DeleteCriticalSection(&yq->cs);
#elif Y_API_UNIX
	for (int i = 0; i < yq->count; i++)
	{
		sem_post(&yq->sem);
	}
	sem_destroy(&yq->sem);
#endif

	Y_delete_thread(yq->consume_thread);

	free(yq);
}

void Y_queue_start(Yqueue *q, Yqueue_callback callback)
{
	q->state = YQUEUE_STATE_RUNNING;
	q->callback = callback;
	Y_create_thread(dequeue_thread_process, q);
}

void Y_queue_set_full_callback(Yqueue *yq, Yqueue_full_callback callback)
{
	yq->full_callback = callback;
}

void Y_queue_enqueue(Yqueue *yq, void *element)
{
	int index = yq->enqueue_index;
	if (index == MAX_QUEUE_SIZE)
	{
		/* 队列满了，从头开始 */
		yq->enqueue_index = 0;
		index = 0;
	}

#ifdef Y_API_WIN32
	EnterCriticalSection(&yq->cs);
#elif Y_API_UNIX
	pthread_mutex_lock(&yq->queue_mutex);
#endif

	yq->elements[index] = element;

	yq->count++;

#ifdef Y_API_WIN32
	LeaveCriticalSection(&yq->cs);
	ReleaseSemaphore(yq->sem, 1, NULL);
#elif Y_API_UNIX
	pthread_mutex_unlock(&yq->queue_mutex);
	sem_post(&yq->sem);
#endif

	/* 计算下一个要入队的元素索引 */
	yq->enqueue_index += 1;
}

void *Y_queue_dequeue(Yqueue *yq)
{
	/* 如果当前要出队的索引是MAX_QUEUE_SIZE，那么重置为0，从头开始继续出队 */
	int index = yq->dequeue_index;
	if (index == MAX_QUEUE_SIZE)
	{
		index = 0;
		yq->dequeue_index = 0;
	}

	/* 等待信号量 */
#ifdef Y_API_WIN32
	WaitForSingleObject(yq->sem, INFINITE);
	EnterCriticalSection(&yq->cs);
#elif Y_API_UNIX
	sem_wait(&yq->sem);
	pthread_mutex_lock(&yq->queue_mutex);
#endif

	void *element = yq->elements[index];

	/* 处理完后置为NULL */
	yq->elements[index] = NULL;

	yq->count--;

#ifdef Y_API_WIN32
	LeaveCriticalSection(&yq->cs);
#elif Y_API_UNIX
	pthread_mutex_unlock(&yq->queue_mutex);
#endif

	/* 计算下一个要出队的索引 */
	yq->dequeue_index += 1;

	return element;
}

int Y_queue_size(Yqueue *yq)
{
	return yq->count;
}


void Y_queue_set_itemsize(Yqueue *yq, size_t size)
{
	yq->itemsize = size;
}

void *Y_queue_begin_enqueue(Yqueue *yq)
{
	int index = yq->enqueue_index;
	if (index == MAX_QUEUE_SIZE)
	{
		/* 队列满了，从头开始 */
		yq->enqueue_index = 0;
		index = 0;
	}

#ifdef Y_API_WIN32
	EnterCriticalSection(&yq->cs);
#elif Y_API_UNIX
	pthread_mutex_lock(&yq->queue_mutex);
#endif

	// 如果下一个要入队的元素为空，那么开辟一段新的内存空间
	void *item = yq->elements[index];
	if (item == NULL)
	{
		item = calloc(1, yq->itemsize);
		yq->elements[index] = item;
	}

	yq->count++;

#ifdef Y_API_WIN32
	LeaveCriticalSection(&yq->cs);
#elif Y_API_UNIX
	pthread_mutex_unlock(&yq->queue_mutex);
#endif

	/* 计算下一个要入队的元素索引 */
	yq->enqueue_index += 1;

	return item;
}

void Y_queue_end_enqueue(Yqueue *yq)
{
#ifdef Y_API_WIN32
	ReleaseSemaphore(yq->sem, 1, NULL);
#elif Y_API_UNIX
	sem_post(&yq->sem);
#endif
}