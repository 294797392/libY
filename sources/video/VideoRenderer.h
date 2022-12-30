/***********************************************************************************
 * @ file    : VideoRenderer.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.10.02 00:20
 * @ brief   : 视频输出设备
 * @ remark  ：
 ************************************************************************************/

#ifndef __VIDEO_RENDERER_H__
#define __VIDEO_RENDERER_H__

#include "libY.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct VideoRenderer VideoRenderer;
    typedef struct VideoRendererActions VideoRendererActions;
    typedef struct VideoRendererOptions VideoRendererOptions;

    typedef int (*VideoRendererInitializeDlg)(VideoRenderer *renderer);
    typedef void (*VideoRendererReleaseDlg)(VideoRenderer *renderer);
    typedef int (*VideoRendererRenderDlg)(VideoRenderer *renderer, char *videoData);

    typedef enum 
    {
        VIDEO_FORMAT_YV12
    }VideoFormats;

    typedef enum
    {
        VIDEO_RENDERER_D3D9
    }VideoRendererTypes;

    struct VideoRenderer
    {
        void *ActionsData;
        VideoRendererActions *Actions;
        VideoRendererOptions *Options;
    };

    struct VideoRendererActions
    {
        VideoRendererTypes Type;
        VideoRendererInitializeDlg Initialize;
        VideoRendererReleaseDlg Release;
        VideoRendererRenderDlg Render;
    };

    struct VideoRendererOptions
    {
        VideoRendererTypes Type;
        int VideoWidth;
        int VideoHeight;
        VideoFormats Format;
        void *Drawable;                         // 用来显示视频的窗口
    };

    YAPI VideoRenderer *VideoRendererCreate(VideoRendererOptions *options);
    YAPI int VideoRendererInitialize(VideoRenderer *renderer);
    YAPI int VideoRendererRelease(VideoRenderer *renderer);
    YAPI int VideoRendererRender(VideoRenderer *renderer, char *videoData);


#ifdef __cplusplus
}
#endif

#endif