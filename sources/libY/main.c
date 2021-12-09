#include "Yfirstinclude.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef Y_API_WIN32
#include <Windows.h>
#elif Y_API_UNIX
#endif

#include "Ybase.h"
#include "Ylog.h"
#include "Yqueue.h"
#include "Ylist.h"
#include "Ypool.h"
#include "Ynet.h"
#include "Ytcpcli.h"

#define CATEGORY	YTEXT("main")

static void Yqueue_callback_handler(void *userdata, void *element)
{
	char *line = (char *)element;
	YLOGI(YTEXT("dequeue, %s"), line);
	free(line);
}

static void demo_Yqueue()
{
	Yqueue *q = Y_create_queue(NULL);
	Y_queue_start(q, Yqueue_callback_handler);

	while (1)
	{
		YLOGI(YTEXT("please input element:"));
		char *line = (char *)calloc(1, 1024);
		fgets(line, sizeof(line), stdin);
		Y_queue_enqueue(q, line);
	}
}

typedef struct Ylist_item_s
{
	void *ptr;
	void *ptr2;
	char *line;
}Ylist_item;

static void Ylist_free_item(void *item)
{
	free(item);
}

static void demo_Ylist()
{
	Ylist *yl = Y_create_list2(Ylist_free_item);

	for (size_t i = 0; i < 100; i++)
	{
		Ylist_item *item = (Ylist_item *)calloc(1, sizeof(Ylist_item));
		char *line = (char *)calloc(1, 1024);
		snprintf(line, 1024, "%d", i);
		item->line = line;
		Y_list_add(yl, item);
	}

	Ylist_item *item = (Ylist_item *)calloc(1, sizeof(Ylist_item));
	item->line = (char *)calloc(1, 1024);
	snprintf(item->line, 1024, "99999");
	Y_list_insert(yl, 0, item);
	Y_list_removeat(yl, 0, 1);
	Y_list_clear(yl);

	while (1)
	{
		char line[1024] = { '\0' };
		fgets(line, sizeof(line), stdin);
	}
}

static void demo_Ypool()
{
	Ypool *yp = Y_create_pool();

	Yobject *objs[10] = { NULL };

	// 先创建10个对象
	for (size_t i = 0; i < 10; i++)
	{
		Yobject *yo = Y_pool_obtain(yp);
		char *msg = (char *)calloc(1, 1024);
		snprintf(msg, 1024, "%d", i);
		Y_object_set_data(yo, msg);
		objs[i] = yo;
	}

	//for (size_t i = 5; i < 10; i++)
	//{
	//	Y_pool_recycle(yp, objs[i]);
	//}

	for (size_t i = 0; i < 10; i++)
	{
		Y_pool_recycle(yp, objs[i]);
	}


	for (size_t i = 0; i < 10; i++)
	{
		Yobject *yo = Y_pool_obtain(yp);
		char *msg = (char *)Y_object_get_data(yo);
		YLOGI(YTEXT("%s"), msg);
	}
}

static void demo_Ynet()
{
	Y_initnet();

	Ysocket s = Y_create_tcp_svc(NULL, 1018);
}


static void Ytcpcli_event_handler(Ytcpcli *ycli, Ytcpcli_event yevt, void *userdata)
{
	switch (yevt)
	{
	case Y_TCPCLI_EVT_CONNECTING:
	{
		YLOGCI(CATEGORY, YTEXT("connecting"));
		break;
	}

	case Y_TCPCLI_EVT_CONNECTED:
	{
		YLOGCI(CATEGORY, YTEXT("connected"));
		break;
	}

	case Y_TCPCLI_EVT_DISCONNECTED:
	{
		YLOGCI(CATEGORY, YTEXT("disconnect"));
		break;
	}

	case Y_TCPCLI_EVT_READ:
	{
		YLOGCI(CATEGORY, YTEXT("read"));
		char buf[2] = { '\0' };
		Y_tcpcli_recv(ycli, buf, 1);
		printf("%s\n", buf);
		break;
	}

	default:
		break;
	}
}

static void demo_Ytcpcli()
{
	Y_initnet();

	Ytcpcli *ycli = Y_create_tcpcli("127.0.0.1", 1018);
	Y_tcpcli_set_event_callback(ycli, Ytcpcli_event_handler, ycli);
	Y_tcpcli_connect(ycli);

	while (1)
	{
		char line[1024] = { '\0' };
		fgets(line, sizeof(line), stdin);
		Y_tcpcli_send(ycli, line, strlen(line) - 1);
		Y_tcpcli_send(ycli, "\r\n", 2);
	}
}

int main(int argc, char **argv)
{
	Y_log_global_init();

	//YLOGI(YTEXT("hello libY"));
	YLOGCI(CATEGORY, YTEXT("hello libY"));

	// demo_Yqueue();

	//demo_Ylist();

	//demo_Ypool();

	//demo_Ynet();

	demo_Ytcpcli();

	while (1)
	{
		char line[1024] = { '\0' };
		fgets(line, sizeof(line), stdin);
		YLOGCI(CATEGORY, YTEXT("your input is : %s"), line);
	}

	return 0;
}
