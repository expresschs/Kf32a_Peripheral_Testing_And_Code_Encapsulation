#ifndef _HW_TIMER_H_
#define _HW_TIMER_H_

#define TIMER_CHANNEL1_INTERVAL		30U

void HwTimerInit(void);
void TimerTickStart(void);
void TimerTickStop(void);
void TimerDelayUs(uint32_t us);
void TimerDelayMs(uint32_t ms);
void TimerRun(uint32_t us);
uint8_t IsTimerOut(void);

/* return logic true if 'a' is after 'b' */
#define TimeAfter(a,b) ((int32_t)((int32_t)(b) - (int32_t)(a)) < 0)

/* »ñÈ¡Ms tick cnt */
uint32_t TimerGetTickCount(void);

#endif

