#ifndef _HW_GPIO_H_
#define _HW_GPIO_H_

#include "def_pins.h"

 /* 初始化GPIO为输出，低电平 */
void GPIO_Init_Output(enum GPIO_Name GPIO_List);

 /* 初始化GPIO为输入 */
void GPIO_Init_Input(enum GPIO_Name GPIO_List);

/* 配置GPIO内部上拉 */
void GPIO_Init_PullUp(enum GPIO_Name GPIO_List);

 /* 配置GPIO内部下拉 */
 void GPIO_Init_PullDown(enum GPIO_Name GPIO_List);
 
 /* GPIO端口输出为高 */
void GPIO_Output_High(enum GPIO_Name GPIO_List);

 /* GPIO端口输出为低 */
void GPIO_Output_Low(enum GPIO_Name GPIO_List);

 /* GPIO端口输出状态翻转 */
void GPIO_Output_Toggle(enum GPIO_Name GPIO_List);

 /* 读取GPIO端口状态 */
uint8_t GPIO_Input_Read(enum GPIO_Name GPIO_List);

 /* 配置STOP MODE唤醒端口 */
void GPIO_Set_Wakeup_Func(void);
 /* 配置CAN端口 */
void GPIO_CAN0_Init(void);
void GPIO_CAN_Init(void);
 /* 配置ADC端口 */
void GPIO_ADC_Init(void);

/* 配置lin端口 */
void GPIO_Lin_Init(void);
/* 配置uart端口 */
void GPIO_Uart0_Init(void);
void GPIO_Uart2_Init(void);
/* 配置spi端口 */
void GPIO_Spi1_Init(void);

void GPIO_EnableInterrupt(void);
void GPIO_DisableInterrupt(void);

#endif

