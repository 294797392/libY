#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#include "cJSON.h"

#include "Y.h"
#include "Ylog.h"
#include "Ydll.h"
#include "Ylist.h"
#include "Yerrno.h"
#include "Yfile.h"
#include "Ythread.h"
#include "Yfactory.h"

struct Ymodule_s
{
	// 模块的输入参数
	cJSON *config;

	// 模块状态
	Ymodule_status status;

	// 回调函数
	Ymodule_initialize_entry initialize;
	Ymodule_release_entry release;

	// 模块内部使用的结构体
	void *context;

	// 模块定义
	Ymodule_manifest *manifest;

	// 模块所在的dll
	Ydll *dll;
};

struct Yfactory_s
{
	// 所有模块实例列表
	Ylist *manifests;

	// 所有模块的运行时列表
	Ylist *runtimes;

	// 用户设置的参数
	Yfactory_options *opts;

	// 异步初始化模块的线程
	Ythread *initial_thread;

	// 配置文件字节数组
	// 必须使用UTF8格式，因为使用cJSON去解析的，UTF8中使用一个字节来表示ASCII码和一些标点符号，所以cJSON应该是可以兼容UTF8的
	YBYTE *config;
};


/// <summary>
/// 把config转换成Ymanifest集合
/// </summary>
/// <param name="config_bytes"></param>
/// <returns></returns>
static int parse_config(const YBYTE *config_bytes, Ylist **list)
{
	cJSON *json = cJSON_Parse(config_bytes);
	if(json == NULL)
	{
		return YERR_INVALID_JSON;
	}

	Ylist *manifests = Y_create_list();

	cJSON *modules = cJSON_GetObjectItem(json, "modules");
	int module_count = cJSON_GetArraySize(modules);
	for(int i = 0; i < module_count; i++)
	{
		cJSON *module = cJSON_GetArrayItem(modules, i);
		cJSON *id = cJSON_GetObjectItem(module, "id");
		cJSON *name = cJSON_GetObjectItem(module, "name");
		cJSON *desc = cJSON_GetObjectItem(module, "desc");
		cJSON *flag = cJSON_GetObjectItem(module, "flag");
		cJSON *init_entry = cJSON_GetObjectItem(module, "init_entry");
		cJSON *release_entry = cJSON_GetObjectItem(module, "release_entry");
		cJSON *lib_path = cJSON_GetObjectItem(module, "lib_path");
		cJSON *input = cJSON_GetObjectItem(module, "input");

		// setlocale函数会对mbstowcs函数产生影响，相当于MultiBytesToWideChar的第一个参数
		char *locale = setlocale(LC_ALL, NULL);
		setlocale(LC_ALL, ".UTF8");
		Ymodule_manifest *manifest = (Ymodule_manifest *)Ycalloc(1, sizeof(Ymodule_manifest));
		mbstowcs(manifest->id, id->valuestring, strlen(id->valuestring));
		mbstowcs(manifest->name, name->valuestring, strlen(name->valuestring));
		mbstowcs(manifest->desc, desc->valuestring, strlen(desc->valuestring));
		mbstowcs(manifest->lib_path, lib_path->valuestring, strlen(lib_path->valuestring));
		manifest->flags = flag->valueint;
		manifest->init_entry = init_entry->valuestring;
		manifest->release_entry = release_entry->valuestring;
		manifest->input = input;
		setlocale(LC_ALL, locale);

		Y_list_add(manifests, manifest);
	}

	*list = manifests;
	return YERR_SUCCESS;
}

static void release_module(Ymodule *module)
{
	if(module != NULL)
	{
		Y_unload_dll(module->dll);
		module->dll = NULL;

		Yfree(module);
	}
}

/// <summary>
/// 创建模块实例，不初始化
/// </summary>
/// <param name="yl"></param>
/// <param name="item"></param>
/// <param name="userdata"></param>
static int foreach_create_module_from_manifest(Ylist *manifests, void *item, void *userdata)
{
	Yfactory *factory = (Yfactory *)userdata;
	Ymodule_manifest *manifest = (Ymodule_manifest *)item;
	
	// 创建实例
	Ymodule *module = (Ymodule *)Ycalloc(1, sizeof(Ymodule));

	// 首先加载对应的dll
	int code = Y_load_dll(module->manifest->lib_path, &module->dll);
	if(code != YERR_SUCCESS)
	{
		release_module(module);
		YLOGE(YTEXT("create module instance failed, load dll failed, %d"), code);
		return YERR_FAILED;
	}

	module->manifest = manifest;

	// 然后获取dll里的函数
	if((module->initialize = (Ymodule_initialize_entry)Y_load_symbol(module->dll, manifest->init_entry)) == NULL)
	{
		release_module(module);
		YLOGE(YTEXT("load initialize function failed, entry = %s"), manifest->init_entry);
		return YERR_FAILED;
	}

	if((module->release = (Ymodule_release_entry)Y_load_symbol(module->dll, manifest->release_entry)) == NULL)
	{
		release_module(module);
		YLOGE(YTEXT("load release function failed, entry = %s"), manifest->release_entry);
		return YERR_FAILED;
	}

	return YERR_SUCCESS;
}

static int foreach_init_module(Ylist *runtimes, void *item, void *userdata)
{
	Yfactory *factory = (Yfactory *)userdata;

	Yfactory_options *opts = factory->opts;
	Ymodule *module = (Ymodule *)item;
	Ymodule_manifest *manifest = module->manifest;

	while(1)
	{
		int code = module->initialize(module);
		if(code != YERR_SUCCESS)
		{
			YLOGE(YTEXT("initialize module failed, %s, code = %d"), manifest->name, code);
			if(opts->on_module_status_changed)
			{
				opts->on_module_status_changed(factory, module, YMOD_STAT_INIT_FAILED);
			}
			Ysleep(opts->initial_interval);
			continue;
		}

		YLOGI(YTEXT("initialize module success, %s"), manifest->name);
		if(opts->on_module_status_changed)
		{
			opts->on_module_status_changed(factory, module, YMOD_STAT_INITIALIZED);
		}

		break;
	}

	return YERR_SUCCESS;
}

/// <summary>
/// 对模块进行初始化操作
/// </summary>
/// <param name="userdata"></param>
static void initial_thread_entry(void *userdata)
{
	Yfactory *factory = (Yfactory *)userdata;
	Y_list_foreach(factory->runtimes, foreach_init_module, factory);

	// 到这里说明所有模块都初始化完了
	if(factory->opts->on_initialized)
	{
		factory->opts->on_initialized(factory);
	}
}





Yfactory *Y_create_factory(Yfactory_options *opts)
{
	Yfactory *factory = (Yfactory *)Ycalloc(1, sizeof(Yfactory));
	factory->opts = opts;
	factory->manifests = Y_create_list();
	factory->runtimes = Y_create_list();
	return factory;
}

int Y_setup_factory_async(Yfactory *factory, Ylist *manifests)
{
	factory->manifests = manifests;

	// 遍历模块定义并创建模块实例
	int rc = Y_list_foreach(factory->manifests, foreach_create_module_from_manifest, factory);
	if(rc != YERR_SUCCESS)
	{
		return rc;
	}

	// 创建线程异步初始化
	factory->initial_thread = Y_create_thread(initial_thread_entry, factory);

	return YERR_SUCCESS;
}

int Y_setup_factory_async2(Yfactory *factory, const YCHAR *config_file)
{
	YBYTE *bytes = NULL;
	uint64_t size = 0;
	int code = Y_file_readbytes(config_file, &bytes, &size);
	if(code != YERR_SUCCESS)
	{
		YLOGE(YTEXT("read config failed, %d, path = %s"), code, config_file);
		return code;
	}

	Ylist *manifests = NULL;
	if((code = parse_config(bytes, &manifests)) != YERR_SUCCESS)
	{
		YLOGE(YTEXT("parse manifest config failed, %d"), code);
		return code;
	}

	int count = Y_list_count(manifests);
	if(count == 0)
	{
		return YERR_SUCCESS;
	}

	return Y_setup_factory_async(factory, manifests);
}

void Y_delete_factory(Yfactory *yf)
{
}



void *Y_module_context(Ymodule *module)
{
	return module->context;
}

int Y_module_config_get_int(Ymodule *module, const char *key, int defval)
{
	cJSON *json = cJSON_GetObjectItem(module->config, key);
	if(json == NULL)
	{
		return defval;
	}

	return json->valueint;
}

void Y_module_config_get_string(Ymodule *module, const char *key, char *buf, size_t bufsize, const char *defval)
{
	cJSON *json = cJSON_GetObjectItem(module->config, key);
	if(json == NULL)
	{
		strncpy(buf, defval, bufsize);
		return;
	}

	strncpy(buf, json->valuestring, bufsize);
}

double Y_module_config_get_double(Ymodule *module, const char *key, double defval)
{
	cJSON *json = cJSON_GetObjectItem(module->config, key);
	if(json == NULL)
	{
		return defval;
	}

	return json->valuedouble;
}




