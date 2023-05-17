#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#if (defined(Y_WIN32)) || (defined(Y_MINGW))
#include <Windows.h>
#elif (defined(Y_UNIX)) || (defined(Y_MSYS))
#include <pthread.h>
#include <locale.h>
#endif

#include "libY.h"

int Y_file_get_size(const char *file_path)
{
#if (defined(Y_WIN32)) || (defined(Y_MINGW))

	WIN32_FILE_ATTRIBUTE_DATA attr_data;
	memset(&attr_data, 0, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
	int rc = GetFileAttributesExA(file_path, GetFileExInfoStandard, &attr_data);
	if(rc == 0)
	{
		rc = GetLastError();
		printf("GetFileAttributesExA failed, %d", rc);
		return -1;
	}

	return ((uint64_t)attr_data.nFileSizeHigh << 32) + attr_data.nFileSizeLow;

#elif (defined(Y_UNIX)) || (defined(Y_MSYS))
	//stat s;
	//fstat()
#endif

}

int Y_file_exist(const char *file_path)
{
	FILE *f = fopen(file_path, "r");
	if(f == NULL)
	{
		if(errno == 2)
		{
			return -1;
		}
	}
	else
	{
		fclose(f);
	}

	return 0;
}

int Y_file_read_all(const char *file_path, char **content, uint64_t *size)
{
	int file_size = Y_file_get_size(file_path);
	if(file_size <= 0)
	{
		return YERR_FAILED;
	}

	FILE *f = fopen(file_path, "r");
	if(f == NULL)
	{
		return YERR_FAILED;
	}
	char *buf = (char *)calloc((size_t)file_size + 1, 1);
	if(buf == NULL)
	{
		return YERR_NO_MEM;
	}

	fread(buf, 1, (size_t)file_size, f);
	fclose(f);

	*size = file_size;
	*content = buf;

	return YERR_SUCCESS;
}

void Y_file_free(char *content)
{
	free(content);
}



