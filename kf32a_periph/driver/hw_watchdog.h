#ifndef _HW_WATCHDOG_H_
#define _HW_WATCHDOG_H_

/* ���к�������ʹ�ú���ʵ�� */
void WDogInit(void);
void WDogClear(void);
void WDogReset(void);
void WDogEnable(uint8_t state);

#endif

