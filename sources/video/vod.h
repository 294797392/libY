/***********************************************************************************
 * @ file    : vod.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.10.02 00:20
 * @ brief   : 视频输出设备
 * @ remark  ：
 ************************************************************************************/

#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "libY.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct video_renderer_s video_renderer;
    typedef struct video_renderer_actions_s video_renderer_actions;
    typedef struct video_renderer_options_s video_renderer_options;

    typedef int (*vr_init_func)(video_renderer *renderer, video_renderer_options *options, void **renderer_ctx);
    typedef void (*vr_release_func)(video_renderer *renderer);
    typedef int (*vr_render_func)(video_renderer *renderer, char *video_data);

    typedef enum 
    {
        VIDEO_FORMAT_YV12
    }VIDEO_FORMAT;

    struct video_renderer_s
    {
        void *renderer_ctx;
        video_renderer_actions *actions;
        video_renderer_options *options;
    };

    struct video_renderer_actions_s
    {
        vr_init_func init;
        vr_release_func release;
        vr_render_func render;
    };

    struct video_renderer_options_s
    {
        int video_width;
        int video_height;
        VIDEO_FORMAT format;
        void *drawable;                         // 用来显示视频的窗口
    };

#ifdef __cplusplus
}
#endif

#endif