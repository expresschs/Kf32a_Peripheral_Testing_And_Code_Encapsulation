#include "common.h"
#include "def_pins.h"
#include "hw_gpio.h"
#include "hw_spi.h"
#include "hw_w25qxx.h"


#define W25X_PageErase_Timeout      (3000UL)    //FLASH擦除命令操作地超时等待时间 单位ms
#define W25X_Operate_Timeout        (200UL)     //FLASH普通命令操作的超时等待时间 单位ms
#define W25X_Dummy_Byte             0xA5

//FLASH 命令定义(常用指令)
#define W25X_WriteEnable            0x06
#define W25X_WriteDisable           0x04
#define W25X_ReadStatusReg1         0x05
#define W25X_ReadStatusReg2         0x35
#define W25X_ReadStatusReg3         0x15
#define W25X_WriteStatusReg1        0x01
#define W25X_WriteStatusReg2        0x31
#define W25X_WriteStatusReg3        0x11
#define W25X_ReadData               0x03
#define W25X_FastReadData           0x0B
#define W25X_FastReadDual           0x3B
#define W25X_PageProgram            0x02
#define W25X_BlockErase             0xD8
#define W25X_SectorErase            0x20
#define W25X_ChipErase              0xC7
#define W25X_PowerDown              0xB9
#define W25X_ReleasePowerDown       0xAB
#define W25X_DeviceID               0xAB
#define W25X_ManufactDeviceID       0x90
#define W25X_JedecDeviceID          0x9F
#define W25X_Enable4ByteAddr        0xB7
#define W25X_Exit4ByteAddr          0xE9


static void w25qxx_delay_us(uint32_t us)
{
    uint32_t delay = 3 * us;

    do {
        asm("nop"); asm("nop"); asm("nop"); asm("nop");
        asm("nop"); asm("nop"); asm("nop"); asm("nop");
        asm("nop"); asm("nop"); asm("nop"); asm("nop");
        asm("nop"); asm("nop"); asm("nop"); asm("nop");
    } while (delay --);

    return;
}

static uint8_t w25qxx_spi_transfer(uint8_t *send, uint8_t *recv, uint32_t size)
{
    uint8_t ret = ERR_COMMON;
    
    if (0 == size) {
        goto exit;
    }
    
    //GPIO_Output_Low(GPIO_SPI1_CS2);
    ret = hwSpiTransfer(1, (uint8_t *)send, recv, size);
    //GPIO_Output_High(GPIO_SPI1_CS2);
    
exit:
    return ret;
}

/*获取Flash芯片的状态 */
static uint8_t w25qxx_wait_ready(uint32_t timeout)
{
    uint8_t ret = ERR_COMMON;
    uint32_t i = 0;
    uint8_t rdata[2] = {0};
    uint8_t command[2] = {0};

    command[0] = W25X_ReadStatusReg1;
    command[1] = W25X_Dummy_Byte;
    for (i = 0; i < timeout; i++) {
        if (ERR_OK != w25qxx_spi_transfer(command, rdata, sizeof(command)/sizeof(command[0]))) {
            goto exit;
        }
        if (0x01 != (rdata[1] & 0x01 )) { 
            ret = ERR_OK;
            goto exit; 
        }
        w25qxx_delay_us(1000);
    }

exit:
    return ret;
}

/* 获取Flash的数据 */
uint8_t w25qxx_read(uint32_t addr, uint8_t *buffer, uint32_t size)
{
    uint8_t command[4] = {0};
    uint8_t ret = ERR_COMMON;

    if (ERR_OK != w25qxx_wait_ready(W25X_Operate_Timeout)) {
        goto exit;
    }
    command[0] = W25X_ReadData;
    command[1] = (addr >> 16);
    command[2] = (addr >> 8);
    command[3] = (addr >> 0);
    if (ERR_OK != w25qxx_spi_transfer(command, NULL, sizeof(command)/sizeof(command[0]))) {
        goto exit;
    }
    if (ERR_OK != w25qxx_spi_transfer(NULL, buffer, size)) {
        goto exit;
    }

exit:
    return ret;
}

/* Flash 软件使能写(禁止写保护) */
static uint8_t _w25qxx_disable_protection(void)
{
    uint8_t cmd = W25X_WriteEnable;
    uint8_t ret = ERR_COMMON;

    if (ERR_OK != w25qxx_spi_transfer(&cmd, NULL, 1)) {
        goto exit;
    }
    ret = w25qxx_wait_ready(W25X_Operate_Timeout);

exit:
    return ret;
}

/*Flash页面编程，是在先前已经擦除FF的内存位置编程写入一个字节到256字节（一页）的数据*/
static int _w25qxx_page_program(uint32_t addr, uint8_t *buffer, uint32_t size)
{
    uint8_t command[4];
    uint8_t ret = ERR_COMMON;

    //解除保护
    if (ERR_OK != _w25qxx_disable_protection()) {
        goto exit;
    }
    command[0] = W25X_PageProgram;
    command[1] = (addr >> 16);
    command[2] = (addr >> 8);
    command[3] = (addr >> 0);
    if (ERR_OK != w25qxx_spi_transfer(command, NULL, sizeof(command)/sizeof(command[0]))) {
        goto exit;
    }
    if (ERR_OK != w25qxx_spi_transfer(buffer, NULL, size)) {
        goto exit;
    }
    ret = w25qxx_wait_ready(W25X_Operate_Timeout);

exit:
    return ret;
}

/*Flash 写入 跨页处理，（不处理跨片，只有一个Flash芯片）*/
uint8_t w25qxx_write(uint32_t addr, uint8_t *buffer, uint32_t size)
{
    uint32_t wrsize = 0;
    uint32_t pageaddr = 0;
    uint8_t ret = ERR_COMMON;

    if (ERR_OK !=w25qxx_wait_ready(W25X_Operate_Timeout)) {
        goto exit;
    }
    pageaddr = addr & W25QXX_WRITE_ADDR_MASK;
    while(size > 0){
        if ((addr + size) > (pageaddr + W25QXX_WRITE_SIZE)){
            wrsize = (pageaddr + W25QXX_WRITE_SIZE) - addr;
        } else {
            wrsize = size;
        }
        if (ERR_OK != _w25qxx_page_program(addr, buffer, wrsize)) {
            goto exit;
        }
        pageaddr += W25QXX_WRITE_SIZE;
        size -= wrsize;
        addr += wrsize;
        buffer += wrsize;
    }

exit:
    return ret;
}

/*根据不同cmd 执行不同擦除命令操作*/
static int _w25qxx_erase(uint8_t cmd, uint32_t addr)
{
    uint8_t command[4];
    uint8_t ret = ERR_COMMON;

    if (ERR_OK != _w25qxx_disable_protection()) {
        goto exit;
    }
    command[0] = cmd;
    command[1] = (addr >> 16);
    command[2] = (addr >> 8);
    command[3] = (addr >> 0);
    if (ERR_OK != w25qxx_spi_transfer(command, NULL, sizeof(command)/sizeof(command[0]))) {
        goto exit;
    }
    ret = w25qxx_wait_ready(W25X_PageErase_Timeout);

exit:
    return ret;
}


/*Flash擦除 -根据地址和范围计算擦除范围*/
uint8_t w25qxx_erase(uint32_t addr, uint32_t size)
{
    uint32_t end;
    uint8_t ret = ERR_COMMON;

    if ((addr & W25QXX_ERASE_ADDR_MASK ) != addr){
        goto exit;
    }
    if (ERR_OK !=w25qxx_wait_ready(W25X_Operate_Timeout)) {
        goto exit;
    }

    end = (addr + size) & W25QXX_ERASE_ADDR_MASK;
    if (size == (size & W25QXX_ERASE_ADDR_MASK)){
        end -= W25QXX_ERASE_SIZE;
    }
    while (addr <= end) {
        if ((0 == (addr & W25QXX_BLOCK_ERASE_ADDR_MASK)) && ((end - addr) >= W25QXX_BLOCK_ERASE_SIZE)) {
            if(ERR_OK != _w25qxx_erase(W25X_BlockErase, addr)) {
                goto exit;
            }
            addr += W25QXX_BLOCK_ERASE_SIZE;
        } else {
            if (ERR_OK != _w25qxx_erase(W25X_SectorErase, addr)) {
                goto exit;
            }
            addr += W25QXX_ERASE_SIZE;
        }
    }
    ret = ERR_OK;

exit:
    return ret;
}

uint8_t w25qxx_get_id(uint16_t *id)
{
    uint8_t command[4] = {0};
    uint8_t idh = 0;
    uint8_t idl = 0;
    uint8_t ret = ERR_COMMON;

    GPIO_Output_Low(GPIO_SPI1_CS2);

    command[0] = W25X_ManufactDeviceID;
    command[1] = 0;
    command[2] = 0;
    command[3] = 0;
    if (ERR_OK != w25qxx_spi_transfer(command, NULL, sizeof(command)/sizeof(command[0]))) {
        goto exit;
    }
    if (ERR_OK != w25qxx_spi_transfer(NULL, &idh, 1)) {
        goto exit;
    }
    if (ERR_OK != w25qxx_spi_transfer(NULL, &idl, 1)) {
        goto exit;
    }
    *id = (idh << 8) | idl;
    ret = ERR_OK;

exit:
    GPIO_Output_High(GPIO_SPI1_CS2);
    return ret;
}


