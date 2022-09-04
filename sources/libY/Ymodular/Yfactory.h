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
#include "Ylist.h"
#include "Ymodule.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef void(*Yfactory_initialized)(Yfactory *factory);
    typedef void(*Yfactory_module_status_changed)(Yfactory *factory, Ymodule *m, Ymodule_status status);

    // 模块对象
    typedef struct Yfactory_s Yfactory;

    typedef struct Yfactory_options_s
    {
        Yfactory_initialized on_initialized;
        Yfactory_module_status_changed on_module_status_changed;
    }Yfactory_options;

    /*
     * 描述：
     * 使用options创建一个工厂
     *
     * 参数：
     * @opts：模块工厂的选项
     *
     * 返回值：
     * Yfactory实例
     */
    YAPI Yfactory *Y_create_factory(Yfactory_options *opts);

    /*
     * 描述：
     * 使用一组模块异步初始化模块工厂
     *
     * 参数：
     * @modules：要加载的模块列表
     *
     * 返回值：
     * Yfactory实例
     */
    YAPI void Y_setup_factory_async(Yfactory *factory, Ylist *modules);

    YAPI void Y_delete_factory(Yfactory *yf);

#ifdef __cplusplus
}
#endif

#endif