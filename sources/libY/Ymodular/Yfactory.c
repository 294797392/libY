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
	// ģ����������
	cJSON *config;

	// ģ��״̬
	Ymodule_status status;

	// �ص�����
	Ymodule_initialize_entry initialize;
	Ymodule_release_entry release;

	// ģ���ڲ�ʹ�õĽṹ��
	void *context;

	// ģ�鶨��
	Ymodule_manifest *manifest;

	// ģ�����ڵ�dll
	Ydll *dll;
};

struct Yfactory_s
{
	// ����ģ��ʵ���б�
	Ylist *manifests;

	// ����ģ�������ʱ�б�
	Ylist *runtimes;

	// �û����õĲ���
	Yfactory_options *opts;

	// �첽��ʼ��ģ����߳�
	Ythread *initial_thread;

	// �����ļ��ֽ�����
	// ����ʹ��UTF8��ʽ����Ϊʹ��cJSONȥ�����ģ�UTF8��ʹ��һ���ֽ�����ʾASCII���һЩ�����ţ�����cJSONӦ���ǿ��Լ���UTF8��
	YBYTE *config;
};


/// <summary>
/// ��configת����Ymanifest����
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

		// setlocale�������mbstowcs��������Ӱ�죬�൱��MultiBytesToWideChar�ĵ�һ������
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
/// ����ģ��ʵ��������ʼ��
/// </summary>
/// <param name="yl"></param>
/// <param name="item"></param>
/// <param name="userdata"></param>
static int foreach_create_module_from_manifest(Ylist *manifests, void *item, void *userdata)
{
	Yfactory *factory = (Yfactory *)userdata;
	Ymodule_manifest *manifest = (Ymodule_manifest *)item;
	
	// ����ʵ��
	Ymodule *module = (Ymodule *)Ycalloc(1, sizeof(Ymodule));

	// ���ȼ��ض�Ӧ��dll
	int code = Y_load_dll(module->manifest->lib_path, &module->dll);
	if(code != YERR_SUCCESS)
	{
		release_module(module);
		YLOGE(YTEXT("create module instance failed, load dll failed, %d"), code);
		return YERR_FAILED;
	}

	module->manifest = manifest;

	// Ȼ���ȡdll��ĺ���
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
/// ��ģ����г�ʼ������
/// </summary>
/// <param name="userdata"></param>
static void initial_thread_entry(void *userdata)
{
	Yfactory *factory = (Yfactory *)userdata;
	Y_list_foreach(factory->runtimes, foreach_init_module, factory);

	// ������˵������ģ�鶼��ʼ������
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

	// ����ģ�鶨�岢����ģ��ʵ��
	int rc = Y_list_foreach(factory->manifests, foreach_create_module_from_manifest, factory);
	if(rc != YERR_SUCCESS)
	{
		return rc;
	}

	// �����߳��첽��ʼ��
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




