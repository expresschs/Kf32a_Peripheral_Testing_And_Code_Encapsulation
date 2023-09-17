#ifndef _HW_WATCHDOG_H_
#define _HW_WATCHDOG_H_

/* 下列函数可以使用宏来实现 */
void WDogInit(void);
void WDogClear(void);
void WDogReset(void);
void WDogEnable(uint8_t state);

#endif

