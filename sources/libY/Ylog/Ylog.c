#include "Yfirstinclude.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <wchar.h>

#ifdef Y_API_WIN32
#include <Windows.h>
#elif Y_API_UNIX
#endif

#include "Yerrno.h"
#include "Ylog.h"
#include "Ybase.h"
#include "Yqueue.h"
#include "Ythread.h"
#include "Ylogbase.h"
#include "Yappender.h"

static Yqueue *consume_log_queue = NULL;

static int num_appender = 0;
extern Yappender Yappender_console;
static Yappender *appenders[32] = { NULL };

static void consume_log_queue_callback(void *userdata, void *element)
{
	Ymsg *ymsg = (Ymsg *)element;

	for (int i = 0; i < num_appender; i++)
	{
		Yappender *appender = appenders[i];
		appender->write(appender->context, ymsg);
	}
}

int Y_log_global_init()
{
	// 初始化appender
	num_appender = 1;
	appenders[0] = &Yappender_console;

	// 启动appender
	for (int i = 0; i < num_appender; i++)
	{
		Yappender *appender = appenders[i];
		appender->context = appender->open("");
	}

	// 启动日志队列
	consume_log_queue = Y_create_queue(NULL);
	Y_queue_set_itemsize(consume_log_queue, sizeof(Ymsg));
	Y_queue_start(consume_log_queue, consume_log_queue_callback);

	return YERR_OK;
}

void Y_log_write(const wchar_t *category, Ylog_level level, int line, const wchar_t *msg, ...)
{
	// 格式化用户输入的日志
	wchar_t message[MAX_MSG_SIZE] = { '\0' };
	va_list ap;
	va_start(ap, msg);
	vswprintf(message, MAX_MSG_SIZE, msg, ap);
	va_end(ap);

	Ymsg *ymsg = (Ymsg *)Y_queue_begin_enqueue(consume_log_queue);

	// 格式化最终要输出的日志
	// 注意宽字符串需要用%ls输出，输出单个宽字符使用%lc

	if (category == NULL)
	{
#ifdef Y_ENV_MINGW
		// mingw环境下没法把__FILE__预定义宏转成多字节字符，暂时先直接使用单字节字符输出
		const wchar_t *format = YTEXT("[%s][%d]%ls\r\n\0");
		swprintf(ymsg->msg, MAX_MSG_SIZE, format, __FILE__, line , message);
#else
		const wchar_t *format = YTEXT("[%ls][%d]%ls\r\n\0");
		swprintf(ymsg->msg, MAX_MSG_SIZE, format, YTEXT(__FILE__), line , message);
#endif
	}
	else
	{
		const wchar_t *format = YTEXT("[%ls][%d]%ls\r\n\0");
		swprintf(ymsg->msg, MAX_MSG_SIZE, format, category, line, message);
	}

	ymsg->level = level;

	Y_queue_end_enqueue(consume_log_queue);
}


