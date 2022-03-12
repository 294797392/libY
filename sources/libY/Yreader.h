/***********************************************************************************
 * @ file    : Yreader.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.03.07 19:42
 *             2022.03.12 01:06
 *             2022.03.12 14:08
 * @ brief   : 实现一个文本读取器
 ************************************************************************************/

#ifndef __YREADER_H__
#define __YREADER_H__

#include "Yfirstinclude.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Ybase.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum Yreader_options_s
    {
        // 设置换行符
        YREADER_OPT_NEWLINE,

        // 是否包含换行符
        YEADER_OPT_INCLUDE_LINEBREAK
    }YREADER_OPTIONS;

    typedef enum Yreader_newline_s
    {
        YREADER_NEWLINE_CR,
        YREADER_NEWLINE_LF,
        YREADER_NEWLINE_CRLF
    }YREADER_NEWLINE;

    // 文件读取器句柄
    typedef struct Yreader_s Yreader;

    /*
     * 描述：
     * 打开一个文件读取器，对文件进行操作
     * 
     * 参数：
     * @path：要打开的文件的路径
     * 
     * 返回值：
     * 文件读取器句柄
     */
    YAPI Yreader *Y_create_reader(const char *path);

    /*
     * 描述：
     * 关闭打开的文件读取器
     */
    YAPI void Y_delete_reader(Yreader *yr);

    /*
     * 描述：
     * 从读取器中读取一行数据
     * 
     * 参数：
     * @yr：读取器对象
     * @linesize：读取到的行的大小
     * 
     * 返回值：
     * 如果读取成功则返回一行数据
     * 读取到文件末尾了返回NULL，linesize返回读取的大小
     */
    YAPI char *Y_reader_readline(Yreader *yr, int *linesize);

    YAPI void Y_read_set_options(Yreader *yr, YREADER_OPTIONS options, void *value);

#ifdef __cplusplus
	extern "C" {
#endif

#endif

