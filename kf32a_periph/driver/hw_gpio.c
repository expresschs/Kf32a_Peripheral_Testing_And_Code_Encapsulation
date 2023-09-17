#include "kf32a156_gpio.h"
#include "def_pins.h"
#include "hw_gpio.h"

#define PortA                 ((uint32_t)0x50000000)
#define PortB                 ((uint32_t)0x50000040)
#define PortC                 ((uint32_t)0x50000080)
#define PortD                 ((uint32_t)0x500000C0)
#define PortE                 ((uint32_t)0x50000100)
#define PortF                 ((uint32_t)0x50000140)
#define PortG                 ((uint32_t)0x50000180)
#define PortH                 ((uint32_t)0x500001C0)

#define Pin0                  ((uint8_t)0x0)
#define Pin1                  ((uint8_t)0x1)
#define Pin2                  ((uint8_t)0x2)
#define Pin3                  ((uint8_t)0x3)
#define Pin4                  ((uint8_t)0x4)
#define Pin5                  ((uint8_t)0x5)
#define Pin6                  ((uint8_t)0x6)
#define Pin7                  ((uint8_t)0x7)
#define Pin8                  ((uint8_t)0x8)
#define Pin9                  ((uint8_t)0x9)
#define Pin10                 ((uint8_t)0xA)
#define Pin11                 ((uint8_t)0xB)
#define Pin12                 ((uint8_t)0xC)
#define Pin13                 ((uint8_t)0xD)
#define Pin14                 ((uint8_t)0xE)
#define Pin15                 ((uint8_t)0xF)

#define PIN_MASK(p)           (uint32_t)(1<<p)

typedef struct {
    enum GPIO_Name Name;
    uint32_t Port;
    uint8_t PinNumber;
}GPIOPin;

static GPIOPin g_GPIO_Pin[] = {
    {GPIO_CAN_RX,           PortC, Pin11},    /*  */
    {GPIO_CAN_TX,           PortC, Pin10},    /*  */
    {GPIO_CAN_STB,          PortC, Pin2},     /*  */

    {GPIO_UART0_RX,         PortF, Pin12},     /*  */
    {GPIO_UART0_TX,         PortF, Pin10},     /*  */
    {GPIO_UART2_RX,         PortE, Pin9},     /*  */
    {GPIO_UART2_TX,         PortE, Pin8},     /*  */

    {GPIO_SPI1_CLK,          PortD, Pin5},    /*  */
    {GPIO_SPI1_MOSI,         PortC, Pin14},    /*  */
    {GPIO_SPI1_MISO,         PortD, Pin4},    /*  */
    {GPIO_SPI1_CS1,          PortF, Pin3},    /* MT9820 */
    {GPIO_SPI1_CS2,          PortA, Pin1},    /* W25QXX */

    {GPIO_ADC_01,            PortB, Pin3},    /*  */
    {GPIO_ADC_02,            PortE, Pin7},     /*  */
    {GPIO_ADC_03,            PortD, Pin6},     /*  */
    {GPIO_ADC_04,            PortD, Pin7},     /*  */
    {GPIO_ADC_05,            PortF, Pin7},     /*  */
};

 /* 初始化GPIO为输出，低电平 */
void GPIO_Init_Output(enum GPIO_Name GPIO_List)
{
    GPIO_InitTypeDef _init;

    _init.m_Mode = GPIO_MODE_OUT;
    _init.m_OpenDrain = GPIO_POD_PP;
    _init.m_PullDown = GPIO_NOPULL;
    _init.m_PullUp = GPIO_NOPULL;
    _init.m_Speed = GPIO_LOW_SPEED;
    _init.m_Pin = PIN_MASK(g_GPIO_Pin[GPIO_List].PinNumber);
    GPIO_Configuration((GPIO_SFRmap *)g_GPIO_Pin[GPIO_List].Port, &_init);

    return;
}

 /* 初始化GPIO为输入 */
void GPIO_Init_Input(enum GPIO_Name GPIO_List)
{
    GPIO_InitTypeDef _init;

    /* Configure PC0 user key input */
    _init.m_Mode = GPIO_MODE_IN;
    _init.m_Pin = PIN_MASK(g_GPIO_Pin[GPIO_List].PinNumber);
    GPIO_Configuration((GPIO_SFRmap *)g_GPIO_Pin[GPIO_List].Port , &_init);
}

/* 配置GPIO内部上拉 */
void GPIO_Init_PullUp(enum GPIO_Name GPIO_List)
{
    GPIO_Pull_Up_Enable((GPIO_SFRmap *)g_GPIO_Pin[GPIO_List].Port, PIN_MASK(g_GPIO_Pin[GPIO_List].PinNumber), TRUE);
}

 /* 配置GPIO内部下拉 */
 void GPIO_Init_PullDown(enum GPIO_Name GPIO_List)
 {
    GPIO_Pull_Down_Enable((GPIO_SFRmap *)g_GPIO_Pin[GPIO_List].Port, PIN_MASK(g_GPIO_Pin[GPIO_List].PinNumber), FALSE);
 }

 /* GPIO端口输出为高 */
void GPIO_Output_High(enum GPIO_Name GPIO_List)
{
    GPIO_Set_Output_Data_Bits((GPIO_SFRmap *)g_GPIO_Pin[GPIO_List].Port,  PIN_MASK(g_GPIO_Pin[GPIO_List].PinNumber), Bit_SET);
}

 /* GPIO端口输出为低 */
void GPIO_Output_Low(enum GPIO_Name GPIO_List)
{
    GPIO_Set_Output_Data_Bits((GPIO_SFRmap *)g_GPIO_Pin[GPIO_List].Port, PIN_MASK(g_GPIO_Pin[GPIO_List].PinNumber), Bit_RESET);
}

 /*GPIO端口输出状态翻转*/
void GPIO_Output_Toggle(enum GPIO_Name GPIO_List)
{
    GPIO_Toggle_Output_Data_Config((GPIO_SFRmap *)g_GPIO_Pin[GPIO_List].Port, PIN_MASK(g_GPIO_Pin[GPIO_List].PinNumber));
}

 /*读取GPIO端口状态*/
uint8_t GPIO_Input_Read(enum GPIO_Name GPIO_List)
{
    uint8_t GPIO_Level;
    GPIO_Level = GPIO_Read_Input_Data_Bit((GPIO_SFRmap *)g_GPIO_Pin[GPIO_List].Port, PIN_MASK(g_GPIO_Pin[GPIO_List].PinNumber));
    return GPIO_Level;
}

 /* 配置STOP MODE唤醒端口 */
void GPIO_Set_Wakeup_Func(void)
{
}

/* 配置CAN端口 */
void GPIO_CAN0_Init(void)
{
}

void GPIO_ADC_Init(void)
{
    GPIO_Write_Mode_Bits((GPIO_SFRmap *)g_GPIO_Pin[GPIO_ADC_01].Port, PIN_MASK(g_GPIO_Pin[GPIO_ADC_01].PinNumber), GPIO_MODE_AN);  /* CH55 PB3 */
    GPIO_Write_Mode_Bits((GPIO_SFRmap *)g_GPIO_Pin[GPIO_ADC_02].Port, PIN_MASK(g_GPIO_Pin[GPIO_ADC_02].PinNumber), GPIO_MODE_AN);  /* CH67 PE7 */

    GPIO_Write_Mode_Bits((GPIO_SFRmap *)g_GPIO_Pin[GPIO_ADC_03].Port, PIN_MASK(g_GPIO_Pin[GPIO_ADC_03].PinNumber), GPIO_MODE_AN);  /* CH95 PD6 */
    GPIO_Write_Mode_Bits((GPIO_SFRmap *)g_GPIO_Pin[GPIO_ADC_04].Port, PIN_MASK(g_GPIO_Pin[GPIO_ADC_04].PinNumber), GPIO_MODE_AN);  /* CH04 PD7 */
    GPIO_Write_Mode_Bits((GPIO_SFRmap *)g_GPIO_Pin[GPIO_ADC_05].Port, PIN_MASK(g_GPIO_Pin[GPIO_ADC_05].PinNumber), GPIO_MODE_AN);  /* CH14 PF7 */

    return;
}

void GPIO_CAN_Init(void)
{
    //PC10=CAN4TX,PC11=CNA4RX
    GPIO_Pull_Up_Enable((GPIO_SFRmap *)g_GPIO_Pin[GPIO_CAN_RX].Port, PIN_MASK(g_GPIO_Pin[GPIO_CAN_RX].PinNumber), TRUE);
    //GPIO_Pull_Up_Enable((GPIO_SFRmap *)g_GPIO_Pin[GPIO_CAN_TX].Port, PIN_MASK(g_GPIO_Pin[GPIO_CAN_TX].PinNumber), TRUE);

    GPIO_Write_Mode_Bits((GPIO_SFRmap *)g_GPIO_Pin[GPIO_CAN_RX].Port, PIN_MASK(g_GPIO_Pin[GPIO_CAN_RX].PinNumber), GPIO_MODE_RMP);
    GPIO_Write_Mode_Bits((GPIO_SFRmap *)g_GPIO_Pin[GPIO_CAN_TX].Port, PIN_MASK(g_GPIO_Pin[GPIO_CAN_TX].PinNumber), GPIO_MODE_RMP);

    GPIO_Pin_RMP_Config((GPIO_SFRmap *)g_GPIO_Pin[GPIO_CAN_RX].Port,  g_GPIO_Pin[GPIO_CAN_RX].PinNumber, GPIO_RMP_AF6);
    GPIO_Pin_RMP_Config((GPIO_SFRmap *)g_GPIO_Pin[GPIO_CAN_TX].Port,  g_GPIO_Pin[GPIO_CAN_TX].PinNumber, GPIO_RMP_AF6);
}

void GPIO_Uart0_Init(void)
{
    GPIO_Write_Mode_Bits((GPIO_SFRmap *)g_GPIO_Pin[GPIO_UART0_RX].Port, PIN_MASK(g_GPIO_Pin[GPIO_UART0_RX].PinNumber), GPIO_MODE_RMP);
    GPIO_Write_Mode_Bits((GPIO_SFRmap *)g_GPIO_Pin[GPIO_UART0_TX].Port, PIN_MASK(g_GPIO_Pin[GPIO_UART0_TX].PinNumber), GPIO_MODE_RMP);
    GPIO_Pin_RMP_Config((GPIO_SFRmap *)g_GPIO_Pin[GPIO_UART0_RX].Port, g_GPIO_Pin[GPIO_UART0_RX].PinNumber, GPIO_RMP_AF3);
    GPIO_Pin_RMP_Config((GPIO_SFRmap *)g_GPIO_Pin[GPIO_UART0_TX].Port, g_GPIO_Pin[GPIO_UART0_TX].PinNumber, GPIO_RMP_AF3);
}

void GPIO_Uart2_Init(void)
{
    GPIO_Write_Mode_Bits((GPIO_SFRmap *)g_GPIO_Pin[GPIO_UART2_RX].Port, PIN_MASK(g_GPIO_Pin[GPIO_UART2_RX].PinNumber), GPIO_MODE_RMP);
    GPIO_Write_Mode_Bits((GPIO_SFRmap *)g_GPIO_Pin[GPIO_UART2_TX].Port, PIN_MASK(g_GPIO_Pin[GPIO_UART2_TX].PinNumber), GPIO_MODE_RMP);
    GPIO_Pin_RMP_Config((GPIO_SFRmap *)g_GPIO_Pin[GPIO_UART2_RX].Port, g_GPIO_Pin[GPIO_UART2_RX].PinNumber, GPIO_RMP_AF3);
    GPIO_Pin_RMP_Config((GPIO_SFRmap *)g_GPIO_Pin[GPIO_UART2_TX].Port, g_GPIO_Pin[GPIO_UART2_TX].PinNumber, GPIO_RMP_AF3);
}

void GPIO_Spi1_Init(void)
{
    GPIO_Write_Mode_Bits((GPIO_SFRmap *)g_GPIO_Pin[GPIO_SPI1_CLK].Port, PIN_MASK(g_GPIO_Pin[GPIO_SPI1_CLK].PinNumber), GPIO_MODE_RMP);
    GPIO_Write_Mode_Bits((GPIO_SFRmap *)g_GPIO_Pin[GPIO_SPI1_MOSI].Port, PIN_MASK(g_GPIO_Pin[GPIO_SPI1_MOSI].PinNumber), GPIO_MODE_RMP);
    GPIO_Write_Mode_Bits((GPIO_SFRmap *)g_GPIO_Pin[GPIO_SPI1_MISO].Port, PIN_MASK(g_GPIO_Pin[GPIO_SPI1_MISO].PinNumber), GPIO_MODE_RMP);

    GPIO_Pin_RMP_Config((GPIO_SFRmap *)g_GPIO_Pin[GPIO_SPI1_CLK].Port, g_GPIO_Pin[GPIO_SPI1_CLK].PinNumber, GPIO_RMP_AF4);
    GPIO_Pin_RMP_Config((GPIO_SFRmap *)g_GPIO_Pin[GPIO_SPI1_MOSI].Port, g_GPIO_Pin[GPIO_SPI1_MOSI].PinNumber, GPIO_RMP_AF4);
    GPIO_Pin_RMP_Config((GPIO_SFRmap *)g_GPIO_Pin[GPIO_SPI1_MISO].Port, g_GPIO_Pin[GPIO_SPI1_MISO].PinNumber, GPIO_RMP_AF4);

    GPIO_Init_Output(GPIO_SPI1_CS1);
    GPIO_Init_Output(GPIO_SPI1_CS2);
    GPIO_Output_High(GPIO_SPI1_CS1);
    GPIO_Output_High(GPIO_SPI1_CS2);
}


