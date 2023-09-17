#include <stdint.h>
#include <string.h>
#include "common.h"
#include "queue.h"
#include "hw_gpio.h"
//#include "kf32a156_gpio.h"
#include "kf32a156_int.h"
#include "kf32a156_can.h"
#include "hw_can.h"


#define     HW_CAN_FRM_LEN    (16)                   /* 16bytes in ram */
#define     HW_CAN_RAM_DEPTH  (64 * HW_CAN_FRM_LEN)  /* max 64 messages */
#define     HW_CAN_MAX_FILTER (9)

#define     T_QUEUE_SIZE      (sizeof(CanMsg))
#define     T_QUEUE_COUNT     16
#define     R_QUEUE_SIZE      (sizeof(CanMsg))
#define     R_QUEUE_COUNT     64

Queue_Type  g_tQueue;
uint8_t     g_tQueue_buf[T_QUEUE_SIZE * T_QUEUE_COUNT];
Queue_Type  g_rQueue;
uint8_t     g_rQueue_buf[R_QUEUE_SIZE * R_QUEUE_COUNT];

static volatile uint16_t g_CanRamOffset = 0;

static uint32_t  Filter_Group_Address[9] = {0x40002890,0x40002900,0x40002908,0x40002910,\
                                            0x40002918,0x40002920,0x40002928,0x40002930,\
                                            0x40002938};


void CanInit(uint8_t ctrl, uint32_t baud, CanFilter *filter, uint8_t filter_num)
{
    /* 1 TODO STB */
    uint8_t idx = 0;
    volatile uint32_t tmpreg = 0;
    volatile CAN_InitTypeDef _init;

    GPIO_CAN_Init();
    _init.m_Enable = TRUE;
    _init.m_Mode = CAN_MODE_NORMAL;
    _init.m_WorkSource = CAN_SOURCE_HFCLK_DIV_2;
    if (100 == baud) {
        _init.m_BaudRate = 4;
    } else if (250 == baud) {
        _init.m_BaudRate = 1;
    } else {
        _init.m_BaudRate = 0;
    }
    _init.m_SyncJumpWidth = 1;
    _init.m_TimeSeg1 = 11;
    _init.m_TimeSeg2 = 2;
    _init.m_BusSample = CAN_BUS_SAMPLE_3_TIMES;
    _init.Filter_Group_Ptr = NULL;

/* RESET */
    g_CanRamOffset = 0;
    CAN_Reset(CAN4_SFR);
    tmpreg = ((uint32_t)_init.m_Enable << CAN_CTLR_CANEN_POS) \
           | _init.m_Mode \
           | _init.m_WorkSource \
           | CAN_CTLR_RSMOD;
    CAN4_SFR->CTLR = SFR_Config (CAN4_SFR->CTLR, ~CAN_CTLR_INIT_MASK, tmpreg);
    CAN4_SFR->CTLR |= (0x01<<12);
    tmpreg = ((uint32_t)_init.m_BaudRate << CAN_BRGR_CANBRP0_POS) \
           | ((uint32_t)_init.m_SyncJumpWidth << CAN_BRGR_SJW0_POS) \
           | ((uint32_t)_init.m_TimeSeg1 << CAN_BRGR_TSEG1_0_POS) \
           | ((uint32_t)_init.m_TimeSeg2 << CAN_BRGR_TSEG2_0_POS) \
           | (_init.m_BusSample);
    CAN4_SFR->BRGR = SFR_Config (CAN4_SFR->BRGR, ~CAN_BRGR_INIT_MASK, tmpreg);
    /* Enable Bus Off Software Recovery*/
    //CAN4_SFR->CTLR &= ~(0x01<<13);
    //CAN4_SFR->CTLR |= (0x01<<13);
    /* Enable Specific Filter */
    if (NULL != filter) {
        if (filter_num > HW_CAN_MAX_FILTER) {
            filter_num = HW_CAN_MAX_FILTER;
        }
        CAN4_SFR->CTLR |= (0x01<<4);
        for (idx = 0; idx < filter_num; idx++) {
            if ((filter + idx)->format == CAN_FORMAT_STANDARD) {
                *(uint32_t *)Filter_Group_Address[idx] = ((filter + idx)->id)<<21;
                *(uint32_t *)(Filter_Group_Address[idx] + 4) = ((filter + idx)->mask)<<21;
                *(uint32_t *)(Filter_Group_Address[idx] + 4) = ((filter + idx)->mask) |= 0x1FFFFF;
            } else if((filter + idx)->format == CAN_FORMAT_EXTENDED) {
                *(uint32_t *)Filter_Group_Address[idx] = ((filter + idx)->id)<<3;
                *(uint32_t *)(Filter_Group_Address[idx] + 4) = ((filter + idx)->mask)<<3;
                *(uint32_t *)(Filter_Group_Address[idx] + 4) = ((filter + idx)->mask) |= 0x07;
            }
        }
    } else {
        *(uint32_t *)(Filter_Group_Address[0] + 4) = 0xFFFFFFFF;
    }
/* AFTER RESET */
    SFR_CLR_BIT_ASM(CAN4_SFR->CTLR, CAN_CTLR_RSMOD_POS);

/* INTERRUPTS */
    CAN4_SFR->IFR &= ~(0x1FFF);
    CAN_Set_INT_Enable(CAN4_SFR, CAN_INT_TRANSMIT, TRUE);
    CAN_Set_INT_Enable(CAN4_SFR, CAN_INT_RECEIVE, TRUE);
    //CAN_Set_INT_Enable(CAN4_SFR, CAN_INT_DATA_OVERFLOW, FALSE);
    //CAN_Set_INT_Enable(CAN4_SFR, CAN_INT_BUS_ERROR, TRUE);
    CAN_Set_INT_Enable(CAN4_SFR, CAN_INT_BUS_OFF, TRUE);
    //CAN_Set_INT_Enable(CAN4_SFR, CAN_INT_ERROR_NEGATIVE, FALSE);
    INT_Interrupt_Enable(INT_CAN4, TRUE);

/* TX RX QUEUES */
    QueueInit(&g_tQueue, g_tQueue_buf, T_QUEUE_SIZE, T_QUEUE_COUNT);
    QueueInit(&g_rQueue, g_rQueue_buf, R_QUEUE_SIZE, R_QUEUE_COUNT);

    return;
}

void CanDeinit(uint8_t ctrl)
{

}

/* 启用和禁用 */
void CanDisable(uint8_t ctrl)
{
    g_CanRamOffset = 0;
    CAN4_SFR->CTLR |= (0x01);
    //IO_PIN_OUT_CAN_STB = 1;
}

void CanEnable(uint8_t ctrl)
{
    g_CanRamOffset = 0;
    CAN4_SFR->CTLR &= ~(0x01);
    //IO_PIN_OUT_CAN_STB = 0;
}

/* 进入睡眠模式 */
void CanGotoSleep(uint8_t ctrl)
{    
    //uint16_t sleeptimeout = 0xFFFF;
    
    //CAN_SetSleepRequest();
    //while(1 != CAN_GetSleepAck() && 0 != sleeptimeout--); //ensure goto sleep
    //IO_PIN_OUT_CAN_STB = 1;
    CAN4_SFR->CTLR |= ((uint32_t)1)<<3U;
}

/* 唤醒 */
void CanWakeUp(uint8_t ctrl)
{
    //IO_PIN_OUT_CAN_STB = 0;
    //CAN_Init(g_baud);
}

/* 设置过滤器 
 * idx = 0 设置第一组过滤器
 * idx = 1 设置第二组过滤器
*/
void CanSetFilter(uint8_t ctrl,
                  uint8_t idx,
                  uint32_t id,
                  uint32_t mask,
                  uint8_t  format,
                  uint8_t  type)
{
    /* TODO */
    return;
}


static void _can_send_msg(uint8_t ctrl, CanMsg *msg)
{
    CAN4_SFR->CanTxBuffer.SFF.TXINFR.RTR = msg->type;
    CAN4_SFR->CanTxBuffer.SFF.TXINFR.IDE = msg->format;
    CAN4_SFR->CanTxBuffer.SFF.TXINFR.DLC = msg->len;
    if (msg->format == CAN_FORMAT_STANDARD) {
        CAN4_SFR->CanTxBuffer.SFF.TXDATA0.ID = msg->id;
        CAN4_SFR->CanTxBuffer.SFF.TXDATA0.DATA0 = msg->data[0];
        CAN4_SFR->CanTxBuffer.SFF.TXDATA0.DATA1 = msg->data[1];
        CAN4_SFR->CanTxBuffer.SFF.TXDATA1.DATA2 = msg->data[2];
        CAN4_SFR->CanTxBuffer.SFF.TXDATA1.DATA3 = msg->data[3];
        CAN4_SFR->CanTxBuffer.SFF.TXDATA1.DATA4 = msg->data[4];
        CAN4_SFR->CanTxBuffer.SFF.TXDATA1.DATA5 = msg->data[5];
        CAN4_SFR->CanTxBuffer.SFF.TXDATA2.DATA6 = msg->data[6];
        CAN4_SFR->CanTxBuffer.SFF.TXDATA2.DATA7 = msg->data[7];
    } else {
        CAN4_SFR->CanTxBuffer.EFF.TXDATA0.ID = msg->id;
        CAN4_SFR->CanTxBuffer.EFF.TXDATA1.DATA0 = msg->data[0];
        CAN4_SFR->CanTxBuffer.EFF.TXDATA1.DATA1 = msg->data[1];
        CAN4_SFR->CanTxBuffer.EFF.TXDATA1.DATA2 = msg->data[2];
        CAN4_SFR->CanTxBuffer.EFF.TXDATA1.DATA3 = msg->data[3];
        CAN4_SFR->CanTxBuffer.EFF.TXDATA2.DATA4 = msg->data[4];
        CAN4_SFR->CanTxBuffer.EFF.TXDATA2.DATA5 = msg->data[5];
        CAN4_SFR->CanTxBuffer.EFF.TXDATA2.DATA6 = msg->data[6];
        CAN4_SFR->CanTxBuffer.EFF.TXDATA2.DATA7 = msg->data[7];
    }
/* TRANSMIT START */
    CAN4_SFR->CTLR |= (0x03<<8);

    return;
}


static void _can_recv_msg(void)
{
    uint8_t i = 0;
    volatile CanRxBufferTypeDef *ptr = NULL;
    CanMsg msg;

    uint8_t cnt = CAN_Get_Receive_Message_Counter(CAN4_SFR);
    for (i = 0; i < cnt; i++) {
        ptr = (volatile CanRxBufferTypeDef *)(CAN4_RECEIVE_ADDR + g_CanRamOffset);

        msg.format = ptr->SFF.RXDATA0.IDE;
        msg.len = ptr->SFF.RXDATA0.DLC;
        msg.type = ptr->SFF.RXDATA0.RTR;
        if (CAN_FRAME_FORMAT_SFF == msg.format) {
            msg.id = ptr->SFF.RXDATA1.ID;
            msg.data[0] = ptr->SFF.RXDATA1.DATA0;
            msg.data[1] = ptr->SFF.RXDATA1.DATA1;
            msg.data[2] = ptr->SFF.RXDATA2.DATA2;
            msg.data[3] = ptr->SFF.RXDATA2.DATA3;
            msg.data[4] = ptr->SFF.RXDATA2.DATA4;
            msg.data[5] = ptr->SFF.RXDATA2.DATA5;
            msg.data[6] = ptr->SFF.RXDATA3.DATA6;
            msg.data[7] = ptr->SFF.RXDATA3.DATA7;
        } else {
            msg.id = ptr->EFF.RXDATA1.ID;
            msg.data[0] = ptr->EFF.RXDATA2.DATA0;
            msg.data[1] = ptr->EFF.RXDATA2.DATA1;
            msg.data[2] = ptr->EFF.RXDATA2.DATA2;
            msg.data[3] = ptr->EFF.RXDATA2.DATA3;
            msg.data[4] = ptr->EFF.RXDATA3.DATA4;
            msg.data[5] = ptr->EFF.RXDATA3.DATA5;
            msg.data[6] = ptr->EFF.RXDATA3.DATA6;
            msg.data[7] = ptr->EFF.RXDATA3.DATA7;
        }
        QueuePut(&g_rQueue, &msg);

        g_CanRamOffset += HW_CAN_FRM_LEN;
        if (g_CanRamOffset > HW_CAN_RAM_DEPTH - HW_CAN_FRM_LEN) {
            g_CanRamOffset = 0;
        }
    }
    CAN_Release_Receive_Buffer(CAN4_SFR, cnt);

    return;
}


/*
* 发送数据
*/
uint8_t CanSend(uint8_t ctrl, CanMsg *msg)
{
    uint8_t ret = ERR_COMMON;

    if ((!CAN_Get_Transmit_Status(CAN4_SFR, CAN_TX_BUFFER_STATUS))) {
        if (0 == QueuePut(&g_tQueue, msg)) {
            ret = ERR_TXFULL;
        } else {
            ret = ERR_OK;
        }
    } else {
        _can_send_msg(ctrl, msg);
    }

    return ret;
}

/*
* 接收数据
*/
uint8_t CanRecieve(uint8_t ctrl, CanMsg *msg)
{ 
    uint8_t ret = ERR_COMMON;

    if (0 == QueueGet(&g_rQueue, msg)) {
        ret = ERR_RXEMPTY;
    } else {
        ret = ERR_OK;
    }

    return ret;
}

uint8_t CanClearBusoff(uint8_t ctrl)
{
    
}

//*****************************************************************************************
//                             	 CAN4 Interrupt Course
//*****************************************************************************************
void __attribute__((interrupt)) _CAN4_exception (void)
{
    CanMsg msg;
/* INT_TRANSMIT */
    if (CAN_Get_INT_Flag(CAN4_SFR, CAN_INT_TRANSMIT)) {
        CAN_Clear_INT_Flag(CAN4_SFR, CAN_INT_TRANSMIT);
        CAN4_SFR->CTLR &= ~(0x03<<8); /* TRANSMIT STOP */
        if (1 == QueueGet(&g_tQueue, &msg)) {
           _can_send_msg(4, &msg);
        }
    }

/* INT_RECEIVE */
    if (CAN_Get_INT_Flag(CAN4_SFR, CAN_INT_RECEIVE)) {
        _can_recv_msg();
    }

/* INT_BUS_OFF */
    if (CAN_Get_INT_Flag(CAN4_SFR, CAN_INT_BUS_OFF)) {
        CAN_Clear_INT_Flag(CAN4_SFR, CAN_INT_BUS_OFF); 
        //CanUserOnBusOff(4);
        CanDisable(4);
        CanEnable(4);
    }

    return;
}



