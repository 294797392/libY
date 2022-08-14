/***********************************************************************************
 * @ file    : Ybstree.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.08.11
 * @ brief   : 实现一个二叉搜索树
 * 
 * 二叉搜索树的性质：
 * 1. 每个左边节点的值都小于父节点的值
 * 2. 每个右边节点的值都大于父节点的值
 * https://www.bilibili.com/video/BV1yB4y1v792?spm_id_from=333.337.search-card.all.click&vd_source=ba260aa99c5032fd90d2cf048781b8ce
 ************************************************************************************/
#ifndef __YBSTREE_H__
#define __YBSTREE_H__

#include "Y.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct ybstree_node_s ybstree_node;

    typedef struct Ybstree_s Ybstree;

    YAPI Ybstree *Y_create_bstree();

    /*
     * 描述：
     * 释放一个集合
     * 如果集合里当前有元素并且你设置了Y_lits_free_func，那么这个函数会帮你调用freefunc并释放集合对象
     * 
     * 参数：
     * @yl：要操作的集合
     */
    YAPI void Y_delete_bstree(Ybstree *ybst);

    /*
     * 描述：
     * 往集合里插入一个元素
     * 如果集合的当前空间不够存储新的元素，那么会自动扩充空间，新扩充的空间的大小是当前大小的两倍
     * 
     * 参数：
     * @yl：要操作的集合
	 * @item：要插入的元素
     */
    YAPI void Y_bstree_add_node(Ybstree *ybst, int value);

    /*
     * 描述：
     * 从二叉搜索树里删除一个节点
     * 
     * 参数：
     * @yl：要操作的集合
	 * @item：要删除的节点
     */
    YAPI void Y_bstree_remove_node(ybstree *ybst, int value);

#ifdef __cplusplus
}
#endif

#endif