#ifndef __TOOL_NET_H__
#define __TOOL_NET_H__

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

#include "toolbase.h"


#ifdef WIN32
#pragma comment(lib, "Ws2_32.lib")
#endif

#ifdef UNIX
#define OS_SOCKET int
#else
#define OS_SOCKET SOCKET
#endif

typedef enum tcp_opt_e
{
    TCPOPT_LINE_BASED = 1,

    // 连接池最大的客户端数量
    TCPOPT_MAX_CLI = 2
}TOOL_TCP_OPTION;

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * 描述：
     * 初始化网络环境
     */
    TOOLSAPI void initnet();

    /*
     * 描述：
     * 断开并释放socket连接
     * 
     * 参数：
     * @socket：socket句柄
     */
    TOOLSAPI void net_closesocket(OS_SOCKET s);

    TOOLSAPI int net_read_until(OS_SOCKET s, char *buf, size_t len, char terminator);

    /*
     * 描述：
     * 向一个文件描述符里写入数据，直到写完len大小或者失败才返回
     *
     * 参数：
     * @fd：要写入的文件描述符
     * @data：要写入的数据地址
     * @len：要写入的数据大小
     *
     * 返回值：
     * 写入成功返回0，写入失败返回1
     */
    TOOLSAPI int net_write(OS_SOCKET s, void *data, size_t len);

    /*
     * 描述：
     * 获取不同平台下的socket错误编码
     */
    #ifdef UNIX
        #define net_error errno
    #else
        #define WSAGetLastError();
    #endif

#ifdef __cplusplus
}
#endif


#endif