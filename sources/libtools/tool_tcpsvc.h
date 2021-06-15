#ifndef __TOOL_TCPSVC_H__
#define __TOOL_TCPSVC_H__

#include "toolbase.h"
#include "tool_net.h"

typedef struct tcpsvc_s tcpsvc;

// 定义一个客户端句柄
#define tcp_client OS_SOCKET

typedef enum
{
    TCPSVC_EVT_CLI_CONNECTED,
    TCPSVC_EVT_CLI_DISCONNECTED,
    TCPSVC_EVT_CLI_MESSAGE,

    // 超过了连接池的最大数量
    TCPSVC_EVT_CLI_FULL,

    // 服务器遇到错误（比如listen失败）
    TCPSVC_EVT_ERROR
}TCPSVC_EVENT;

#ifdef __cplusplus
extern "C" {
#endif

    typedef int (*tcpsvc_event_callback)(tcpsvc *svc, tcp_client cli, TCPSVC_EVENT event, void *data, size_t size, void *userdata);

    /*
     * 描述：
     * 初始化tcpsvc对象
     * 
     * 参数：
     * @ipaddr：要绑定的ip地址（绑定的网卡）,填NULL就使用ANY（0.0.0.0）
     * @port：要连接的端口号
     * 
     * 返回值：
     * tcpsvc对象
     */
    TOOLSAPI tcpsvc *new_tcpsvc(const char *ipaddr, int port);

    /*
     * 描述：
     * 连接服务器
     * 注意，该函数为阻塞函数，如果连不上，会尝试重新连接，一直到连上为止
     *
     * 参数：
     * @ipaddr：要连接的ip地址
     * @port：要连接的端口号
     */
    TOOLSAPI void tcpsvc_start(tcpsvc *svc);

    /*
     * 描述：
     * 关闭tcp客户端连接
     */
    TOOLSAPI void tcpsvc_stop(tcpsvc *svc);

    TOOLSAPI void tcpsvc_setopt(tcpsvc *svc, TOOL_TCP_OPTION opt, char *data, size_t size);

    /*
     * 描述：
     * 关闭并释放tcp客户端连接
     */
    TOOLSAPI void free_tcpsvc(tcpsvc *svc);

    TOOLSAPI void tcpsvc_set_event_callback(tcpsvc *svc, tcpsvc_event_callback callback, void *userdata);

    /*
     * 描述：
     * 向远程服务器发送数据
     *
     * 参数：
     * @cli：tcpcli指针
     * @data：要发送的数据
     * @size：要发送的数据大小
     *
     * 返回值：
     * 返回0表示发送成功，否则失败
     */
    TOOLSAPI int tcpsvc_sendto(tcpsvc *svc, tcp_client cli, char *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif