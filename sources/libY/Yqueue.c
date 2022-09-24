#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "Ylog.h"
#include "Yerrno.h"
#include "Yqueue.h"

#define DEFAULT_CAPACITY        512

struct Yqueue_s
{
	void **array;
	int head;
	int tail;

	// 当前队列里元素的总数量
	int size;
	// 队列的容量
	int capacity;
};


/// <summary>
/// 对queue进行扩容
/// </summary>
/// <param name="yq"></param>
/// <param name="count">要存储的元素的数量</param>
static void ensure_capacity(Yqueue *yq, int count)
{
	if(yq->capacity == 0)
	{
		yq->capacity = count;
		yq->array = calloc(yq->capacity, sizeof(void *));
	}
	else if(yq->capacity < count)
	{
		// 要扩充的元素数量
		int new_capacity = yq->capacity * 2;

		// 要扩充的大小
		int size = sizeof(void *) * new_capacity;

		// 先判断当前的指针是否有足够的连续空间，如果有，扩大mem_address指向的地址，并且将mem_address返回，
		// 如果空间不够，先按照newsize指定的大小分配空间，将原有数据从头到尾拷贝到新分配的内存区域，而后释放原来mem_address所指内存区域（注意：原来指针是自动释放，不需要使用free）
		// 同时返回新分配的内存区域的首地址。即重新分配存储器块的地址。
		yq->array = (void **)realloc(yq->array, size);
		if(yq->array == NULL)
		{
			perror("realloc == NULL");
		}

		yq->capacity = new_capacity;
	}
	else
	{
		/* code */
	}

	yq->head = 0;
	yq->tail = ((yq->size != yq->capacity) ? yq->size : 0);
}



Yqueue *Y_create_queue()
{
	Yqueue *yq = (Yqueue *)Ycalloc(1, sizeof(Yqueue));
	yq->capacity = 0;
	yq->head = 0;
	yq->tail = 0;
	yq->size = 0;
	ensure_capacity(yq, DEFAULT_CAPACITY);
	return yq;
}

void Y_delete_queue(Yqueue *yq)
{
	free(yq);
}

void Y_queue_enqueue(Yqueue *yq, void *item)
{
	if(yq->size == yq->capacity)
	{
		int num = (int)((long long)yq->capacity * 200L / 100);
		if(num < yq->capacity + 4)
		{
			num = yq->capacity + 4;
		}

		ensure_capacity(yq, num);
	}

	yq->array[yq->tail] = item;
	yq->tail = (yq->tail + 1) % yq->capacity;
	yq->size++;
}

void *Y_queue_dequeue(Yqueue *yq)
{
	if(yq->size == 0)
	{
		return NULL;
	}

	void *result = yq->array[yq->head];
	yq->array[yq->head] = NULL;
	yq->head = (yq->head + 1) % yq->capacity;
	yq->size--;
	return result;
}

int Y_queue_size(Yqueue *yq)
{
	return yq->size;
}

void Y_queue_clear(Yqueue *yq)
{
	yq->size = 0;
	yq->head = 0;
	yq->tail = 0;
}
