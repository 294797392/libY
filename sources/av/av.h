#ifndef __AV_H__
#define __AV_H__

#define AVAPI

// 描述一个音频波形信息
typedef struct awave_s
{
    int channels;
    int bit_per_sample;
    int sample_rate;
}awave;

#endif