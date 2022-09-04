#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Y.h"
#include "Ylog.h"
#include "Ylist.h"
#include "Ythread.h"
#include "Yfactory.h"

typedef struct Ymodule_runtime_s
{
	// 模块定义
	Ymodule_manifest *manifest;
	Ymodule *module;
}Ymodule_runtime;

struct Yfactory_s
{
	// 所有模块实例列表
	Ylist *modules;

	// 用户设置的参数
	Yfactory_options *opts;

	// 异步初始化模块的线程
	Ythread *initial_thread;
};

static void foreach_module_manifest(Ylist *yl, void *item, void *userdata)
{
	Yfactory *factory = (Yfactory *)userdata;
	Ymodule_manifest *manifest = (Ymodule_manifest *)item;

	Ymodule_runtime *mrt = (Ymodule_runtime *)Ycalloc(1, sizeof(Ymodule_runtime));
	if(mrt->manifest->load_type == YMOD_LOAD_TYPE_EXTERNAL)
	{

	}
	else if(mrt->manifest->load_type == YMOD_LOAD_TYPE_INNER)
	{
	}
	else
	{
		return;
	}
}

static void initial_thread_entry(void *userdata)
{

}

Yfactory *Y_create_factory(Yfactory_options *opts)
{
	Yfactory *factory = (Yfactory *)Ycalloc(1, sizeof(Yfactory));
	factory->opts = opts;
	return factory;
}

void Y_setup_factory_async(Yfactory *factory, Ylist *modules)
{
	// 遍历模块定义并创建模块实例
	Y_list_foreach(factory->modules, foreach_module_manifest, factory);

	// 创建线程异步初始化
	factory->initial_thread = Y_create_thread(initial_thread_entry, factory);
}

void Y_delete_factory(Yfactory *yf)
{
}
