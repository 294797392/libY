#include "Yfirstinclude.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include "Ybuilder.h"

#define DEFAULT_SIZE        4096
#define RESIZE_UNIT         4096        // 每次扩容都是这个数的倍数

struct Ybuilder_s
{
    // 存储字符串的缓冲区
    char *buffer;

    // 缓冲区总大小
    int bufsize;

    // 当前存储位置
    int offset;

    // 缓冲区中剩余的字节数
    int remain;
};

// minsize：至少要扩容minsize个字节数的空间
static void resize(Ybuilder *yb, int minsize)
{
    int newsize = ceil(minsize / RESIZE_UNIT) * RESIZE_UNIT;
    yb->buffer = realloc(yb->buffer, newsize);
    yb->remain = newsize - yb->offset;
}

Ybuilder *Y_create_builder()
{
    Ybuilder *yb = (Ybuilder*)calloc(1, sizeof(Ybuilder));
    yb->buffer = (char*)calloc(1, DEFAULT_SIZE);
    yb->offset = 0;
    yb->remain = DEFAULT_SIZE;
    yb->bufsize = DEFAULT_SIZE;

    return yb;
}

void Y_delete_builder(Ybuilder *yb)
{
    free(yb->buffer);
    free(yb);
}

void Y_builder_append(Ybuilder *yb, const char *str)
{
    int len = strlen(str);
    if(yb->remain < len)
    {
        // 缓冲区不够了, 要扩容
        resize(yb, len - yb->remain);
    }

    strncpy(yb->buffer + yb->offset, str, yb->remain);
    yb->offset += len;
}

void Y_builder_append_format(Ybuilder *yb, const char *str, ...)
{
    va_list ap;
    int args;
	va_start(ap, args);
	// vsnprintf(message, MAX_MSG_SIZE, msg, ap);
	va_end(ap);
}

void Y_builder_append_char(Ybuilder *yb, const char c)
{}

char *Y_builder_tostring(Ybuilder *yb)
{
    if(yb->offset == 0)
    {
        return NULL;
    }

    char *string = (char*)calloc(1, yb->offset + 1);
    strncpy(string, yb->buffer, yb->offset + 1);
    return string;
}

void Y_builder_reset(Ybuilder *yb)
{
    yb->offset = 0;
    yb->remain = yb->bufsize;
}

int Y_builder_getsize(Ybuilder *yb)
{
    return yb->offset + 1;
}

