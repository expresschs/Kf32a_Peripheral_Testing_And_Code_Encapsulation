#ifndef _HW_ADC_H_
#define _HW_ADC_H_

enum{
    ADC0_DEV,
    ADC1_DEV,
};

/* ADC��ʼ�� */
void ADCInit(void);

/* ����͹���ģʽ */
void ADCGotoSleep(void);

/* �ָ�����ģʽ */
void ADCWakeUp(void);

/* ��ȡADC��ֵ */
uint16_t ADCGet(uint8_t dev, uint8_t channel);

#endif

