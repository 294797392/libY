#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Y.h"
#include "Ylist.h"
#include "Ythread.h"
#include "Yfactory.h"

struct Yfactory_s
{
	// ����ģ��ʵ���б�
	Ylist *modules;

	// �û����õĲ���
	Yfactory_options *opts;

	// �첽��ʼ��ģ����߳�
	Ythread *initial_thread;
};

static void foreach_module_manifest(Ylist *yl, void *item, void *userdata)
{
	Yfactory *factory = (Yfactory *)userdata;
	Ymodule_manifest *manifest = (Ymodule_manifest *)item;

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
	// ����ģ�鶨�岢����ģ��ʵ��
	Y_list_foreach(factory->modules, foreach_module_manifest, factory);

	// �����߳��첽��ʼ��
	factory->initial_thread = Y_create_thread(initial_thread_entry, factory);
}

void Y_delete_factory(Yfactory *yf)
{
}
