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
	// 模块定义
	Ymodule_manifest *manifest;

	// 模块实例
	Ymodule *module;

	// 模块所在的dll
	Ydll *dll;
}Ymodule_runtime;

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
/// 创建模块实例，不初始化
/// </summary>
/// <param name="yl"></param>
/// <param name="item"></param>
/// <param name="userdata"></param>
static int foreach_create_module_from_manifest(Ylist *manifests, void *item, void *userdata)
{
	Yfactory *factory = (Yfactory *)userdata;
	Ymodule_manifest *manifest = (Ymodule_manifest *)item;
	Ymodule_runtime *mrt = (Ymodule_runtime *)Ycalloc(1, sizeof(Ymodule_runtime));

	// 首先加载对应的dll
	int code = Y_load_dll(mrt->manifest->lib_path, &mrt->dll);
	if(code != YERR_SUCCESS)
	{
		release_module_runtime(mrt);
		YLOGE(YTEXT("create module instance failed, load dll failed, %d"), code);
		return YERR_FAILED;
	}

	// 创建实例
	mrt->module = (Ymodule *)Ycalloc(1, sizeof(Ymodule));
	mrt->module->manifest = manifest;

	// 然后获取dll里的函数
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
	FILE *file = Y_file_open(config_file, "r");
}

void Y_delete_factory(Yfactory *yf)
{
}