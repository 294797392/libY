#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

#include "toolbase.h"

typedef struct linkeditem_s linkeditem;

struct linkeditem_s
{
	void *data;
	linkeditem *prev;
	linkeditem *next;
};

typedef struct linkedlist_s linkedlist;

#ifdef __cplusplus
extern "C" {
#endif

	typedef int (*list_foreach_action)(void *data);

	TOOLSAPI linkedlist *new_linkedlist();

	TOOLSAPI void free_linkedlist(linkedlist *list);

	TOOLSAPI linkeditem *linkedlist_addlast(linkedlist *list, void *data);

	TOOLSAPI linkeditem *linkedlist_removelast(linkedlist *list);

	TOOLSAPI int linkedlist_count(linkedlist *list);

	TOOLSAPI void linkedlist_foreach(linkedlist *list, list_foreach_action action);

	TOOLSAPI void linkedlist_clear(linkedlist *list);

#ifdef __cplusplus
}
#endif

#endif