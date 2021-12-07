#include "Yfirstinclude.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "Yerrno.h"
#include "Ybase.h"
#include "Yqueue.h"
#include "Ythread.h"
#include "Ypool.h"
#include "Ylogbase.h"
#include "Yappender.h"

static Yqueue *consume_log_queue = NULL;
static Ypool *pool = NULL;

static int num_appender = 0;
extern Yappender Yappender_console;
static Yappender *appenders[32] = { NULL };

static void consume_log_queue_callback(void *userdata, void *element)
{
	Yobject *yo = (Yobject *)element;
	const Ymsg *ymsg = (const Ymsg *)Y_object_get_data(yo);

	for (int i = 0; i < num_appender; i++)
	{
		Yappender *appender = appenders[i];
		appender->write(appender->context, ymsg);
	}

	memset(ymsg->msg, '\0', sizeof(ymsg->msg));
	Y_pool_recycle(pool, yo);
}

int Ylog_global_init()
{
	pool = Y_create_pool();

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
	Y_queue_start(consume_log_queue, consume_log_queue_callback);

	return YERR_OK;
}

void Ylog_write(const char *category, Ylog_level level, const char *msg, ...)
{
	Yobject *obj = Y_pool_obtain(pool);
	Ymsg *ymsg = (Ymsg *)Y_object_get_data(obj);
	if (ymsg == NULL)
	{
		ymsg = (Ymsg *)calloc(1, sizeof(Ymsg));
		Y_object_set_data(obj, ymsg);
	}

	// 格式化用户输入的日志
	char message[MAX_MSG_SIZE] = { '\0' };
	va_list ap;
	va_start(ap, msg);
	vsnprintf(message, sizeof(message), msg, ap);
	va_end(ap);

	// 格式化最终要输出的日志
	ymsg->level = level;
	/*snprintf(ymsg->msg, sizeof(ymsg->msg), "[%s][%s]%s\r\n", __FILE__, __LINE__, message);*/
	snprintf(ymsg->msg, sizeof(ymsg->msg), "[%s][%d]%s\r\n", __FILE__, __LINE__, message);

	Y_queue_enqueue(consume_log_queue, obj);
}

