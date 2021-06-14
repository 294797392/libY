#ifndef __TOOL_TCPCLI_H__
#define __TOOL_TCPCLI_H__

#include "toolbase.h"
#include "tool_net.h"

typedef struct tcpcli_s tcpcli;

typedef enum
{
    TCPCLI_EVT_CONNECTING,
    TCPCLI_EVT_CONNECTED,
    TCPCLI_EVT_DISCONNECTED,

    TCPCLI_EVT_RECVMSG
}TCPCLI_EVENT;

#ifdef __cplusplus
extern "C" {
#endif

    typedef int (*tcpcli_event_callback)(tcpcli *cli, TCPCLI_EVENT event, void *data, size_t size, void *userdata);

    /*
     * 描述：
     * 初始化tcpcli对象
     * 
     * 参数：
     * @ipaddr：要连接的ip地址
     * @port：要连接的端口号
     * 
     * 返回值：
     * tcpcli对象
     */
    TOOLSAPI tcpcli *new_tcpcli(const char *ipaddr, int port);

    /*
     * 描述：
     * 连接服务器
     * 注意，该函数为阻塞函数，如果连不上，会尝试重新连接，一直到连上为止
     *
     * 参数：
     * @ipaddr：要连接的ip地址
     * @port：要连接的端口号
     */
    TOOLSAPI void tcpcli_start(tcpcli *cli);

    /*
     * 描述：
     * 关闭tcp客户端连接
     */
    TOOLSAPI void tcpcli_stop(tcpcli *cli);

    TOOLSAPI void tcpcli_setopt(tcpcli *cli, TOOL_TCP_OPTION opt, char *data, size_t size);

    /*
     * 描述：
     * 关闭并释放tcp客户端连接
     */
    TOOLSAPI void free_tcpcli(tcpcli *cli);

    TOOLSAPI void tcpcli_set_event_callback(tcpcli *cli, tcpcli_event_callback callback, void *userdata);

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
    TOOLSAPI int tcpcli_send(tcpcli *cli, char *data, size_t size);

#ifdef __cplusplus
}
#endif


#endif