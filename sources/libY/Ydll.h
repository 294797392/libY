/***********************************************************************************
 * @ file    : Ydll.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.09.04 13:32
 * @ brief   : ��װdll��صĲ���
 * @ remark  ��
 ************************************************************************************/

#ifndef __YDLL_H__
#define __YDLL_H__

#include "Y.h"
#include "Ylist.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct Ydll_s Ydll;

	/// <summary>
	/// ��̬����һ��dll
	/// </summary>
	/// <param name="dll_path">Ҫ���ص�dll��·��</param>
	/// <returns>dllʵ�����������ʧ���򷵻ؿ�</returns>
	YAPI int Y_load_dll(char *dll_path, Ydll **outdll);

	/// <summary>
	/// �ͷ�һ�����ص�dll
	/// </summary>
	/// <param name="dll"></param>
	/// <returns></returns>
	YAPI void Y_unload_dll(Ydll *dll);

	YAPI void *Y_load_symbol(Ydll *dll, const char *symbol);

#ifdef __cplusplus
}
#endif

#endif