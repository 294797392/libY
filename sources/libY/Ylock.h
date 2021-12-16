/***********************************************************************************
 * @ file    : Ylock.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2021.12.16 22:29
 * @ brief   : 封裝不同平台下的锁操作
 ************************************************************************************/
#ifndef __YLIST_H__
#define __YLIST_H__

#include "Ybase.h"


#ifdef __cplusplus
extern "C" {
#endif

    typedef struct Ylock_s Ylock;

    YAPI Ylock *Y_create_lock();

    /*
     * 描述：
     * 删除一把锁
     * 
     * 参数：
     * @yl：要删除的锁
     */
    YAPI void Y_delete_lock(Ylock *yl);

    /*
     * 描述：
     * 执行锁操作
     * 
     * 参数：
     * @yl：要锁住的锁
     */
    YAPI void Y_lock_lock(Ylock *yl);

    /*
     * 描述：
     * 执行解锁操作
     * 
     * 参数：
     * @yl：要解锁的锁
     */
    YAPI void Y_lock_unlock(Ylock *yl);

#ifdef __cplusplus
}
#endif

#endif