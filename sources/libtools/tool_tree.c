#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tool_tree.h"

struct tool_tree_s
{
    // 根节点
    tool_treenode *root;

    int foreach_status;
};

tool_tree *new_tree()
{
    tool_tree *tree = (tool_tree*)calloc(1, sizeof(tool_tree));
    return tree;
}

static void foreach_node(tool_tree *tree, tool_treenode *parent, tree_foreach_action foreach_action, void *userdata)
{
    if(parent == NULL)
    {
        return;
    }

    // 终止了foreach
    if(tree->foreach_status == 0)
    {
        return;
    }

    for(int i = 0; i < parent->num_child; i++)
    {
        tool_treenode *node = parent->children[i];
        tree->foreach_status = foreach_action(parent, node, userdata);

        // 设置终止foreach的标记
        if(tree->foreach_status == 0)
        {
            return;
        }
        
        // 继续遍历node的子节点
        foreach_node(tree, node, foreach_action, userdata);
    }
}

void tree_foreach(tool_tree *tree, tree_foreach_action foreach_action, void *userdata)
{
    if(tree->root == NULL)
    {
        return;
    }

    if(foreach_action(tree, tree->root, userdata) == 0)
    {
        return;
    }

    foreach_node(tree, tree->root, foreach_action, userdata);

    // foreach完了重置
    tree->foreach_status = 1;
}

tool_treenode *tree_initroot(tool_tree *tree, void *data)
{
    tool_treenode *node = (tool_treenode*)calloc(1, sizeof(tool_treenode));
    node->data = data;
    tree->root = node;
    return node;
}



