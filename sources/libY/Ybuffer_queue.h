/***********************************************************************************
 * @ file    : Yqueue.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2021.12.06
 * @ brief   : 使用环形缓冲区实现一个生产者 - 消费者模型的队列
 * @ remark  ：如果入队的速度比出队的速度快，那么有可能会出现还没出队的元素被新的数据覆盖掉的情况
 ************************************************************************************/

#ifndef __YBUFFER_QUEUE_H__
#define __YBUFFER_QUEUE_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Y.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef enum
	{
		YQUEUE_STATE_IDLE,
		YQUEUE_STATE_RUNNING
	}Yqueue_state;

	typedef void(*Yqueue_callback)(void *userdata, void *element);
	typedef void(*Yqueue_full_callback)(void *element, void *userdata);
	typedef struct Ybuffer_queue_s Ybuffer_queue;

	/*
	 * 描述：
	 * 创建一个缓冲队列
	 * 该队列使用单独的线程消费元素，并提供消费者callback给调用者使用
	 * 入队和出队是在不同线程进行，互不影响
	 * 队列有着固定的大小，类似于环形缓冲区
	 * 如果队列满了而此时还在继续入队，那么丢弃最早入队的元素，并把入队的元素从头开始插入
	 *
	 * 参数：
	 * @userdata：用户自定义对象，在回调里会当成参数回调给用户
	 *
	 * 返回值：
	 * Yqueue对象
	 *
	 * 不要直接调用Y_queue_enqueue和Y_queue_dequeue函数，Y_queue_start会自动调用该函数并通过回调的方式把element返回
	 * 除非你没有调用Y_queue_start，那么请永远不要直接调用这两个函数
	 */
	YAPI Ybuffer_queue *Y_create_buffer_queue(void *userdata);

	/*
	 * 描述：
	 * 删除队列并释放所占用的资源
	 *
	 * 参数：
	 * @q：要删除的队列
	 */
	YAPI void Y_delete_buffer_queue(Ybuffer_queue *yq);

	/*
	 * 描述：
	 * 创建一个线程并开始消费元素
	 *
	 * 参数：
	 * @q：要操作的队列对象
	 * @num_thread：消费者线程的数量
	 * @callback：每消费了一个元素，就会通过该回调回调给用户，用户可以在该回调里做操作
	 */
	YAPI void Y_buffer_queue_start(Ybuffer_queue *yq, int num_thread, Yqueue_callback callback);

	/*
	 * 描述：
	 * 设置当缓冲队列满了之后，溢出的元素如何处理的回调
	 *
	 * 参数：
	 * @yq：要设置的队列对象
	 * @callback：当缓冲区溢出的时候，回调溢出的元素
	 */
	YAPI void Y_buffer_queue_set_full_callback(Ybuffer_queue *yq, Yqueue_full_callback callback);

	/*
	 * 描述：
	 * 出队函数
	 *
	 * 参数：
	 * @q：要出队的对象
	 *
	 * 返回值：
	 * 出队的对象
	 */
	YAPI void *Y_buffer_queue_dequeue(Ybuffer_queue *yq);

	/*
	 * 描述：
	 * 入队函数
	 *
	 * 参数：
	 * @q：要入队的对象
	 * 
	 * 返回值：
	 * 0表示成功，否则参考错误码
	 */
	YAPI void Y_buffer_queue_enqueue(Ybuffer_queue *yq, void *element);

	/*
	 * 描述：
	 * 获取当前队列的大小
	 */
	YAPI int Y_buffer_queue_size(Ybuffer_queue *yq);


	/*
	 * 描述：
	 * 清空缓冲队列里的元素
	 */
	YAPI void Y_buffer_queue_clear(Ybuffer_queue *yq);

#ifdef __cplusplus
}
#endif

#endif

