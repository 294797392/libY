#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <wchar.h>
#include <locale.h>

#if (defined(Y_WIN32))
#include <Windows.h>
#elif (defined(Y_UNIX))
#endif

#include "Y.h"
#include "Yerrno.h"
#include "Ylog.h"
#include "Yqueue.h"
#include "Ythread.h"
#include "Yappender.h"

static Yqueue *consume_log_queue = NULL;

static int num_appender = 0;
extern Yappender Yappender_console;
static Yappender *appenders[32] = { NULL };

static void consume_log_queue_callback(void *userdata, void *element)
{
	Ymsg *ymsg = (Ymsg *)element;

	for(int i = 0; i < num_appender; i++)
	{
		Yappender *appender = appenders[i];
		appender->write(appender->context, ymsg);
	}
}

int Y_log_init()
{
	// 初始化appender
	num_appender = 1;
	appenders[0] = &Yappender_console;

	// 启动appender
	for(int i = 0; i < num_appender; i++)
	{
		Yappender *appender = appenders[i];
		appender->context = appender->open("");
	}

	// 启动日志队列
	consume_log_queue = Y_create_queue(NULL, sizeof(Ymsg));
	Y_queue_start(consume_log_queue, 1, consume_log_queue_callback);

	return YERR_SUCCESS;
}

void Y_log_write(const YCHAR *category, Ylog_level level, int line, const YCHAR *msg, ...)
{
	// 格式化用户输入的日志
	YCHAR message[MAX_MSG_SIZE1] = { '\0' };
	va_list ap;
	va_start(ap, msg);
#ifdef UNICODE
	vswprintf(message, MAX_MSG_SIZE1, msg, ap);
#else
	vsnprintf(message, MAX_MSG_SIZE1, msg, ap);
#endif
	va_end(ap);

	Ymsg *ymsg = (Ymsg *)Y_queue_prepare_enqueue(consume_log_queue);

	// 格式化最终要输出的日志
	const char *format = "[%s][%d]%s\r\n\0";
#ifdef UNICODE
	// 如果是Unicode字符把Unicode转成多字节字符输出
	char *locale = setlocale(LC_ALL, NULL);
	setlocale(LC_ALL, ".ACP");
	
	char mbsmsg[MAX_MSG_SIZE2] = { '\0' };
	wcstombs(mbsmsg, message, sizeof(mbsmsg) - 1);

	char mbscate[1024] = { '\0' };
	wcstombs(mbscate, category, sizeof(mbscate) - 1);
	
	setlocale(LC_ALL, locale);
	snprintf(ymsg->msg, sizeof(ymsg->msg), format, mbscate, line, mbsmsg);
#else
	snprintf(ymsg->msg, sizeof(ymsg->msg), format, cateogrty, line, message);
#endif

	ymsg->level = level;

	Y_queue_commit_enqueue(consume_log_queue);
}


