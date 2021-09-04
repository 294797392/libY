#ifndef __TOOLKIT_BUFFER_QUEUE_H__
#define __TOOLKIT_BUFFER_QUEUE_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_QUEUE_SIZE 4096

typedef enum
{
	QUEUE_STATE_IDLE,
	QUEUE_STATE_RUNNING
}queue_state;

typedef void(*buffer_queue_callback)(void *userdata, void *element);
typedef struct buffer_queue_s buffer_queue;

buffer_queue *create_buffer_queue(void *userdata);
void start_buffer_queue(buffer_queue *queue, buffer_queue_callback callback);
void buffer_queue_enqueue(buffer_queue *queue, void *element);
void *buffer_queue_dequeue(buffer_queue *queue);
int buffer_queue_size(buffer_queue *q);

#endif

