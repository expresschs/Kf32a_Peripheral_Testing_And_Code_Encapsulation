/******************************************************************************
 *                  Shanghai ChipON Micro-Electronic Co.,Ltd                  *
 ******************************************************************************
 *  $File Name$       : main.c                                         	      *
 *  $Author$          : ChipON AE/FAE Group                                   *
 *  $Data$            : 2021-07-8                                             *
 *  $AutoSAR Version  : V2.0	                                              *
 *  $Description$     : Flash the LED0 every 500ms on the KF32A156 Mini EVB,  *
 * 						and press the key to flip another LED1				  *
 ******************************************************************************
 *  Copyright (C) by Shanghai ChipON Micro-Electronic Co.,Ltd                 *
 *  All rights reserved.                                                      *
 *                                                                            *
 *  This software is copyrght protected and proprietary to                    *
 *  Shanghai ChipON Micro-Electronic Co.,Ltd.                                 *
 ******************************************************************************
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  *
 *                     		REVISON HISTORY                               	  *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  *
 *  Data       Version  Author        Description                             *
 *  ~~~~~~~~~~ ~~~~~~~~ ~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  *
 *  2021-07-08 00.02.00 FAE Group     new creat                               *
 *                                                                            *
 *                                                                            *
 *****************************************************************************/

/******************************************************************************
**                      		Include Files                                **
******************************************************************************/
#include "system_init.h"
#include "common.h"
#include "def_pins.h"
#include "hw_gpio.h"
#include "hw_timer.h"
#include "hw_can.h"
#include "hw_adc.h"
#include "hw_lin.h"
#include "hw_watchdog.h"
#include "hw_flash.h"
#include "hw_uart.h"
#include "hw_spi.h"
#include "hw_afe.h"
#include "hw_w25qxx.h"
#include "debug_uart.h"
/******************************************************************************
**                      	Private variables                                **
******************************************************************************/
static volatile uint32_t TimingDelay;

#define MAIN_TASK_BCM_INTERVAL    20UL
static uint32_t g_bcmTs;


/*******************************************************************************
**                     		   Global Functions 		             	      **
*******************************************************************************/
/**
 *  @brief :Initialize the different GPIO ports
 * 			PC0 ------------- S2
 * 			PD12 ------------ LED1
 * 			PH3 ------------- LED2
 *  @param in :None
 *  @param out :None 
 *  @retval :None
 */
void BoardGpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Configure PD12 LED1 output */
	GPIO_InitStructure.m_Mode = GPIO_MODE_OUT;
	GPIO_InitStructure.m_OpenDrain = GPIO_POD_PP;
	GPIO_InitStructure.m_PullDown = GPIO_NOPULL;
	GPIO_InitStructure.m_PullUp = GPIO_NOPULL;
	GPIO_InitStructure.m_Speed = GPIO_LOW_SPEED;
	GPIO_InitStructure.m_Pin = GPIO_PIN_MASK_12;
	GPIO_Configuration(GPIOD_SFR , &GPIO_InitStructure);
	
	/* Configure PH3 LED2 output */
	GPIO_InitStructure.m_Pin = GPIO_PIN_MASK_3;
	GPIO_Configuration(GPIOH_SFR , &GPIO_InitStructure);
	
	 /* Configure PC0 user key input */
	GPIO_InitStructure.m_Mode = GPIO_MODE_IN;
	GPIO_InitStructure.m_Pin = GPIO_PIN_MASK_0;
	GPIO_Configuration(GPIOC_SFR , &GPIO_InitStructure);
}

void test_gpio(void)
{
	/* Initialize the system clock is 120M*/
	SystemInit(120);
	/* Setup SysTick Timer as delay function, and input frequency is 120M */
	systick_delay_init(120);
	/* Initialize the user IOs */
	BoardGpioInit();
	while(1)
	{
		/* Toggle LED1 */
		if (++TimingDelay >= 10)
		{
			TimingDelay = 0;
			GPIO_Toggle_Output_Data_Config(GPIOD_SFR, GPIO_PIN_MASK_12);
		}
		/* Turn ON/OFF the dedicate LED2 by user key */
		if (GPIO_Read_Input_Data_Bit(GPIOC_SFR, GPIO_PIN_MASK_0) == Bit_RESET)
		{
			GPIO_Set_Output_Data_Bits(GPIOH_SFR, GPIO_PIN_MASK_3, Bit_SET);
		}
		else
		{
			GPIO_Set_Output_Data_Bits(GPIOH_SFR, GPIO_PIN_MASK_3, Bit_RESET);
		}
		systick_delay_ms(50);
	}		

}


CanFilter g_filter[] = {
    {0x0, 0xFFFFFFFF, CAN_FORMAT_STANDARD},
};
CanMsg g_rmsg;
CanMsg g_tmsg;

void test_can(void)
{
    /* Initialize the system clock is 120M*/
    SystemInit(120);
    /* Setup SysTick Timer as delay function, and input frequency is 120M */
    systick_delay_init(120);
    CanInit(4, 500, NULL, 0);
    INT_All_Enable(TRUE);

    g_tmsg.format = CAN_FORMAT_STANDARD;
    g_tmsg.type  = CAN_TYPE_DATA;
    g_tmsg.len = 8;

    while(1) {
#if 1
        /* test1 */
        if (ERR_OK == CanRecieve(4, &g_rmsg)) {
            CanSend(4, &g_rmsg);
        }
#else
        /* test2 */
        g_tmsg.id = 0xa5;
        CanSend(4, &g_tmsg);
        //g_tmsg.id = 0x22;
        //CanSend(4, &g_tmsg);
        //g_tmsg.id = 0x33;
        //CanSend(4, &g_tmsg);
        //g_tmsg.id = 0x44;
        //CanSend(4, &g_tmsg);
        //g_tmsg.id = 0x55;
        //CanSend(4, &g_tmsg);
        //g_tmsg.id = 0x66;
        //CanSend(4, &g_tmsg);
        systick_delay_ms(5000);
#endif
    }

    return;
}

void test_can1(void)
{
    uint8_t cnt = 0;

    /* Initialize the system clock is 120M*/
    SystemInit(120);
    /* Setup SysTick Timer as delay function, and input frequency is 120M */
    systick_delay_init(120);
    CanInit(4, 500, NULL, 0);
    HwTimerInit();
    INT_All_Enable(TRUE);
    g_tmsg.format = CAN_FORMAT_STANDARD;
    g_tmsg.type  = CAN_TYPE_DATA;
    g_tmsg.len = 8;

    while(1) {
    /* test1 */
        g_bcmTs = TimerGetTickCount() + MAIN_TASK_BCM_INTERVAL;

        if (++cnt >= 20) {
            cnt = 0;
            //CanInit(4, 500, NULL, 0);
            CanGotoSleep(4);
            TimerTickStop();
            asm("SLEEP");
            TimerTickStart();
        }
        CanSend(4, &g_rmsg);
        if (ERR_OK == CanRecieve(4, &g_rmsg)) {
            CanSend(4, &g_rmsg);
        }

        /* sleep for reach 10 ms interval */    
        while(TimeAfter(g_bcmTs, TimerGetTickCount()));
    }

    return;
}

uint32_t g_cnt = 0;
void test_timer(void)
{
    /* Initialize the system clock is 120M*/
    SystemInit(120);
    /* Setup SysTick Timer as delay function, and input frequency is 120M */
    systick_delay_init(120);

    HwTimerInit();
    INT_All_Enable(TRUE);

    while(1) {
        g_bcmTs = TimerGetTickCount() + MAIN_TASK_BCM_INTERVAL;

        for (g_cnt = 0; g_cnt < 300; g_cnt++) {
         TimerDelayUs(50*1000);
        }


        /* sleep for reach 10 ms interval */    
        while(TimeAfter(g_bcmTs, TimerGetTickCount()));
    }
}


void test_adc(void)
{
    /* Initialize the system clock is 120M*/
    SystemInit(120);
    /* Setup SysTick Timer as delay function, and input frequency is 120M */
    systick_delay_init(120);

    HwTimerInit();
    UartInit(2, 115200);

    ADCInit();

    INT_All_Enable(TRUE);

    while(1) {
        g_bcmTs = TimerGetTickCount() + MAIN_TASK_BCM_INTERVAL;

        ADCGet(0, 0);



        /* sleep for reach 10 ms interval */    
        while(TimeAfter(g_bcmTs, TimerGetTickCount()));
    }
}

LinMsg g_lin_msg;
uint8_t g_lin_tbuf[] = {1,2,3,4,5,6,7,8};
void test_lin(void)
{
    /* Initialize the system clock is 120M*/
    SystemInit(120);
    /* Setup SysTick Timer as delay function, and input frequency is 120M */
    systick_delay_init(120);

    HwTimerInit();

    LinInit();

    INT_All_Enable(TRUE);

    g_lin_msg.id = 0x34;
    g_lin_msg.len = 8;
    memcpy(g_lin_msg.data, g_lin_tbuf, 8);

    while(1) {
        g_bcmTs = TimerGetTickCount() + MAIN_TASK_BCM_INTERVAL;

        LinSend(&g_lin_msg, 0, 1);


        /* sleep for reach 10 ms interval */    
        while(TimeAfter(g_bcmTs, TimerGetTickCount()));
    }
}

void test_watchdog(void)
{
    /* Initialize the system clock is 120M*/
    SystemInit(120);
    /* Setup SysTick Timer as delay function, and input frequency is 120M */
    systick_delay_init(120);

    HwTimerInit();

    WDogInit();

    INT_All_Enable(TRUE);

    while(1) {
        g_bcmTs = TimerGetTickCount() + MAIN_TASK_BCM_INTERVAL;

        WDogClear();

        /* sleep for reach 10 ms interval */    
        while(TimeAfter(g_bcmTs, TimerGetTickCount()));
    }
}


void test_watchdog1(void)
{
    uint8_t cnt = 0;

    GPIO_InitTypeDef GPIO_InitStructure;

    /* Initialize the system clock is 120M*/
    SystemInit(120);
    /* Setup SysTick Timer as delay function, and input frequency is 120M */
    systick_delay_init(120);

    HwTimerInit();

    //WDogInit();

    INT_All_Enable(TRUE);

    /* Configure PD12 LED1 output */
    GPIO_InitStructure.m_Mode = GPIO_MODE_OUT;
    GPIO_InitStructure.m_OpenDrain = GPIO_POD_PP;
    GPIO_InitStructure.m_PullDown = GPIO_NOPULL;
    GPIO_InitStructure.m_PullUp = GPIO_NOPULL;
    GPIO_InitStructure.m_Speed = GPIO_LOW_SPEED;
    GPIO_InitStructure.m_Pin = GPIO_PIN_MASK_12;
    GPIO_Configuration(GPIOD_SFR , &GPIO_InitStructure);
    GPIO_Set_Output_Data_Bits(GPIOD_SFR, GPIO_PIN_MASK_12, 0);

    while(1) {
        g_bcmTs = TimerGetTickCount() + MAIN_TASK_BCM_INTERVAL;

        //WDogReset();

        /* sleep for reach 10 ms interval */    
        while(TimeAfter(g_bcmTs, TimerGetTickCount()));
        GPIO_Set_Output_Data_Bits(GPIOD_SFR, GPIO_PIN_MASK_12, 1);
    }
}


uint8_t flash_buf[2000] = {0};
uint32_t addr = 0;
void test_flash(void)
{
    uint32_t i = 0;
    uint32_t tmp = 0;

    /* Initialize the system clock is 120M*/
    SystemInit(120);
    /* Setup SysTick Timer as delay function, and input frequency is 120M */
    systick_delay_init(120);

    HwTimerInit();

    INT_All_Enable(TRUE);

    while(1) {
        g_bcmTs = TimerGetTickCount() + MAIN_TASK_BCM_INTERVAL;

        for (i = 0; i < sizeof(flash_buf); i++) {
            flash_buf[i] = tmp + i;
        }
        tmp += 10;

       // FlashCodeLoadDriver();
        FlashCodeErase(0x8000, 222);
        FlashCodeWrite(0x8000, flash_buf, 50*32);
        addr = (uint32_t)FlashCodeGetDrvAddr();
        /* sleep for reach 10 ms interval */    
        while(TimeAfter(g_bcmTs, TimerGetTickCount()));
    }
}


void wakeup_key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EINT_InitTypeDef EINT_InitStructure;

    /* Configure PD12 LED1 output */
    GPIO_InitStructure.m_Mode = GPIO_MODE_OUT;
    GPIO_InitStructure.m_OpenDrain = GPIO_POD_PP;
    GPIO_InitStructure.m_PullDown = GPIO_NOPULL;
    GPIO_InitStructure.m_PullUp = GPIO_NOPULL;
    GPIO_InitStructure.m_Speed = GPIO_LOW_SPEED;
    GPIO_InitStructure.m_Pin = GPIO_PIN_MASK_12;
    GPIO_Configuration(GPIOD_SFR , &GPIO_InitStructure);

    /* Configure PD7 user key input */
    GPIO_InitStructure.m_Mode = GPIO_MODE_IN;
    GPIO_InitStructure.m_Pin = GPIO_PIN_MASK_7;
    GPIO_Configuration(GPIOD_SFR , &GPIO_InitStructure);
    /* Configure the external interrupt */
    EINT_InitStructure.m_Fall = TRUE;
    EINT_InitStructure.m_Line = INT_EXTERNAL_INTERRUPT_7;
    EINT_InitStructure.m_Mask = TRUE;
    EINT_InitStructure.m_Rise = TRUE;
    EINT_InitStructure.m_Source = INT_EXTERNAL_SOURCE_PD;
    INT_External_Configuration(&EINT_InitStructure);

    return;
}
//*****************************************************************************************
//                              EXTI9TO5 Interrupt Course
//*****************************************************************************************	
void __attribute__((interrupt)) _EINT9TO5_exception (void)
{
    /* Get the interrupt flag on EXTI7 */
    if(INT_Get_External_Flag(INT_EXTERNAL_INTERRUPT_7) != RESET)
    {
        /* Clear the interrupt flag */
        INT_External_Clear_Flag(INT_EXTERNAL_INTERRUPT_7);
    }
}

uint32_t TEST_INTERVAL = 10;
void test_sleep(void)
{
    uint32_t i = 0;
    uint32_t tmp = 0;
    uint32_t cnt = 0;

    /* Initialize the system clock is 120M*/
    SystemInit(120);
    /* Setup SysTick Timer as delay function, and input frequency is 120M */
    systick_delay_init(120);

    wakeup_key_init();
    /* Enable and set EXTI9_5 Interrupt to the lowest priority */
    INT_Interrupt_Enable(INT_EINT9TO5, TRUE);
    INT_Interrupt_Priority_Config(INT_EINT9TO5, 7, 1);
    /* Configure interrupt priority group, default is 3VS1 */
    INT_Priority_Group_Config(INT_PRIORITY_GROUP_3VS1);


    HwTimerInit();
    WDogInit();

    INT_All_Enable(TRUE);

    while(1) {
        GPIO_Set_Output_Data_Bits(GPIOD_SFR, GPIO_PIN_MASK_12, 0);
        g_bcmTs = TimerGetTickCount() + TEST_INTERVAL;
        if (++cnt >= 10) {
            cnt = 0;
            GPIO_Set_Output_Data_Bits(GPIOD_SFR, GPIO_PIN_MASK_12, 1);
            TimerTickStop();
            WDogEnable(FALSE);
            asm("SLEEP");
            WDogEnable(TRUE);
            TimerTickStart();
            TEST_INTERVAL += 10;
        }

        WDogClear();
        /* sleep for reach 10 ms interval */    
        while(TimeAfter(g_bcmTs, TimerGetTickCount()));
    }
}

uint8_t g_uart_data[32] = {0};
uint16_t g_uart_len = 0;
void test_uart(void)
{
    uint32_t i = 0;
    uint32_t tmp = 0;
    const uint8_t mark[8] = {0xB5,0xB5,0x00,0x00,0x00,0x00,0x00,0x00};

    /* Initialize the system clock is 120M*/
    SystemInit(120);
    /* Setup SysTick Timer as delay function, and input frequency is 120M */
    systick_delay_init(120);
    HwTimerInit();
    UartInit(0, 115200);
    INT_All_Enable(TRUE);

    while(1) {
        g_bcmTs = TimerGetTickCount() + 2000;
        UartSend(0, "hello,world!", sizeof("hello,world!"));
        //DbgPrintfV("hello,world! by %s", "chs");
        UartRecv(0, g_uart_data, &g_uart_len);
        if (0 != g_uart_len) {
        	//UartSend(0, g_uart_data, g_uart_len);
            g_uart_len = 0;
        }
        /* sleep for reach 10 ms interval */    
        while(TimeAfter(g_bcmTs, TimerGetTickCount()));
    }
}

#pragma pack (1)
typedef struct {
	uint8_t a1 :1;
	uint8_t a2 :1;
	uint8_t a3 :1;
	uint16_t a4 :13;
	uint16_t a5 :12;
	uint16_t a6 :12;
	uint16_t a7 :12;
} test01;
#pragma pack ()
volatile test01 _test = {1,0,1, 5000, 4000, 3000, 2000};

uint8_t g_spi_tx_data[32] = "hello,world!";
uint8_t g_spi_rx_data[32] = {0};
void test_spi(void)
{
    uint32_t i = 0;
    uint32_t tmp = 0;

    /* Initialize the system clock is 120M*/
    SystemInit(120);
    /* Setup SysTick Timer as delay function, and input frequency is 120M */
    systick_delay_init(120);

    HwTimerInit();
    INT_All_Enable(TRUE);

    HwSpiInit(3, 1000000);

    _test.a6 = 4094;
    while(1) {
        g_bcmTs = TimerGetTickCount() + MAIN_TASK_BCM_INTERVAL;
        g_spi_tx_data[13] = i++;
        GPIO_Output_Low(GPIO_SPI1_CS1);
        hwSpiTransfer(1, g_spi_tx_data, g_spi_rx_data, /*strlen(g_spi_tx_data) + 2*/ 3);
        GPIO_Output_High(GPIO_SPI1_CS1);
        /* sleep for reach 10 ms interval */    
        while(TimeAfter(g_bcmTs, TimerGetTickCount()));
    }
}

void test_afe(void)
{
    uint32_t cnt = 0;
    uint8_t i = 0;
    uint16_t v = 0;
    uint8_t t = 0;

    /* Initialize the system clock is 120M*/
    SystemInit(120);
    /* Setup SysTick Timer as delay function, and input frequency is 120M */
    systick_delay_init(120);

    HwTimerInit();
    INT_All_Enable(TRUE);
    UartInit(2, 115200);

    HwSpiInit(1, 1000000);
    hwSpiTransfer(1, g_spi_tx_data, g_spi_rx_data, /*strlen(g_spi_tx_data) + 2*/ 1);
    AfeInit();

    while(1) {
        g_bcmTs = TimerGetTickCount() + 10;
        AfePoll();
        if (20 == ++cnt) {
            cnt = 0;
            DbgPrintfV("PEC ERR:%d\n\r", AfePecErrGet());
            DbgPrintfV("CELL V:");
            for (i = 0; i < 70; i++) {
                if (ERR_OK != AfeCellVoltageGet(i, &v)) {
                    v = 65535;
                }
                DbgPrintfV("%d:%d ", i + 1, v);
            }
            DbgPrintfV("\n\r");

            DbgPrintfV("GPIO V:");
            for (i = 0; i < 14; i++) {
                if (ERR_OK != AfeGpioVoltageGet(i, &v)) {
                    v = 65535;
                }
                DbgPrintfV("%d:%d ", i + 1, v);
            }
            DbgPrintfV("\n\r");
        }
        /* sleep for reach 10 ms interval */
        while(TimeAfter(g_bcmTs, TimerGetTickCount()));
    }
}

uint16_t w25qxx_id = 0;
void test_w25qxx(void)
{
    uint32_t i = 0;
    uint32_t tmp = 0;

    /* Initialize the system clock is 120M*/
    SystemInit(120);
    /* Setup SysTick Timer as delay function, and input frequency is 120M */
    systick_delay_init(120);

    HwTimerInit();
    INT_All_Enable(TRUE);
    UartInit(2, 115200);
    HwSpiInit(1, 1000000);

    while(1) {
        g_bcmTs = TimerGetTickCount() + 10;
        w25qxx_get_id(&w25qxx_id);
        DbgPrintfV("id:%04x\n\r", w25qxx_id);
        /* sleep for reach 10 ms interval */    
        while(TimeAfter(g_bcmTs, TimerGetTickCount()));
    }
}

/*******************************************************************************
**                     			Main Functions 		             	     	  **
*******************************************************************************/
/**
 *  @brief :Main program
 *  @param in :None
 *  @param out :None
 *  @retval :None
 */
int main()
{
    //test_can();
    //test_can1();
    //test_gpio();
    //test_timer();
    //test_adc();
    //test_lin();
    //test_watchdog();
    //test_watchdog1();
    //test_flash();
    //test_sleep();
    //test_canfd();
    //test_canfd1();
    //test_uart();
    //test_spi();
    test_afe();
    //test_w25qxx();

    return 0;
}

/**
 *  @brief : Reports the name of the source file and the source line number
 *           where the assert_param error has occurred.
 *  @param in :	file pointer to the source file name
 * 	@param in :	line assert_param error line source number
 *  @param out :None
 *  @retval :None
 */
void check_failed(uint8_t* File, uint32_t Line)
{
	/* User can add his own implementation to report the file name and line number,
		ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while(1)
	{
		;
	}
};
