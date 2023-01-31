/***********************************************************************************
 * @ file    : VideoDecode.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.10.02 11:50
 * @ brief   : 视频解码器结构体定义
 * @ remark  ：
 ************************************************************************************/

#ifndef __VIDEO_DECODE_H__
#define __VIDEO_DECODE_H__

#include "libY.h"

#include "Video.h"
#include "AVFormats.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct VideoFrame VideoFrame;
    typedef struct VideoDecode VideoDecode;
    typedef struct VideoDecodeActions VideoDecodeActions;
    typedef struct VideoDecodeOptions VideoDecodeOptions;
    typedef struct VideoDecodeInput VideoDecodeInput;

    typedef int (*VideoDecodeInitializeDlg)(VideoDecode *decode);
    typedef void (*VideoDecodeReleaseDlg)(VideoDecode *decode);
    typedef int (*VideoDecodeDecodeDlg)(VideoDecode *decode, VideoDecodeInput *decodeInput);

    typedef enum
    {
        H264_NALU_UNUSED = 0,
    }H264_NALU_TYPES;

    struct VideoDecodeInput
    {
        char *videoData;
        int videoDataSize;
        char *decodeData;
        int decodeDataSize;
        int width;
        int height;
    };

    struct VideoDecode
    {
        AVFormats AVFormat;
        VideoDecodeActions *Actions;
        void *ActionsData;
        VideoDecodeOptions *Options;
    };

    struct VideoDecodeActions
    {
        char *Name;
        int SupportedFormats[64];
        VideoDecodeInitializeDlg Initialize;
        VideoDecodeReleaseDlg Release;
        VideoDecodeDecodeDlg Decode;
    };

    struct VideoDecodeOptions
    {
        AVFormats AVFormat;
        int VideoWidth;
        int VideoHeight;
    };

    YAPI VideoDecode *VideoDecodeCreate(VideoDecodeOptions *options);
    YAPI void VideoDecodeDelete(VideoDecode *decode);
    YAPI int VideoDecodeInitialize(VideoDecode *decode);
    YAPI void VideoDecodeRelease(VideoDecode *decode);
    YAPI int VideoDecodeDecode(VideoDecode *decode, VideoDecodeInput decodeInput);
    YAPI int VideoDecodeAddFilter(VideoDecode *decode, VideoFilter filter);

#ifdef __cplusplus
}
#endif

#endif