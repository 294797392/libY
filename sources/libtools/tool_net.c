#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#ifdef UNIX
#else
#include <WinSock2.h>
#endif

#include "tool_net.h"

void initnet()
{
	#ifdef UNIX
	{
		/*
		 * 如果客户端socket已经关闭，服务端继续向socket写数据，内核会发出SIGPIPE信号。
		 * 默认的操作是终止进程，但该信号可以被捕获并处理。
		 * 设置SIG_IGN选项忽略该信号
		*/
		signal(SIGPIPE, SIG_IGN);
	}
    #else
	{
		WSADATA data;
		WSAStartup(MAKEWORD(2, 2), &data);
	}
    #endif
}

void net_closesocket(OS_SOCKET socket)
{
#ifdef UNIX
    shutdown(fd, SHUT_RDWR);
    close(fd);
#else
    shutdown(socket, SD_BOTH);
    closesocket(socket);
#endif
}

int net_read_until(OS_SOCKET fd, char *buf, size_t len, char terminator)
{
	size_t ret;
	int rd = 0;

	char c[2] = { '\0' };
	while ((ret = recv(fd, c, 1, 0)) != 0)
	{
		if (ret == -1)
		{
			if (errno == EINTR)
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
			if (c[0] == terminator)
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

int net_write(OS_SOCKET fd, void *data, size_t len)
{
	size_t ret, nr;
	size_t l = len;
	char *m = (char *)data;
	while (l != 0 && (ret = send(fd, m, l, 0)) != 0)
	{
		if (ret == -1)
		{
			if (errno == EINTR)
			{
				continue;
			}
			else
			{
				TLOGE("pushsvc send failed, %d", ret);
				return 1;
			}
		}

		l -= ret;
		m += ret;
	}
	return 0;
}
