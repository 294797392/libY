#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#ifdef UNIX
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#else
#include <WinSock2.h>
#endif

// 增加pthread的库引用
#ifdef WIN32
#pragma comment(lib, "pthreadVC2.lib")
#pragma comment(lib, "pthreadVCE2.lib")
#pragma comment(lib, "pthreadVSE2.lib")
#endif

#include "toolbase.h"
#include "tool_net.h"
#include "tool_tcpsvc.h"

// 轮询socket状态间隔
#define SELECT_TIMEOUT_MSEC         20
#define DEFAULT_CLI_FDS             1024
#define INVALID_CLI_FD              -1
#define MAX_RECEIVE_BUF_SIZE        4096
#define DEFAULT_TERMINATOR          '\n'

struct tcpsvc_s
{
	int linebased;
	char terminator;

	// 服务器支持的客户端连接数
	int max_cli;
	// 当前已连接的数量
	int num_cli;
	// 服务器socket文件描述符
	OS_SOCKET fd;

	// 是否正在运行
	int running;
	// 服务器主线程
	pthread_t worker_thread;

	tcpsvc_event_callback evt_cb;
	void *userdata;

	// 可以保存的最大的客户端文件描述符数量
	OS_SOCKET clifds[DEFAULT_CLI_FDS];
	int last_fdidx;  // 最后一个文件描述符索引
	OS_SOCKET maxfd;	// maximum of client socket fd
};

static void notify_event(tcpsvc *svc, tcp_client cli, TCPSVC_EVENT evt, void *data, size_t size)
{
	if (svc->evt_cb != NULL)
	{
		svc->evt_cb(svc, cli, evt, data, size, svc->userdata);
	}
}

static OS_SOCKET maxfd(tcpsvc *svc)
{
	OS_SOCKET max = svc->fd;

	for (size_t i = 0; i <= last_fdidx; i++)
	{
		OS_SOCKET fd = svc->clifds[i];
		if(fd == INVALID_CLI_FD)
		{
			continue;
		}

		if(fd > max)
		{
			max = fd;
		}
	}

	return max;
}

static OS_SOCKET new_client(tcpsvc *svc)
{
	struct sockaddr_in client_addr;
	unsigned int sin_size = sizeof(struct sockaddr_in);
	OS_SOCKET clientfd = 0;
	if ((clientfd = accept(svc->fd, (struct sockaddr *)&client_addr, &sin_size)) == -1)
	{
		TLOGE("accept clinet failed, %d", errno);
		return -1;
	}

	/* 使用inet_ntop函数和ntohs函数转换客户端IP地址和端口号 */
	//char ipaddr[64] = {'\0'};
	//inet_ntop(AF_INET, &client_addr.sin_addr, ipaddr, INET6_ADDRSTRLEN);
	//int port = ntohs(client_addr.sin_port);

	//TLOGI("client connected, ipaddr=%s, port=%d\n", ipaddr, port);
	return clientfd;
}

static int add_clifd(tcpsvc *svc, OS_SOCKET clifd)
{
	int len = sizeof(svc->clifds) / sizeof(OS_SOCKET);
	for (int i = 0; i < len; i++)
	{
		if (svc->clifds[i] == INVALID_CLI_FD)
		{
			svc->clifds[i] = clifd;

			if (i > svc->last_fdidx)
			{
				svc->last_fdidx = i;
			}
			return 1;
		}
	}

	return 0;
}

static int process_clifds(tcpsvc *svc, fd_set *rdfds, fd_set *exfds)
{
	// 是否要更新FD_SET的标志
	int should_update = 0;

	for (int i = 0; i <= svc->last_fdidx; i++)
	{
		OS_SOCKET fd = svc->clifds[i];
		if (fd == INVALID_CLI_FD)
		{
			continue;
		}

		if (FD_ISSET(fd, rdfds))
		{
			// 有数据发送过来，开始读取
			if (svc->linebased)
			{
				char buf[MAX_RECEIVE_BUF_SIZE] = { '\0' };
				int rd = net_read_until(fd, buf, sizeof(buf), svc->terminator);
				if (rd == 0)
				{
					// 接收数据失败，直接关闭客户端
					net_closesocket(fd);
					svc->num_cli--;
					svc->clifds[i] = INVALID_CLI_FD;
					notify_event(svc, 0, TCPSVC_EVT_CLI_DISCONNECTED, NULL, 0);
					should_update = 1;
				}
				else
				{
					notify_event(svc, fd, TCPSVC_EVT_CLI_MESSAGE, buf, rd);
				}
			}
		}

		if (FD_ISSET(fd, exfds))
		{
			// 客户端有异常（断开连接？要在不同操作系统试一下）
			TLOGI("client %d except", fd);
		}
	}

	return should_update;
}

// 清除不用监控的fds
static void cleanup_clifds(tcpsvc *svc, FD_SET *rdfds, FD_SET *wrfds, FD_SET *exfds)
{
	FD_ZERO(rdfds);
	FD_ZERO(wrfds);
	FD_ZERO(exfds);
	FD_SET(svc->fd, rdfds);
	for (int i = 0; i <= svc->last_fdidx; i++)
	{
		OS_SOCKET fd = svc->clifds[i];
		if (fd != INVALID_CLI_FD)
		{
			FD_SET(fd, rdfds);
			FD_SET(fd, exfds);
		}
	}
}

static void *worker_thread_proc(void *state)
{
	tcpsvc *svc = (tcpsvc *)state;

	if (listen(svc->fd, 5) == -1)
	{
		TLOGE("tcpsvc listen failed, %d", errno);
		return NULL;
	}

	fd_set rdfds;
	fd_set wrfds;
	fd_set exfds;

	/*
	 * FD_ZERO:从指定集合中删除所有的文件描述符
	 * 在声明了一个描述符集之后，必须用FD_ZERO将这个描述符集置为0，然后在其中设置我们关心的描述符
	 */
	FD_ZERO(&rdfds);
	FD_ZERO(&wrfds);
	FD_ZERO(&exfds);

	/* 监控服务端socket可读，当有客户端连接时，socket改变为可读状态 */
	FD_SET(svc->fd, &rdfds);

	/* 最大的文件描述符的值，用来当做select函数的第一个参数 */
	svc->maxfd = svc->fd;

	while (1)
	{
		int ret;

		/*
		 * 为提升效率，可以把要监控的文件描述符缓存起来
		 * 在调用select之前，赋值给一个临时变量直接传递给select函数就可以
		 */
		fd_set readfds = rdfds;
		fd_set writefds = wrfds;
		fd_set exceptfds = exfds;

		/* 设置等待文件描述符事件的超时时间 */
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = SELECT_TIMEOUT_MSEC * 1000;

		/*
		 * select函数的作用：当任何一个文件描述符I/O就绪时进行通知
		 * maxfdp1:
		 * 		最大描述符的值加1，通过指定这个值，内核就只会监控在此范围内的描述符，而不会监控超过此范围的描述符
		 * 		windows系统下，这个参数填0就可以
		 * tvptr：
		 * 		等待文件描述符事件的超时时间。
		 * 		tvptr == NULL：永远等待。如果捕捉到一个信号，则select返回-1，errno被设置为EINTR
		 */
#ifdef WIN32
		ret = select(0, &readfds, &writefds, &exceptfds, &tv);
#else
		ret = select(svc->maxfd + 1, &readfds, &writefds, &exceptfds, &tv);
#endif
		if (ret == 0)
		{
			/* timeout */
			continue;
		}
		else if (ret < 0)
		{
			/* error */
			TLOGE("socket select failed, %d", WSAGetLastError());
			switch (errno)
			{
				case EBADF: // 存在非法文件描述符
					break;

				case EINTR: // 等待时捕获了一个信号，可以重新发起调用
					continue;

				case EINVAL: // 参数非法，第一个参数是负数或者超时时间非法
					break;

				case ENOMEM: // 没有内存了
					break;
			}
		}
		else
		{
			/* one or more fd has io event */

			/*
			 * FD_ISSET检查一个文件描述符是否在给定集合中。如果在，则返回非0值，否则返回0.
			 * 服务端socket文件描述符可读，说明有客户端连接
			 */
			if (FD_ISSET(svc->fd, &readfds))
			{
				OS_SOCKET clifd = new_client(svc);
				if (clifd == -1)
				{
					// 接收客户端连接的时候发生错误
				}
				else
				{
					// 超出了最大连接数
					if (svc->num_cli >= svc->max_cli || !add_clifd(svc, clifd))
					{
						// 没有位置了
						notify_event(svc, clifd, TCPSVC_EVT_CLI_FULL, NULL, 0);
						TLOGI("tcpsvc is full, disconnect client");
						// 断开客户端连接
						net_closesocket(clifd);
					}
					else
					{
						// 接收客户端连接成功
						svc->num_cli++;

						// 刷新监控描述符
						cleanup_clifds(svc, &rdfds, &wrfds, &exfds);
						svc->maxfd = maxfd(svc);

						// 通知外部模块
						notify_event(svc, clifd, TCPSVC_EVT_CLI_CONNECTED, clifd, sizeof(OS_SOCKET));
					}
				}
			}

			if (process_clifds(svc, &readfds, &exceptfds))
			{
				// 到这里表示需要刷新FD_SET
				cleanup_clifds(svc, &rdfds, &wrfds, &exfds);
				svc->maxfd = maxfd(svc);
			}
		}
	}

	return NULL;
}

tcpsvc *new_tcpsvc(const char *ipaddr, int port)
{
	initnet();

	tcpsvc *svc = (tcpsvc *)calloc(1, sizeof(tcpsvc));
	memset(svc->clifds, INVALID_CLI_FD, sizeof(svc->clifds));
	svc->max_cli = DEFAULT_CLI_FDS;

	// 创建socket实例
	svc->fd = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in bdaddr;
	bdaddr.sin_family = PF_INET;
	bdaddr.sin_port = htons(port);
	bdaddr.sin_addr.s_addr = ipaddr == NULL ? INADDR_ANY : inet_addr(ipaddr);
	memset(&(bdaddr.sin_zero), 0, sizeof(bdaddr.sin_zero));

	// 绑定到指定网络接口
	if (bind(svc->fd, (struct sockaddr *)&bdaddr, sizeof(struct sockaddr)) == -1)
	{
		TLOGE("tcpsvc bind failed, %d", errno);
		free(svc);
		return NULL;
	}

	return svc;
}

void tcpsvc_start(tcpsvc *svc)
{
	svc->running = 1;
	// 直接开启后台工作线程
	pthread_create(&svc->worker_thread, NULL, worker_thread_proc, svc);
	pthread_detach(svc->worker_thread);
}

void tcpsvc_stop(tcpsvc *svc)
{
	svc->running = 0;
	net_closesocket(svc->fd);
}

void tcpsvc_setopt(tcpsvc *svc, TOOL_TCP_OPTION opt, char *data, size_t size)
{
	if (opt == TCPOPT_LINE_BASED)
	{
		svc->linebased = 1;
		if (data == NULL)
		{
			// 默认使用\n换行符
			svc->terminator = DEFAULT_TERMINATOR;
		}
		else
		{
			svc->terminator = data[0];
		}
	}
	else if (opt == TCPOPT_MAX_CLI)
	{
		memcpy(&svc->max_cli, data, size);
	}
}

void free_tcpsvc(tcpsvc *svc)
{
	if (svc != NULL)
	{
		svc->running = 0;
		net_closesocket(svc->fd);
		free(svc);
	}
}

void tcpsvc_set_event_callback(tcpsvc *svc, tcpsvc_event_callback callback, void *userdata)
{
	svc->evt_cb = callback;
	svc->userdata = userdata;
}

int tcpsvc_sendto(tcpsvc *svc, tcp_client cli, char *data, size_t size)
{
	return net_write(cli, data, size);
}


