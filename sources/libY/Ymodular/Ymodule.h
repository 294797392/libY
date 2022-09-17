/***********************************************************************************
 * @ file    : Ymodule.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.08.31 19:16
 * @ brief   : ģ��
 * @ remark  ��
 ************************************************************************************/

#ifndef __YMODULE_H__
#define __YMODULE_H__

#include "cJSON.h"
#include "Y.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEF_STR_LEN			64

	typedef int(*Ymodule_initialize_entry)();
	typedef void(*Ymodule_release_entry)();

	// ģ���ѡ��
	typedef enum {
		YMOD_FLAGS_NONE,
		YMOD_FLAGS_DISABLED
	}Ymodule_flags;

	// ģ��״̬
	typedef enum {
		YMOD_STAT_UN_INITIALIZED,
		YMOD_STAT_INITIALIZED,
		YMOD_STAT_INIT_FAILED,
		YMOD_STAT_INITIALIZING,
		YMOD_STAT_INIT_EXCEPTION
	}Ymodule_status;

	typedef struct Ymodule_manifest_s {
		YCHAR id[DEF_STR_LEN];
		YCHAR name[DEF_STR_LEN];
		YCHAR desc[DEF_STR_LEN];
		YCHAR author[DEF_STR_LEN];
		YCHAR lib_path[YMAX_PATH];
		int flags;

		char *init_entry;					// ��ڵ㺯����
		char *release_entry;				// �ͷź�����

		cJSON *input;
	}Ymodule_manifest;

	// ģ�����
	typedef struct Ymodule_s {
		YCHAR *id;
		YCHAR *name;
		YCHAR *desc;
		Ymodule_manifest *manifest;
		// ģ����������
		void *config;
		Ymodule_initialize_entry initialize;
		Ymodule_release_entry release;
		// ģ���ڲ�ʹ�õĽṹ��
		void *context;
	} Ymodule;

#ifdef __cplusplus
}
#endif

#endif