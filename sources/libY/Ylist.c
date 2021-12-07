#include "Yfirstinclude.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Ylog.h"
#include "Ylist.h"

#define DEFAULT_CAPACITY        512

struct Ylist_s
{
	// 数组的总容量
	int capacity;

	// 数组对象
	void **array;

	// 当前数组里元素的数量
	int length;

	Ylist_free_func freefunc;
};

// 把元素从从start_index的地方向后移动一个元素
static void move_right(Ylist *yl, int start_index)
{
	for (size_t i = yl->length; i > start_index; i--)
	{
		yl->array[i] = yl->array[i - 1];
	}
}

// 把元素从从start_index的地方向前移动一个元素
static void move_left(Ylist *yl, int start_index)
{
	for (size_t i = start_index; i < yl->length; i++)
	{
		yl->array[i - 1] = yl->array[i];
	}
}

static void foreach_free(Ylist *yl, void *item, void *userdata)
{
	yl->freefunc(item);
}

static void ensure_capacity(Ylist *yl, int count)
{
	if (yl->capacity == 0)
	{
		yl->capacity = DEFAULT_CAPACITY;
		yl->array = calloc(yl->capacity, sizeof(void *));
	}
	else if (yl->capacity < count)
	{
		// 要扩充的元素数量
		int new_capacity = yl->capacity * 2;

		// 要扩充的大小
		int size = sizeof(void *) * new_capacity;

		//先判断当前的指针是否有足够的连续空间，如果有，扩大mem_address指向的地址，并且将mem_address返回，
		// 如果空间不够，先按照newsize指定的大小分配空间，将原有数据从头到尾拷贝到新分配的内存区域，而后释放原来mem_address所指内存区域（注意：原来指针是自动释放，不需要使用free）
		// 同时返回新分配的内存区域的首地址。即重新分配存储器块的地址。
		yl->array = (void **)realloc(yl->array, size);

		yl->capacity = new_capacity;
	}
	else
	{
		/* code */
	}
}

Ylist *Y_create_list()
{
	Ylist *yl = (Ylist *)calloc(1, sizeof(Ylist));
	return yl;
}

Ylist *Y_create_list2(Ylist_free_func freefunc)
{
	Ylist *yl = Y_create_list();
	yl->freefunc = freefunc;
	return yl;
}

void Y_delete_list(Ylist *yl)
{
	if (yl->array != NULL)
	{
		Y_list_clear(yl);
		free(yl->array);
	}
	free(yl);
}

void Y_list_foreach(Ylist *yl, Ylist_foreach_func ff, void *userdata)
{
	for (int i = 0; i < yl->length; i++)
	{
		ff(yl, yl->array[i], userdata);
	}
}

void Y_list_add(Ylist *yl, void *item)
{
	ensure_capacity(yl, yl->length + 1);

	yl->array[yl->length++] = item;
}

void *Y_list_clear(Ylist *yl)
{
	if (yl->freefunc != NULL && yl->length > 0)
	{
		Y_list_foreach(yl, foreach_free, NULL);
		memset(yl->array, 0, sizeof(void *) * yl->length);
		yl->length = 0;
	}
}

int Y_list_count(Ylist *yl)
{
	return yl->length;
}

int Y_list_contains(Ylist *yl, void *item)
{
	if (yl->length == 0)
	{
		return -1;
	}

	for (int i = 0; i < yl->length; i++)
	{
		if (yl->array[i] == item)
		{
			return i;
		}
	}

	return -1;
}

void Y_list_insert(Ylist *yl, int index, void *item)
{
	// 不能向末尾插入元素
	if (index > yl->length)
	{
		YLOGE(YTEXT("insertions at the end are legal"));
		return;
	}

	ensure_capacity(yl, yl->length + 1);

	move_right(yl, index);
	yl->array[index] = item;
	yl->length++;
}

void Y_list_remove(Ylist *yl, void *item, int free)
{
	int index = Y_list_contains(yl, item);
	if (index < 0)
	{
		// 元素不存在
		YLOGE(YTEXT("item not found"));
		return;
	}

	Y_list_removeat(yl, index, free);
}

void Y_list_removeat(Ylist *yl, int at, int free)
{
	if (at < 0 || at > yl->length)
	{
		YLOGE(YTEXT("index outof range"));
		return;
	}

	void *item = yl->array[at];

	if (free == 1 && yl->freefunc != NULL)
	{
		yl->freefunc(item);
	}

	move_left(yl, at);
	yl->length--;
}
