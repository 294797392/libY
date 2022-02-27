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
    // return (short)convert(bytes, offset, 2);
}

int Y_conv_bytes2int(char *bytes, int offset)
{
    return bytes[offset] | (bytes[offset + 1] << 8) | (bytes[offset + 2] << 16) | (bytes[offset + 3] << 24);
    // return (int)convert(bytes, offset, 4);
}

void Y_conv_int2bytes(int value, char *bytes, int offset)
{
    // 直接用int值修改数组里某个元素的地址里的值
    *(char*)bytes[offset] = value;
}
