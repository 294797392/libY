#ifndef __TOOL_TREE_H__
#define __TOOL_TREE_H__

#include "toolbase.h"

#define MAX_CHILD_COUNT 1024

typedef struct tool_tree_s tool_tree;
typedef struct tool_treenode_s tool_treenode;

struct tool_treenode_s
{
    // 该节点保存的数据
    void *data;

    // 子节点列表
    tool_treenode *children[MAX_CHILD_COUNT];

    // 子节点数量
    int num_child;

    // 父节点
    tool_treenode *parent;
};

#ifdef __cplusplus
extern "C" {
#endif

    typedef void (*tree_free_func)(void *data);

	typedef int (*tree_foreach_action)(tool_tree *tree, tool_treenode *node, void *userdata);

    /*
     * 描述：
     * 实例化一棵树
     */
    TOOLSAPI tool_tree *new_tree();

    TOOLSAPI tool_tree *new_tree2(tree_free_func freefunc);

    TOOLSAPI tool_treenode *tree_initroot(tool_tree *tree, void *data);

    /*
     * 描述：
     * 对tree进行遍历操作
     * 
     * 参数：
     * @tree：tree对象
     * @foreach_action：遍历函数
     * @userdata：用户自定义数据
     */
    TOOLSAPI void tree_foreach(tool_tree *tree, tree_foreach_action foreach_action, void *userdata);

    /*
     * 描述：
     * 删除某个节点以及子节点
     * 
     * 参数：
     * @tree：tree对象
     * @node：要删除的节点
     */
    TOOLSAPI void tree_delete(tool_tree *tree, tool_treenode *node);

#ifdef __cplusplus
}
#endif

#endif