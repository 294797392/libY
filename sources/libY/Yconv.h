/***********************************************************************************
 * @ file    : Yconv.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.02.27 11:50
 * @ brief   : 提供各种数据类型之间的转换函数
 ************************************************************************************/



#ifndef __YCONV_H__
#define __YCONV_H__

#include "Ybase.h"

#ifdef __cplusplus
extern "C" {
#endif

	/*
	 * 描述：
	 * 把一个字节数组转换成short类型的整形数据
	 *
	 * 参数：
	 * @bytes：要转换的字节数组
	 * @offset：字节数组里的起始位置
	 *
	 * 返回值：
	 * 转换后的short值
	 */
	YAPI short Y_conv_bytes2short(char *bytes, int offset);
    YAPI int Y_conv_bytes2int(char *bytes, int offset);

	YAPI void Y_conv_int2bytes(int value, char *bytes, int offset);


#ifdef __cplusplus
}
#endif

#endif
