#ifndef _HW_ADC_H_
#define _HW_ADC_H_

enum{
    ADC0_DEV,
    ADC1_DEV,
};

/* ADC初始化 */
void ADCInit(void);

/* 进入低功耗模式 */
void ADCGotoSleep(void);

/* 恢复正常模式 */
void ADCWakeUp(void);

/* 读取ADC数值 */
uint16_t ADCGet(uint8_t dev, uint8_t channel);

#endif

