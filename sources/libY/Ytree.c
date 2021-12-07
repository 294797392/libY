#include "Yfirstinclude.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Yerrno.h"
#include "Ytree.h"

struct Ytree_s
{
    // 根节点
    Ytreenode *root;

    int foreach_status;

    // 释放data的函数
    Ytree_free_func free;
};

static void foreach_delete(Ytree *tree, Ytreenode *parent)
{
    if(parent->num_child > 0)
    {
        for (int i = 0; i < parent->num_child; i++)
        {
            Y_tree_delete(tree, parent->children[i]);
        }
    }

    if(tree->free)
    {
        tree->free(parent->data);
    }

    free(parent);
}

static void foreach_node(Ytree *tree, Ytreenode *parent, Ytree_foreach_action foreach_action, void *userdata)
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

        Ytreenode *node = parent->children[i];
        tree->foreach_status = foreach_action(tree, node, userdata);

        // 继续遍历node的子节点
        foreach_node(tree, node, foreach_action, userdata);
    }
}

static void foreach_node2(Ytree *tree, Ytreenode *parent, Ytree_foreach_action foreach_action, void *userdata)
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
        Ytreenode *node = parent->children[i];
        if((tree->foreach_status = foreach_action(tree, node, userdata)) == 0)
        {
            return;
        }
    }

    // 再遍历每个子节点下的子节点
    for (int i = 0; i < parent->num_child; i++)
    {
        Ytreenode *node = parent->children[i];
        foreach_node2(tree, node, foreach_action, userdata);
    }
}

Ytree *Y_create_tree()
{
    Ytree *tree = (Ytree*)calloc(1, sizeof(Ytree));
    return tree;
}

Ytree *Y_create_tree2(Ytree_free_func freefunc)
{
    Ytree *tree = Y_create_tree();
    tree->free = freefunc;
    return tree;
}

Ytreenode *Y_tree_initroot(Ytree *tree, void *data)
{
    Ytreenode *node = (Ytreenode*)calloc(1, sizeof(Ytreenode));
    node->data = data;
    tree->root = node;
    return node;
}

Ytreenode *Y_tree_newnode(Ytreenode *parent, void *data)
{
	Ytreenode *node = (Ytreenode*)calloc(1, sizeof(Ytreenode));
	node->data = data;
	node->parent = parent;
	parent->children[parent->num_child] = node;
	parent->num_child++;
	return node;
}

int Y_tree_isempty(Ytree *tree)
{
    return tree->root == NULL;
}

void Y_tree_foreach(Ytree *tree, Ytree_foreach_action foreach_action, void *userdata)
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

void Y_tree_foreach2(Ytree *tree, Ytree_foreach_action foreach_action, void *userdata)
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

void Y_tree_delete(Ytree *tree, Ytreenode *node)
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
        Ytreenode *parent = node->parent;
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
        size_t dst = sizeof(Ytreenode*) * index;            // 要拷贝到的内存的偏移量
        size_t src = sizeof(Ytreenode*) * (index + 1);      // 从内存的哪个偏移量开始拷贝
        size_t size = total_size - src;                         // 要拷贝的内存的大小
		memmove(parent->children + index, parent->children + index + 1, size);
    }
}

void Y_tree_clear(Ytree *tree)
{
    if(tree->root != NULL)
    {
        Y_tree_delete(tree, tree->root);
    }
}