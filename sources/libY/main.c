#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#if (defined(Y_WIN32)) || (defined(Y_MINGW))
#include <Windows.h>
#elif (defined(Y_UNIX))
#endif

#include "Ylog.h"
#include "Yqueue.h"
#include "Ylist.h"
#include "Ypool.h"
#include "Ythread.h"

static void Yqueue_callback_handler(void *userdata, void *element)
{
	char *line = (char *)element;
	printf("dequeue, %s\n", line);
	free(line);
}

static void demo_Yqueue()
{
	Yqueue *q = Y_create_queue(NULL, 1024);
	Y_queue_start(q, 1, Yqueue_callback_handler);

	while (1)
	{
		printf("please input element:\n");
		char *line = (char*)Y_queue_prepare_enqueue(q);
		fgets(line, sizeof(line), stdin);
		Y_queue_commit_enqueue(q);
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

	for (int i = 0; i < 100; i++)
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

	for (int i = 0; i < 10; i++)
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

static void demo_Ylog()
{
	while (1)
	{
		YLOGI(YTEXT("test log, %d"), errno);
	}
}

int main(int argc, char **argv)
{
	Y_log_init(YTEXT("E:\\oheiheiheiheihei\\libY\\msvc\\Debug\\Ylog.json"));

	YLOGD(YTEXT("你好"));

	//char perm[1024] = {'\0'};
	//Y_file_read_linux_perm("/home/oheiheiheiheihei/code/oheiheiheiheihei/tools/cmake/Ydemo", perm);
	//printf("%s\n", perm);

	//YLOGI(YTEXT("hello libY"));
	// YLOGCI(CATEGORY, YTEXT("hello libY"));

	// demo_Yqueue();

	//demo_Ylist();

	//demo_Ypool();

	//demo_Ynet();

	//Y_create_thread(Ytcpsvc_thread_proc, NULL);
	//Y_create_thread(Ytcpcli_thread_proc, NULL);

	// demo_Ylog();

	// int num_line;
	// char **lines = Y_file_read_lines("E:\\oheiheiheiheihei\\tools\\msvc\\Debug\\CMakeCache.txt", &num_line);
	// for(int i = 0; i<num_line;i++)
	// {
	// 	printf("%s\n", lines[i]);
	// 	fflush(stdout);
	// }

	//char buf[1024] = { '\0' };
	//FILE *f = fopen("e://2.txt", "r");
	//fread(buf, 1, sizeof(buf), f);
	//fclose(f);

	//wchar_t widechar[1024] = { '\0' };
	//MultiByteToWideChar(CP_UTF8, NULL, buf, -1, widechar, 1024);

	//char buf1[1024] = { '\0' };
	//WideCharToMultiByte(936, NULL, widechar, -1, buf1, sizeof(buf1), '\0', NULL);

	//printf("%s\n", buf1);

	//wchar_t *t = L"你好";
	//WideCharToMultiByte(CP_UTF8, NULL, t, -1, buf, sizeof(buf), '\0', NULL);

	// 指定转换后的多字节序列是GB2312编码的字节序列
	// GB2312字节序列和终端使用的字节序列相同，所以可以显示正确的中文
	// WideCharToMultiByte(936, NULL, t, -1, buf, sizeof(buf), '\0', NULL);

	//printf("%s\n", buf);

	while (1)
	{
		char line[1024] = { '\0' };
		fgets(line, sizeof(line), stdin);
		YLOGI(YTEXT("your input is : %s"), line);
	}

	return 0;
}
