#include "kf32a156_bkp.h"
#include "kf32a156_pm.h"
#include "kf32a156_iwdt.h"
#include "hw_watchdog.h"

/* TIMEOUT 250ms */
void WDogInit(void)
{
    /*enable BKP write and read*************************************************/
    BKP_Write_And_Read_Enable(TRUE);
    /*Exit reset state**********************************************************/
    PM_Independent_Watchdog_Reset_Config(PERIPHERAL_OUTRST_STATUS); 
    /*select INTLF as the IWDG clock source*************************************/
    PM_Internal_Low_Frequency_Enable(TRUE);
    /*T = 1/32K*Prescaler*Overflow=250ms*******************************************/
    IWDT_Overflow_Config(125);
    IWDT_Prescaler_Config(IWDT_PRESCALER_64);
    /*enable IWDG*/
    IWDT_Enable(TRUE);

    return;
}

void WDogEnable(uint8_t state)
{
    /*enable BKP write and read*************************************************/
    BKP_Write_And_Read_Enable(TRUE);
    /*Exit reset state**********************************************************/
    PM_Independent_Watchdog_Reset_Config(PERIPHERAL_OUTRST_STATUS); 
    /*enable IWDG*/
    IWDT_Enable(state);

    return;
}

void WDogClear(void)
{
   IWDT_Feed_The_Dog();
}

void WDogReset(void)
{
    IWDT_Overflow_Config(0);
}

