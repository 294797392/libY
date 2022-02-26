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
#include "Ytcpcli.h"

#define SELECT_TIMEOUT_MSEC         500							// 指定select轮询间隔时间，单位毫秒
#define RECONNECT_INTERVAL			2000						// 指定重连服务器的间隔时间，单位毫秒
#define MAX_IPADDR_LEN				64
#define MAX_DATA_ERROR_TIMES		3							// 当接收或者发送数据失败超过3次后触发DISCONNECT回调
#define DEFAULT_CONSUMER_THREADS	5							// 最多同时有5个线程处理接收到的数据包		

#define TCP_SEND_OR_RECV(func)			if (func == 0)													\
										{																\
											ycli->error_times = 0;										\
											return size;												\
										}																\
										else															\
										{																\
											if (ycli->error_times > MAX_DATA_ERROR_TIMES)				\
											{															\
												Y_tcpcli_disconnect(ycli);								\
												notify_event(ycli, Y_TCPCLI_EVT_DISCONNECTED);			\
											}															\
											else														\
											{															\
												ycli->error_times++;									\
											}															\
											return -1;													\
										}																\

#define CATE    YTEXT("Ytcpcli")

typedef struct Ypacket_s
{
	char *buf;
}Ypacket;

struct Ytcpcli_s
{
	// 当前是否正在工作
	int working;

	Ythread *thread;
	Ysocket fd;
	void *userdata;
	Ytcpcli_event_callback evt_cb;

	char ipaddr[MAX_IPADDR_LEN];
	int svcport;

	// 当前接收或者发送数据的失败的次数
	int error_times;

	// 消费数据包的队列
	Yqueue *consume_packet_queue;
};


static void notify_event(Ytcpcli *ycli, Ytcpcli_event evt)
{
	if (ycli->evt_cb != NULL)
	{
		ycli->evt_cb(ycli, evt, ycli->userdata);
	}
}

static void worker_thread_entry(void *state)
{
	Ytcpcli *ycli = (Ytcpcli *)state;

	struct sockaddr_in bdaddr;
	bdaddr.sin_family = PF_INET;
	bdaddr.sin_port = htons(ycli->svcport);
	bdaddr.sin_addr.s_addr = inet_addr(ycli->ipaddr);
	memset(&(bdaddr.sin_zero), 0, sizeof(bdaddr.sin_zero));
	wchar_t wipaddr[MAX_IPADDR_LEN] = { '\0' };
	mbstowcs(wipaddr, ycli->ipaddr, MAX_IPADDR_LEN);

	// 首先通知外部模块，正在连接
	notify_event(ycli, Y_TCPCLI_EVT_CONNECTING);

	// 连接指定服务器
	while (ycli->working)
	{
		int rc = connect(ycli->fd, (struct sockaddr *)&bdaddr, sizeof(struct sockaddr_in));
		if (rc == -1)
		{
			YLOGCW(CATE, YTEXT("connect svc failed, %ls:%d, retry..."), wipaddr, ycli->svcport);
			Ysleep(RECONNECT_INTERVAL);
			continue;
		}
		else
		{
			notify_event(ycli, Y_TCPCLI_EVT_CONNECTED);
			break;
		}
	}

	// 运行到这里说明连接服务器成功
	YLOGCI(CATE, YTEXT("svc connect success..., %ls:%d"), wipaddr, ycli->svcport);

	// 使用selecte模型监控网络事件
	fd_set rdfds;
	FD_ZERO(&rdfds);
	FD_SET(ycli->fd, &rdfds);

	while (ycli->working)
	{
		fd_set readfds = rdfds;

#if (defined(Y_API_WIN32))
		int ret = select(0, &readfds, NULL, NULL, NULL);
#elif (defined(Y_API_UNIX))
		int ret = select(ycli->fd + 1, &readfds, NULL, NULL, NULL);
#endif
		if (ret == 0)
		{
			/* timeout */
			continue;
		}
		else if (ret < 0)
		{
			/* error */
			switch (errno)
			{
			case EINTR: // 等待时捕获了一个信号，可以重新发起调用
				continue;
			default:
				YLOGCE(CATE, YTEXT("select Ytcpcli failed, %d"), errno);
				break;
			}
		}
		else
		{
			/* one or more fd has io event */
			if (FD_ISSET(ycli->fd, &readfds))
			{
				// 先读取头部数据
				char head[4] = {'\0'};
				if(Y_read_socket(ycli->fd, head, sizeof(head)) < 0)
				{
					YLOGCE(CATE, YTEXT("receive data failed, %d"), Y_socket_error());
					break;
				}
			}
		}
	}

	ycli->working = 0;
	YLOGCI(CATE, YTEXT("tcpcli thread exit"));
}

static void consume_packet_handler(void *userdata, void *element)
{
	Ytcpcli *ycli = (Ytcpcli*)userdata;

	// 接收到的数据包
	Ypacket *ypkt = (Ypacket*)element;
}

Ytcpcli *Y_create_tcpcli(const char *ipaddr, int port)
{
	Ytcpcli *ycli = (Ytcpcli *)calloc(1, sizeof(Ytcpcli));
	ycli->fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	ycli->svcport = port;
	strncpy(ycli->ipaddr, ipaddr, sizeof(ycli->ipaddr));

	// 创建数据包消费队列
	ycli->consume_packet_queue = Y_create_queue(ycli, sizeof(Ypacket));
	Y_queue_start(ycli->consume_packet_queue, DEFAULT_CONSUMER_THREADS, consume_packet_handler);

	return ycli;
}

void Y_delete_tcpcli(Ytcpcli *ycli)
{
	Y_tcpcli_disconnect(ycli);
	free(ycli);
}

void Y_tcpcli_connect(Ytcpcli *ycli)
{
	// 直接开启后台工作线程
	if (ycli->working)
	{
		return;
	}

	ycli->working = 1;
	ycli->thread = Y_create_thread(worker_thread_entry, ycli);
}

void Y_tcpcli_disconnect(Ytcpcli *ycli)
{
	if (ycli->working == 0)
	{
		return;
	}

	ycli->working = 0;
	Y_close_socket(ycli->fd);		// 此时应该会接收失败或者连接失败
	Y_delete_thread(ycli->thread);	// 等待线程退出后再释放Ythread对象
}

void Y_tcpcli_set_event_callback(Ytcpcli *ycli, Ytcpcli_event_callback callback, void *userdata)
{
	ycli->evt_cb = callback;
	ycli->userdata = userdata;
}

int Y_tcpcli_send(Ytcpcli *ycli, const char *data, size_t size)
{
	TCP_SEND_OR_RECV(Y_write_socket(ycli->fd, data, size));
}
