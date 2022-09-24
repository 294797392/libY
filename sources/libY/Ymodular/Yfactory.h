/***********************************************************************************
 * @ file    : Yfactory.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.08.31 19:33
 * @ brief   : ģ�鹤��
 * @ remark  ��
 ************************************************************************************/

#ifndef __YFACTORY_H__
#define __YFACTORY_H__

#include <cJSON.h>

#include "Y.h"
#include "Ylist.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct Yfactory_s Yfactory;
	typedef struct Yfactory_options_s Yfactory_options;
	typedef struct Ymodule_s Ymodule;
	typedef struct Ymodule_manifest_s Ymodule_manifest;
	typedef enum Ymodule_flags_e Ymodule_flags;
	typedef enum Ymodule_status_e Ymodule_status;

	typedef int(*Ymodule_initialize_entry)(Ymodule *module, void **context);
	typedef void(*Ymodule_release_entry)(Ymodule *module);

	typedef void(*Yfactory_initialized)(Yfactory *factory);
	typedef void(*Yfactory_module_status_changed)(Yfactory *factory, Ymodule *m, Ymodule_status status);

	// ģ���ѡ��
	enum Ymodule_flags_e {
		YMOD_FLAGS_NONE,
		YMOD_FLAGS_DISABLED
	};

	// ģ��״̬
	enum Ymodule_status_e {
		YMOD_STAT_UN_INITIALIZED,
		YMOD_STAT_INITIALIZED,
		YMOD_STAT_INIT_FAILED,
		YMOD_STAT_INITIALIZING,
		YMOD_STAT_INIT_EXCEPTION
	};

	struct Ymodule_manifest_s {
		YCHAR id[256];
		YCHAR name[256];
		YCHAR desc[256];
		YCHAR author[256];
		YCHAR lib_path[YMAX_PATH];
		int flags;
		char *init_entry;					// ��ڵ㺯����
		char *release_entry;				// �ͷź�����
		cJSON *input;
	};

	struct Yfactory_options_s
	{
		// ��ʼ����ɵĻص�
		Yfactory_initialized on_initialized;
		// ģ��״̬�����ı�Ļص�
		Yfactory_module_status_changed on_module_status_changed;

		// ��ĳ��ģ���ʼ��ʧ�ܵ�ʱ���Զ����Գ�ʼ���ļ��ʱ��
		// ��λ����
		int initial_interval;
	};

	/*
	 * ������
	 * ʹ��options����һ������
	 *
	 * ������
	 * @opts��ģ�鹤����ѡ��
	 *
	 * ����ֵ��
	 * Yfactoryʵ��
	 */
	YAPI Yfactory *Y_create_factory(Yfactory_options *opts);

	/*
	 * ������
	 * ʹ��һ��ģ���첽��ʼ��ģ�鹤��
	 *
	 * ������
	 * @modules��Ҫ���ص�ģ���б�, Ymodule_manifest�б�
	 *
	 * ����ֵ��
	 * Yfactoryʵ��
	 */
	YAPI int Y_setup_factory_async(Yfactory *factory, Ylist *manifests);

	YAPI int Y_setup_factory_async2(Yfactory *factory, const YCHAR *config_file);

	YAPI void Y_delete_factory(Yfactory *factory);


	// module�����ӿ�
	YAPI void *Y_module_context(Ymodule *module);
	YAPI int Y_module_config_get_int(Ymodule *module, const char *key, int defval);
	YAPI void Y_module_config_get_string(Ymodule *module, const char *key, char *buf, size_t bufsize, const char *defval);
	YAPI double Y_module_config_get_double(Ymodule *module, const char *key, double defval);

#ifdef __cplusplus
}
#endif

#endif
