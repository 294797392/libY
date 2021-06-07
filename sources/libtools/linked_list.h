#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

#include "toolbase.h"

typedef struct linkeditem_s linkeditem;

struct linkeditem_s
{
	// 保存在item里的数据
	void *data;

	// 该节点的上一个节点
	linkeditem *prev;

	// 该节点的下一个节点
	linkeditem *next;
};

typedef struct linkedlist_s linkedlist;

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * 描述：
     * 释放data的函数
     */
	typedef void (*list_free_func)(void *data);

	typedef int (*list_foreach_action)(linkedlist *list, linkeditem *item, void *userdata);

	TOOLSAPI linkedlist *new_linkedlist();

	TOOLSAPI linkedlist *new_linkedlist2(list_free_func freefunc);

	TOOLSAPI void free_linkedlist(linkedlist *list);

	TOOLSAPI linkeditem *linkedlist_addlast(linkedlist *list, void *data);

	TOOLSAPI void *linkedlist_removelast(linkedlist *list);

	TOOLSAPI int linkedlist_count(linkedlist *list);

	TOOLSAPI void linkedlist_foreach(linkedlist *list, list_foreach_action action, void *userdata);

	TOOLSAPI void linkedlist_clear(linkedlist *list);

#ifdef __cplusplus
}
#endif

#endif