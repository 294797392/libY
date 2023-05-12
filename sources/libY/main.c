#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#if (defined(Y_WIN32)) || (defined(Y_MINGW))
#include <Windows.h>
#elif (defined(Y_UNIX))
#endif

#include "libY.h"

// static void Yqueue_callback_handler(void *userdata, void *element)
// {
// 	char *line = (char *)element;
// 	printf("dequeue, %s\n", line);
// 	free(line);
// }

// static void demo_Ybuffer_queue()
// {
// 	Ybuffer_queue *q = Y_create_buffer_queue(NULL);
// 	Y_buffer_queue_start(q, 1, Yqueue_callback_handler);

// 	while (1)
// 	{
// 		printf("please input element:\n");
// 		char *line = (char *)calloc(1, 1024);
// 		fgets(line, sizeof(line), stdin);
// 		Y_buffer_queue_enqueue(q, line);
// 	}
// }

static void demo_Yqueue()
{
	Yqueue *q = Y_create_queue(NULL);

	for(int i = 0; i < 100; i++)
	{
		char *buf = (char *)calloc(1, 64);
		snprintf(buf, 64, "%d", i);
		Y_queue_enqueue(q, buf);
	}

	for(int i = 0; i < 100; i++)
	{
		char *buf = (char *)Y_queue_dequeue(q);
		printf("%s\n", buf);
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

	for(int i = 0; i < 100; i++)
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

	while(1)
	{
		char line[1024] = { '\0' };
		fgets(line, sizeof(line), stdin);
	}
}

static void demo_Ypool()
{
	Y_pool_init(1024, 1024);

	for (int i = 0; i < 10; i++)
	{
		void *block = Y_pool_obtain(1024);
		char *msg = (char *)block;
		snprintf(msg, 1024, "%d", i);
		YLOGI(("%s"), msg);
		Y_pool_recycle(block, 1024);
	}
}

static void demo_Ylog()
{
	while(1)
	{
		YLOGI(("test log, %d"), errno);
	}
}

typedef struct test_s
{
	void *p1;
	void *p2;
}test;

int main(int argc, char **argv)
{
	//char *content = NULL;
	//uint64_t size = 0;
	//Y_file_read_all("E:\\oheiheiheiheihei\\libY\\msvc\\build\\libY.pdb", &content, &size);

	Y_log_init(NULL);
	demo_Ypool();

	YLOGI(("123"));

	//for(size_t i = 0; i < 99999999; i++)
	//{
	//	YLOGD(("%d"), i);
	//	Ysleep(1);
	//}

	//demo_Yqueue();

	//char perm[1024] = {'\0'};
	//Y_file_read_linux_perm("/home/oheiheiheiheihei/code/oheiheiheiheihei/tools/cmake/Ydemo", perm);
	//printf("%s\n", perm);

	//YLOGI(("hello libY"));
	// YLOGCI(CATEGORY, ("hello libY"));

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
		YLOGI(("your input is : %s"), line);
	}

	return 0;
}
