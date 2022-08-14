#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Ylog.h"
#include "Ybstree.h"

typedef enum
{
	NODE_TYPE_LEFT,
	NODE_TYPE_RIGHT
} NODE_TYPE;

struct Ybstree_node_s
{
	int value;			  // 该节点的值
	void *userdata;		  // 该节点所带的用户数据
	ybstree_node *left;	  // 左边的节点
	ybstree_node *right;  // 右边的节点
	ybstree_node *parent; // 父节点
	NODE_TYPE type;		  // 表示是左边节点还是右边节点
};

struct Ybstree_s
{
	ybstree_node *root;
};

// 新建一个节点
static ybstree_node *new_node(ybstree_node *parent, NODE_TYPE type, int value)
{
	ybstree_node *node = (ybstree_node *)calloc(1, sizeof(ybstree_node));
	node->value = value;
	node->type = type;
	node->parent = parent;
	return node;
}

static void add_tree_node(ybstree_node *parent, int value)
{
	if (value < parent->value)
	{
		// 如果新的节点的值小于parent节点的值，插到左子树
		if (parent->left == NULL)
		{
			// 左边节点为空，直接插入到这个位置
			parent->left = new_node(parent, NODE_TYPE_LEFT, value);
		}
		else
		{
			add_tree_node(parent->left, value);
		}
	}
	else if (value > parent->value)
	{
		// 如果新的节点的值小于parent节点的值，那么插入到右子树
		if (parent->right == NULL)
		{
			// 右边节点为空，直接插入到这个位置
			parent->right = new_node(parent, NODE_TYPE_LEFT, value);
		}
		else
		{
			add_tree_node(parent->right, value);
		}
	}
	else
	{
		// 新的节点的值和parent节点相等...
		// TODO:处理
	}
}

// 根据value找到指定的节点
static ybstree_node *find_tree_node(ybstree_node *parent, int value)
{
	if (value < parent->value)
	{
		// 要删除的节点在左子树，继续遍历左子节点
		return remove_tree_node(parent->left, value);
	}
	else
	{
		// 要删除的节点在右子树，继续遍历右子节点
		return remove_tree_node(parent->right, value);
	}
	else
	{
		// 此时value于parent的value相等，说明找到了
		return parent;
	}
}

// 把target放到source的位置
static void replace_tree_node(ybstree_node *source, ybstree_node *target)
{
	target->parent = source->parent;
	target->type = source->type;
	if(target->type == NODE_TYPE_LEFT)
	{
		target->parent->left = target;
	}
	else
	{
		target->parent->right = target;
	}
	target->left = source->left;
	target->right = source->right;

	source->left->parent = target;
	source->right->parent = target;
}

// 找到start节点的前驱节点
// 前驱节点：小于该节点的最大节点
static ybstree_node *find_precursor_node(ybstree_node *start, int value)
{
	if(start->left == NULL)
	{
		return start;
	}

	ybstree_node *current = start;
	while (current->left != NULL)
	{
		current = current->left;
	}
	return current;
}

// 找到start节点的后记节点
// 后继节点：大于该节点的最小节点
static ybstree_node *find_successor_node(ybstree_node *start)
{
	if(start->right == NULL)
	{
		return start;
	}

	ybstree_node *current = start;
	while (current->right != NULL)
	{
		current = current->right;
	}
	return current;
}

static void remove_tree_node(ybstree_node *parent)
{
	if (parent->left == NULL && parent->right == NULL)
	{
		// 要删除的节点下没有其他节点了，直接删除
		if (parent->type == NODE_TYPE_LEFT)
		{
			parent->parent->left = NULL;
		}
		else
		{
			parent->parent->right = NULL;
		}
	}
	else if (parent->left != NULL && parent->right != NULL)
	{
		// 要删除的节点下有两个子节点
		// 有两种办法去删除：
		// 1. 找到要删除的节点的前驱节点，把前驱节点与要删除的节点进行替换。前驱节点：小于该节点的最大节点
		// 2. 找到要删除的节点的后记节点，把后记节点与要删除的节点进行替换。后记节点：大于该节点的最小节点
		// 这里使用后记节点去对要删除的节点进行替换
		ybstree_node *precursor_node = find_precursor_node(parent);
		if(precursor_node->right == NULL)
		{
			// 这里已经找到了前驱节点，前驱节点是不存在左节点的
			// 所以只判断右节点有没有值就可以了
			// 如果右节点有值，那么就先用右节点替换前驱节点
			replace_tree_node(precursor_node, precursor_node->right);
		}

		// 最后再把前驱节点替换到要删除的节点
		replace_tree_node(parent, precursor_node);
	}
	else
	{
		// 要删除的节点下有一个子节点
		// 此时直接用子节点替换掉要删除的节点就行
		ybstree_node *child = parent->left != NULL ? parent->left : parent->right;
		replace_tree_node(parent, child);
	}
	free(parent);
}

Ybstree *Y_create_bstree()
{
	Ybstree *ybst = (Ybstree *)calloc(1, sizeof(Ybstree));
	return ybst;
}

void Y_delete_bstree(Ybstree *ybst)
{
	free(ybst);
}

void Y_bstree_add_node(Ybstree *ybst, int value)
{
	ybstree_node *node = (ybstree_node *)calloc(1, sizeof(ybstree_node));
	node->value = value;

	// 如果根节点为空，那么直接插入根节点
	if (ybst->root == NULL)
	{
		ybst->root = node;
		return;
	}

	add_tree_node(ybst->root, value);
}

void Y_bstree_remove_node(ybstree *ybst, int value)
{
	if (ybst->root == NULL)
	{
		return;
	}

	ybstree_node *to_delete = find_tree_node(ybst->root, value);
	if(to_delete == NULL)
	{
		return;
	}

	remove_tree_node(to_delete);
}
