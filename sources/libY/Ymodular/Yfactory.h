/***********************************************************************************
 * @ file    : Yfactory.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.08.31 19:33
 * @ brief   : 模块工厂
 * @ remark  ：
 ************************************************************************************/

#ifndef __YFACTORY_H__
#define __YFACTORY_H__

#include "Y.h"

#ifdef __cplusplus
extern "C" {
#endif

    // 模块对象
    typedef struct Yfactory_s Yfactory;

    YAPI Yfactory *Y_create_factory(const YCHAR *config_path);

    YAPI void Y_delete_factory(Yfactory *yf);

#ifdef __cplusplus
}
#endif

#endif