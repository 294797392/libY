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

#include <cJSON.h>

#include "Y.h"
#include "Yerrno.h"
#include "Ylog.h"
#include "Yqueue.h"
#include "Ybuffer_queue.h"
#include "Ythread.h"
#include "Yappender.h"
#include "Yfile.h"
#include "Ystring.h"
#include "Ypool.h"

#define DEFAULT_LEVEL				YLOG_LEVEL_DEBUG
#define DEFAULT_PATH				("Ylog.txt")
#define DEFAULT_SIZE				4194304
#define DEFAULT_FORMAT				("")
#define YMSG_POOL_SIZE				8192


typedef struct Ylog_s
{
	// 全局配置
	Ylogger_options *global_options;

	// 所有配置的appender
	Yappender *appenders[32];
	int num_appenders;	

	Ylogger *logger[256];

	Ypool *msg_pool;
	Ybuffer_queue *consume_queue;

	cJSON *config;
}Ylog;

struct Ylogger_s
{
	char name[64];
	Ylog *log;
};

extern Yappender Yappender_console;
static Yappender *appenders[] =
{
	&Yappender_console,
	NULL
};
static Ylog *log = NULL;

static void consume_log_queue_callback(void *userdata, void *element)
{
	Yobject *yo = (Yobject *)element;
	Ymsg *ymsg = (Ymsg *)Y_object_get_data(yo);

	for(int i = 0; i < log->num_appenders; i++)
	{
		Yappender *appender = log->appenders[i];
		appender->write(appender->context, ymsg);
	}

	Y_pool_recycle(yo);
}




static Ylogger_options *get_default_options()
{
	Ylogger_options *default_options = (Ylogger_options*)Ycalloc(1, sizeof(Ylogger_options));
	default_options->level = DEFAULT_LEVEL;
	Ystrcpy(default_options->path, DEFAULT_PATH, YARRAY_LENGTH(default_options->path));
	default_options->max_size_bytes = DEFAULT_SIZE;
	Ystrcpy(default_options->format, DEFAULT_FORMAT, YARRAY_LENGTH(default_options->format));
	return default_options;
}

static void init_options(Ylog *log, cJSON *json)
{
	log->global_options = get_default_options();

	cJSON *options = cJSON_GetObjectItem(json, "options");
	if(options == NULL)
	{
		// 如果没定义全局config，那么使用一个默认的值
		return;
	}

	cJSON *level = cJSON_GetObjectItem(options, "level");
	if(level != NULL)
	{
		log->global_options->level = level->valueint;
	}
}

static Yappender *get_appender(const char *type)
{
	size_t len = YARRAY_LENGTH(appenders);
	for(size_t i = 0; i < len; i++)
	{
		if(appenders[i] == NULL)
		{
			return NULL;
		}

		if(strcmp(appenders[i]->type, type) == 0)
		{
			return appenders[i];
		}
	}

	return NULL;
}

static void init_appenders(Ylog *log, cJSON *json)
{
	cJSON *appenders = cJSON_GetObjectItem(json, "appenders");
	if(appenders == NULL)
	{
		// 如果没定义appender，那么使用默认appender
		log->appenders[0] = &Yappender_console;
		log->num_appenders = 1;
		return;
	}

	int size = cJSON_GetArraySize(appenders);
	for(int i = 0; i < size; i++)
	{
		cJSON *appender = cJSON_GetArrayItem(appenders, i);
		cJSON *type = cJSON_GetObjectItem(appender, "type");
		Yappender *appender1 = get_appender(type->valuestring);
		if(appender1 == NULL)
		{
			printf("%s log appender not found\n", type->valuestring);
			continue;
		}
		appender1->context = appender1->open(appender);
		log->appenders[i] = appender1;
		log->num_appenders++;
	}
}





int Y_log_init(const char *config)
{
	if(log != NULL)
	{
		return YERR_SUCCESS;
	}

	char *bytes = NULL;
	uint64_t size = 0;
	int code = Y_file_readbytes(config, &bytes, &size);
	if(code != YERR_SUCCESS)
	{
		perror("init log failed");
		return code;
	}

	cJSON *json = cJSON_Parse(bytes);
	if(json == NULL)
	{
		//perror("invalid log.json format");
		return YERR_INVALID_JSON;
	}

	log = (Ylog*)Ycalloc(1, sizeof(Ylog));
	log->config = json;
	log->msg_pool = Y_create_pool(sizeof(Ymsg), YMSG_POOL_SIZE);

	// 先解析全局配置
	init_options(log, json);

	// 再解析appender配置
	init_appenders(log, json);

	// 启动日志队列
	log->consume_queue = Y_create_buffer_queue(NULL);
	Y_buffer_queue_start(log->consume_queue, 1, consume_log_queue_callback);

	return YERR_SUCCESS;
}

Ylogger *Y_log_get_logger(const char *name)
{
	Ylogger *logger = (Ylogger*)Ycalloc(1, sizeof(Ylogger));
	logger->log = log;
	Ystrcpy(logger->name, (char *)name, YARRAY_LENGTH(logger->name));
	return logger;
}

void Y_log_write(Ylogger *logger, Ylog_level level, int line, char *msg, ...)
{
	char message[MAX_MSG_SIZE1] = {'\0'};
	va_list ap;
	va_start(ap, msg);
	vsnprintf(message, MAX_MSG_SIZE1, msg, ap);
	va_end(ap);

	Yobject *yo = Y_pool_obtain(log->msg_pool, sizeof(Ymsg));
	Ymsg *ymsg = (Ymsg *)Y_object_get_data(yo);
	ymsg->level = level;

	// 格式化最终要输出的日志
	const char *format = "[%s][%d]%s\r\n\0";
	snprintf(ymsg->msg, sizeof(ymsg->msg), format, logger->name, line, message);

	ymsg->level = level;

	Y_buffer_queue_enqueue(log->consume_queue, yo);
}



