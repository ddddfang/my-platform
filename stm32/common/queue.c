#include "queue.h"


#define USART_QUEUE_SIZE	16
static u8 rx_data[USART_QUEUE_SIZE];
queue usart_queue = {rx_data, USART_QUEUE_SIZE, 0, 0};


//----------------------------------------------------------

u8 queue_len(queue *q) {
    return ((q->w_pos - q->r_pos) & (q->size - 1));
}

void enqueue(queue *q, u8 data) {
    if (queue_len(q) == (q->size - 1))
        return ;
    rx_data[q->w_pos] = data;
    q->w_pos = (q->w_pos + 1) & (q->size - 1);
}

u8 dequeue(queue *q) {
    u8 data;
    if (queue_len(q) == 0)
        return 0;
    data = rx_data[q->r_pos];
    q->r_pos = (q->r_pos + 1) & (q->size - 1);
    return data;
}

