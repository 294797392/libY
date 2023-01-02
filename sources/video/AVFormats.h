#ifndef __VIDEO_CODECS_H__
#define __VIDEO_CODECS_H__

typedef enum
{
    /// <summary>
    /// H264视频格式
    /// </summary>
    AV_FORMAT_H264,

    /// <summary>
    /// PCM音频原始数据格式
    /// </summary>
    AV_FORMAT_PCM,

    /// <summary>
    /// G711音频编码方式
    /// </summary>
    AV_FORMAT_G711_ALAW,
    AV_FORMAT_G711_ULAW
} AVFormats;

#endif