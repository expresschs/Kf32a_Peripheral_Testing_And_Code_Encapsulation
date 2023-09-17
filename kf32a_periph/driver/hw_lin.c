#include <string.h>
//#include "kf32a156_gpio.h"
#include "kf32a156_int.h"
#include "kf32a156_usart.h"
#include "hw_gpio.h"
#include "hw_timer.h"
#include "queue.h"
#include "hw_lin.h"

typedef enum{
    LIN_IDLE = 0,
    LIN_BREAK,
    LIN_SYNC,
    LIN_PID,
    LIN_DATA,
    LIN_CHECKSUM,
    LIN_CHECKSUM_AFTER,
    LIN_ERR,
}LinState;

typedef struct {
    LinState state;
    LinMode mode;
    LinCheckSum cst;
    uint8_t id;
    uint8_t pid;
    uint8_t data[8];
    uint8_t len;
    uint8_t idx;
    uint8_t checksum;
    uint32_t ts;
} LinFrm;

#define LIN_DEV_HANDLE      USART5_SFR

#define LIN_FRM_TIMEOUT     (1000UL)
#define LIN_BIT(byte, bit)  ((byte >> bit) & 0x01)

#define     LIN_QUEUE_SIZE  (sizeof(LinMsg))
#define     LIN_QUEUE_COUNT 3

Queue_Type  g_linQueue;
uint8_t     g_linQueueBuff[LIN_QUEUE_SIZE * LIN_QUEUE_COUNT];

LinFrm cur_frm;

static uint8_t _linCalcPid(uint8_t id)
{
    uint8_t pid;

    pid = id & 0x3F;

    pid |= (LIN_BIT(id, 0) ^ LIN_BIT(id, 1) ^ LIN_BIT(id, 2) ^ LIN_BIT(id, 4)) << 6;
    pid |= (!(LIN_BIT(id, 1) ^ LIN_BIT(id, 3) ^ LIN_BIT(id, 4) ^ LIN_BIT(id, 5))) << 7;

    return pid;
}

static uint8_t _linCalcChecksum(uint8_t pid, uint8_t *data, uint8_t len)
{
    uint8_t i = 0;
    uint16_t checksum = pid;

    for (i = 0; i < len; i++) {
        checksum += data[i];
        if (checksum > 0xFF) {
            checksum -= 0xFF;
        }
    }

    return (0xFF - checksum);
}
#if 0
void LINGpioIinit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure PA7/PA8 remap mode */
    GPIO_InitStructure.m_Mode = GPIO_MODE_RMP;
    GPIO_InitStructure.m_OpenDrain = GPIO_POD_PP;
    GPIO_InitStructure.m_PullDown = GPIO_NOPULL;
    GPIO_InitStructure.m_PullUp = GPIO_NOPULL;
    GPIO_InitStructure.m_Speed = GPIO_LOW_SPEED;
    GPIO_InitStructure.m_Pin = GPIO_PIN_MASK_7|GPIO_PIN_MASK_8;
    GPIO_Configuration(GPIOA_SFR , &GPIO_InitStructure);
    /* If there is no external pull-up resistor, pull-up is required */
    /* GPIO_Pull_Up_Enable(GPIOA_SFR, GPIO_PIN_MASK_8, TRUE); */
    /* Configure PA3 remap function to AF13 */
    GPIO_Pin_RMP_Config(GPIOA_SFR, GPIO_Pin_Num_7, GPIO_RMP_AF13);
    /* Configure PA8 remap function to AF13 */
    GPIO_Pin_RMP_Config(GPIOA_SFR, GPIO_Pin_Num_8, GPIO_RMP_AF13);

    return;
}
#endif
void LinInit(void)
{
    USART_InitTypeDef _init;

    QueueInit(&g_linQueue, g_linQueueBuff, LIN_QUEUE_SIZE, LIN_QUEUE_COUNT);

    //LINGpioIinit();
    GPIO_Lin_Init();
    /* Reset and enable USARTx */
    USART_Reset(LIN_DEV_HANDLE); 
    /* configure USARTx to LIN mode */
    USART_Struct_Init(&_init);
    _init.m_Mode = USART_MODE_FULLDUPLEXASY;
    _init.m_TransferDir = USART_DIRECTION_FULL_DUPLEX;
    _init.m_WordLength = USART_WORDLENGTH_8B;
    _init.m_StopBits = USART_STOPBITS_1;
    _init.m_BaudRateBRCKS = USART_CLK_HFCLK;
    _init.m_BRAutoDetect = USART_ABRDEN_OFF;
    /** Use 16M clock as an example to list the following baud rates 
     *  4800    z:208    x:0    y:0
     *  9600    z:104    x:0    y:0
     *  19200   z:52     x:0    y:0
     *  115200  z:8      x:1    y:13 */
    /* Integer part z, get value range is 0 ~ 0xFFFF */
    _init.m_BaudRateInteger = 52;
    /* Numerator part x, get value range is 0 ~ 0x0f */
    _init.m_BaudRateNumerator = 0;
    /* Denominator part y, get value range is 0 ~ 0x0f */
    _init.m_BaudRateDenominator = 0;
    USART_Configuration(LIN_DEV_HANDLE, &_init);

    USART_Receive_Data_Enable(LIN_DEV_HANDLE, TRUE);
    /* SET USARTx RDR interrupt enable */
    USART_RDR_INT_Enable(LIN_DEV_HANDLE, TRUE);
    /*Interrupt enables a peripheral or kernel interrupt vector */
    INT_Interrupt_Enable(INT_USART5, TRUE);

    /* Enable LIN moudle */
    USART_RESHD_Enable (LIN_DEV_HANDLE, TRUE);
    USART_Cmd(LIN_DEV_HANDLE, TRUE);

    return;
}

uint8_t LinSend(LinMsg *msg, LinMode mode, LinCheckSum checkSum)
{
    uint8_t i = 0;

    if (msg->len > 8) {
        return RLIN_ERROR;
    }

    if (LIN_IDLE == cur_frm.state) {
        cur_frm.ts = TimerGetTickCount() + LIN_FRM_TIMEOUT;
    } else {
        if (!TimeAfter(cur_frm.ts, TimerGetTickCount())) {
            cur_frm.state = LIN_IDLE;
            cur_frm.ts = TimerGetTickCount() + LIN_FRM_TIMEOUT;
        } else {
            return RLIN_BUSY;
        }
    }

    cur_frm.cst = checkSum;
    cur_frm.id = msg->id;
    cur_frm.pid = _linCalcPid(msg->id);
    cur_frm.len = msg->len;
    if (RLIN_SEND == (cur_frm.mode = mode)) {
        for (i = 0; i < msg->len; i++) {
            *(cur_frm.data + i) = *(msg->data + i);
        }
    }
    USART_Transmit_Data_Enable(LIN_DEV_HANDLE, FALSE); 
    USART_Send_Blank_Enable(LIN_DEV_HANDLE, TRUE);
    USART_Transmit_Data_Enable(LIN_DEV_HANDLE, TRUE);
    cur_frm.state = LIN_BREAK;

    return RLIN_OK;
}

uint8_t LinRecieve(LinMsg *msg)
{
    uint8_t ret = 1;

    if (QueueGet(&g_linQueue, msg) == 1)
        ret = 0;

    return ret;
}

static void _LinFsm(void)
{
    uint8_t c = 0;
    uint8_t pid = 0;
    uint8_t _state = LIN_IDLE;

    if (USART_Get_Receive_BUFR_Ready_Flag(LIN_DEV_HANDLE)) {
        c = LIN_DEV_HANDLE->RBUFR;
        switch (cur_frm.state) {
            case LIN_BREAK:
                if (0x00 == c) {
                    LIN_DEV_HANDLE->TBUFR = 0x55;
                    _state = LIN_SYNC;
                }
                break;
            case LIN_SYNC:
                if (0x55 == c) {
                    LIN_DEV_HANDLE->TBUFR = cur_frm.pid;
                    cur_frm.idx = 0;
                    _state = LIN_PID;
                }
                break;
            case LIN_PID:
                if (cur_frm.pid == c) {
                    if (RLIN_SEND == cur_frm.mode) {
                        LIN_DEV_HANDLE->TBUFR = cur_frm.data[cur_frm.idx++];
                    }
                    _state = LIN_DATA;
                }
                break;
            case LIN_DATA:
                if (RLIN_SEND == cur_frm.mode) {
                    LIN_DEV_HANDLE->TBUFR = cur_frm.data[cur_frm.idx++];
                } else {
                    cur_frm.data[cur_frm.idx++] = c;
                }
                if (cur_frm.idx == cur_frm.len) {
                    _state = LIN_CHECKSUM;
                } else {
                    _state = LIN_DATA;
                }
                break;
            case LIN_CHECKSUM:
                if (RLIN_ENHANCED == cur_frm.cst) {
                    pid = cur_frm.pid;
                }
                cur_frm.checksum = _linCalcChecksum(pid, cur_frm.data, cur_frm.len);
                if (RLIN_SEND == cur_frm.mode) {
                    LIN_DEV_HANDLE->TBUFR = cur_frm.checksum;
                    _state = LIN_CHECKSUM_AFTER;
                } else {
                    if (cur_frm.checksum == c) {
                        /* RECV FRM SUCCESS */
                        /* copy process */
                    }
                }
                break;
            case LIN_CHECKSUM_AFTER:
                if (cur_frm.checksum == c) {
                    /* SEND FRM SUCCESS */
                    /* copy process */
                }
                break;
        }
    }

    cur_frm.state = _state;

    return;
}

static void LinFsm(void)
{
    uint8_t c = 0;
    uint8_t pid = 0;
    uint8_t _state = LIN_ERR;
    uint8_t i = 0;
    LinMsg msg;

    if (USART_Get_Transmitter_Empty_Flag(LIN_DEV_HANDLE)) {
        if (USART_Get_Receive_BUFR_Ready_Flag(LIN_DEV_HANDLE)) {
            c = LIN_DEV_HANDLE->RBUFR;
            switch (cur_frm.state) {
                case LIN_BREAK:
                    if (0x00 == c) {
                        LIN_DEV_HANDLE->TBUFR = 0x55;
                        _state = LIN_SYNC;
                    }
                    break;
                case LIN_SYNC:
                    if (0x55 == c) {
                        LIN_DEV_HANDLE->TBUFR = cur_frm.pid;
                        cur_frm.idx = 0;
                        _state = LIN_PID;
                    }
                    break;
                case LIN_PID:
                    if (cur_frm.pid == c) {
                        if (RLIN_SEND == cur_frm.mode) {
                            LIN_DEV_HANDLE->TBUFR = cur_frm.data[cur_frm.idx++];
                        }
                        _state = LIN_DATA;
                    }
                    break;
                case LIN_DATA:
                    if (RLIN_SEND == cur_frm.mode) {
                        LIN_DEV_HANDLE->TBUFR = cur_frm.data[cur_frm.idx++];
                    } else {
                        cur_frm.data[cur_frm.idx++] = c;
                    }
                    if (cur_frm.idx == cur_frm.len) {
                        _state = LIN_CHECKSUM;
                    } else {
                        _state = LIN_DATA;
                    }
                    break;
                case LIN_CHECKSUM:
                    if (RLIN_ENHANCED == cur_frm.cst) {
                        pid = cur_frm.pid;
                    }
                    cur_frm.checksum = _linCalcChecksum(pid, cur_frm.data, cur_frm.len);
                    if (RLIN_SEND == cur_frm.mode) {
                        LIN_DEV_HANDLE->TBUFR = cur_frm.checksum;
                        _state = LIN_CHECKSUM_AFTER;
                    } else {
                        if (cur_frm.checksum == c) {
                            /* FRAME RECEIVE SUCCESS */
                            msg.id = cur_frm.id;
                            msg.len = cur_frm.len;
                            for (i = 0; i < msg.len; i++) {
                                *(msg.data + i) = *(cur_frm.data + i);
                            }
                            QueuePut(&g_linQueue, &msg);
                            _state = LIN_IDLE;
                        }
                    }
                    break;
                case LIN_CHECKSUM_AFTER:
                    if (cur_frm.checksum == c) {
                        /* FRAME SEND SUCCESS */
                        _state = LIN_IDLE;
                    }
                    break;
            }
        }
    }

    if (LIN_ERR == _state) {
        //LinUserOnBusOff();
        _state = LIN_IDLE;
    }

    cur_frm.state = _state;

    return;
}


//*****************************************************************************************
//                               USART5 Interrupt Course
//*****************************************************************************************
void __attribute__((interrupt)) _USART5_exception (void)
{
    LinFsm();

    return;
}

