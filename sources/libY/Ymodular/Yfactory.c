#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "Y.h"
#include "Ylog.h"
#include "Ydll.h"
#include "Ylist.h"
#include "Yerrno.h"
#include "Yfile.h"
#include "Ythread.h"
#include "Yfactory.h"

typedef struct Ymodule_runtime_s
{
	// ģ�鶨��
	Ymodule_manifest *manifest;

	// ģ��ʵ��
	Ymodule *module;

	// ģ�����ڵ�dll
	Ydll *dll;
}Ymodule_runtime;

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
};


static int parse_config(const char *config, Ymodule_manifest **manifests)
{
}

static void release_module_runtime(Ymodule_runtime *runtime)
{
	if(runtime->dll != NULL)
	{
		Y_unload_dll(runtime->dll);
		runtime->dll = NULL;
	}

	if(runtime->module != NULL)
	{
		Yfree(runtime->module);
		runtime->module = NULL;
	}

	Yfree(runtime);
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
	Ymodule_runtime *mrt = (Ymodule_runtime *)Ycalloc(1, sizeof(Ymodule_runtime));

	// ���ȼ��ض�Ӧ��dll
	int code = Y_load_dll(mrt->manifest->lib_path, &mrt->dll);
	if(code != YERR_SUCCESS)
	{
		release_module_runtime(mrt);
		YLOGE(YTEXT("create module instance failed, load dll failed, %d"), code);
		return YERR_FAILED;
	}

	// ����ʵ��
	mrt->module = (Ymodule *)Ycalloc(1, sizeof(Ymodule));
	mrt->module->manifest = manifest;

	// Ȼ���ȡdll��ĺ���
	if((mrt->module->initialize = (Ymodule_initialize_entry)Y_load_symbol(mrt->dll, manifest->init_entry)) == NULL)
	{
		release_module_runtime(mrt);
		YLOGE(YTEXT("load initialize function failed, entry = %s"), manifest->init_entry);
		return YERR_FAILED;
	}

	if((mrt->module->release = (Ymodule_release_entry)Y_load_symbol(mrt->dll, manifest->release_entry)) == NULL)
	{
		release_module_runtime(mrt);
		YLOGE(YTEXT("load release function failed, entry = %s"), manifest->release_entry);
		return YERR_FAILED;
	}

	return YERR_SUCCESS;
}

static int foreach_init_module(Ylist *runtimes, void *item, void *userdata)
{
	Yfactory *factory = (Yfactory *)userdata;

	Yfactory_options *opts = factory->opts;
	Ymodule_runtime *mrt = (Ymodule_runtime *)item;
	Ymodule_manifest *manifest = mrt->manifest;
	Ymodule *module = mrt->module;

	while(1)
	{
		int code = module->initialize();
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
	FILE *file = Y_file_open(config_file, "r");
}

void Y_delete_factory(Yfactory *yf)
{
}