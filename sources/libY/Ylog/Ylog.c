﻿#include <stdlib.h>
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

#include "Yappender.h"
#include "libY.h"

#define DEFAULT_LEVEL				YLOG_LEVEL_DEBUG
#define DEFAULT_PATH				("Ylog.txt")
#define DEFAULT_SIZE				4194304
#define DEFAULT_FORMAT				("")

// Ylog是否已经初始化
static int initialized = 0;

typedef struct Ylog_s
{
	// 全局配置
	Ylogger_options *global_options;

	// 所有配置的appender
	Yappender *appenders[32];
	int num_appenders;	

	Ylogger *logger[256];

	cJSON *config;

	/// <summary>
	/// 写日志的锁
	/// </summary>
	Ylock write_lock;
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
static Ylog *Ylog_instance = NULL;




static Ylogger_options *get_default_options()
{
	Ylogger_options *default_options = (Ylogger_options*)calloc(1, sizeof(Ylogger_options));
	default_options->level = DEFAULT_LEVEL;
	// default_options->path = default_options->path;
	default_options->max_size_bytes = DEFAULT_SIZE;
	// default_options->format = default_options->format;
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
	size_t len = sizeof(appenders) / sizeof(void*);
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
	if(Ylog_instance != NULL)
	{
		return YERR_SUCCESS;
	}

	//char *bytes = NULL;
	//uint64_t size = 0;
	//int code = Y_file_readbytes(config, &bytes, &size);
	//if(code != YERR_SUCCESS)
	//{
	//	perror("init log failed");
	//	return code;
	//}

	//cJSON *json = cJSON_Parse(bytes);
	//if(json == NULL)
	//{
	//	//perror("invalid log.json format");
	//	return YERR_INVALID_JSON;
	//}

	Ylog_instance = (Ylog*)calloc(1, sizeof(Ylog));
	Y_create_lock(Ylog_instance->write_lock);
	//log->config = json;

	//// 先解析全局配置
	//init_options(log, json);

	//// 再解析appender配置
	//init_appenders(log, json);
	Yappender *appender1 = get_appender("console");
	Ylog_instance->appenders[0] = appender1;
	Ylog_instance->num_appenders = 1;

	return YERR_SUCCESS;
}

Ylogger *Y_log_get_logger(const char *name)
{
	Ylogger *logger = (Ylogger*)calloc(1, sizeof(Ylogger));
	logger->log = Ylog_instance;
	return logger;
}

void Y_log_write(Ylogger *logger, Ylog_level level, int line, char *msg, ...)
{
	if(initialized == 0)
	{
		initialized = 1;
		Y_log_init(NULL);
	}

	char message[MAX_LOG_LINE] = {'\0'};
	va_list ap;
	va_start(ap, msg);
	vsnprintf(message, MAX_LOG_LINE, msg, ap);
	va_end(ap);

	Ymsg ymsg;
	ymsg.level = level;

	// 格式化最终要输出的日志
	const char *format = "[%s][%d]%s\r\n\0";
	snprintf(ymsg.msg, sizeof(ymsg.msg), format, logger->name, line, message);

	Y_lock_lock(Ylog_instance->write_lock);
	for(int i = 0; i < Ylog_instance->num_appenders; i++)
	{
		Yappender *appender = Ylog_instance->appenders[i];
		appender->write(appender->context, &ymsg);
	}
	Y_lock_unlock(Ylog_instance->write_lock);
}



