#include "Yfirstinclude.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Ybase.h"
#include "Ylog.h"
#include "Ybuilder.h"
#include "Yreader.h"

#define DEFAULT_BUFFER_SIZE                 8192    // 一次性从文件里读取这么多字节的数据
#define DEFAULT_LINE_SIZE                   8192    // 默认行长度是8192个字节

struct Yreader_s
{
	char buffer[DEFAULT_BUFFER_SIZE];       // 缓存读取到的数据
	int offset;                             // 指向缓冲区中当前读取到的位置的指针
	int remain;                             // 缓冲区中剩余的数据大小
	int size;                               // 缓冲区中数据的真实大小
	int bufsize;                            // 每次从缓冲区中读取的数据大小

	FILE *file;

	char *line;                             // 存储当前行的数据
	size_t linesize;                        // 当前行的长度
	size_t linebufsize;                     // line的缓冲区长度

    YREADER_NEWLINE newline;                // 换行符格式
    int include_linebreak;                  // 返回的行数据里是否包含换行符
};

Yreader *Y_create_reader(const char *path)
{
	FILE *file = fopen(path, "r");
	if (file == NULL)
	{
		YLOGE(YTEXT("open file failed, %s, %d"), path, errno);
		return NULL;
	}

	Yreader *yr = (Yreader *)calloc(1, sizeof(Yreader));
	yr->file = file;
	yr->bufsize = DEFAULT_BUFFER_SIZE;
	yr->remain = 0;
	yr->size = 0;
	yr->linesize = 0;
	yr->linebufsize = DEFAULT_LINE_SIZE;
	yr->line = (char *)calloc(DEFAULT_LINE_SIZE, 1);
    yr->newline = YREADER_NEWLINE_LF;
    yr->include_linebreak = 0;
	return yr;
}

void Y_delete_reader(Yreader *yr)
{
	if (yr->file)
	{
		fclose(yr->file);
	}
	if (yr->line)
	{
		free(yr->line);
	}
	free(yr);
}

char *Y_reader_readline(Yreader *yr, int *linesize)
{
    // 指示一行是否结束了
    int line_end = 0;
	do
	{
		int offset = yr->offset;
		int remain = yr->remain;
		if (remain == 0)
		{
			yr->size = fread(yr->buffer, 1, yr->bufsize, yr->file);
			yr->remain = yr->size;
			yr->offset = 0;
			remain = yr->size;
		}

		for (int i = 0; i < remain; i++)
		{
			yr->offset++;
			yr->remain--;

			char c = yr->buffer[offset + i];
            if(c == '\r' && yr->newline == YREADER_NEWLINE_CR)
            {
                line_end = 1;
            }
            else if(c == '\n')
            {
                if(yr->newline == YREADER_NEWLINE_LF)
                {
                    line_end = 1;
                }
                else if(yr->newline == YREADER_NEWLINE_CRLF && yr->line[yr->linesize - 1] == '\r')
                {
                    // 如果line缓冲区中上一个字符是\r
                    // 那么说明一行结束
                    line_end = 1;
                }
            }

			if (line_end)
			{
				// 说明一行结束了
				*linesize = yr->linesize;
				char *newline = (char *)calloc(1, yr->linesize + 1);    // yr->linesize + 1，这个加1是存储\0使用的
				strncpy(newline, yr->line, yr->linesize);

				// 重置line缓冲区
				memset(yr->line, 0, yr->linebufsize);
				yr->linesize = 0;
				return newline;
			}

			if (yr->linesize == yr->linebufsize)
			{
				// 缓冲区不够用了，需要重新分配内存
				// 每次按照上一次的两倍大小扩容缓冲区
				size_t newsize = yr->linebufsize * 2;
				yr->line = (char *)realloc(yr->line, newsize);
				yr->linebufsize = newsize;
			}

			yr->line[yr->linesize++] = c;
		}
	} while (!feof(yr->file));

	return NULL;
}

void Y_read_set_options(Yreader *yr, YREADER_OPTIONS options, void *value)
{
	switch (options)
	{
	case YREADER_OPT_NEWLINE:
    {
        yr->newline = *((int*)value);
		break;
    }

    case YEADER_OPT_INCLUDE_LINEBREAK:
    {
        yr->include_linebreak = *((int*)value);
        break;
    }

	default:
		break;
	}
}
