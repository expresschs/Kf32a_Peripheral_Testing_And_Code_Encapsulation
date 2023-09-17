#include "kf32a156_gpio.h"
#include "kf32a156_dma.h"
#include "kf32a156_adc.h"
#include "hw_timer.h"
#include "hw_gpio.h"
#include "debug_uart.h"
#include "hw_adc.h"

uint32_t adc_buf[32] = {0};

static void _adc_gpio_init(void)
{
    GPIO_ADC_Init();

    return;
}

static void _adc_dma_init(void)
{
    DMA_InitTypeDef _init;
    /*************************************************************************/
    /*reset DMA0 moduel*******************************************************/
    DMA_Reset (DMA0_SFR);
    /* Transfer 32 bytes at a time through the DMA0 channel 5*****************/
    _init.m_Number = 5;
    /*DMATransmission direction：peripheral to memory*************************/
    _init.m_Direction = DMA_PERIPHERAL_TO_MEMORY;
    /*select low Priority for DMA0 chanel*************************************/
    _init.m_Priority = DMA_CHANNEL_LOWER;
    /* Set Peripheral data size：32bits***************************************/
    _init.m_PeripheralDataSize = DMA_DATA_WIDTH_32_BITS;
    /* Set Memory data size：32bits ******************************************/
    _init.m_MemoryDataSize = DMA_DATA_WIDTH_32_BITS;
    /* Disable Peripheral address increment mode in oder to read only ADC0_DATA
    register  ****************************************************************/
    _init.m_PeripheralInc = FALSE;
    /* Enable memory address increment mode in oder to write adc_buf**********/
    _init.m_MemoryInc = TRUE;
    /* Enable DMA0 chanel 5 because of ADC0 corresponds to DMA0 chanel 5 *****/
    _init.m_Channel = DMA_CHANNEL_5;
    /*Adc trigger once to DMA transmit once***********************************/
    _init.m_BlockMode = DMA_TRANSFER_BYTE;
    /* Enable loop mode*******************************************************/
    _init.m_LoopMode = TRUE;
    /* used ADC0_DATA register as the DMA0 peripheral address*****************/
    _init.m_PeriphAddr = (uint32_t) &ADC0_DATA; 
    /* used adc_buf as the DMA0 memory start address**************************/
    _init.m_MemoryAddr = (uint32_t) adc_buf;
    /* configure DMA0 through dmaNewStruct************************************/
    DMA_Configuration (DMA0_SFR, &_init);
    /* enable DMA0 chanel 5************************************************* */
    DMA_Channel_Enable (DMA0_SFR, DMA_CHANNEL_5, TRUE);

    return;
}

/* scangroup config */
static void _adc_sg_cfg(void)
{
    ADC_Regular_Channel_Config(ADC0_SFR, ADC_CHANNEL_55, 01); /* PB3 */
    ADC_Regular_Channel_Config(ADC0_SFR, ADC_CHANNEL_67, 02); /* PE7 */

    ADC_Regular_Channel_Config(ADC0_SFR, ADC_CHANNEL_95, 03); /* PD6 */
    ADC_Regular_Channel_Config(ADC0_SFR, ADC_CHANNEL_4, 04);  /* PD7 */
    ADC_Regular_Channel_Config(ADC0_SFR, ADC_CHANNEL_14, 05); /* PF7 */

    return;
}

/* ADC初始化 */
void ADCInit(void)
{
    ADC_InitTypeDef _init;

    _adc_dma_init();
    _adc_gpio_init();

    ADC_Reset (ADC0_SFR);
/* ADC0 init */
    /*select HFCLK as the ADC0 sampling clock source***************************
    **You can also select SCLK or LFCLK as the ADC0 sampling clock************/
    _init.m_Clock = ADC_HFCLK;
    /*ADC sampling clock Divider configuration********************************/
    _init.m_ClockDiv = ADC_CLK_DIV_32;
    /*disable adc scan mode***************************************************/
    _init.m_ScanMode = TRUE;
    /*select continuous mode as adc vonversion mode **************************/
    _init.m_ContinuousMode = ADC_CONTINUOUS_MODE;
    /*Right-aligned sampling result data *************************************/
    _init.m_DataAlign = ADC_DATAALIGN_RIGHT;
    /*disable External trigger************************************************/
    _init.m_ExternalTrig_EN = FALSE;
    _init.m_ExternalTrig = ADC_EXTERNALTRIG_CCP1_CH1; /* 必须为0 FIXME */
    _init.m_HPExternalTrig_EN = FALSE;
    _init.m_HPExternalTrig = ADC_HPEXTERNALTRIG_CCP1_CH1; /* 必须为0 FIXME */
    _init.m_VoltageRef = ADC_REF_AVDD;
    _init.m_NumOfConv = 5;
    _init.m_NumOfHPConv = 1; /* 必须为1 FIXME */

    ADC_Configuration (ADC0_SFR, &_init);
    ADC_Cmd (ADC0_SFR, TRUE);
    TimerDelayUs(10);

    _adc_sg_cfg();

    /*Enable normal channel DMA memory access mode.***************************/
    ADC_Regular_Channel_DMA_Cmd(ADC0_SFR, TRUE);

    /*software trig adc Conversion,In continuous conversion mode,only need to
    trig for the first time***************************************************/
    ADC_Software_Start_Conv(ADC0_SFR);

    return;
}



/* 读取ADC数值 */
uint16_t ADCGet(uint8_t dev, uint8_t channel)
{
    uint16_t AD_Value = 0;

    if (ADC_CHANNEL_55 == channel) {
        AD_Value = adc_buf[0];
    } else if (ADC_CHANNEL_67 == channel) {
        AD_Value = adc_buf[1];
    } else if (ADC_CHANNEL_95 == channel) {
        AD_Value = adc_buf[2];
    } else if (ADC_CHANNEL_4 == channel) {
        AD_Value = adc_buf[3];
    } else if (ADC_CHANNEL_14 == channel) {
        AD_Value = adc_buf[4];
    }
    //DbgPrintfV("adc0:%d adc1:%d adc2:%d adc3:%d adc4:%d\n\r", adc_buf[0], adc_buf[1], adc_buf[2], adc_buf[3], adc_buf[4]);

    return AD_Value;
}

/* 进入低功耗模式 */
void ADCGotoSleep(void)
{
    return;
}


