/***********************************************************************************
 * @ file    : vodops.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.04.28 00:38
 * @ brief   : Video Output Device，用来渲染视频数据
 ************************************************************************************/

#ifndef __VODOPS_H__
#define __VODOPS_H__

#include <libY/Ybase.h>

#ifdef __cplusplus
extern "C" {
#endif

    struct VODController
    {
        int(*Initialize)();
    };

#ifdef __cplusplus
}
#endif

#endif
