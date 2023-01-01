#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libY.h"

// 默认的hash表长度
#define DEFAULT_YMAP_SIZE		128

// 默认的负载因子
#define DEFAULT_LOAD_FACTOR		0.7

typedef struct Yentry_s Yentry;

struct Yentry_s
{
	void *key;
	void *value;

	// 当扩容的时候，记录该元素的新的位置
	int new_pos;

#ifdef Y_MAP_CHAIN
	Yentry *next;
#endif
};

struct Ymap_s
{
	Yentry **entries;

	// 对key进行计算的hash函数
	Ymap_hash_func hash;
	Ymap_keycmp_func keycmp;
	Ymap_free_func free;

	// 负载因子，当 count/capacity >= load_factor的时候，需要resize操作
	// 目的是为了保证查表速度
	double load_factor;

	// hash表里元素的数量
	int count;

	// 哈希表可以容纳的元素的总数
	int capacity;
};

// 除留余数法计算索引
static int makeindex(Ymap *ym, void *key)
{
	return ym->hash(key) % ym->capacity;
}

/*
 * 描述：
 * 对hash表进行resize操作
 *
 * 参数：
 * @ym：要扩容的哈希表对象
 * @new_size：新的哈希表元素的数量
 */
static void resize(Ymap *ym, int new_capacity)
{
	// 新的哈希表的内存长度
	int new_size = sizeof(Yentry *) * new_capacity;
	ym->entries = (Yentry **)realloc(ym->entries, new_size);
	ym->capacity = new_capacity;

	// 重新计算索引值并保存到新的数组里
	Yentry **old_entries = (Yentry **)calloc(ym->count, sizeof(Yentry *));
	for (int i = 0; i < ym->count; i++)
	{
		Yentry *entry = ym->entries[i];
		if (entry != NULL)
		{
			entry->new_pos = makeindex(ym, entry->key);
		}
		old_entries[i] = entry;
	}

	// 对原有哈希表进行重新赋值操作
	memset(ym->entries, 0, new_size);
	for (int i = 0; i < ym->count; i++)
	{
		Yentry *entry = old_entries[i];
		if (entry != NULL)
		{
			ym->entries[entry->new_pos] = entry;
		}
	}

	// resize完了一定要释放内存
	free(old_entries);
}


int Y_map_hash_string_java(void *key)
{
	const char *str = (const char *)key;
	size_t h = 0;
	size_t len = strlen(str);
	if (h == 0 && len > 0)
	{
		for (int i = 0; i < len; i++)
		{
			h = 31 * h + (int)str[i];
		}
	}
	return h;
}

int Y_map_keycmp_string(void *key1, void *key2)
{
	const char *str1 = (const char *)key1;
	const char *str2 = (const char *)key2;
	return strncmp(str1, str2, strlen(str1)) ? 0 : 1;
}

int Y_map_keycmp_pointer(void *key1, void *key2)
{
	return key1 == key2 ? 1 : 0;
}

Ymap *Y_create_map(Ymap_hash_func hash, Ymap_keycmp_func keycmp, Ymap_free_func free)
{
	Ymap *ym = (Ymap *)calloc(1, sizeof(Ymap));
	ym->capacity = DEFAULT_YMAP_SIZE;
	ym->load_factor = DEFAULT_LOAD_FACTOR;
	ym->entries = (Yentry **)calloc(ym->capacity, sizeof(Yentry *));
	ym->hash = hash;
	ym->keycmp = keycmp;
	ym->free = free;
	return ym;
}

void Y_delete_map(Ymap *ym)
{
	Y_map_clear(ym);
	free(ym);
}

void *Y_map_get(Ymap *ym, void *key)
{
	int index = makeindex(ym, key);
	Yentry *entry = ym->entries[index];
	if (entry == NULL)
	{
		// 不存在元素
		return NULL;
	}

	if (ym->keycmp(entry->key, key))
	{
		// key相等，直接返回
		return entry->value;
	}
	else
	{
		// key不相等，说明插入的时候产生了hash冲突
#ifdef Y_MAP_CHAIN

		// 链式扩展法就是遍历链表的每个节点，看看节点的key是否和要查找的key相等
		// 如果相等则返回
		entry = entry->next;
		while (entry != NULL)
		{
			if (ym->keycmp(entry->key, key))
			{
				return entry->value;
			}
			entry = entry->next;
		}
#endif

		// 没查找到元素
		return NULL;
	}
}

void *Y_map_set(Ymap *ym, void *key, void *value)
{
	// 先检查是否要扩容
	if (ym->count / ym->capacity > ym->load_factor)
	{
		resize(ym, ym->capacity * 2);
	}

	Yentry *new_entry = (Yentry *)calloc(1, sizeof(Yentry));
	new_entry->key = key;
	new_entry->value = value;

	int index = makeindex(ym, key);
	Yentry *entry = ym->entries[index];
	if (entry == NULL)
	{
		// 位置是空的，直接存放
		ym->entries[index] = new_entry;
		ym->count++;
		return new_entry;
	}
	else
	{
		/*
		 * 那么首先得判断key是不是相等的，如果key是相等的，说明该key的位置已经有value了
		 * 如果key不相等，那么说明该key的位置被其他key存储了，产生了hash冲突
		 */
		if (ym->keycmp(entry->key, key))
		{
			// key值相等，说明放入了重复的key
			void *old_value = entry->value;
			entry->value = value;
			// 返回被替换掉的值
			return old_value;
		}

		// 运行到这里说明产生了hash冲突
#ifdef Y_MAP_CHAIN
		while (entry->next != NULL)
		{
			entry = entry->next;
		}
		entry->next = new_entry;
#endif
		ym->count++;
		return value;
	}
}

void Y_map_remove(Ymap *ym, void *key)
{
	int index = makeindex(ym, key);
	Yentry *entry = ym->entries[index];
	if (entry == NULL)
	{
		// 不存在该元素
		return;
	}

	if (!ym->keycmp(entry->key, key))
	{
		// 键不一致，说明产生过hash冲突
#ifdef Y_MAP_CHAIN
		entry = entry->next;
		while (entry != NULL && ym->keycmp(entry->key, key) == 0)
		{
			entry = entry->next;
		}
#endif
	}

	// 没有找到对应的entry
	if (entry == NULL)
	{
		return;
	}

	// 找到了对应的entry，开始删除
	if (ym->free) { ym->free(entry->value); }
#ifdef Y_MAP_CHAIN
	// 如果是链式扩展法的话，那么直接指向下一个指针，以便于下次get的时候可以找到元素
	ym->entries[index] = entry->next;
#endif
	free(entry);

	ym->count--;

	return;
}

void Y_map_clear(Ymap *ym)
{
	for (int i = 0; i < ym->count; i++)
	{
		Yentry *entry = ym->entries[i];
#ifdef Y_MAP_CHAIN
		while (entry)
		{
			if (ym->free)
			{
				ym->free(entry->value);
			}
			Yentry *next = entry->next;
			free(entry);
			entry = next;
		}
		ym->entries[i] = NULL;
#endif
	}

	ym->count = 0;
}

int Y_map_count(Ymap *ym)
{
	return ym->count;
}

void Y_map_foreach(Ymap *ym, Ymap_foreach_func foreach, void *userdata)
{
	if (ym->count == 0)
	{
		return;
	}

#ifdef Y_MAP_CHAIN
	for (int i = 0; i < ym->count; i++)
	{
		Yentry *entry = ym->entries[i];
		while (entry)
		{
			foreach(ym, entry->key, entry->value, userdata);
			entry = entry->next;
		}
	}
#endif
}