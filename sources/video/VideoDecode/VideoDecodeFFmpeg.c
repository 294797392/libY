/***********************************************************************************
 * @ file    : VideoRendererFFmpeg.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2023.1.31 19:26
 * @ brief   : FFmpeg解码器
 * @ remark  ：滤波器使用：https://blog.csdn.net/locahuang/article/details/119187896
 ************************************************************************************/

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

#include "libY.h"

#include "VideoDecode.h"
#include "AVFormats.h"

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "postproc.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")

typedef struct FFmpegDecode
{
	const AVCodec *codec;
	AVCodecContext *avctx;
	AVFrame *frame;                             // 保存解码后的YUV数据

	struct SwsContext *swsCtx;
	uint8_t *rgb24[4];
	int linesize[4];
}FFmpegDecode;

static int init_once = 0;

int VideoDecodeActionsFFmpegInitialize(VideoDecode *decode)
{
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

	FFmpegDecode *ffmpegDecode = (FFmpegDecode *)calloc(1, sizeof(FFmpegDecode));
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
	av_free((void *)ffmpegDecode->codec);
	av_frame_free(&ffmpegDecode->frame);
	free(ffmpegDecode);
	decode->ActionsData = NULL;
}

int VideoDecodeActionsFFmpegDecode(VideoDecode *decode, VideoDecodeInput *decodeInput)
{
	FFmpegDecode *ffmpegDecode = (FFmpegDecode *)decode->ActionsData;

	// // 传递给AVPacket必须是一个完整的NALU数据，所以先判断NALU的头部
	// char *nalu = NULL;
	// while(true)
	// {

	// }

	// AVPacket用来保存解码前的数据
	AVPacket *avpkt = av_packet_alloc();
	avpkt->data = (uint8_t *)decodeInput->videoData;
	avpkt->size = decodeInput->videoDataSize;
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

	// 初始化像素转换
	if(ffmpegDecode->swsCtx == NULL)
	{
		int srcW = ffmpegDecode->frame->width;
		int srcH = ffmpegDecode->frame->height;
		int dstW = ffmpegDecode->frame->width;
		int dstH = ffmpegDecode->frame->height;
		ffmpegDecode->swsCtx = sws_getContext(srcW, srcH, ffmpegDecode->frame->format, dstW, dstH, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
		av_image_alloc(ffmpegDecode->rgb24, ffmpegDecode->linesize, dstW, dstH, AV_PIX_FMT_RGB24, 1);
	}

	int errnum = sws_scale(ffmpegDecode->swsCtx, (uint8_t const *const *)ffmpegDecode->frame->data, ffmpegDecode->frame->linesize, 0, ffmpegDecode->frame->height, ffmpegDecode->rgb24, ffmpegDecode->linesize);
	if(errnum <= 0)
	{
		char errbuf[1024] = { '\0' };
		av_strerror(errnum, errbuf, sizeof(errbuf));
		YLOGE("sws_scale failed, %s", errbuf);
		return YERR_FAILED;
	}

	decodeInput->decodeData = ffmpegDecode->rgb24[0];
	decodeInput->decodeDataSize = errnum * ffmpegDecode->frame->width * 3;
	decodeInput->width = ffmpegDecode->frame->width;
	decodeInput->height = errnum;

	return YERR_SUCCESS;
}

// int VideoDecodeActionsFFmpegAddFilter(VideoDecode *decode, VideoFilter filter)
// {
// 	FFmpegDecode *ffmpegDecode = (FFmpegDecode*)decode->ActionsData;
// 	if(ffmpegDecode->filterGraph == NULL)
// 	{
// 		ffmpegDecode->filterGraph = avfilter_graph_alloc();
// 	}
	// // 解码成功后初始化Filter（如果需要滤波器的话）
	// if(decode->Options->Flip != VIDEO_FLIP_NONE)
	// {
	// 	const AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    // 	const AVFilter *buffersink = avfilter_get_by_name("buffersink");
	// 	const AVFilter *flipFilter = NULL;

	// 	switch (decode->Options->Flip)
	// 	{
	// 	case VIDEO_FLIP_X: flipFilter = avfilter_get_by_name("hflip"); break;
	// 	case VIDEO_FLIP_Y: flipFilter = avfilter_get_by_name("vflip"); break;
	// 	default: break;
	// 	}

	// 	char args[1024] = {'\0'};
	// 	snprintf(args, sizeof(args), "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d", ffmpegDecode->frame->width, errnum, AV_PIX_FMT_RGB24, 1, 25, 1, 1);
	// 	avfilter_graph_create_filter(&ffmpegDecode->bufferSrcFilterCtx, buffersrc, "in", args, NULL, ffmpegDecode->filterGraph);
	// 	avfilter_graph_create_filter(&ffmpegDecode->bufferSinkFilterCtx, buffersink, "out", NULL, NULL, ffmpegDecode->filterGraph);
	// 	avfilter_graph_create_filter(&ffmpegDecode->flipFilter, flipFilter, "flip", NULL, NULL, ffmpegDecode->filterGraph);
	// }
// }

struct VideoDecodeActions VideoDecodeActionsFFmpeg =
{
	.Name = "FFmpeg Decode",
	.SupportedFormats = { AV_FORMAT_H264, -1 },
	.Initialize = VideoDecodeActionsFFmpegInitialize,
	.Release = VideoDecodeActionsFFmpegRelease,
	.Decode = VideoDecodeActionsFFmpegDecode,
};
