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

int Y_file_stat(const char *file_path, Yfstat *stat)
{
#if (defined(Y_WIN32)) || (defined(Y_MINGW))
	WIN32_FILE_ATTRIBUTE_DATA attr_data;
	memset(&attr_data, 0, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
	int rc = GetFileAttributesExA(file_path, GetFileExInfoStandard, &attr_data);
	if(rc == 0)
	{
		rc = GetLastError();
		return YERR_FILE_STAT_FAILED;
	}
	stat->exist = (attr_data.dwFileAttributes & 0x10) == 0;
	stat->length = (stat->length | attr_data.nFileSizeHigh) << 32 | attr_data.nFileSizeLow;
#elif (defined(Y_UNIX)) || (defined(Y_MSYS))
	//stat s;
	//fstat()
#endif

	return YERR_SUCCESS;
}

int Y_file_readbytes(const char *file_path, char **bytes, uint64_t *size)
{
	Yfstat stat;
	int rc = Y_file_stat(file_path, &stat);
	if(rc != YERR_SUCCESS)
	{
		return rc;
	}

	if(stat.exist == 0)
	{
		return YERR_FILE_NOT_FOUND;
	}

	if(stat.length == 0)
	{
		return YERR_SUCCESS;
	}

	FILE *f = fopen(file_path, "r");
	if(f == NULL)
	{
		return YERR_FAILED;
	}
	char *buf = (char *)calloc((size_t)stat.length + 1, 1);
	fread(buf, 1, (size_t)stat.length, f);
	fclose(f);
	
	*size = stat.length;
	*bytes = buf;

	return YERR_SUCCESS;
}

void Y_file_free(char *content)
{
	free(content);
}



