#ifndef _QUEUE_H_
#define _QUEUE_H_

/*
 * 循环队列, 用于流输入输出缓冲.
 */
typedef struct {
	volatile uint16_t head;
	volatile uint16_t tail;
    uint16_t itemsize;   //缓冲区中每个元素的大小
	uint16_t itemcount;  //缓冲区中能够保存元素的总个数
	uint8_t *buf;
} Queue_Type;

static int QueueIsEmpty(Queue_Type *q)
{
	return (q->head == q->tail);
}

static int QueueIsFull(Queue_Type *q)
{
	return (q->head == q->tail - 1) || ((q->tail == 0) && (q->head == q->itemcount - 1));
}

void QueueInit(Queue_Type *q, uint8_t *buf, uint16_t itemsize, uint16_t itemcount);
int QueuePut(Queue_Type *q, void *data);
int QueueGet(Queue_Type *q, void *data);
int QeueuGetSize(Queue_Type *q);
int QueuePeek(Queue_Type *q, void *data);

#endif // _QUEUE_H_

