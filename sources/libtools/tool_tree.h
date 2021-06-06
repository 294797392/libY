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

	typedef int (*tree_foreach_action)(tool_tree *tree, tool_treenode *node, void *userdata);

    TOOLSAPI tool_tree *new_tree();

    TOOLSAPI tool_treenode *tree_initroot(tool_tree *tree, void *data);

    TOOLSAPI void tree_foreach(tool_tree *tree, tree_foreach_action foreach_action, void *userdata);

#ifdef __cplusplus
}
#endif

#endif