#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "VideoDecode.h"

uint64_t ByteToU64LittleEnd(uint8_t *ByteBuf)
{
	uint64_t u64Value = 0;

	for(int8_t i = 7; i >= 0; i--)
	{
		u64Value <<= 8;
		u64Value |= ByteBuf[i];
	}

	return u64Value;
}

uint32_t ByteToU64BigEnd(uint8_t *ByteBuf)
{
	uint32_t u64Value = 0;

	for(uint8_t i = 0; i < 4; i++)
	{
		u64Value <<= 4;
		u64Value |= ByteBuf[i];
	}

	return u64Value;
}

int main(int argc, char **argv)
{
	Y_log_init(NULL);

	int u = sizeof(size_t);

	VideoDecodeOptions options =
	{
		.AVFormat = AV_FORMAT_H264
	};
	VideoDecode *videoDecode = VideoDecodeCreate(&options);
	VideoDecodeInitialize(videoDecode);

	FILE *f = fopen("d://h264", "rb");
	while(1)
	{
		int videoSize = 0;
		fread(&videoSize, 1, 4, f);

		char *videoData = malloc(videoSize);
		fread(videoData, 1, videoSize, f);

		char *decodeData;
		int decodeSize;
		//VideoDecodeDecode(videoDecode, videoData, videoSize, &decodeData, &decodeSize);

		free(videoData);
	}

	return 0;
}
