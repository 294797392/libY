#include "Yfirstinclude.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if (defined(Y_API_WIN32))
#elif (defined(Y_API_UNIX))
#include <sys/stat.h>
#endif

#include "Ylog.h"
#include "Ylist.h"
#include "Ystring.h"
#include "Ylog/Ylog.h"
#include "Yfile.h"
#include "Yreader.h"

#define DEFAULT_LINES               64
#define MAX_LINE_SIZE               16384

// 读取linux文件权限所需要的宏定义
#define N_BITS 3
static const char *PERMISSIONS[] = { "---","--x","-w-","-wx","r--","r-x","rw-","rwx"};

const char *Y_file_read(const char *path, int *size)
{
    // 先使用fstat函数获取文件大小，然后开辟内存读取
    struct stat status;
    if(fstat(path, &status) < 0)
    {
        YLOGE(YTEXT("Y_file_read fstat failed, error = %d, path = %s"), errno, path);
        return NULL;
    }

    *size = status.st_size;

    FILE *file = fopen(path, "r");
    if(file == NULL)
    {
        YLOGE(YTEXT("Y_file_read fopen failed, error = %d, path = %s"), errno, path);
        return NULL;
    }

    char *content = calloc(status.st_size, 1);
    fread(content, 1, status.st_size, file);
    fclose(file);
    return content;
}

void Y_file_free(const char *content)
{
    free(content);
}

int Y_file_write(const char *path, const char *content)
{
    FILE *f = fopen(path, "w");
    if(f == NULL)
    {
        YLOGE(YTEXT("open file failed, %s"), path);
        return -1;
    }

    size_t len = strlen(content);

    fwrite(content, 1, len, f);
    fflush(f);
    fclose(f);

    return 0;
}

char **Y_file_read_lines(const char *path, int *numlines)
{
    Yreader *reader = Y_create_reader(path);
    if(reader == NULL)
    {
        return NULL;
    }

    int totalline = DEFAULT_LINES;
    int linecount = 0;
    char **lines = (char**)calloc(totalline, sizeof(char*));
    int linesize;
    char *line = NULL;
    while((line = Y_reader_readline(reader, &linesize)))
    {
        if(linecount == totalline)
        {
            totalline = totalline * 2;
            lines = (char**)realloc(lines, totalline * sizeof(char*));
        }
        lines[linecount++] = line;
    }
    *numlines = linecount;
    return lines;
}

void Y_file_free_lines(char **lines, int numlines)
{
    for(int line = 0; line < numlines; line++)
    {
        free(lines[line]);
    }
}

#if (defined(Y_API_WIN32))
#elif (defined(Y_API_UNIX))
int Y_file_read_linux_perm(const char *path, char *permissions)
{
	struct stat stats;
    if(stat(path, &stats) < 0)
    {
        YLOGE(YTEXT("stat failed"));
        return -1;
    }

    unsigned int i,mask = 0700;
    for(i = 3; i; --i)
    {
        char perm[128] = {'\0'};
        snprintf(perm, sizeof(perm), "%3s", PERMISSIONS[(stats.st_mode & mask) >> (i - 1) * N_BITS]);
        strcat(permissions, perm);
        mask >>= N_BITS;
    }

    return 0;
}
#endif
