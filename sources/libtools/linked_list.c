#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "linked_list.h"

struct linkedlist_s
{
	linkeditem *first;
	linkeditem *last;
	int num;
};

linkedlist *new_linkedlist()
{
	linkedlist *list = (linkedlist *)calloc(1, sizeof(linkedlist));

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
}

linkeditem *linkedlist_removelast(linkedlist *list)
{
	if (list->first == NULL)
	{
		return NULL;
	}

	linkeditem *last = list->last;

	// 如果第一个元素和最后一个元素一样，那么说明只有一个元素
	if (last == list->first)
	{
		list->first = NULL;
		list->last = NULL;
		return last;
	}

	// 把最后一个元素设置为最后一个元素的上一个元素
	list->last = last->prev;
	list->last->next = NULL;

	list->num -= 1;

	return last;
}

int linkedlist_count(linkedlist *list)
{
	return list->num;
}

void linkedlist_foreach(linkedlist *list, list_foreach_action action)
{
	if (list->num == 0)
	{
		return;
	}

	linkeditem *current = list->first;

	while (current)
	{
		action(current->data);

		current = current->next;
	}
}

void linkedlist_clear(linkedlist *list)
{
	memset(list, 0, sizeof(linkedlist));
}



