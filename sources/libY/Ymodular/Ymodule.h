/***********************************************************************************
 * @ file    : Ymodule.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.08.31 19:16
 * @ brief   : 模块
 * @ remark  ：
 ************************************************************************************/

#ifndef __YMODULE_H__
#define __YMODULE_H__

#include "Y.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_STRING_SIZE                 2048

	// 模块的选项
	typedef enum {
		YMOD_FLAGS_NONE,
		YMOD_FLAGS_DISABLED
	}Ymodule_flags;

	// 模块状态
	typedef enum {
		YMOD_STAT_UN_INITIALIZED,
		YMOD_STAT_INITIALIZED,
		YMOD_STAT_INIT_FAILED,
		YMOD_STAT_INITIALIZING,
		YMOD_STAT_INIT_EXCEPTION
	}Ymodule_status;

	// 模块加载方式
	typedef enum {
		YMOD_LOAD_TYPE_INNER,                   // 模块就在进程内部
		YMOD_LOAD_TYPE_EXTERNAL                 // 从外部dll里加载一个模块
	}Ymodule_load_type;

	typedef struct Ymodule_manifest_s {
		char *id;
		char *description;
		char *lib_path;
		int flags;
		int load_type;
	}Ymodule_manifest;

	// 模块对象
	typedef struct Ymodule_s {
		YCHAR *id;
		YCHAR *name;
		YCHAR *desc;

		// 模块的输入参数
		void *config;
		int(*initialize)();
		void(*release)();
	} Ymodule;

	YAPI Ymodule *Y_create_module();

	/*
	 * 描述：
	 * 删除一个事件
	 *
	 * 参数：
	 * @ye：要删除的事件
	 */
	YAPI void Y_delete_module(Ymodule *ym);

#ifdef __cplusplus
}
#endif

#endif