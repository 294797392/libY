/***********************************************************************************
 * @ file    : Ymodule.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.08.31 19:16
 * @ brief   : 模块
 * @ remark  ：
 ************************************************************************************/

#ifndef __YMODULE_H__
#define __YMODULE_H__

#include "Y.h"

#ifdef __cplusplus
extern "C" {
#endif

    // 模块对象
    typedef struct Ymodule_s Ymodule;

    YAPI Ymodule *Y_create_module();

    /*
     * 描述：
     * 删除一个事件
     *
     * 参数：
     * @ye：要删除的事件
     */
    YAPI void Y_delete_module(Ymodule *ym);

#ifdef __cplusplus
}
#endif

#endif