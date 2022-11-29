#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <avcodec.h>

#include "Yerrno.h"
#include "Ylog.h"
#include "vdec.h"

typedef struct ffmpeg_decode_ctx_s
{
    AVCodec *codec;
    AVCodecContext *codec_ctx;
    AVFrame *frame;                             // 保存解码后的YUV数据
}ffmpeg_decode_ctx;

static int init_once = 0;

int ffmpeg_decoder_init(video_decoder *decoder, void **decode_ctx)
{
    if(init_once == 0)
    {
        init_once = 1;
        av_register_all();
    }

    AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if(codec == NULL)
    {
        YLOGE(("avcodec_find_decoder failed"));
        return YERR_FAILED;
    }

    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    codec_ctx->frame_number = 1;
    codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
    codec_ctx->bit_rate = 0;
	codec_ctx->time_base.num = 1;
	codec_ctx->time_base.den = 15;
	codec_ctx->width = decoder->video_width;
	codec_ctx->height = decoder->video_height;

    int rc = avcodec_open2(codec_ctx, codec, NULL);
    if(rc < 0)
    {
        YLOGE(("avcodec_open2 failed, %s"), rc);
        return YERR_FAILED;
    }

    ffmpeg_decode_ctx *ctx = (ffmpeg_decode_ctx*)calloc(1, sizeof(ffmpeg_decode_ctx));
    ctx->codec = codec;
    ctx->codec_ctx = codec_ctx;
    ctx->frame = av_frame_alloc();

    *decode_ctx = ctx;

    return YERR_SUCCESS;
}

void ffmpeg_decoder_release(video_decoder *decoder)
{
    ffmpeg_decode_ctx *ctx = (ffmpeg_decode_ctx*)decoder->decode_ctx;
    avcodec_close(ctx->codec_ctx);
    av_free(ctx->codec);
    av_frame_free(ctx->frame);
}

int ffmpeg_decoder_decode(video_decoder *decoder, char *indata, size_t indata_size)
{
    ffmpeg_decode_ctx *ctx = (ffmpeg_decode_ctx*)decoder->decode_ctx;

    // AVPacket需要NALU数据，所以先判断NALU的头部
    char *nalu = NULL;
    while(true)
    {
        
    }


    // AVPacket用来保存解码前的数据
    AVPacket avpacket;
    av_init_packet(&avpacket);
    int got_picture = 0;
    int len = avcodec_decode_video2(ctx->codec_ctx, ctx->frame, &got_picture, &avpacket);
    av_free_packet(&avpacket);

    if(got_picture == 0)
    {
        YLOGE(("decode failed, codec = %d"), decoder->codec);
        return YERR_FAILED;
    }

}

struct video_decoder_actions_s ffmpeg_decoder_actions = 
{
    .supported_codecs = { VIDEO_CODEC_H264 },
    .init = ffmpeg_decoder_init,
    .release = ffmpeg_decoder_release,
    .decode = ffmpeg_decoder_decode
};
