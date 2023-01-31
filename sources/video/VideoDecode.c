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
        for(int j = 0;; j++)
        {
            if(decodeActions->SupportedFormats[j] == -1)
            {
                break;
            }

            if(decodeActions->SupportedFormats[j] == options->AVFormat)
            {
                YLOGI("create VideoDecode success, %s", decodeActions->Name);

                VideoDecode *videoDecode = (VideoDecode *)calloc(1, sizeof(VideoDecode));
                videoDecode->Options = options;
                videoDecode->Actions = decodeActions;
                return videoDecode;
            }
        }
    }
    return NULL;
}

void VideoDecodeDelete(VideoDecode *decode)
{
    free(decode);
}

int VideoDecodeInitialize(VideoDecode *decode)
{
    return decode->Actions->Initialize(decode);
}

void VideoDecodeRelease(VideoDecode *decode)
{
    decode->Actions->Release(decode);
}

int VideoDecodeDecode(VideoDecode *decode, VideoDecodeInput *decodeInput)
{
    return decode->Actions->Decode(decode, decodeInput);
}

int VideoDecodeAddFilter(VideoDecode *decode, VideoFilter filter)
{
    return decode->Actions->AddFilter(decode, filter);
}