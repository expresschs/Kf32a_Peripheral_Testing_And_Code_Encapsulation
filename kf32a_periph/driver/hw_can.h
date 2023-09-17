#ifndef _HW_CAN_H_
#define _HW_CAN_H_

/* CAN ID��ʽ */
typedef enum{
    CAN_FORMAT_STANDARD = 0,  /**< ��׼IDģʽ */
    CAN_FORMAT_EXTENDED,      /**< ��չIDģʽ */
} CanMsgFormat;

/* CAN ֡���� */
typedef enum{
    CAN_TYPE_DATA = 0,  /**< ����������֡ */
    CAN_TYPE_REMOTE,    /**< ������Զ��֡ */
    CAN_TYPE_BOTH,      /**< ͬʱ����Զ��֡������֡ */
}CanMsgType;

/* CAN��Ϣ */
typedef struct 
{
	uint32_t id;                 /**< 29 bit or 11 bit identifier         */
	uint8_t  data[8];            /**< Data field                          */
	uint8_t  len;                /**< Length of data field in bytes       */
	uint8_t  format;             /**< 0 - STANDARD, 1- EXTENDED IDENTIFIER*/
	uint8_t  type;               /**< 0 - DATA FRAME, 1 - REMOTE FRAME    */
}CanMsg;

/* CAN���� */
typedef struct 
{
	uint32_t id;                 /**< 29 bit or 11 bit identifier         */
	uint32_t mask;               /**< mask                          */
	uint8_t  format;             /**< 0 - STANDARD, 1- EXTENDED IDENTIFIER*/
}CanFilter;

/* ��ʼ����ȥ��ʼ�� */
void CanInit(uint8_t ctrl, uint32_t baud, CanFilter *filter, uint8_t filter_num);
void CanDeinit(uint8_t ctrl);

/* ���úͽ��� */
void CanDisable(uint8_t ctrl);
void CanEnable(uint8_t ctrl);

/* ����˯��ģʽ */
void CanGotoSleep(uint8_t ctrl);

/* ���� */
void CanWakeUp(uint8_t ctrl);

/* ���ù����� 
 * idx = 0 ���õ�һ�������
 * idx = 1 ���õڶ��������
*/
void CanSetFilter(uint8_t  ctrl,
                  uint8_t  idx,
                  uint32_t id,
                  uint32_t mask,
                  uint8_t  format,
                  uint8_t  type);

/*
* ��������
*/
uint8_t CanSend(uint8_t ctrl, CanMsg *msg);

/*
* ��������
*/
uint8_t CanRecieve(uint8_t ctrl, CanMsg *msg);


/* ��Ҫ�û�ʵ�ֵĺ��������ж������ĵ��� */
/* �յ���Ϣʱ������
   ����0����ʾ��Ϣ��Ҫ��������У������ڽ�����������ͨ��CanRecieve������ȡ��
   ����1����ʾ��Ϣ�Ѿ��������������У������ڽ�����������ͨ��CanRecieve������ȡ��
   Ӧ�ó�������ڴ�ʵ�����ID���ˣ�����ֱ�Ӵ����ġ�
*/
uint8_t CanUserOnMessage(uint8_t ctrl, CanMsg *msg);



/* Busoffʱ������ */
void CanUserOnBusOff(uint8_t ctrl);

/* ���߻���ʱ������ */
void CanUserOnWakeUp(void);

void CanOnMessage(void);

#endif

