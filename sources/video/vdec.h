/***********************************************************************************
 * @ file    : vdec.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.10.02 11:50
 * @ brief   : 视频解码器结构体定义
 * @ remark  ：
 ************************************************************************************/

#ifndef __VDEC_H__
#define __VDEC_H__

#include "libY.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct video_decoder_s video_decoder;
    typedef struct video_decoder_actions_s video_decoder_actions;
    typedef struct video_frame_s video_frame;

    typedef int (*decode_init_func)(video_decoder *decoder, void **decode_ctx);
    typedef void (*decode_release_func)(video_decoder *decoder);
    typedef int (*decode_func)(video_decoder *decoder, char *indata, size_t indata_size, video_frame *frame);

    typedef enum 
    {
        VIDEO_CODEC_H264,
        VIDEO_CODEC_H265
    }VIDEO_CODECS;

    typedef enum
    {
        H264_NALU_UNUSED = 0,
        H264_NALU_
    }H264_NALU_TYPES;

    struct video_decoder_s
    {
        VIDEO_CODECS codec;
        int video_width;
        int video_height;
        void *decode_ctx;
        video_decoder_actions *actions;
    };

    struct video_decoder_actions_s
    {
        VIDEO_CODECS *supported_codecs;
        decode_init_func init;
        decode_release_func release;
        decode_func decode;
    };

    struct video_frame_s
    {
        char *data;
    };

    YAPI int open_video_decoder(VIDEO_CODECS codec, video_decoder **decoder);

    YAPI void close_video_decoder(video_decoder *decoder);

#ifdef __cplusplus
}
#endif

#endif