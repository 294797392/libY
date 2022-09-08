/***********************************************************************************
 * @ file    : Yfile.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.09.09 06:43
 * @ brief   : ��װ�ļ����������ݿ��ַ��͵��ֽ��ַ�
 * @ remark  ��
 ************************************************************************************/

#ifndef __YFILE_H__
#define __YFILE_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Y.h"

#ifdef __cplusplus
extern "C" {
#endif

	YAPI FILE *Y_file_open(const YCHAR *file_path, const char *mode);

	YAPI void Y_file_close(FILE *f);

	YAPI size_t Y_file_size(const YCHAR *file_path);

#ifdef __cplusplus
}
#endif

#endif