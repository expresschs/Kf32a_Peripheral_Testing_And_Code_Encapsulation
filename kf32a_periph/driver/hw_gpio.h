#ifndef _HW_GPIO_H_
#define _HW_GPIO_H_

#include "def_pins.h"

 /* ��ʼ��GPIOΪ������͵�ƽ */
void GPIO_Init_Output(enum GPIO_Name GPIO_List);

 /* ��ʼ��GPIOΪ���� */
void GPIO_Init_Input(enum GPIO_Name GPIO_List);

/* ����GPIO�ڲ����� */
void GPIO_Init_PullUp(enum GPIO_Name GPIO_List);

 /* ����GPIO�ڲ����� */
 void GPIO_Init_PullDown(enum GPIO_Name GPIO_List);
 
 /* GPIO�˿����Ϊ�� */
void GPIO_Output_High(enum GPIO_Name GPIO_List);

 /* GPIO�˿����Ϊ�� */
void GPIO_Output_Low(enum GPIO_Name GPIO_List);

 /* GPIO�˿����״̬��ת */
void GPIO_Output_Toggle(enum GPIO_Name GPIO_List);

 /* ��ȡGPIO�˿�״̬ */
uint8_t GPIO_Input_Read(enum GPIO_Name GPIO_List);

 /* ����STOP MODE���Ѷ˿� */
void GPIO_Set_Wakeup_Func(void);
 /* ����CAN�˿� */
void GPIO_CAN0_Init(void);
void GPIO_CAN_Init(void);
 /* ����ADC�˿� */
void GPIO_ADC_Init(void);

/* ����lin�˿� */
void GPIO_Lin_Init(void);
/* ����uart�˿� */
void GPIO_Uart0_Init(void);
void GPIO_Uart2_Init(void);
/* ����spi�˿� */
void GPIO_Spi1_Init(void);

void GPIO_EnableInterrupt(void);
void GPIO_DisableInterrupt(void);

#endif

