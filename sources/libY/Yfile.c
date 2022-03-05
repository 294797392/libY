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

#define MAX_LINES                   1024
#define MAX_LINE_SIZE               16384

#if (defined(Y_API_WIN32))
#elif (defined(Y_API_UNIX))
#define N_BITS 3
static const char *PERMISSIONS[] = { "---","--x","-w-","-wx","r--","r-x","rw-","rwx"};
#endif

const char *Y_file_read(const char *path)
{
    // 先使用fstat函数获取文件大小，然后开辟内存读取
    // fstat()
}

void Y_file_free(const char *content)
{

}


char **Y_file_read_lines(const char *path, int *num_line)
{
    FILE *f = fopen(path, "r");
    if(f == NULL)
    {
        YLOGE(YTEXT("open file failed, %s"), path);
        return NULL;
    }

    char **lines = (char**)calloc(MAX_LINES, sizeof(char*));

    int line_num = 0;
    while(!feof(f))
    {
        char *line = (char*)calloc(1, MAX_LINE_SIZE);
        fgets(line, MAX_LINE_SIZE, f);
        Ystr_trim_right(line, '\n');
        lines[line_num++] = line;
    }
    *num_line = line_num;

    fclose(f);

    return lines;
}

void Y_file_free_lines(char **lines, int num_line)
{
    // for(int line = 0; line < num_line; line++)
    // {
    //     free(lines[line]);
    // }

    for(int line = 0; line < MAX_LINES; line++)
    {
        free(lines[line]);
    }
}

int Y_file_write_all(const char *path, const char *content, size_t size)
{
    FILE *f = fopen(path, "w");
    if(f == NULL)
    {
        YLOGE(YTEXT("open file failed, %s"), path);
        return -1;
    }

    fwrite(content, 1, size, f);
    fflush(f);
    fclose(f);
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
