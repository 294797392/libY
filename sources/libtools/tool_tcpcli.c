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
#include "tool_tcpcli.h"

// 指定两千毫秒重连服务器
#define RECONNECT_INTERVAL  2000
// 接收数据的缓冲区大小
#define RECEIVE_BUF_SIZE    4096
#define DEFAULT_TERMINATOR  '\n'

// 指定最多接收5次数据，如果5次都接收失败，那么判断客户端断开连接
#define MAX_RECEIVE_TIMES   3

struct tcpcli_s
{
    int linebased;
    char terminator;
    int recv_times;

    OS_SOCKET fd;
    void *userdata;
    tcpcli_event_callback evt_cb;
    pthread_t worker_thread;
    int running;

    char ipaddr[64];
    int svcport;
};

static void notify_event(tcpcli *cli, TCPCLI_EVENT evt, void *data, size_t size)
{
    if (cli->evt_cb != NULL)
    {
        cli->evt_cb(cli, evt, data, size, cli->userdata);
    }
}

static void* worker_thread_proc(void *state)
{
    tcpcli *cli = (tcpcli*)state;

    struct sockaddr_in bdaddr;
	bdaddr.sin_family = PF_INET;
	bdaddr.sin_port = htons(cli->svcport);
	bdaddr.sin_addr.s_addr = inet_addr(cli->ipaddr);
    memset(&(bdaddr.sin_zero), 0, sizeof(bdaddr.sin_zero));

    // 首先通知外部模块，正在连接
    notify_event(cli, TCPCLI_EVT_CONNECTING, NULL, 0);

    // 连接指定服务器
    while (1)
    {
        int rc = connect(cli->fd, (struct sockaddr *)&bdaddr, sizeof(struct sockaddr_in));
        if (rc == -1)
        {
            printf("connect svc failed, %s:%d, retry...\n", cli->ipaddr, cli->svcport);
            os_sleep(RECONNECT_INTERVAL);
            continue;
        }
        else
        {
            notify_event(cli, TCPCLI_EVT_CONNECTED, NULL, 0);
            break;
        }
    }

    // 运行到这里说明连接服务器成功
    TLOGI("svc connect success..., %s:%d", cli->ipaddr, cli->svcport);

    // 开始接收数据
    while (1)
    {
        if(cli->linebased)
        {
            char buf[RECEIVE_BUF_SIZE] = {'\0'};
            int rd = net_read_until(cli->fd, buf, sizeof(buf), cli->terminator);
            if(rd == 0)
            {
                // 接收失败
                cli->recv_times++;
                if(cli->recv_times >= MAX_RECEIVE_TIMES)
                {
                    TLOGE("receive failed many times, disconnect");
                    net_closesocket(cli->fd);
                    notify_event(cli, TCPCLI_EVT_DISCONNECTED, NULL, 0);
                    break;
                }
                else
                {
                    TLOGW("receive failed has %d times...", cli->recv_times);
                    continue;
                }
            }

            // 接收数据成功
            cli->recv_times = 0;

            // 通知外部服务器
            notify_event(cli, TCPCLI_EVT_RECVMSG, buf, rd);
        }
        else
        {
            // not supported
        }
    }

    cli->running = 0;
    TLOGI("tcpcli thread exit");

    return NULL;
}

tcpcli *new_tcpcli(const char *ipaddr, int port)
{
    initnet();

    tcpcli *cli = (tcpcli*)calloc(1, sizeof(tcpcli));
    cli->fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    cli->svcport = port;
    strncpy(cli->ipaddr, ipaddr, sizeof(cli->ipaddr));
    return cli;
}

void tcpcli_start(tcpcli *cli)
{
    cli->running = 1;
    // 直接开启后台工作线程
    pthread_create(&cli->worker_thread, NULL, worker_thread_proc, cli);
    pthread_detach(cli->worker_thread);
}

void tcpcli_stop(tcpcli *cli)
{
    cli->running = 0;
    net_closesocket(cli->fd);
}

void tcpcli_setopt(tcpcli *cli, TOOL_TCP_OPTION opt, char *data, size_t size)
{
    if(opt == TCPOPT_LINE_BASED)
    {
        cli->linebased = 1;
        if(data == NULL)
        {
            // 默认使用\n换行符
            cli->terminator = DEFAULT_TERMINATOR;
        }
        else
        {
            cli->terminator = data[0];
        }
    }
}

void free_tcpcli(tcpcli *cli)
{
    if(cli != NULL)
    {
        cli->running = 0;
        net_closesocket(cli->fd);
        free(cli);
    }
}

void tcpcli_set_event_callback(tcpcli *cli, tcpcli_event_callback callback, void *userdata)
{
    cli->evt_cb = callback;
    cli->userdata = userdata;
}

int tcpcli_send(tcpcli *cli, char *data, size_t size)
{
    return net_write(cli->fd, data, size);
}
