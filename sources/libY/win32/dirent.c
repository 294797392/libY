#include "Yfirstinclude.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Windows.h>

#include "dirent.h"

struct DIR_struct
{
    void *p;

    WIN32_FIND_DATA dir_data;
};

struct dirent
{
    void *p;
};

DIR *opendir(const char *dir)
{
    // FindFirstFile()
}

int closedir(DIR *dir)
{
}

struct dirent *readdir(DIR *dir)
{
}
