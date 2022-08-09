#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "Ybase.h"
#include "Ylog.h"
#include "Yqueue.h"
#include "Ythread.h"
#include "Ylock.h"
#include "Ysem.h"

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

Yqueue *Y_create_queue(void *userdata, size_t itemsize)
{
	Yqueue *yq = (Yqueue *)calloc(1, sizeof(Yqueue));
	if (!yq)
	{
		//YLOGE(YTEXT("create Yqueue instance failed"));
		return NULL;
	}

	yq->itemsize = itemsize;
	yq->state = YQUEUE_STATE_IDLE;
	yq->userdata = userdata;
	Y_create_lock(yq->lock);
	Y_create_sem(yq->sem, MAX_QUEUE_SIZE);

	return yq;
}

void Y_delete_queue(Yqueue *yq)
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

void Y_queue_start(Yqueue *yq, int num_thread, Yqueue_callback callback)
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

void Y_queue_set_full_callback(Yqueue *yq, Yqueue_full_callback callback)
{
	yq->full_callback = callback;
}

void *Y_queue_dequeue(Yqueue *yq)
{
	// 等待生产者线程的数据
	Y_sem_wait(yq->sem);

	Y_lock_lock(yq->lock);

	/* 如果当前要出队的索引是MAX_QUEUE_SIZE，那么重置为0，从头开始继续出队 */
	int index = yq->dequeue_index;
	if (index == MAX_QUEUE_SIZE)
	{
		index = 0;
		yq->dequeue_index = 0;
	}

	void *element = yq->elements[index];

	yq->count--;

	/* 计算下一个要出队的索引 */
	yq->dequeue_index += 1;

	Y_lock_unlock(yq->lock);

	return element;
}

int Y_queue_size(Yqueue *yq)
{
	return yq->count;
}

void *Y_queue_prepare_enqueue(Yqueue *yq)
{
	Y_lock_lock(yq->lock);

	int index = yq->enqueue_index;
	if (index == MAX_QUEUE_SIZE)
	{
		/* 队列满了，从头开始 */
		yq->enqueue_index = 0;
		index = 0;
	}

	// 如果下一个要入队的元素为空，那么开辟一段新的内存空间
	void *item = yq->elements[index];
	if (item == NULL)
	{
		item = calloc(1, yq->itemsize);
		yq->elements[index] = item;
	}

	yq->count++;

	/* 计算下一个要入队的元素索引 */
	yq->enqueue_index += 1;

	Y_lock_unlock(yq->lock);

	return item;
}

void Y_queue_commit_enqueue(Yqueue *yq)
{
	Y_sem_post(yq->sem);
}