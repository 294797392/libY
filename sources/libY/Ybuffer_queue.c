#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "Ybuffer_queue.h"
#include "Ythread.h"
#include "Ylock.h"
#include "Ysem.h"
#include "Yqueue.h"

#define MAX_QUEUE_SIZE 4096

struct Ybuffer_queue_s
{
	Yqueue_full_callback full_callback;

	Yqueue_callback callback;
	Yqueue_state state;
	void *userdata;
	
	Yqueue *base_queue;

	// 队列的容量
	int max_pool_size;

	// 消费者线程列表
	Ythread **consume_threads;
	int num_thread;

	// 队列锁
	Ylock lock;

	// 信号量
	Ysem sem;
};

static void consume_thread_process(void *state)
{
	Ybuffer_queue *q = (Ybuffer_queue *)state;
	while (q->state == YQUEUE_STATE_RUNNING)
	{
		/* 取下一个元素 */
		void *element = Y_buffer_queue_dequeue(q);

		/* 有信号量了，回调 */
		if (q->callback)
		{
			q->callback(q->userdata, element);
		}
	}
}

Ybuffer_queue *Y_create_buffer_queue(void *userdata)
{
	Ybuffer_queue *yq = (Ybuffer_queue *)Ycalloc(1, sizeof(Ybuffer_queue));
	if (!yq)
	{
		//YLOGE(YTEXT("create Yqueue instance failed"));
		return NULL;
	}

	yq->state = YQUEUE_STATE_IDLE;
	yq->userdata = userdata;
	yq->max_pool_size = MAX_QUEUE_SIZE;
	yq->base_queue = Y_create_queue();
	Y_create_lock(yq->lock);
	Y_create_sem(yq->sem, yq->max_pool_size);
	return yq;
}

void Y_delete_buffer_queue(Ybuffer_queue *yq)
{
	yq->state = YQUEUE_STATE_IDLE;
	yq->callback = NULL;
	yq->full_callback = NULL;
	Y_delete_lock(yq->lock);
	Y_delete_sem(yq->sem);

	for(int i = 0; i < yq->num_thread; i++)
	{
		Y_delete_thread(yq->consume_threads[i]);
	}

	// 此时消费者线程肯定运行完了，所有元素也肯定都被消费了
	free(yq);
}

void Y_buffer_queue_start(Ybuffer_queue *yq, int num_thread, Yqueue_callback callback)
{
	yq->state = YQUEUE_STATE_RUNNING;
	yq->callback = callback;
	yq->num_thread = num_thread;
	yq->consume_threads = (Ythread**)calloc(num_thread, sizeof(Ythread*));

	for(int i = 0; i < num_thread; i++)
	{
		yq->consume_threads[i] = Y_create_thread(consume_thread_process, yq);
	}
}

void Y_buffer_queue_set_full_callback(Ybuffer_queue *yq, Yqueue_full_callback callback)
{
	yq->full_callback = callback;
}

void *Y_buffer_queue_dequeue(Ybuffer_queue *yq)
{
	// 等待生产者线程的数据
	Y_sem_wait(yq->sem);

	Y_lock_lock(yq->lock);

	void *item = Y_queue_dequeue(yq->base_queue);

	Y_lock_unlock(yq->lock);

	return item;
}

void Y_buffer_queue_enqueue(Ybuffer_queue *yq, void *element)
{
	Y_lock_lock(yq->lock);

	int queue_size = Y_queue_size(yq->base_queue);
	if(yq->max_pool_size > queue_size)
	{
		Y_queue_enqueue(yq->base_queue, element);
		Y_sem_post(yq->sem);
	}
	else
	{
		void *abandon = Y_queue_dequeue(yq->base_queue);
		if(yq->full_callback)
		{
			yq->full_callback(abandon, yq->userdata);
		}
		Y_queue_enqueue(yq->base_queue, element);
	}

	Y_lock_unlock(yq->lock);
}

int Y_buffer_queue_size(Ybuffer_queue *yq)
{
	return Y_queue_size(yq->base_queue);
}

void Y_buffer_queue_clear(Ybuffer_queue *yq)
{
	Y_lock_lock(yq->lock);

	int size = Y_queue_size(yq->base_queue);
	if(size > 0)
	{
		for(int i = 0; i < size; i++)
		{
			Y_sem_wait(yq->sem);
		}

		Y_queue_clear(yq->base_queue);
	}

	Y_lock_unlock(yq->lock);
}

