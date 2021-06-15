#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "toolcode.h"
#include "tool_tree.h"

struct tool_tree_s
{
    // 根节点
    tool_treenode *root;

    int foreach_status;

    // 释放data的函数
    tree_free_func free;
};

static void foreach_delete(tool_tree *tree, tool_treenode *parent)
{
    if(parent->num_child > 0)
    {
        for (int i = 0; i < parent->num_child; i++)
        {
            tree_delete(tree, parent->children[i]);
        }
    }

    if(tree->free)
    {
        tree->free(parent->data);
    }

    free(parent);
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
        // 设置终止foreach的标记
        if(tree->foreach_status == 0)
        {
            return;
        }

        tool_treenode *node = parent->children[i];
        tree->foreach_status = foreach_action(tree, node, userdata);

        // 继续遍历node的子节点
        foreach_node(tree, node, foreach_action, userdata);
    }
}

static void foreach_node2(tool_tree *tree, tool_treenode *parent, tree_foreach_action foreach_action, void *userdata)
{
    if(parent == NULL)
    {
        return;
    }

    if(tree->foreach_status == 0)
    {
        return;
    }

    // 先遍历所有的子节点
    for (int i = 0; i < parent->num_child; i++)
    {
        tool_treenode *node = parent->children[i];
        if((tree->foreach_status = foreach_action(tree, node, userdata)) == 0)
        {
            return;
        }
    }

    // 再遍历每个子节点下的子节点
    for (int i = 0; i < parent->num_child; i++)
    {
        tool_treenode *node = parent->children[i];
        foreach_node2(tree, node, foreach_action, userdata);
    }
}

tool_tree *new_tree()
{
    tool_tree *tree = (tool_tree*)calloc(1, sizeof(tool_tree));
    return tree;
}

tool_tree *new_tree2(tree_free_func freefunc)
{
    tool_tree *tree = new_tree();
    tree->free = freefunc;
    return tree;
}

void tree_foreach(tool_tree *tree, tree_foreach_action foreach_action, void *userdata)
{
    tree->foreach_status = 1;

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

void tree_foreach2(tool_tree *tree, tree_foreach_action foreach_action, void *userdata)
{
    tree->foreach_status = 1;

    if(tree->root == NULL)
    {
        return;
    }

    if(foreach_action(tree, tree->root, userdata) == 0)
    {
        return;
    }

    foreach_node2(tree, tree->root, foreach_action, userdata);

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

tool_treenode *tree_newnode(tool_treenode *parent, void *data)
{
	tool_treenode *node = (tool_treenode *)calloc(1, sizeof(tool_treenode));
	node->data = data;
	node->parent = parent;
	parent->children[parent->num_child] = node;
	parent->num_child++;
	return node;
}

int tree_isempty(tool_tree *tree)
{
    return tree->root == NULL;
}

void tree_delete(tool_tree *tree, tool_treenode *node)
{
    if(tree->root == node)
    {
        // 删除的是根元素
        tree->root = NULL;
        foreach_delete(tree, node);
    }
    else
    {
        int index = -1;
        tool_treenode *parent = node->parent;
        for (int i = 0; i < parent->num_child; i++)
        {
            if(parent->children[i] == node)
            {
                index = i;
                break; 
            }
        }

        // 在树上没找到这个节点
        if(index == -1)
        {
            return;
        }

        foreach_delete(tree, node);
        parent->num_child--;

        size_t total_size = sizeof(parent->children);
        size_t dst = sizeof(tool_treenode*) * index;            // 要拷贝到的内存的偏移量
        size_t src = sizeof(tool_treenode*) * (index + 1);      // 从内存的哪个偏移量开始拷贝
        size_t size = total_size - src;                         // 要拷贝的内存的大小
		memmove(parent->children + index, parent->children + index + 1, size);
    }
}

void tree_clear(tool_tree *tree)
{
    if(tree->root != NULL)
    {
        tree_delete(tree, tree->root);
    }
}