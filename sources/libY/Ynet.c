#include "Yfirstinclude.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef Y_API_WIN32
#include <WinSock2.h>
#elif Y_API_UNIX
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#endif

#include "Ylog.h"
#include "Ynet.h"

// 如果是win32环境，那么要连接ws2_32库
#ifdef Y_ENV_WIN32
#pragma comment(lib, "Ws2_32.lib")
#endif

void Y_initnet()
{
#ifdef Y_API_WIN32
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
#elif Y_API_UNIX
	/*
	 * 如果客户端socket已经关闭，服务端继续向socket写数据，内核会发出SIGPIPE信号。
	 * 默认的操作是终止进程，但该信号可以被捕获并处理。
	 * 设置SIG_IGN选项忽略该信号
	*/
	signal(SIGPIPE, SIG_IGN);
#endif
}

void Y_close_socket(Ysocket s)
{
#ifdef Y_API_WIN32
	shutdown(s, SD_BOTH);
	closesocket(s);
#elif Y_API_UNIX
	shutdown(s, SHUT_RDWR);
	close(s);
#endif
}

int Y_read_socket(Ysocket s, char *data, size_t len)
{
	size_t ret;
	size_t l = len;
	char *m = data;
	while (l != 0 && (ret = recv(s, m, l, 0)) != 0)
	{
		if (ret == -1)
		{
			if (errno == EINTR)
			{
				continue;
			}
			else
			{
				YLOGE(YTEXT("Y_read_socket failed, %ld"), ret);
				return -1;
			}
		}

		l -= ret;
		m += ret;
	}
	return 0;
}

int Y_write_socket(Ysocket s, char *data, size_t len)
{
	size_t ret;
	size_t l = len;
	char *m = data;
	while (l != 0 && (ret = send(s, m, l, 0)) != 0)
	{
		if (ret == -1)
		{
			if (errno == EINTR)
			{
				continue;
			}
			else
			{
				YLOGE(YTEXT("Y_write_socket failed, %ld"), ret);
				return -1;
			}
		}

		l -= ret;
		m += ret;
	}
	return 0;
}

int Y_socket_error()
{
#ifdef Y_API_WIN32
	return WSAGetLastError();
#elif Y_API_UNIX
	return errno;
#endif
}

Ysocket Y_create_tcp_svc(const char *bindto, int port)
{
	Ysocket s = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in bdaddr;
	bdaddr.sin_family = PF_INET;
	bdaddr.sin_port = htons(port);
	bdaddr.sin_addr.s_addr = bindto == NULL ? INADDR_ANY : inet_addr(bindto);
	memset(&(bdaddr.sin_zero), 0, sizeof(bdaddr.sin_zero));

	// 绑定到指定网络接口
	if (bind(s, (struct sockaddr *)&bdaddr, sizeof(struct sockaddr)) < 0)
	{
		YLOGE(YTEXT("Y_create_tcp_svc bind failed, %d"), Y_socket_error());
		Y_close_socket(s);
		return -1;
	}

	if (listen(s, 5) < 0)
	{
		YLOGE(YTEXT("Y_create_tcp_svc bind failed, %d"), Y_socket_error());
		Y_close_socket(s);
		return -1;
	}

	return s;
}



