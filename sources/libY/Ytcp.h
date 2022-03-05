/***********************************************************************************
 * @ file    : Ytcp.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.02.26 09:32
 * @ brief   : 定义tcp框架（包含服务器和客户端）的共用的一些函数、宏定义...
 ************************************************************************************/

#ifndef __YTCP_H__
#define __YTCP_H__

#include "Ybase.h"
#include "Ynet.h"

/******************************************************
 * TCP报文格式：
 * 1 - 4    数据包序号
 * 5 - 8    指令类型（如果有）
 * 9 - 12   数据大小
 * 13 - 31  预留
 ******************************************************/

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct Ypacket_s
    {
        int seq;            // 数据包序号
        int cmd;            // 命令类型
		int code;			// 返回结果
        char *data;         // 数据
        int size;        // 数据大小
    }Ypacket;

	/*
	 * 描述：
	 * 收一个完整的数据包
	 *
	 * 参数：
	 * @ipaddr：要连接的ip地址
	 * @port：要连接的端口号
	 * @userdata：事件回调里的用户自定义数据
	 *
	 * 返回值：
	 * tcpcli对象
	 */
	YAPI int Y_tcp_receive_packet(Ysocket fd, Ypacket *packet);

	YAPI int Y_tcp_send_packet(Ysocket fd, int seq, int cmd, int code, char *data, int datasize);

#ifdef __cplusplus
}
#endif

#endif
