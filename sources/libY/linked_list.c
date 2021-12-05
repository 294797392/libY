#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "linked_list.h"

struct linkedlist_s
{
	linkeditem *first;
	linkeditem *last;
	int num;

	list_free_func free;
};

linkedlist *new_linkedlist()
{
	linkedlist *list = (linkedlist *)calloc(1, sizeof(linkedlist));

	return list;
}

linkedlist *new_linkedlist2(list_free_func freefunc)
{
	linkedlist *list = new_linkedlist();
	list->free = freefunc;
	return list;
}

void free_linkedlist(linkedlist *list)
{
	if (list)
	{
		free(list);
	}
}

linkeditem *linkedlist_addlast(linkedlist *list, void *data)
{
	linkeditem *item = (linkeditem *)calloc(1, sizeof(linkeditem));
	item->data = data;

	if (list->first == NULL)
	{
		list->first = item;
	}
	else
	{
		list->last->next = item;
		item->prev = list->last;
	}

	list->last = item;
	list->num += 1;

	return item;
}

void *linkedlist_removelast(linkedlist *list)
{
	if (list->first == NULL)
	{
		return NULL;
	}

	linkeditem *last = list->last;

	// 如果第一个元素和最后一个元素一样，那么说明只有一个元素
	if (last == list->first)
	{
		void *data = last->data;
		list->first = NULL;
		list->last = NULL;
		list->num = 0;
		free(last);
		return data;
	}

	// 把最后一个元素设置为最后一个元素的上一个元素
	list->last = last->prev;
	list->last->next = NULL;
	list->num -= 1;

	void *data = last->data;
	free(last);

	return data;
}

int linkedlist_count(linkedlist *list)
{
	return list->num;
}

void linkedlist_foreach(linkedlist *list, list_foreach_action action, void *userdata)
{
	if (list->num == 0)
	{
		return;
	}

	linkeditem *current = list->first;

	while (current)
	{
		action(list, current, userdata);

		current = current->next;
	}
}

void linkedlist_clear(linkedlist *list)
{
	linkeditem *current = list->first;

	while (current)
	{
		linkeditem *next = current->next;

		if(list->free)
		{
			list->free(current->data);			
		}

		free(current);

		current = next;
	}

	memset(list, 0, sizeof(linkedlist));
}



