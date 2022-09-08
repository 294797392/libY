#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "Y.h"
#include "Yerrno.h"
#include "Ystring.h"
#include "Yfile.h"

FILE *Y_file_open(const YCHAR *file_path, const char *mode)
{
	return fopen(file_path, mode);
}

void Y_file_close(FILE *f)
{
	fclose(f);
}

size_t Y_file_size(const YCHAR *file_path)
{
	fstat()
}
