#include "Yfirstinclude.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#if (defined(Y_API_WIN32))
#include <WinSock2.h>
#elif (defined(Y_API_UNIX))
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#endif

#include "Ynet.h"
#include "Ylog.h"
#include "Ybase.h"
#include "Ythread.h"
#include "Yqueue.h"
#include "Ytcp.h"
#include "Ytcpsvc.h"

// 轮询socket状态间隔
#define SELECT_TIMEOUT_MSEC         20
#define DEFAULT_CLI_FDS             1024
#define INVALID_CLI_FD              -1
#define MAX_RECEIVE_BUF_SIZE        4096
#define DEFAULT_TERMINATOR          '\n'

struct Ytcpsvc_s
{
	int linebased;
	char terminator;

	int port;			// 服务监听的端口号

	// 服务器socket文件描述符
	Ysocket fd;

	// 是否正在运行
	int running;
	// 服务器主线程
	Ythread *accept_thread;

	Ytcpsvc_event_callback callback;
	void *userdata;

	// 可以保存的最大的客户端文件描述符数量
	Ysocket clients[DEFAULT_CLI_FDS];
};

typedef struct Ytcpcli_s
{
	Ysocket fd;
	Ytcpsvc *svc;
}Ytcpcli;

static void notify_event(Ytcpsvc *svc, Ysocket cli,  Ytcpsvc_event event, void *data, size_t datasize)
{
	if (svc->callback != NULL)
	{
		svc->callback(svc, cli, event, data, datasize, svc->userdata);
	}
}

static void client_thread_proc(void *state)
{
	Ytcpcli *client = (Ytcpcli*)state;

	while(1)
	{
		// fixme:需要设计接收缓冲区模型
		Ypacket *packet = (Ypacket*)calloc(1, sizeof(Ypacket));
		Y_tcp_receive_packet(client->fd, packet);
		notify_event(client->svc, client->fd, Y_TCPSVC_EVT_PACKET_RECEIVED, packet, 0);
		free(packet);
	}
}

static void accept_thread_proc(void *state)
{
	Ytcpsvc *svc = (Ytcpsvc *)state;

	if (listen(svc->fd, 5) == -1)
	{
		YLOGE(YTEXT("tcpsvc listen failed, %d"), Y_socket_error());
		return;
	}

	YLOGI(YTEXT("tcpsvc started, port = %d"), svc->port);

	while (1)
	{
		struct sockaddr_in client_addr;
		unsigned int sin_size = sizeof(struct sockaddr_in);
		Ysocket client = 0;
		if ((client = accept(svc->fd, (struct sockaddr *)&client_addr, &sin_size)) == -1)
		{
			YLOGE(YTEXT("accept clinet failed, %d"), Y_socket_error());
			continue;
		}

		// fixme:需要创建一个client结构体来保存客户端的各种数据，包括线程句柄，以便在不使用的时候释放掉
		// 时间有限，这里先暴力开启一个线程读写..
		Ytcpcli *cli = (Ytcpcli*)calloc(1, sizeof(Ytcpcli));
		cli->fd = client;
		cli->svc = svc;
		Y_create_thread(client_thread_proc, cli);
	}
}

Ytcpsvc *Y_create_tcpsvc(const char *ipaddr, int port)
{
	Y_initnet();

	Ytcpsvc *svc = (Ytcpsvc *)calloc(1, sizeof(Ytcpsvc));

	// 创建socket实例
	svc->fd = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in bdaddr;
	bdaddr.sin_family = PF_INET;
	bdaddr.sin_port = htons(port);
	bdaddr.sin_addr.s_addr = ipaddr == NULL ? INADDR_ANY : inet_addr(ipaddr);
	memset(&(bdaddr.sin_zero), 0, sizeof(bdaddr.sin_zero));

	svc->port = port;

	// 绑定到指定网络接口
	if (bind(svc->fd, (struct sockaddr *)&bdaddr, sizeof(struct sockaddr)) == -1)
	{
		YLOGE(YTEXT("tcpsvc bind failed, %d"), Y_socket_error());
		free(svc);
		return NULL;
	}

	return svc;
}

void Y_delete_tcpsvc(Ytcpsvc *svc)
{
	if (svc != NULL)
	{
		svc->running = 0;
		Y_close_socket(svc->fd);
		free(svc);
	}
}

void Y_tcpsvc_start(Ytcpsvc *svc)
{
	svc->running = 1;
	// 直接开启后台工作线程
	svc->accept_thread = Y_create_thread(accept_thread_proc, svc);
}

void Y_tcpsvc_stop(Ytcpsvc *svc)
{
	svc->running = 0;
	Y_close_socket(svc->fd);
	Y_delete_thread(svc->accept_thread);
}

void Y_tcpsvc_set_event_callback(Ytcpsvc *svc, Ytcpsvc_event_callback callback, void *userdata)
{
	svc->callback = callback;
	svc->userdata = userdata;
}
