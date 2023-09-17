#include "kf32a156_int.h"
#include "kf32a156_btim.h"
#include "kf32a156_gtim.h"
#include "hw_timer.h"

/*Initialize Timer 14 and configuration Timer 14 interrupt****************
Select 1/(15+1) of HFCLK as clock source,Counting period is 1000, every
1ms enter once T14 Interrupt********************************************/
#define     CNT_T14         (1000u)
#define     PRESCALE_T14    (15u)

#define     CNT_T18         (50000u) /* 50ms overflow */
#define     PRESCALE_T18    (15u)

#define     CNT_T19         (50000u) /* 50ms overflow */
#define     PRESCALE_T19    (15u)

volatile static uint32_t jiffies_ms = 0;
volatile static uint32_t timeout_cnt = 0;


uint32_t TimerGetTickCount(void)
{
    return jiffies_ms;
}

void TimerTickStart(void)
{
    BTIM_Cmd(T14_SFR, TRUE);
    //INT_Interrupt_Enable(INT_T14, TRUE);
}

void TimerTickStop(void)
{
    BTIM_Cmd(T14_SFR, FALSE);
    //INT_Interrupt_Enable(INT_T14, FALSE);
}

void TimerDelayMs(uint32_t ms)
{
    uint32_t count;

    count = TimerGetTickCount() + ms;
    while(TimeAfter(count, TimerGetTickCount()));

    return;
}

/* max delay 50ms 50*1000us */
void TimerDelayUs(uint32_t us)
{
    if (us > 50*1000) {
        us = 50*1000;
    }

    GPTIM_Set_Counter(T18_SFR, 0);
    GPTIM_Cmd(T18_SFR, TRUE);
    while(GPTIM_Get_Counter(T18_SFR) < us);
    GPTIM_Cmd(T18_SFR, FALSE);

    return;
}


void TimerRun(uint32_t us)
{
    if (us > 50*1000) {
        timeout_cnt = 50*1000;
    } else {
        timeout_cnt = us;
    }
    GPTIM_Set_Counter(T19_SFR, 0);
    GPTIM_Cmd(T19_SFR, TRUE);

    return;
}

uint8_t IsTimerOut(void)
{
    uint8_t ret = 0;

    if (GPTIM_Get_Counter(T19_SFR) > timeout_cnt) {
        GPTIM_Cmd(T19_SFR, FALSE);
        GPTIM_Set_Counter(T19_SFR, 0);
        timeout_cnt = 0;
        ret = 1;
    }

    return ret;
}


void HwTimerInit(void)
{
/* timer14 for jiffiers */
    TIM_Reset(T14_SFR);
    BTIM_Updata_Immediately_Config(T14_SFR, TRUE);
    BTIM_Updata_Enable(T14_SFR, TRUE);
    BTIM_Work_Mode_Config(T14_SFR, BTIM_TIMER_MODE);
    BTIM_Set_Counter(T14_SFR, 0);
    BTIM_Set_Period(T14_SFR, CNT_T14);
    BTIM_Set_Prescaler(T14_SFR, PRESCALE_T14);
    BTIM_Counter_Mode_Config(T14_SFR, BTIM_COUNT_UP_OF);
    /*Select internal high frequency as clock source*/
    BTIM_Clock_Config(T14_SFR, BTIM_HFCLK);
    INT_Interrupt_Priority_Config(INT_T14, 4, 0);
    BTIM_Overflow_INT_Enable(T14_SFR, TRUE);
    INT_Interrupt_Enable(INT_T14, TRUE);
    INT_Clear_Interrupt_Flag(INT_T14);
    BTIM_Cmd(T14_SFR, TRUE);
/* FIXME */
    INT_Stack_Align_Config(INT_STACK_SINGLE_ALIGN);

/* timer18 for us delay */
    /*reset peripherals of Timer 18 and enable peripherals clock source.*******/
    TIM_Reset(T18_SFR);
    /*Configure the T18 immediate update control bit to update immediately*****/
    GPTIM_Updata_Immediately_Config(T18_SFR, TRUE);
    /*Enable T18 update controller*********************************************/
    GPTIM_Updata_Enable(T18_SFR, TRUE);
    /*Set to timing mode as working mode of timer 18***************************/
    GPTIM_Work_Mode_Config(T18_SFR, GPTIM_TIMER_MODE);
    /*Set the counting period**************************************************/
    GPTIM_Set_Period(T18_SFR,CNT_T18);
    /*Set prescaler************************************************************/
    GPTIM_Set_Prescaler(T18_SFR,PRESCALE_T18);
    /*Set up and down count****************************************************/
    GPTIM_Counter_Mode_Config(T18_SFR,GPTIM_COUNT_UP_OF);
    /*Select SCLK  as clock source of timer 18*********************************/
    GPTIM_Clock_Config(T18_SFR,GPTIM_HFCLK);

/* timer19 for timeout query */
    /*reset peripherals of Timer 19 and enable peripherals clock source.*******/
    TIM_Reset(T19_SFR);
    /*Configure the T19 immediate update control bit to update immediately*****/
    GPTIM_Updata_Immediately_Config(T19_SFR, TRUE);
    /*Enable T19 update controller*********************************************/
    GPTIM_Updata_Enable(T19_SFR, TRUE);
    /*Set to timing mode as working mode of timer 19***************************/
    GPTIM_Work_Mode_Config(T19_SFR, GPTIM_TIMER_MODE);
    /*Set the counting period**************************************************/
    GPTIM_Set_Period(T19_SFR,CNT_T19);
    /*Set prescaler************************************************************/
    GPTIM_Set_Prescaler(T19_SFR,PRESCALE_T19);
    /*Set up and down count****************************************************/
    GPTIM_Counter_Mode_Config(T19_SFR,GPTIM_COUNT_UP_OF);
    /*Select SCLK  as clock source of timer 19*********************************/
    GPTIM_Clock_Config(T19_SFR,GPTIM_HFCLK);

    return;
}


void __attribute__((interrupt))_T14_exception (void)
{
    BTIM_Clear_Updata_INT_Flag(T14_SFR);
    BTIM_Clear_Overflow_INT_Flag (T14_SFR);
    jiffies_ms += 1;
}



