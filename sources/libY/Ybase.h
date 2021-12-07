﻿#ifndef __YBASE_H__
#define __YBASE_H__

#include "Yfirstinclude.h"

#ifdef Y_ENV_WIN32
    #ifdef Y_EXPORT
        #define YAPI __declspec(dllexport)
    #else
        #define YAPI __declspec(dllimport)
    #endif
#else
    #define YAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * 描述：
     * 睡眠指定的毫秒数
     * 
     * 参数：
     * @mseconds：要睡眠的毫秒数
     */
    YAPI void Ysleep(int mseconds);

#ifdef UNIX

    YAPI int fd_read_until(int fd, char *buf, size_t len, char terminator);

    /*
     * 描述：
     * 向一个文件描述符里写入数据，直到写完len大小或者失败才返回
     * 
     * 参数：
     * @fd：要写入的文件描述符
     * @data：要写入的数据地址
     * @len：要写入的数据大小
     * 
     * 返回值：
     * 写入成功返回0，写入失败返回1
     */
    YAPI int fd_write(int fd, void *data, size_t len);

#endif

    YAPI int getflag(int value, int flag);

    YAPI int setflag(int value, int flag);

#ifdef __cplusplus
}
#endif

#endif


