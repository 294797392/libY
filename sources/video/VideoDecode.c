#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libY.h"

#include "VideoDecode.h"

extern VideoDecodeActions VideoDecodeActionsFFmpeg;

VideoDecodeActions *VideoDecodeList[] = 
{
    &VideoDecodeActionsFFmpeg,
    NULL
};

VideoDecode *VideoDecodeCreate(VideoDecodeOptions *options)
{
    size_t length = sizeof(VideoDecodeList) / sizeof(VideoDecodeActions*);
	for (size_t i = 0; i < length; i++)
	{
        VideoDecodeActions *decodeActions = VideoDecodeList[i];
        if(!decodeActions->IsCodecSupported(options->CodecType))
        {
            continue;
        }

        YLOGI("create VideoDecode success, %s", decodeActions->Name);

        VideoDecode *videoDecode = (VideoDecode*)calloc(1, sizeof(VideoDecode));
        videoDecode->Options = options;
        videoDecode->Actions = decodeActions;
        return videoDecode;
    }
    return NULL;
}

int VideoDecodeInitialize(VideoDecode *decode)
{
    return decode->Actions->Initialize(decode);
}

void VideoDecodeRelease(VideoDecode *decode)
{
    decode->Actions->Release(decode);
}

int VideoDecodeDecode(VideoDecode *decode, char *videoData, int dataSize, char **decodeData, int *size)
{
    return decode->Actions->Decode(decode, videoData, dataSize, decodeData, size);
}

