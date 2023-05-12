#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "libY.h"

#define MAX_POOL_SIZE		512

typedef struct Ypool_s Ypool;

typedef struct Ybucket_s
{
	int block_size;
	int max_blocks;
	int num_blocks;
	Yqueue *queue;
	Ylock lock;
	// 所属的缓冲池
	Ypool *pool;
}Ybucket;

struct Ypool_s
{
	int max_block_size;
	int max_block_per_bucket;
	Ylock lock;

	Ybucket **buckets;
	int num_buckets;
};


/// <summary>
/// 全局缓冲区管理对象
/// </summary>
static Ypool *pool = NULL;


static int select_bucket_index(uint32_t buffer_size)
{
	uint32_t bitsRemaining = ((uint32_t)buffer_size - 1) >> 4;

	int poolIndex = 0;
	if(bitsRemaining > 0xFFFF) { bitsRemaining >>= 16; poolIndex = 16; }
	if(bitsRemaining > 0xFF) { bitsRemaining >>= 8; poolIndex += 8; }
	if(bitsRemaining > 0xF) { bitsRemaining >>= 4; poolIndex += 4; }
	if(bitsRemaining > 0x3) { bitsRemaining >>= 2; poolIndex += 2; }
	if(bitsRemaining > 0x1) { bitsRemaining >>= 1; poolIndex += 1; }

	return poolIndex + (int)bitsRemaining;
}

static int get_block_size_for_bucket(int binIndex)
{
	int maxSize = 16 << binIndex;
	return maxSize;
}

static Ybucket *create_bucket(int block_size, int max_blocks)
{
	Ybucket *bucket = (Ybucket *)calloc(1, sizeof(Ybucket));
	bucket->block_size = block_size;
	bucket->max_blocks = max_blocks;
	Y_create_lock(bucket->lock);
	bucket->queue = Y_create_queue();
	return bucket;
}

static void *bucket_obtain(Ybucket *bucket)
{
	Y_lock_lock(bucket->lock);

	int queue_count = Y_queue_size(bucket->queue);
	if(queue_count == 0)
	{
		if(bucket->num_blocks < bucket->max_blocks)
		{
			bucket->num_blocks++;
			void *block = calloc(1, bucket->block_size);
			//printf("queue_count = 0, alloc block, %d, max_blocks = %d\n", bucket->num_blocks, bucket->max_blocks);
			Y_lock_unlock(bucket->lock);
			return block;
		}
		else
		{
			// 这里说明所有的block都在使用中，程序可能出现了什么异常..
			// 返回空并加一个日志
			printf("too many blocks in used, max_block = %d", bucket->max_blocks);
			Y_lock_unlock(bucket->lock);
			return NULL;
		}
	}
	else
	{
		//printf("reuse block\n");

		void *block = Y_queue_dequeue(bucket->queue);
		Y_lock_unlock(bucket->lock);
		return block;
	}
}

static void bucket_recycle(Ybucket *bucket, void *block)
{
	Y_lock_lock(bucket->lock);

	memset(block, 0, bucket->block_size);
	Y_queue_enqueue(bucket->queue, block);
	//int queue_count = Y_queue_size(bucket->queue);
	//printf("queue_size = %d\n", queue_count);

	Y_lock_unlock(bucket->lock);
}


int Y_pool_init(int max_block_size, int max_blocks)
{
	Ypool *yp = (Ypool *)calloc(1, sizeof(Ypool));
	Y_create_lock(yp->lock);

	yp->max_block_size = max_block_size;
	yp->max_block_per_bucket = max_blocks;

	int max_buckets = select_bucket_index(max_block_size) + 1;
	yp->buckets = (Ybucket **)calloc(max_buckets, sizeof(Ybucket *));
	yp->num_buckets = max_buckets;
	for(int i = 0; i < max_buckets; i++)
	{
		int block_size = get_block_size_for_bucket(i);
		Ybucket *bucket = create_bucket(block_size, max_blocks);
		bucket->pool = yp;
		yp->buckets[i] = bucket;
	}

	pool = yp;
	return YERR_SUCCESS;
}

void *Y_pool_obtain(int blocksize)
{
	int bucket_index = select_bucket_index(blocksize);

	// 先查找该大小是否有对应的bucket
	// 如果申请的大小大于缓冲池里可申请的内存最大大小，那么就找不到bucket，这种情况下返回NULL
	if(bucket_index > pool->num_buckets - 1)
	{
		printf("request blocksize too big, %d", blocksize);
		return NULL;
	}

	Ybucket *bucket = pool->buckets[bucket_index];
	return bucket_obtain(bucket);
}

void *Y_pool_resize(void *block, int blocksize, int newsize)
{
	int bucket_index = select_bucket_index(blocksize);
	if(bucket_index > pool->num_buckets - 1)
	{
		printf("request blocksize too big, %d", blocksize);
		return NULL;
	}

	void *newblock = realloc(block, newsize);
	return newblock;
}

void Y_pool_recycle(void *block, int blocksize)
{
	int bucket_index = select_bucket_index(blocksize);
	if(bucket_index > pool->num_buckets - 1)
	{
		return;
	}

	Ybucket *bucket = pool->buckets[bucket_index];

	bucket_recycle(bucket, block);
}
