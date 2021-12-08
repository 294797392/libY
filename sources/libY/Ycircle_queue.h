/************************************************************************************
 * @ file    : Ycircle_queue.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2021.12.08 12:53
 * @ brief   : 实现一个环形队列缓冲区，该环形队列复用每个元素所开辟的内存空间
 ************************************************************************************/
#ifndef __YCIRCLE_QUEUE_H__
#define __YCIRCLE_QUEUE_H__

#include "Ybase.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct Ycircle_queue_s Ycircle_queue;


	/*
     * 描述：
     * 创建一个环形缓冲区队列
     * 
     *
     * 参数：
     * @yq：要入队的对象
     *
     * 返回值：
     * 环形缓冲区中下一个入队的元素
     */
    YAPI Ycircle_queue *Y_create_circle_queue(int size);

    YAPI void *Y_circle_queue_dequeue();

#ifdef __cplusplus
    extern "C" {
#endif


#endif

