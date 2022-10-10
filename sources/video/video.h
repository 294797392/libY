/***********************************************************************************
 * @ file    : video.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.10.02 00:20
 * @ brief   : 视频相关的结结构体定义
 * @ remark  ：
 ************************************************************************************/

#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "libY.h"

#ifdef __cplusplus
extern "C" {
#endif

    YAPI char *find_nalu(const char *data, size_t data_len);

#ifdef __cplusplus
}
#endif

#endif