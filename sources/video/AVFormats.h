#ifndef __VIDEO_CODECS_H__
#define __VIDEO_CODECS_H__

typedef enum
{
    /// <summary>
    /// H264��Ƶ��ʽ
    /// </summary>
    AV_FORMAT_H264,

    /// <summary>
    /// PCM��Ƶԭʼ���ݸ�ʽ
    /// </summary>
    AV_FORMAT_PCM,

    /// <summary>
    /// G711��Ƶ���뷽ʽ
    /// </summary>
    AV_FORMAT_G711_ALAW,
    AV_FORMAT_G711_ULAW
} AVFormats;

#endif