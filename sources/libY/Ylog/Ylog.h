#ifndef __YLOG_H__
#define __YLOG_H__

#include "Yfirstinclude.h"

#include "Ybase.h"

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * 描述：
     * Ylog全局初始化函数，该函数在整个应用程序里只调用一次
     * 
     * 返回值：
     * tcpsvc对象
     */
    YAPI int Ylog_global_init();

#ifdef __cplusplus
}
#endif

#endif