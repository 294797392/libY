/***********************************************************************************
 * @ file    : Ybuilder.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.03.05 19:19
 * @ brief   : 实现一个StringBuilder
 ************************************************************************************/

#ifndef __YBUILDER_H__
#define __YBUILDER_H__

#include "Ybase.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct Ybuilder_s Ybuilder;

    YAPI Ybuilder *Y_create_builder();

    YAPI void Y_delete_builder(Ybuilder *yb);

    YAPI void Y_builder_append(Ybuilder *yb, const char *str);

    YAPI void Y_builder_append_format(Ybuilder *yb, const char *str, ...);

    YAPI void Y_builder_append_char(Ybuilder *yb, const char c);

    /*
     * 描述：
     * 创建一个新的字符串，并把builder里的字符串拷贝到这个新的字符串里并返回
     * 
     * 参数：
     * @yb：builder对象
     * 
     * 返回值：字符串指针
     */
    YAPI char *Y_builder_tostring(Ybuilder *yb);

    /*
     * 描述：
     * 清空builder里存储的字符串
     * 
     * 参数：
     * @yb：要清空的builder对象
     */
    YAPI void Y_builder_reset(Ybuilder *yb);

    /*
     * 描述：
     * 获取当前builder里字符串长度
     * 
     * 参数：
     * @yb：builder对象
     * 
     * 返回值：字符串长度
     */
    YAPI int Y_builder_getsize(Ybuilder *yb);

#ifdef __cplusplus
}
#endif

#endif
