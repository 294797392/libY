/***********************************************************************************
 * @ file    : Yfile.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.09.09 06:43
 * @ brief   : 封装文件操作，兼容宽字符和单字节字符
 * @ remark  ：
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

	/*
	 * 描述：
	 * 以字节为单位返回文件大小
	 *
	 * 参数：
	 * @file_path：要读取的文件的完整路径
	 *
	 * 返回值：
	 * 文件大小，以字节为单位
	 */
	YAPI size_t Y_file_size(const YCHAR *file_path);


    /*
	 * 描述：
	 * 读取一个文件里的所有内容
	 *
	 * 参数：
	 * @file_path：要读取的文件的完整路径
	 * @content：保存文件内容
	 * 
	 * 返回值：
	 * 文件内容的长度
	 */
	YAPI int Y_file_readall(const YCHAR *file_path, YCHAR **content);

	/*
	 * 描述：
	 * 释放使用Y_file_readall函数开辟的内存空间
	 *
	 * 参数：
	 * @content：要释放的内存空间
	 */
	YAPI void Y_file_free(const YCHAR *content);

#ifdef __cplusplus
}
#endif

#endif