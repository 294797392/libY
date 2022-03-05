#include "Yfirstinclude.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Yconv.h"

// 从第一个字节开始存储，然后往左移8位，继续存储
static long long convert(char *bytes, int offset, int count)
{
    long long value = 0;
    for(int i = 0; i < count; i++)
    {
        value = value | bytes[offset + i];
        if(i < count - 1)
        {
            value = value << 8;
        }
    }
    return value;
}

short Y_conv_bytes2short(char *bytes, int offset)
{
    return (short)(bytes[offset] | (bytes[offset + 1] << 8));
}

int Y_conv_bytes2int(char *bytes, int offset)
{
    // 这里必须转成unsigned char，不然如果bytes里出现一个字节出现负数的话，转换会有问题
    // 暂时不知道原因...
    return (unsigned char)bytes[offset] | ((unsigned char)bytes[offset + 1] << 8) | ((unsigned char)bytes[offset + 2] << 16) | ((unsigned char)bytes[offset + 3] << 24);
}

void Y_conv_int2bytes(int value, char *bytes, int offset)
{
    bytes[offset] = value & 0xFF;
    bytes[offset + 1] = value >> 8 & 0xFF;
    bytes[offset + 2] = value >> 16 & 0xFF;
    bytes[offset + 3] = value >> 24 & 0xFF;
}
