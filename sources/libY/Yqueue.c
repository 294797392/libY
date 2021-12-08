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

	int size;

	// 消费者队列
	Ythread *dequeue_thread;

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

void Y_delete_queue(Yqueue *q)
{
	// todo:实现
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

void Y_queue_enqueue(Yqueue *q, void *element)
{
	if (element == NULL)
	{
		return;
	}

	int increament = 0; /* 是否要把信号量加1的标志 */
#ifdef Y_API_WIN32
	EnterCriticalSection(&q->cs);
#elif Y_API_UNIX
	pthread_mutex_lock(&q->queue_mutex);
#endif

	int index = q->enqueue_index;
	if (index == MAX_QUEUE_SIZE)
	{
		/* 队列满了，从头开始 */
		q->enqueue_index = 0;
		index = 0;
	}

	if (q->elements[index] == NULL)
	{
		// 如果队列里的元素为空，说明被消费完了或者没有被使用过，那么把信号量加1
		increament = 1;
	}
	else
	{
		// 如果队列里的元素不为空，说明还没有被消费到, 丢弃最早的元素
		// 此时把丢弃的元素回调给用户，用户需要释放内存空间，不然会造成内存泄漏
		if(q->full_callback != NULL)
		{
			q->full_callback(q->elements[index], q->userdata);
		}
		else
		{
			//YLOGW(YTEXT("Yqueue缓冲区已满，被丢弃的元素没有做处理，可能会造成内存泄漏"));
		}
	}
	q->elements[index] = element;

	/* 计算下一个要入队的元素索引 */
	q->enqueue_index += 1;

	q->size++;

#ifdef Y_API_WIN32
	LeaveCriticalSection(&q->cs);
#elif Y_API_UNIX
	pthread_mutex_unlock(&q->queue_mutex);
#endif

	if (increament == 1)
	{
#ifdef Y_API_WIN32
		ReleaseSemaphore(q->sem, 1, NULL);
#elif Y_API_UNIX
		sem_post(&queue->sem);
#endif
	}
}

void *Y_queue_dequeue(Yqueue *q)
{
	/* 等待信号量 */
#ifdef Y_API_WIN32
	WaitForSingleObject(q->sem, INFINITE);
	EnterCriticalSection(&q->cs);
#elif Y_API_UNIX
	sem_wait(&q->sem);
	pthread_mutex_lock(&q->queue_mutex);
#endif

	/* 如果当前要出队的索引是MAX_QUEUE_SIZE，那么重置为0，从头开始继续出队 */
	int index = q->dequeue_index;
	if (index == MAX_QUEUE_SIZE)
	{
		index = 0;
		q->dequeue_index = 0;
	}

	void *element = q->elements[index];

	/* 处理完后置为NULL */
	q->elements[index] = NULL;

	/* 计算下一个要出队的索引 */
	q->dequeue_index += 1;

	q->size--;

#ifdef Y_API_WIN32
	LeaveCriticalSection(&q->cs);
#elif Y_API_UNIX
	pthread_mutex_unlock(&queue->queue_mutex);
#endif

	return element;
}

int Y_queue_size(Yqueue *q)
{
	return q->size;
}