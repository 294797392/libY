/***********************************************************************************
 * @ file    : vod.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.04.28 00:03
 * @ brief   : Video Output Device，用来渲染视频数据
 ************************************************************************************/

#ifndef __VOD_H__
#define __VOD_H__

#include <libY/Ybase.h>

#ifdef __cplusplus
extern "C" {
#endif

    struct VODState;

	YAPI struct VODState *CreateVOD();

    YAPI int DeleteVOD(struct VODState *state);

    YAPI int VOD_SetSurface(void *surface);

    YAPI int VOD_RenderFrame(const char *frame, size_t frameSize);

#ifdef __cplusplus
}
#endif

#endif
