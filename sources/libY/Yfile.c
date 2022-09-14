#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#if (defined(Y_WIN32))
#include <Windows.h>
#elif (defined(Y_UNIX))
#include <pthread.h>
#endif

#include "Y.h"
#include "Yerrno.h"
#include "Ystring.h"
#include "Yfile.h"

int Y_file_stat(const YCHAR *file_path, Yfstat *stat)
{
#if (defined(Y_WIN32))
	WIN32_FILE_ATTRIBUTE_DATA attr_data;
	memset(&attr_data, 0, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
	GetFileAttributesEx(file_path, GetFileExInfoStandard, &attr_data);
	stat->exist = (attr_data.dwFileAttributes & 0x10) == 0;
	stat->length = ((int64_t)(attr_data.nFileSizeHigh << 32)) | (attr_data.nFileSizeLow & UINT_MAX);
#elif (defined(Y_UNIX))
	//stat s;
//fstat()
#endif

	return YERR_SUCCESS;
}

int Y_file_readall(const YCHAR *file_path, char **content, uint64_t *size)
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

	FILE *f = _wfopen(file_path, YTEXT("r"));
	if(f == NULL)
	{
		return YERR_FAILED;
	}
	char *buf = (char *)Ycalloc(stat.length, 1);
	fread(buf, 1, stat.length, f);
	fclose(f);
	
	*size = stat.length;
	*content = buf;

	return YERR_SUCCESS;
}

void Y_file_free(const char *content)
{
	Yfree(content);
}



