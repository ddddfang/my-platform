#ifndef __QUEUE_H
#define __QUEUE_H


#include "stm32f10x.h"

typedef struct queue{
    u8 *buf;
    u8 size;
    u8 r_pos;
    u8 w_pos;
} queue;

extern queue usart_queue;

u8 queue_len(queue *q);
void enqueue(queue *q, u8 data);
u8 dequeue(queue *q);

#endif
