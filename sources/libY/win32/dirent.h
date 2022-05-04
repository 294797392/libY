/***********************************************************************************
 * @ file    : Ydirent.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.03.06 16:30
 * @ brief   : 在Windows下模拟linux下的dirent函数和结构体
 *              编译Windows版本的libY的时候需要编译dirent.c
 ************************************************************************************/
#ifndef __YDIRENT_H__
#define __YDIRENT_H__

#include "Ybase.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct struct_DIR DIR;

    struct dirent;

    YAPI DIR *opendir(const char *dir);

    YAPI int closedir(DIR *dir);

    YAPI struct dirent *readdir(DIR *dir);

#ifdef __cplusplus
}
#endif

#endif


