#ifndef __TOOL_NET_H__
#define __TOOL_NET_H__

#include "toolbase.h"

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * 描述：
     * 初始化一个tcp服务器SOCKET
     * 
     * 参数：
     * @port：要绑定的端口号
     * 
     * 返回值：
     * 负数为错误码，正确返回socket
     */
    TOOLSAPI int create_tcpsvc_socket(int port);

    /*
     * 描述：
     * 初始化一个tcp客户端SOCKET
     */
    TOOLSAPI int create_tcpcli_socket();

#ifdef __cplusplus
}
#endif


#endif