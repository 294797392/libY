#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
#include <libavutil/pixfmt.h>

#include "VideoDecode.h"

typedef struct FFmpegDecode
{
    const AVCodec *codec;
    AVCodecContext *avctx;
    AVFrame *frame;                             // 保存解码后的YUV数据
}FFmpegDecode;

static int init_once = 0;

int VideoDecodeActionsFFmpegInitialize(VideoDecode *decode)
{
    if(init_once == 0)
    {
        init_once = 1;
        //av_register_all();
    }

    const AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if(codec == NULL)
    {
        YLOGE(("avcodec_find_decoder failed"));
        return YERR_FAILED;
    }

    AVCodecContext *codecContext = avcodec_alloc_context3(codec);
    // codecContext->frame_number = 1;
    // codecContext->codec_type = AVMEDIA_TYPE_VIDEO;
    // codecContext->bit_rate = 0;
	// codecContext->time_base.num = 1;
	// codecContext->time_base.den = 15;
    // codecContext->width = decode->Options->VideoWidth;
    // codecContext->height = decode->Options->VideoHeight;

    int rc = avcodec_open2(codecContext, codec, NULL);
    if(rc < 0)
    {
        YLOGE("avcodec_open2 failed, %s", rc);
        return YERR_FAILED;
    }

    FFmpegDecode *ffmpegDecode = (FFmpegDecode*)calloc(1, sizeof(FFmpegDecode));
    ffmpegDecode->codec = codec;
    ffmpegDecode->avctx = codecContext;
    ffmpegDecode->frame = av_frame_alloc();

    decode->ActionsData = ffmpegDecode;

    return YERR_SUCCESS;
}

void VideoDecodeActionsFFmpegRelease(VideoDecode *decode)
{
    FFmpegDecode *ffmpegDecode = (FFmpegDecode *)decode->ActionsData;
    avcodec_close(ffmpegDecode->avctx);
    av_free(ffmpegDecode->codec);
    av_frame_free(&ffmpegDecode->frame);
}

int VideoDecodeActionsFFmpegDecode(VideoDecode *decode, char *videoData, int dataSize, char **decodeData, int *size)
{
    FFmpegDecode *ffmpegDecode = (FFmpegDecode *)decode->ActionsData;

    // // 传递给AVPacket必须是一个完整的NALU数据，所以先判断NALU的头部
    // char *nalu = NULL;
    // while(true)
    // {
        
    // }

    // AVPacket用来保存解码前的数据
    AVPacket *avpkt = av_packet_alloc();
    avpkt->data = (uint8_t *)videoData;
    avpkt->size = dataSize;
    int got_picture = 0;
    int ret = avcodec_send_packet(ffmpegDecode->avctx, avpkt);
    if(ret < 0)
    {
        YLOGE("avcodec_send_packet failed, %d", ret);
        return YERR_FAILED;
    }

    ret = avcodec_receive_frame(ffmpegDecode->avctx, ffmpegDecode->frame);
    if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
    {
        return YERR_FAILED;
    }
    else if(ret < 0)
    {
        return YERR_FAILED;
    }

    //int len = avcodec_decode_video2(ffmpegDecode->avctx, ffmpegDecode->frame, &got_picture, avPacket);
    //if(len < 0 || got_picture == 0)
    //{
    //    YLOGE("decode failed, codec = %s", ffmpegDecode->codec->name);
    //    av_free_packet(avPacket);
    //    return YERR_FAILED;
    //}
    //av_free_packet(avPacket);

    return YERR_SUCCESS;
}

int VideoDecodeActionsFFmpegIsCodecSupporte(VideoCodecs codecType)
{
    return 1;
}

struct VideoDecodeActions VideoDecodeActionsFFmpeg =
{
    .Name = "FFmpeg Decode",
    .Initialize = VideoDecodeActionsFFmpegInitialize,
    .Release = VideoDecodeActionsFFmpegRelease,
    .Decode = VideoDecodeActionsFFmpegDecode,
    .IsCodecSupported = VideoDecodeActionsFFmpegIsCodecSupporte
};
