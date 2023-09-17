#include <string.h>
#include "common.h"
#include "queue.h"

void QueueInit(Queue_Type *q, uint8_t *buf, uint16_t itemsize, uint16_t itemcount)
{
	q->itemsize = itemsize;
    q->itemcount = itemcount;
	q->buf = (void*)buf;
	q->head = 0;
	q->tail = 0; 

    return;
}

int QueuePut(Queue_Type *q, void *data)
{
	int ret = 0;

	if(QueueIsFull(q) == 0)
	{
		//((uint8_t *)(q->buf))[q->head] = ch;
        memcpy(q->buf + q->itemsize * q->head, data, q->itemsize);
		q->head++;
		if(q->head == q->itemcount)
			q->head = 0;

		ret = 1;
	}
	
	return ret;
}

int QueueGet(Queue_Type *q, void *data)
{
	int ret = 0;

	if(QueueIsEmpty(q) == 0)
	{
		//*ch = ((uint8_t *)(q->buf))[q->tail];
        memcpy(data, q->buf + q->itemsize * q->tail, q->itemsize);   
		q->tail++;
		if(q->tail == q->itemcount)
			q->tail = 0;

		ret = 1;
	}
	
	return ret;
}

int QeueuGetSize(Queue_Type *q)
{
    int size;
    
    if (q->head >= q->tail){
        size = q->head - q->tail;
    } else {
        size = q->itemcount - q->tail + q->head;
    }

    return size;
}

//不取出数据，仅查看
int QueuePeek(Queue_Type *q, void *data)
{
	int ret = 0;

	if(QueueIsEmpty(q) == 0)
	{
		//*data = ((uint32_t *)(q->buf))[q->tail];
        memcpy(data, q->buf + q->itemsize * q->tail, q->itemsize);
		ret = 1;
	}
	
	return ret;
}

