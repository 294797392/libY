﻿#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#if (defined(Y_WIN32))
#include <Windows.h>
#elif (defined(Y_UNIX))
#include <unistd.h>
#endif

#include "Y.h"

void Ysleep(int mseconds)
{
	#if (defined(Y_WIN32))
    Sleep(mseconds);
	#elif (defined(Y_UNIX))
    sleep(mseconds / 1000);
	#endif
}

#ifdef UNIX

int fd_read_until(int fd, char *buf, size_t len, char terminator)
{
    size_t ret;
	int rd = 0;

	char c[2] = {'\0'};
	while((ret = read(fd, c, 1)) != 0)
	{
		if(ret == -1)
		{
			if(errno == EINTR)
			{
				continue;
			}
			else
			{
				TLOGE("fd_read_line failed, %d", errno);
				return 0;
			}
		}
		else
		{
			if(c[0] == terminator)
			{
				return rd;
			}
			//if(c[0] != '\0')
			else
			{
				memcpy(buf + rd, c, 1);
				rd++;
			}
		}
	}

	return rd;
}

int fd_write(int fd, void *data, size_t len)
{
	size_t ret, nr;
	size_t l = len;
	char *m = (char*)data;
	while(l != 0 && (ret = write(fd, m, l)) != 0)
	{
		if(ret == -1)
		{
			if(errno == EINTR)
			{
				continue;
			}
			else
			{
				TLOGE("fd_write failed, %ld", ret);
				return 1;
			}
		}

		l -= ret;
		m += ret;
	}
	return 0;
}

#endif

int getflag(int value, int flag)
{
	return (value & flag) == flag ? 1 : 0;
}

int setflag(int value, int flag)
{
	return value |= flag;
}