/***********************************************************************************
 * @ file    : Ytcpcli.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2021.12.09 18：46
 * @ brief   : 一个简易的TCP客户端库，使用select模型监控socket事件
 ************************************************************************************/

#ifndef __YTCPCLI_H__
#define __YTCPCLI_H__

#include "Ybase.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct Ytcpcli_s Ytcpcli;

	typedef enum
	{
		Y_TCPCLI_EVT_CONNECTING,
		Y_TCPCLI_EVT_CONNECTED,

		// 当发送数据或者接受数据连续失败超过多次之后，会触发该事件
		Y_TCPCLI_EVT_DISCONNECTED,

		// 当可以从服务器读取数据的时候触发
		Y_TCPCLI_EVT_READ
	}Ytcpcli_event;

	// TCP客户端的事件回调函数
	typedef void (*Ytcpcli_event_callback)(Ytcpcli *ycli, Ytcpcli_event yevt, void *userdata);

	/*
	 * 描述：
	 * 初始化tcpcli对象
	 *
	 * 参数：
	 * @ipaddr：要连接的ip地址
	 * @port：要连接的端口号
	 * @userdata：事件回调里的用户自定义数据
	 *
	 * 返回值：
	 * tcpcli对象
	 */
	YAPI Ytcpcli *Y_create_tcpcli(const char *ipaddr, int port);

	/*
	 * 描述：
	 * 释放Ytcpcli对象
	 * 如果Tcp连接没有关闭，那么会先关闭tcp连接
	 *
	 * 参数：
	 * @ycli：要释放的对象
	 */
	YAPI void Y_delete_tcpcli(Ytcpcli *ycli);

	/*
	 * 描述：
	 * 连接服务器
	 * 注意，该函数为非阻塞函数，如果连不上，会尝试重新连接，一直到连上为止
	 * 连上后会通过事件回调通知调用者
	 *
	 * 参数：
	 * @ipaddr：要连接的ip地址
	 * @port：要连接的端口号
	 */
	YAPI void Y_tcpcli_connect(Ytcpcli *ycli);

	YAPI void Y_tcpcli_disconnect(Ytcpcli *ycli);

	YAPI void Y_tcpcli_set_event_callback(Ytcpcli *ycli, Ytcpcli_event_callback callback, void *userdata);

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
	 * 发送完成返回size，失败返回-1
	 */
	YAPI int Y_tcpcli_send(Ytcpcli *ycli, const char *data, size_t size);

	/*
	 * 描述：
	 * 从远程服务器接收指定大小的数据
	 * 如果接收到的数据大小小于size，那么会组塞住一直接收，直到收到了size个字节的时候才返回
	 * 你应该在触发了Y_TCPCLI_EVT_READ事件的时候再去调用这个函数，Y_TCPCLI_EVT_READ事件表示可以从socket里读取数据了
	 *
	 * 参数：
	 * @cli：tcpcli指针
	 * @data：保存接收的数据的缓冲区
	 * @size：要接收的数据大小
	 *
	 * 返回值：
	 * 接收失败返回-1，成功返回size
	 */
	YAPI int Y_tcpcli_recv(Ytcpcli *ycli, char *buf, size_t size);

#ifdef __cplusplus
}
#endif


#endif