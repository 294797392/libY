/***********************************************************************************
 * @ file    : Ytcpsvc.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.02.27 14:59
 * @ brief   : 一个简易的TCP服务器库，每个客户端请求来了之后新开了一个线程去读写数据
 * @ remark  ：Ytcpsvc和Ytcpcli要配合使用
 ************************************************************************************/

#ifndef __YTCPSVC_H__
#define __YTCPSVC_H__

#include "Ybase.h"
#include "Ynet.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct Ytcpsvc_s Ytcpsvc;

    typedef enum
    {
        Y_TCPSVC_EVT_CLI_CONNECTED,
        Y_TCPSVC_EVT_CLI_DISCONNECTED,
        Y_TCPSVC_EVT_PACKET_RECEIVED,

        // 超过了连接池的最大数量
        Y_TCPSVC_EVT_CLI_FULL,

        // 服务器遇到错误（比如listen失败）
        Y_TCPSVC_EVT_ERROR
    }Ytcpsvc_event;

    typedef int (*Ytcpsvc_event_callback)(Ytcpsvc *svc, Ysocket cli, Ytcpsvc_event event, void *data, size_t datasize, void *userdata);

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
    YAPI Ytcpsvc *Y_create_tcpsvc(const char *ipaddr, int port);

    /*
     * 描述：
     * 关闭并释放tcp客户端连接
     */
    YAPI void Y_delete_tcpsvc(Ytcpsvc *svc);

    /*
     * 描述：
     * 连接服务器
     * 注意，该函数为阻塞函数，如果连不上，会尝试重新连接，一直到连上为止
     *
     * 参数：
     * @ipaddr：要连接的ip地址
     * @port：要连接的端口号
     */
    YAPI void Y_tcpsvc_start(Ytcpsvc *svc);

    /*
     * 描述：
     * 关闭tcp客户端连接
     */
    YAPI void Y_tcpsvc_stop(Ytcpsvc *svc);

    YAPI void Y_tcpsvc_set_event_callback(Ytcpsvc *svc, Ytcpsvc_event_callback callback, void *userdata);

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
    YAPI int Y_tcpsvc_sendto(Ytcpsvc *svc, Ysocket cli, char *data, size_t datasize);

#ifdef __cplusplus
}
#endif

#endif