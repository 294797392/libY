/***********************************************************************************
 * @ file    : Yfile.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.01.29 10:10
 *             2022.03.12 01:12 - Y_file_read_lines改成使用Yreader去读取所有行
 * @ brief   : 封装文件操作函数
 ************************************************************************************/
#ifndef __YFILE_H__
#define __YFILE_H__

#include "Ybase.h"

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * 描述：
     * 读取一个完整的文件
     * 该函数要和Y_file_free配对使用
     * 当读取的文件内容不再需要的时候，要调用Y_file_free函数释放内存
     * 
     * 参数：
     * @path：要读取的文件的路径
     * @size：读取的文件大小
     * 
     * 返回值：
     * 如果打开文件失败返回NULL，如果读取成功返回文件内容
     */
    YAPI const char *Y_file_read(const char *path, int *size);

    /*
     * 描述：
     * 释放使用Y_file_read函数读取的文件内容
     * 
     * 参数：
     * @path：文件内容
     */
    YAPI void Y_file_free(const char *content);

    /*
     * 描述：
     * 写入一个文本文件
     * 
     * 参数：
     * @path：要写入的文件的路径
     * @content：要写入的文件内容，以\0为结束符
     * 
     * 返回值：
     * 如果打开文件失败返回NULL，如果写入成功返回0
     */
    YAPI int Y_file_write(const char *path, const char *content);

    /*
     * 描述：
     * 读取一个文件里的所有行
     * 
     * 参数：
     * @path：要读取的文件的路径
     * 
     * 返回值：
     * 行数组
     */
    YAPI char **Y_file_read_lines(const char *path, int *numlines);

    YAPI void Y_file_free_lines(char **lines, int numlines);

#if (defined(Y_API_WIN32))
#elif (defined(Y_API_UNIX))
    YAPI int Y_file_read_linux_perm(const char *path, char *permissions);
#endif

#ifdef __cplusplus
}
#endif

#endif