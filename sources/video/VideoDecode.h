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

#include "VideoCodecs.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct VideoFrame VideoFrame;
    typedef struct VideoDecode VideoDecode;
    typedef struct VideoDecodeActions VideoDecodeActions;
    typedef struct VideoDecodeOptions VideoDecodeOptions;

    typedef int (*VideoDecodeSupportedDlg)(VideoCodecs codec);
    typedef int (*VideoDecodeInitializeDlg)(VideoDecode *decode);
    typedef void (*VideoDecodeReleaseDlg)(VideoDecode *decode);
    typedef int (*VideoDecodeDecodeDlg)(VideoDecode *decode, char *videoData, int dataSize, char **decodeData, int *size);

    typedef enum
    {
        H264_NALU_UNUSED = 0,
        H264_NALU_
    }H264_NALU_TYPES;

    struct VideoDecode
    {
        VideoCodecs Type;
        VideoDecodeActions *Actions;
        void *ActionsData;
        VideoDecodeOptions *Options;
    };

    struct VideoDecodeActions
    {
        char *Name;
        VideoDecodeSupportedDlg IsCodecSupported;
        VideoDecodeInitializeDlg Initialize;
        VideoDecodeReleaseDlg Release;
        VideoDecodeDecodeDlg Decode;
    };

    struct VideoDecodeOptions
    {
        VideoCodecs CodecType;
        int VideoWidth;
        int VideoHeight;
    };

    YAPI VideoDecode *VideoDecodeCreate(VideoDecodeOptions *options);
    YAPI int VideoDecodeInitialize(VideoDecode *decode);
    YAPI void VideoDecodeRelease(VideoDecode *decode);
    YAPI int VideoDecodeDecode(VideoDecode *decode, char *videoData, int dataSize, char **decodeData, int *size);

#ifdef __cplusplus
}
#endif

#endif