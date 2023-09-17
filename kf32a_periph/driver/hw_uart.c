#include "kf32a156_int.h"
#include "kf32a156_usart.h"
#include "hw_gpio.h"
#include "hw_timer.h"
#include "queue.h"
#include "hw_uart.h"

#define UART0_HANDLE    USART0_SFR
#define UART1_HANDLE    USART1_SFR
#define UART2_HANDLE    USART2_SFR

static Queue_Type g_ch0TxQueue;
static Queue_Type g_ch0RxQueue;
static uint8_t g_ch0TxBuf[512];
static uint8_t g_ch0RxBuf[512];

static Queue_Type g_ch1TxQueue;
static Queue_Type g_ch1RxQueue;
static uint8_t g_ch1TxBuf[256];
static uint8_t g_ch1RxBuf[256];

static Queue_Type g_ch2TxQueue;
static Queue_Type g_ch2RxQueue;
static uint8_t g_ch2TxBuf[1024];
static uint8_t g_ch2RxBuf[256];

static void _ch0_port_init(void)
{
    GPIO_Uart0_Init();

    return;
}

static void _ch1_port_init(void)
{
    /* TODO */
    return;
}

static void _ch2_port_init(void)
{
    GPIO_Uart2_Init();

    return;
}

/*
    ch: 0 
        2
    baud: 9600(default) 
          115200
*/
uint8_t UartInit(uint8_t ch, uint32_t baud)
{
    uint8_t ret = 1;
    USART_InitTypeDef USART_InitStructure;
    USART_Struct_Init(&USART_InitStructure);

    if (ch > 2) {
        return ret;
    }

    if (0 == ch) {
        QueueInit(&g_ch0TxQueue, g_ch0TxBuf, 1, sizeof(g_ch0TxBuf));
        QueueInit(&g_ch0RxQueue, g_ch0RxBuf, 1, sizeof(g_ch0RxBuf));
        _ch0_port_init();
        /* Set Usart To Async Mode */
        USART_InitStructure.m_Mode = USART_MODE_FULLDUPLEXASY;
        USART_InitStructure.m_TransferDir = USART_DIRECTION_FULL_DUPLEX;
        USART_InitStructure.m_WordLength = USART_WORDLENGTH_8B;
        USART_InitStructure.m_StopBits = USART_STOPBITS_1;

        USART_InitStructure.m_BaudRateBRCKS = USART_CLK_HFCLK;
        if (115200 == baud) {
            USART_InitStructure.m_BaudRateInteger = 8;
            USART_InitStructure.m_BaudRateNumerator = 1;
            USART_InitStructure.m_BaudRateDenominator = 13;
        } else {
            USART_InitStructure.m_BaudRateInteger = 50;
            USART_InitStructure.m_BaudRateNumerator = 26;
            USART_InitStructure.m_BaudRateDenominator = 24;
        }
        /* Set Usart to send enable */
        USART_Reset(UART0_HANDLE);
        USART_Configuration(UART0_HANDLE, &USART_InitStructure);
        USART_Passageway_Select_Config(UART0_HANDLE, USART_U7816R_PASSAGEWAY_TX0);
        USART_RESHD_Enable(UART0_HANDLE, TRUE);
        USART_Transmit_Data_Enable(UART0_HANDLE, TRUE);
        /* Resetting USARTx sends the BUF interrupt bit */
        USART_Receive_Data_Enable(UART0_HANDLE, TRUE);
        /* SET USARTx RDR interrupt enable */
        USART_RDR_INT_Enable(UART0_HANDLE, TRUE);

        //USART_Frame_ERROE_INT_Enable(UART0_HANDLE, TRUE);

        /*Interrupt enables a peripheral or kernel interrupt vector */
        INT_Interrupt_Enable(INT_USART0, TRUE);
        USART_Cmd(UART0_HANDLE, TRUE);
        ret = 0;
    } else if (1 == ch) {
        /* TODO */
    } else if (2 == ch) {
        QueueInit(&g_ch2TxQueue, g_ch2TxBuf, 1, sizeof(g_ch2TxBuf));
        QueueInit(&g_ch2RxQueue, g_ch2RxBuf, 1, sizeof(g_ch2RxBuf));
        _ch2_port_init();
        /* Set Usart To Async Mode */
        USART_InitStructure.m_Mode = USART_MODE_FULLDUPLEXASY;
        USART_InitStructure.m_TransferDir = USART_DIRECTION_FULL_DUPLEX;
        USART_InitStructure.m_WordLength = USART_WORDLENGTH_8B;
        USART_InitStructure.m_StopBits = USART_STOPBITS_1;

        USART_InitStructure.m_BaudRateBRCKS = USART_CLK_HFCLK;
        if (115200 == baud) {
            USART_InitStructure.m_BaudRateInteger = 8;
            USART_InitStructure.m_BaudRateNumerator = 1;
            USART_InitStructure.m_BaudRateDenominator = 13;
        } else {
            USART_InitStructure.m_BaudRateInteger = 50;
            USART_InitStructure.m_BaudRateNumerator = 26;
            USART_InitStructure.m_BaudRateDenominator = 24;
        }
        /* Set Usart to send enable */
        USART_Reset(UART2_HANDLE);
        USART_Configuration(UART2_HANDLE, &USART_InitStructure);
        USART_Passageway_Select_Config(UART2_HANDLE, USART_U7816R_PASSAGEWAY_TX0);
        USART_RESHD_Enable(UART2_HANDLE, TRUE);
        USART_Transmit_Data_Enable(UART2_HANDLE, TRUE);
        /* Resetting USARTx sends the BUF interrupt bit */
        USART_Receive_Data_Enable(UART2_HANDLE, TRUE);
        /* SET USARTx RDR interrupt enable */
        USART_RDR_INT_Enable(UART2_HANDLE, TRUE);
        /*Interrupt enables a peripheral or kernel interrupt vector */
        INT_Interrupt_Enable(INT_USART2, TRUE);
        USART_Cmd(UART2_HANDLE, TRUE);
        ret = 0;
    }

    return ret;
}

uint8_t UartSend(uint8_t ch, uint8_t *data, uint16_t len)
{
    uint8_t ret = 1;
    uint16_t i = 0;
    uint8_t c= 0;

    if (len < 1 || ch > 2 || data == NULL) {
        return ret;
    }

    if (0 == ch) {
        for (i = 0; i < len; i++){
            ret = QueuePut(&g_ch0TxQueue, data + i);
        }
        USART_TXE_INT_Enable(UART0_HANDLE, TRUE);
        ret = 0;
    } else if (1 == ch) {
        /* TODO */
    } else if (2 == ch) {
        for (i = 0; i < len; i++){
            ret = QueuePut(&g_ch2TxQueue, data + i);
        }
        USART_TXE_INT_Enable(UART2_HANDLE, TRUE);
        ret = 0;
    }

    return ret;
}

uint8_t UartRecv(uint8_t ch, uint8_t *data, uint16_t *len)
{
    uint8_t ret = 1;
    uint16_t _len = 0;
    uint16_t i = 0;

    if (len == NULL || ch > 2 || data == NULL) {
        return ret;
    }

    if (0 == ch) {
        if (1 == IsTimerOut()) {
            _len =  QeueuGetSize(&g_ch0RxQueue);
            for(i = 0; i < _len; i++){
                QueueGet(&g_ch0RxQueue, data + i);
            }
            *len = _len;
            ret = 0;
        }
    } else if (1 == ch) {
        /* TODO */
    } else if (2 == ch) {
        _len =  QeueuGetSize(&g_ch2RxQueue);
        for(i = 0; i < _len; i++){
            QueueGet(&g_ch2RxQueue, data + i);
        }
        *len = _len;
        ret = 0;
    }

    return ret;
}

uint8_t UartDeinit(uint8_t ch)
{
    uint8_t ret = 1;

    /* TODO */

    return ret;
}

void __attribute__((interrupt)) _USART0_exception (void)
{
    uint8_t c = 0;
    /*	If gets the status of the USART data ready interrupt flag */
    if (UART0_HANDLE->STR & USART_STR_RDRIF) {
        c = UART0_HANDLE->RBUFR;
        QueuePut(&g_ch0RxQueue, &c);
        TimerRun(1000);
    }
    if (UART0_HANDLE->STR & USART_STR_TXEIF) {
        if(0 == QueueGet(&g_ch0TxQueue, &c)) {
            USART_TXE_INT_Enable(UART0_HANDLE, FALSE);
        } else {
            UART0_HANDLE->TBUFR = c;
        }
    }
    //if (UART0_HANDLE->STR & USART_STR_FREIF) {
    //    USART_Clear_Frame_ERROR_INT_Flag(UART0_HANDLE);
    //}

    return;
}

void __attribute__((interrupt)) _USART2_exception (void)
{
    uint8_t c = 0;
    /*  If gets the status of the USART data ready interrupt flag */
    if (UART2_HANDLE->STR & USART_STR_RDRIF) {
        c = UART2_HANDLE->RBUFR;
        QueuePut(&g_ch2RxQueue, &c);
        TimerRun(1000);
    }
    if (UART2_HANDLE->STR & USART_STR_TXEIF) {
        if(0 == QueueGet(&g_ch2TxQueue, &c)) {
            USART_TXE_INT_Enable(UART2_HANDLE, FALSE);
        } else {
            UART2_HANDLE->TBUFR = c;
        }
    }

    return;
}


