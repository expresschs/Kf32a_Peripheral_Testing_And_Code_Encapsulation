#include "kf32a156_int.h"
#include "kf32a156_spi.h"
#include "kf32a156_dma.h"

#include "hw_gpio.h"
#include "hw_timer.h"
#include "hw_spi.h"

#if 0
enum {
    SPI_TRANSFER_WAIT,
    SPI_TRANSFER_COMPLETE,
    SPI_TRANSFER_ERROR
};

#define SPI3_DEV_HANDLE      SPI3_SFR
#define SPI3_DMA_HANDLE      DMA0_SFR
#define SPI3_DMA_TX_CH       DMA_CHANNEL_6
#define SPI3_DMA_RX_CH       DMA_CHANNEL_7

static volatile uint8_t g_spi3_tansfer_state = SPI_TRANSFER_WAIT;
static volatile uint8_t g_spi3_lock = 0;

static void _spi3_port_init(void)
{
    GPIO_Spi3_Init();

    return;
}

static void _spi3_dev_init(uint32_t baud)
{
    SPI_InitTypeDef Spi_ConfigPtr;

    /* SPI mode */
    Spi_ConfigPtr.m_Mode = SPI_MODE_MASTER_CLKDIV4;
    /* SPI clock */
    Spi_ConfigPtr.m_Clock = SPI_CLK_SCLK;
    /* Data transfer start control */
    Spi_ConfigPtr.m_FirstBit = SPI_FIRSTBIT_MSB;
    ///* Spi idle state */
    //Spi_ConfigPtr.m_CKP = SPI_CKP_LOW;
    ///* Spi clock phase(Data shift edge) */
    //Spi_ConfigPtr.m_CKE = SPI_CKE_1EDGE;

    Spi_ConfigPtr.m_CKP = SPI_CKP_HIGH;
    /* Spi clock phase(Data shift edge) */
    Spi_ConfigPtr.m_CKE = SPI_CKE_2EDGE;


    /* Data width */
    Spi_ConfigPtr.m_DataSize = SPI_DATASIZE_8BITS;
    /* Baud rate :Fck_spi=Fck/2(m_BaudRate+1)*/
    //Spi_ConfigPtr.m_BaudRate = 0x59;
    //Spi_ConfigPtr.m_BaudRate = 15; /* 1Mhz */
    Spi_ConfigPtr.m_BaudRate = 63; /* 250khz */

    /* Spi reset */
    SPI_Reset(SPI3_DEV_HANDLE);
    /* Configure SPI module */
    SPI_Configuration(SPI3_DEV_HANDLE, &Spi_ConfigPtr);

    return;
}

static void _spi3_dma_init(void)
{
    DMA_InitTypeDef Dma_ConfigPtr;
    DMA_Reset(SPI3_DMA_HANDLE);
    /* Receive channel */
    /* Transmit data size */
    Dma_ConfigPtr.m_Number = 0;
    /* DMA transmit direction */
    Dma_ConfigPtr.m_Direction = DMA_PERIPHERAL_TO_MEMORY;
    /* DMA channel priority */
    Dma_ConfigPtr.m_Priority = DMA_CHANNEL_TOP;
    /* Peripheral data width */
    Dma_ConfigPtr.m_PeripheralDataSize = DMA_DATA_WIDTH_8_BITS;
    /* Memory data width */
    Dma_ConfigPtr.m_MemoryDataSize = DMA_DATA_WIDTH_8_BITS;
    /* Peripheral address incremental mode enable */
    Dma_ConfigPtr.m_PeripheralInc = FALSE;
    /* Memory address incremental mode enable */
    Dma_ConfigPtr.m_MemoryInc = TRUE;
    /* DMA channel select */
    Dma_ConfigPtr.m_Channel = SPI3_DMA_RX_CH;
    /* DMA data block transfer mode */
    Dma_ConfigPtr.m_BlockMode = DMA_TRANSFER_BYTE;
    /* Cycle mode enable */
    Dma_ConfigPtr.m_LoopMode = TRUE;
    /* Peripheral start address */
    Dma_ConfigPtr.m_PeriphAddr = (uint32_t) & (SPI3_DEV_HANDLE->BUFR);
    /* Memory start address */
    Dma_ConfigPtr.m_MemoryAddr = NULL;

    /* Configure DMA0 */
    DMA_Configuration(SPI3_DMA_HANDLE, &Dma_ConfigPtr);

    /* Receive DMA */
    SPI_Receive_DMA_INT_Enable(SPI3_DEV_HANDLE, TRUE);
    /* DMA channel enable */
    //DMA_Channel_Enable(SPI3_DMA_HANDLE, DMA_CHANNEL_7, TRUE);
    /* DMA receive finish interrupt */
    DMA_Finish_Transfer_INT_Enable(SPI3_DMA_HANDLE, SPI3_DMA_RX_CH, TRUE);
    /* DMA receive error interrupt */
    DMA_Error_Transfer_INT_Enable (SPI3_DMA_HANDLE, SPI3_DMA_RX_CH, TRUE);

    /* Send channal */
    /* Transmit data size */
    Dma_ConfigPtr.m_Number = 0;
    /* DMA transmit direction */
    Dma_ConfigPtr.m_Direction = DMA_MEMORY_TO_PERIPHERAL;
    /* DMA channel select */
    Dma_ConfigPtr.m_Channel = SPI3_DMA_TX_CH;
    /* Cycle mode enable */
    Dma_ConfigPtr.m_LoopMode = FALSE;
    /* Peripheral start address */
    Dma_ConfigPtr.m_PeriphAddr = (uint32_t) & (SPI3_DEV_HANDLE->BUFR);
    /* Memory start address */
    Dma_ConfigPtr.m_MemoryAddr = NULL;

    /* Configure DMA1 */
    DMA_Configuration(SPI3_DMA_HANDLE, &Dma_ConfigPtr);

    /* Send DMA */
    SPI_Transmit_DMA_INT_Enable(SPI3_DEV_HANDLE, TRUE);
    /* DMA channel enable */
    //DMA_Channel_Enable(SPI3_DMA_HANDLE, DMA_CHANNEL_6, TRUE);
    /* DMA finsh interrupt enable */
    DMA_Finish_Transfer_INT_Enable(SPI3_DMA_HANDLE, SPI3_DMA_TX_CH, TRUE);
    /* DMA transmit error interrupt */
    DMA_Error_Transfer_INT_Enable (SPI3_DMA_HANDLE, SPI3_DMA_TX_CH, TRUE);

    /* Enable SPI module */
    SPI_Cmd(SPI3_DEV_HANDLE, TRUE);

    /* triger spi dma transmmit */
    SPI_I2S_SendData8(SPI3_SFR, 0x00);
    TimerDelayMs(100);

    /* Configure Interrupt */
    INT_Interrupt_Enable(INT_DMA0, TRUE);

    return;
}

uint8_t HwSpiInit(uint8_t ch, uint32_t baud)
{
    uint8_t ret = 1;

    if (3 == ch) {
        _spi3_port_init();
        _spi3_dev_init(baud);
        _spi3_dma_init();
        ret = 0;
    }

    return ret;
}

void HwSpiDeinit(uint8_t ch)
{
    /* TODO */

    return;
}

uint8_t hwSpiTransfer(uint8_t ch, uint8_t *send, uint8_t *recv, uint32_t size)
{
    uint8_t ret = 1;
    uint32_t ts = 0;

    if (NULL != send && NULL != recv && 0 != size) {
        if (3 == ch && 0 == g_spi3_lock) {
            g_spi3_lock = 1;
            g_spi3_tansfer_state = SPI_TRANSFER_WAIT;
            /* transfer number config */
            DMA_Transfer_Number_Config(SPI3_DMA_HANDLE, SPI3_DMA_TX_CH, size);
            DMA_Transfer_Number_Config(SPI3_DMA_HANDLE, SPI3_DMA_RX_CH, size);
            /* memory start address */
            DMA_Memory_Start_Address_Config(SPI3_DMA_HANDLE, SPI3_DMA_TX_CH, (uint32_t)send);
            DMA_Memory_Start_Address_Config(SPI3_DMA_HANDLE, SPI3_DMA_RX_CH, (uint32_t)recv);
            /* enable DMA channel */
            DMA_Channel_Enable(SPI3_DMA_HANDLE, SPI3_DMA_TX_CH, TRUE);
            DMA_Channel_Enable(SPI3_DMA_HANDLE, SPI3_DMA_RX_CH, TRUE);
            /* wait for dma operate in 10ms */
#if 0
            ts = TimerGetTickCount() + 10UL;
            while (SPI_TRANSFER_WAIT == g_spi3_tansfer_state && TimeAfter(ts, TimerGetTickCount())) {
                /* NTD */
            }
#endif
            while (SPI_TRANSFER_WAIT == g_spi3_tansfer_state) {
                /* NTD */
            }
            if (SPI_TRANSFER_COMPLETE == g_spi3_tansfer_state) {
                ret = 0;
            }
        }
            g_spi3_lock = 0;
    }

    return ret;
}


uint8_t _hwSpiTransfer(uint8_t ch, uint8_t *send, uint8_t *recv, uint32_t size)
{
    uint8_t ret = 1;
    uint32_t ts = 0;

    if (NULL != send && NULL != recv && 0 != size) {
        if (3 == ch && 0 == g_spi3_lock) {
            if (1 == size) {
                SPI_I2S_SendData8(SPI3_SFR, *send);
            } else {

            g_spi3_lock = 1;
            g_spi3_tansfer_state = SPI_TRANSFER_WAIT;
            /* transfer number config */
            DMA_Transfer_Number_Config(SPI3_DMA_HANDLE, SPI3_DMA_TX_CH, size);
            DMA_Transfer_Number_Config(SPI3_DMA_HANDLE, SPI3_DMA_RX_CH, size);
            /* memory start address */
            DMA_Memory_Start_Address_Config(SPI3_DMA_HANDLE, SPI3_DMA_TX_CH, (uint32_t)send);
            DMA_Memory_Start_Address_Config(SPI3_DMA_HANDLE, SPI3_DMA_RX_CH, (uint32_t)recv);
            /* enable DMA channel */
            DMA_Channel_Enable(SPI3_DMA_HANDLE, SPI3_DMA_TX_CH, TRUE);
            DMA_Channel_Enable(SPI3_DMA_HANDLE, SPI3_DMA_RX_CH, TRUE);
            /* wait for dma operate in 10ms */
#if 0
            ts = TimerGetTickCount() + 10UL;
            while (SPI_TRANSFER_WAIT == g_spi3_tansfer_state && TimeAfter(ts, TimerGetTickCount())) {
                /* NTD */
            }
#endif

            while (SPI_TRANSFER_WAIT == g_spi3_tansfer_state) {
                /* NTD */
            }
            if (SPI_TRANSFER_COMPLETE == g_spi3_tansfer_state) {
                ret = 0;
            }
            }
            g_spi3_lock = 0;
        }
    }

    return ret;
}



volatile uint8_t g_stub = 0;
//*****************************************************************************************
//                              DMA0 Interrupt Course
//*****************************************************************************************
void __attribute__((interrupt)) _DMA0_exception(void)
{
    /* Waiting for DMA to receive full  */
    if (DMA_Get_Finish_Transfer_INT_Flag(SPI3_DMA_HANDLE, SPI3_DMA_RX_CH)) {
        DMA_Clear_INT_Flag(SPI3_DMA_HANDLE, SPI3_DMA_RX_CH, DMA_INT_FINISH_TRANSFER);
        g_spi3_tansfer_state = SPI_TRANSFER_COMPLETE;
    }

    /* Waiting for DMA to send complete */
    if (DMA_Get_Finish_Transfer_INT_Flag(SPI3_DMA_HANDLE, SPI3_DMA_TX_CH)) {
        /* NOTE Waiting for spi to send no data */
        //while (SPI_Get_BUSY_Flag(SPI3_SFR) == SET);
        DMA_Clear_INT_Flag(SPI3_DMA_HANDLE, SPI3_DMA_TX_CH, DMA_INT_FINISH_TRANSFER);
    }

    /* Transfer Error */
    if (DMA_Get_Error_Transfer_INT_Flag(SPI3_DMA_HANDLE, SPI3_DMA_TX_CH)) {
        g_spi3_tansfer_state = SPI_TRANSFER_ERROR;
    }
    if (DMA_Get_Error_Transfer_INT_Flag(SPI3_DMA_HANDLE, SPI3_DMA_RX_CH)) {
        g_spi3_tansfer_state = SPI_TRANSFER_ERROR;
    }

    return;
}
#endif

#if 0
static void _spi3_port_init(void)
{
    GPIO_Spi3_Init();

    return;
}

void _spi3_dev_init(uint32_t baud)
{
    SPI_InitTypeDef Spi_ConfigPtr;

    /* SPI mode */
    Spi_ConfigPtr.m_Mode = SPI_MODE_MASTER_CLKDIV4;
    /* SPI clock */
    Spi_ConfigPtr.m_Clock = SPI_CLK_SCLK;
    /* Data transfer start control */
    Spi_ConfigPtr.m_FirstBit = SPI_FIRSTBIT_MSB;
    /* Spi idle state */
    Spi_ConfigPtr.m_CKP = SPI_CKP_HIGH;
    /* Spi clock phase(Data shift edge) */
    Spi_ConfigPtr.m_CKE = SPI_CKE_2EDGE;

    //Spi_ConfigPtr.m_CKP = SPI_CKP_LOW;
    //Spi_ConfigPtr.m_CKE = SPI_CKE_1EDGE;



    /* Data width */
    Spi_ConfigPtr.m_DataSize = SPI_DATASIZE_8BITS;
    /* Baud rate :Fck_spi=Fck/2(m_BaudRate+1)*/
    Spi_ConfigPtr.m_BaudRate = 63;
    /* Spi reset */
    SPI_Reset(SPI3_SFR);
    /* Configure SPI module */
    SPI_Configuration(SPI3_SFR, &Spi_ConfigPtr);
    /* Enable SPI module */
    SPI_Cmd(SPI3_SFR, TRUE);

    return;
}

#if 0
uint8_t hwSpiTransfer(uint8_t ch, uint8_t *send, uint8_t *recv, uint32_t size)
{
    uint8_t ret = 1;
    uint32_t i = 0;

    if (NULL != send && NULL != recv && 0 != size) {
        if (3 == ch) {
            for (i = 0; i < size; i++) {
                SPI3_SFR->BUFR = *(send + i);
                while (!(SPI3_SFR->STR & SPI_STR_RNE));
                *(recv + i) = SPI3_SFR->BUFR;
            }
            ret = 0;
        }
    }

    return ret;
}
#else
uint8_t hwSpiTransfer(uint8_t ch, uint8_t *send, uint8_t *recv, uint32_t size)
{
    uint8_t ret = 1;
    uint32_t i = 0;
    uint8_t tx = 0xFF;
    uint8_t rx = 0;

    if (0 != size) {
        if (3 == ch) {
            for (i = 0; i < size; i++) {
                if (NULL != send) {
                    SPI3_SFR->BUFR = *(send + i);
                } else {
                    SPI3_SFR->BUFR = tx;
                }
                while (!(SPI3_SFR->STR & SPI_STR_RNE));
                if (NULL != recv) {
                    *(recv + i) = SPI3_SFR->BUFR;
                } else {
                   rx = SPI3_SFR->BUFR;
                }
            }
            ret = 0;
        }
    }

    return ret;
}
#endif

uint8_t HwSpiInit(uint8_t ch, uint32_t baud)
{
    uint8_t ret = 1;

    if (3 == ch) {
        _spi3_port_init();
        _spi3_dev_init(baud);
        //_spi3_dma_init();
        ret = 0;
    }

    return ret;
}
#endif

static void _spi1_port_init(void)
{
    GPIO_Spi1_Init();

    return;
}

void _spi1_dev_init(uint32_t baud)
{
    SPI_InitTypeDef Spi_ConfigPtr;

    /* SPI mode */
    Spi_ConfigPtr.m_Mode = SPI_MODE_MASTER_CLKDIV4;
    /* SPI clock */
    Spi_ConfigPtr.m_Clock = SPI_CLK_SCLK;
    /* Data transfer start control */
    Spi_ConfigPtr.m_FirstBit = SPI_FIRSTBIT_MSB;
    /* Spi idle state */
    Spi_ConfigPtr.m_CKP = SPI_CKP_HIGH;
    /* Spi clock phase(Data shift edge) */
    Spi_ConfigPtr.m_CKE = SPI_CKE_2EDGE;

    //Spi_ConfigPtr.m_CKP = SPI_CKP_LOW;
    //Spi_ConfigPtr.m_CKE = SPI_CKE_1EDGE;

    /* Data width */
    Spi_ConfigPtr.m_DataSize = SPI_DATASIZE_8BITS;
    /* Baud rate :Fck_spi=Fck/2(m_BaudRate+1)*/
    Spi_ConfigPtr.m_BaudRate = 63;
    /* Spi reset */
    SPI_Reset(SPI1_SFR);
    /* Configure SPI module */
    SPI_Configuration(SPI1_SFR, &Spi_ConfigPtr);
    /* Enable SPI module */
    SPI_Cmd(SPI1_SFR, TRUE);

    return;
}

#if 0
uint8_t hwSpiTransfer(uint8_t ch, uint8_t *send, uint8_t *recv, uint32_t size)
{
    uint8_t ret = 1;
    uint32_t i = 0;

    if (NULL != send && NULL != recv && 0 != size) {
        if (3 == ch) {
            for (i = 0; i < size; i++) {
                SPI3_SFR->BUFR = *(send + i);
                while (!(SPI3_SFR->STR & SPI_STR_RNE));
                *(recv + i) = SPI3_SFR->BUFR;
            }
            ret = 0;
        }
    }

    return ret;
}
#else
uint8_t hwSpiTransfer(uint8_t ch, uint8_t *send, uint8_t *recv, uint32_t size)
{
    uint8_t ret = 1;
    uint32_t i = 0;
    uint8_t tx = 0x00;
    uint8_t rx = 0;

    if (0 != size) {
        if (1 == ch) {
            for (i = 0; i < size; i++) {
                if (NULL != send) {
                    SPI1_SFR->BUFR = *(send + i);
                } else {
                    SPI1_SFR->BUFR = tx;
                }
                while (!(SPI1_SFR->STR & SPI_STR_RNE));
                if (NULL != recv) {
                    *(recv + i) = SPI1_SFR->BUFR;
                } else {
                   rx = SPI1_SFR->BUFR;
                }
            }
            ret = 0;
        }
    }

    return ret;
}
#endif

uint8_t HwSpiInit(uint8_t ch, uint32_t baud)
{
    uint8_t ret = 1;

    if (1 == ch) {
        _spi1_port_init();
        _spi1_dev_init(baud);
        //_spi1_dma_init();
        ret = 0;
    }

    return ret;
}

