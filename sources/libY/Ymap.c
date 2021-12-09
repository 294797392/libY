#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ymap.h"

// 默认的hash表长度
#define DEFAULT_YMAP_SIZE   1024

typedef struct Yentry_s Yentry;

struct Yentry_s
{
    void *key;
    void *value;

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

    // hash表里元素的数量
    int count;
};


int Y_map_hash_string_java(void *key)
{
    const char *str = (const char*)key;
    int h = 0;
    int len = strlen(str);
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


Ymap *Y_create_map(Ymap_hash_func hash, Ymap_keycmp_func keycmp, Ymap_free_func free)
{
    Ymap *ym = (Ymap*)calloc(1, sizeof(Ymap));
    ym->count = DEFAULT_YMAP_SIZE;
    ym->entries = (Yentry**)calloc(ym->count, sizeof(Yentry*));
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
    int hashcode = ym->hash(key);
    Yentry *entry = ym->entries[hashcode];
    if (entry == NULL)
    {
        // 不存在元素
        return NULL;
    }

    if(ym->keycmp(entry->key, key))
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
        while(entry != NULL)
        {
            if(ym->keycmp(entry->key, key))
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
    Yentry *new_entry = (Yentry*)calloc(1, sizeof(Yentry));
    new_entry->key = key;
    new_entry->value = value;

    int hashcode = ym->hash(key);
    Yentry *entry = ym->entries[hashcode];
    if(entry == NULL)
    {
        // 位置是空的，直接存放
        ym->entries[hashcode] = new_entry;
        ym->count++;
        return new_entry;
    }
    else
    {
        /*
         * 那么首先得判断key是不是相等的，如果key是相等的，说明该key的位置已经有value了
         * 如果key不相等，那么说明该key的位置被其他key存储了，产生了hash冲突
         */
        if(ym->keycmp(entry->key, key))
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
    int hashcode = ym->hash(key);
    Yentry *entry = ym->entries[hashcode];
    if(entry == NULL)
    {
        // 不存在该元素
        return;
    }

    if(!ym->keycmp(entry->key, key))
    {
        // 键不一致，说明产生过hash冲突
#ifdef Y_MAP_CHAIN
        entry = entry->next;
        while(entry != NULL && ym->keycmp(entry->key, key) == 0)
        {
            entry = entry->next;
        }
#endif
    }

    // 没有找到对应的entry
    if(entry == NULL)
    {
        return;
    }

    // 找到了对应的entry，开始删除
    if(ym->free) { ym->free(entry->value); }
#ifdef Y_MAP_CHAIN
    // 如果是链式扩展法的话，那么直接指向下一个指针，以便于下次get的时候可以找到元素
    ym->entries[hashcode] = entry->next;
#endif
    free(entry);

    ym->count--;

    return;
}

void Y_map_clear(Ymap *ym)
{

}

