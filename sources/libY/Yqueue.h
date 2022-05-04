/***********************************************************************************
 * @ file    : Yqueue.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2021.12.06
 * @ brief   : 使用环形缓冲区实现一个生产者 - 消费者模型的队列
 * @ remark  ：如果入队的速度比出队的速度快，那么有可能会出现还没出队的元素被新的数据覆盖掉的情况
 ************************************************************************************/

#ifndef __YQUEUE_H__
#define __YQUEUE_H__

#include "Yfirstinclude.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Ybase.h"

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
	typedef struct Yqueue_s Yqueue;

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
	 * @itemsize：队列里每个元素的大小
	 *
	 * 返回值：
	 * Yqueue对象
	 *
	 * 不要直接调用Y_queue_enqueue和Y_queue_dequeue函数，Y_queue_start会自动调用该函数并通过回调的方式把element返回
	 * 除非你没有调用Y_queue_start，那么请永远不要直接调用这两个函数
	 */
	YAPI Yqueue *Y_create_queue(void *userdata, size_t itemsize);

	/*
	 * 描述：
	 * 删除队列并释放所占用的资源
	 *
	 * 参数：
	 * @q：要删除的队列
	 */
	YAPI void Y_delete_queue(Yqueue *yq);

	/*
	 * 描述：
	 * 创建一个线程并开始消费元素
	 *
	 * 参数：
	 * @q：要操作的队列对象
	 * @num_thread：消费者线程的数量
	 * @callback：每消费了一个元素，就会通过该回调回调给用户，用户可以在该回调里做操作
	 */
	YAPI void Y_queue_start(Yqueue *yq, int num_thread, Yqueue_callback callback);

	/*
	 * 描述：
	 * 设置当缓冲队列满了之后，溢出的元素如何处理的回调
	 *
	 * 参数：
	 * @yq：要设置的队列对象
	 * @callback：当缓冲区溢出的时候，回调溢出的元素
	 */
	YAPI void Y_queue_set_full_callback(Yqueue *yq, Yqueue_full_callback callback);

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
	YAPI void *Y_queue_dequeue(Yqueue *yq);

	/*
	 * 描述：
	 * 获取当前队列的大小
	 */
	YAPI int Y_queue_size(Yqueue *yq);

	/*
	 * 描述：
	 * 入队函数
	 * 返回环形缓冲区中下一个入队的元素的地址
	 * 如果环形缓冲区中下一个入队的元素的地址不为空，那么直接返回元素的地址，否则会先开辟一段内存然后再返回，这样做到内存重复利用的功能
	 * 注意该函数要和Y_queue_end_enqueue成对出现，一个常见的应用场景是：
	 * struct *s = (struct*)Y_queue_begin_enqueue(yq);
	 * s->i = 1;
	 * s->j = 2;
	 * Y_queue_end_enqueue(yq);
	 * 调用Y_queue_end_enqueue的时候会通知消费者线程开始消费下一个元素，所以在调用Y_queue_end_enqueue之前需要把你用到的数据准备好
	 *
	 * 参数：
	 * @yq：要入队的对象
	 *
	 * 返回值：
	 * 环形缓冲区中下一个入队的元素
	 */
	YAPI void *Y_queue_prepare_enqueue(Yqueue *yq);

	/*
	 * 描述：
	 * 通知消费者线程开始消费下一个元素
	 *
	 * 参数：
	 * @yq：要入队的对象
	 */
	YAPI void Y_queue_commit_enqueue(Yqueue *yq);

#ifdef __cplusplus
}
#endif

#endif

