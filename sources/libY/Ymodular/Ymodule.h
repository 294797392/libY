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

	typedef int(*Ymodule_initialize_entry)();
	typedef void(*Ymodule_release_entry)();

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

	typedef struct Ymodule_manifest_s {
		char *id;
		char *name;
		char *desc;
		char *author;
		char *lib_path;
		int flags;
		char *init_entry;					// 入口点函数名
		char *release_entry;				// 释放函数名
	}Ymodule_manifest;

	// 模块对象
	typedef struct Ymodule_s {
		YCHAR *id;
		YCHAR *name;
		YCHAR *desc;
		Ymodule_manifest *manifest;
		// 模块的输入参数
		void *config;
		Ymodule_initialize_entry initialize;
		Ymodule_release_entry release;
		// 模块内部使用的结构体
		void *context;
	} Ymodule;

#ifdef __cplusplus
}
#endif

#endif