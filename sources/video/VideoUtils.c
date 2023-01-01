#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "video.h"

char *find_nalu(const char *data, size_t data_len)
{
    for (size_t i = 0; i < data_len; i++)
    {
        if(data[i] == 0x00 && data[i + 1] == 0x00 && data[i + 2] == 0x00 && data[i + 3] == 0x01)
        {

        }
    }
}

