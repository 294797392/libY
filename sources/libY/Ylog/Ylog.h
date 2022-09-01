/***********************************************************************************
 * @ file    : Ylog.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2021.12.06 13:15
 * @ brief   : 一个简易的日志记录器，支持多线程
 * @ remark  : wprintf and printf cannot use in same env
 ************************************************************************************/

#ifndef __YLOG_H__
#define __YLOG_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "Y.h"

#define MAX_MSG_SIZE        4096

 // 内部使用，调用者用不到这个枚举
typedef enum Ylog_level_e
{
	YLOG_LEVEL_DEBUG,
	YLOG_LEVEL_INFO,
	YLOG_LEVEL_WARN,
	YLOG_LEVEL_ERROR
}Ylog_level;

typedef struct Ymsg_s
{
	Ylog_level level;
	YCHAR msg[MAX_MSG_SIZE];
}Ymsg;

#define YLOGD(...) Y_log_write(NULL, YLOG_LEVEL_DEBUG, __LINE__, __VA_ARGS__)
#define YLOGI(...) Y_log_write(NULL, YLOG_LEVEL_INFO, __LINE__, __VA_ARGS__)
#define YLOGE(...) Y_log_write(NULL, YLOG_LEVEL_ERROR, __LINE__, __VA_ARGS__)
#define YLOGW(...) Y_log_write(NULL, YLOG_LEVEL_WARN, __LINE__, __VA_ARGS__)

#define YLOGCD(cate, ...) Y_log_write(cate, YLOG_LEVEL_DEBUG, __LINE__, __VA_ARGS__)
#define YLOGCI(cate, ...) Y_log_write(cate, YLOG_LEVEL_INFO, __LINE__, __VA_ARGS__)
#define YLOGCE(cate, ...) Y_log_write(cate, YLOG_LEVEL_ERROR, __LINE__, __VA_ARGS__)
#define YLOGCW(cate, ...) Y_log_write(cate, YLOG_LEVEL_WARN, __LINE__, __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

	/*
	 * 描述：
	 * Ylog全局初始化函数，该函数在整个应用程序里只调用一次
	 *
	 * 返回值：
	 * YERRNO
	 */
	YAPI int Y_log_init();

	YAPI void Y_log_write(const YCHAR *category, Ylog_level level, int line, const YCHAR *msg, ...);

#ifdef __cplusplus
}
#endif

#endif

