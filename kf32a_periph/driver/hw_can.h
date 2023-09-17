#ifndef _HW_CAN_H_
#define _HW_CAN_H_

/* CAN ID格式 */
typedef enum{
    CAN_FORMAT_STANDARD = 0,  /**< 标准ID模式 */
    CAN_FORMAT_EXTENDED,      /**< 扩展ID模式 */
} CanMsgFormat;

/* CAN 帧类型 */
typedef enum{
    CAN_TYPE_DATA = 0,  /**< 仅接收数据帧 */
    CAN_TYPE_REMOTE,    /**< 仅接收远程帧 */
    CAN_TYPE_BOTH,      /**< 同时接收远程帧和数据帧 */
}CanMsgType;

/* CAN消息 */
typedef struct 
{
	uint32_t id;                 /**< 29 bit or 11 bit identifier         */
	uint8_t  data[8];            /**< Data field                          */
	uint8_t  len;                /**< Length of data field in bytes       */
	uint8_t  format;             /**< 0 - STANDARD, 1- EXTENDED IDENTIFIER*/
	uint8_t  type;               /**< 0 - DATA FRAME, 1 - REMOTE FRAME    */
}CanMsg;

/* CAN过滤 */
typedef struct 
{
	uint32_t id;                 /**< 29 bit or 11 bit identifier         */
	uint32_t mask;               /**< mask                          */
	uint8_t  format;             /**< 0 - STANDARD, 1- EXTENDED IDENTIFIER*/
}CanFilter;

/* 初始化和去初始化 */
void CanInit(uint8_t ctrl, uint32_t baud, CanFilter *filter, uint8_t filter_num);
void CanDeinit(uint8_t ctrl);

/* 启用和禁用 */
void CanDisable(uint8_t ctrl);
void CanEnable(uint8_t ctrl);

/* 进入睡眠模式 */
void CanGotoSleep(uint8_t ctrl);

/* 唤醒 */
void CanWakeUp(uint8_t ctrl);

/* 设置过滤器 
 * idx = 0 设置第一组过滤器
 * idx = 1 设置第二组过滤器
*/
void CanSetFilter(uint8_t  ctrl,
                  uint8_t  idx,
                  uint32_t id,
                  uint32_t mask,
                  uint8_t  format,
                  uint8_t  type);

/*
* 发送数据
*/
uint8_t CanSend(uint8_t ctrl, CanMsg *msg);

/*
* 接收数据
*/
uint8_t CanRecieve(uint8_t ctrl, CanMsg *msg);


/* 需要用户实现的函数，在中断上下文调用 */
/* 收到消息时被调用
   返回0，表示消息需要放入队列中，可以在进程上下文中通过CanRecieve函数读取。
   返回1，表示消息已经处理，无需放入队列，不能在进程上下文中通过CanRecieve函数读取。
   应用程序可以在此实现软件ID过滤，或者直接处理报文。
*/
uint8_t CanUserOnMessage(uint8_t ctrl, CanMsg *msg);



/* Busoff时被调用 */
void CanUserOnBusOff(uint8_t ctrl);

/* 总线唤醒时被调用 */
void CanUserOnWakeUp(void);

void CanOnMessage(void);

#endif

