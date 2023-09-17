#ifndef _QUEUE_H_
#define _QUEUE_H_

/*
 * ѭ������, �����������������.
 */
typedef struct {
	volatile uint16_t head;
	volatile uint16_t tail;
    uint16_t itemsize;   //��������ÿ��Ԫ�صĴ�С
	uint16_t itemcount;  //���������ܹ�����Ԫ�ص��ܸ���
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

