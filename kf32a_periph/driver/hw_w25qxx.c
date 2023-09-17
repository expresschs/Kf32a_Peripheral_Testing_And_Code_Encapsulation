#include <stdint.h>
#include <stddef.h>

#include "common.h"
#include "hw_gpio.h"
#include "hw_spi.h"
#include "hw_w25qxx.h"

#define W25X_PageErase_Timeout                  (3000UL)    //ms
#define W25X_Operate_Timeout                    (200UL)     //ms
#define W25X_Dummy_Byte                         (0xA5UL)

//FLASH COMMAND
#define W25X_WriteEnable                        (0x06UL)
#define W25X_WriteDisable                       (0x04UL)
#define W25X_ReadStatusReg1                     (0x05UL)
#define W25X_ReadStatusReg2                     (0x35UL)
#define W25X_ReadStatusReg3                     (0x15UL)
#define W25X_WriteStatusReg1                    (0x01UL)
#define W25X_WriteStatusReg2                    (0x31UL)
#define W25X_WriteStatusReg3                    (0x11UL)
#define W25X_ReadData                           (0x03UL)
#define W25X_FastReadData                       (0x0BUL)
#define W25X_FastReadDual                       (0x3BUL)
#define W25X_PageProgram                        (0x02UL)
#define W25X_BlockErase                         (0xD8UL)
#define W25X_SectorErase                        (0x20UL)
#define W25X_ChipErase                          (0xC7UL)
#define W25X_PowerDown                          (0xB9UL)
#define W25X_ReleasePowerDown                   (0xABUL)
#define W25X_DeviceID                           (0xABUL)
#define W25X_ManufactDeviceID                   (0x90UL)
#define W25X_JedecDeviceID                      (0x9FUL)
#define W25X_Enable4ByteAddr                    (0xB7UL)
#define W25X_Exit4ByteAddr                      (0xE9UL)

static void platform_delay_us(uint32_t us)
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

#define w25qxx_delay_us(us)                     platform_delay_us(us)
#define w25qxx_spi_cs_low()                     GPIO_Output_Low(GPIO_SPI1_CS2)
#define w25qxx_spi_cs_high()                    GPIO_Output_High(GPIO_SPI1_CS2)
#define w25qxx_spi_transfer(send, recv, size)   hwSpiTransfer(1, (uint8_t *)send, recv, size)


static uint8_t w25qxx_wait_ready(uint32_t timeout)
{
    uint8_t ret = ERR_COMMON;
    uint32_t i = 0;
    uint8_t rdata[2] = {0};
    uint8_t command[2] = {0};

    command[0] = W25X_ReadStatusReg1;
    command[1] = W25X_Dummy_Byte;
    for (i = 0; i < timeout; i++) {
        w25qxx_spi_cs_low();
        ret = w25qxx_spi_transfer(command, rdata, sizeof(command)/sizeof(command[0]));
        w25qxx_spi_cs_high();
        if (ERR_OK == ret) {
            if (0x01 != (rdata[1] & 0x01)) { 
                ret = ERR_OK;
                goto exit;
            }
        } else {
            goto exit;
        }
        w25qxx_delay_us(1000);
    }

exit:
    return ret;
}

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
    w25qxx_spi_cs_low();
    ret = w25qxx_spi_transfer(command, NULL, sizeof(command)/sizeof(command[0]));
    if (ERR_OK == ret) {
        ret = w25qxx_spi_transfer(NULL, buffer, size);
    }
    w25qxx_spi_cs_high();

exit:
    return ret;
}

static uint8_t _w25qxx_disable_protection(void)
{
    uint8_t cmd = W25X_WriteEnable;
    uint8_t ret = ERR_COMMON;

    w25qxx_spi_cs_low();
    ret = w25qxx_spi_transfer(&cmd, NULL, 1);
    w25qxx_spi_cs_high();
    if (ERR_OK == ret) {
        ret = w25qxx_wait_ready(W25X_Operate_Timeout);
    }

    return ret;
}

static int _w25qxx_page_program(uint32_t addr, uint8_t *buffer, uint32_t size)
{
    uint8_t command[4] = {0};
    uint8_t ret = ERR_COMMON;

    if (ERR_OK != _w25qxx_disable_protection()) {
        goto exit;
    }
    command[0] = W25X_PageProgram;
    command[1] = (addr >> 16);
    command[2] = (addr >> 8);
    command[3] = (addr >> 0);
    w25qxx_spi_cs_low();
    ret = w25qxx_spi_transfer(command, NULL, sizeof(command)/sizeof(command[0]));
    if (ERR_OK == ret) {
        ret = w25qxx_spi_transfer(buffer, NULL, size);
    }
    w25qxx_spi_cs_high();
    if (ERR_OK == ret) {
        ret = w25qxx_wait_ready(W25X_Operate_Timeout);
    }

exit:
    return ret;
}

uint8_t w25qxx_write(uint32_t addr, uint8_t *buffer, uint32_t size)
{
    uint32_t wrsize = 0;
    uint32_t pageaddr = 0;
    uint8_t ret = ERR_COMMON;

    if (ERR_OK != w25qxx_wait_ready(W25X_Operate_Timeout)) {
        goto exit;
    }
    pageaddr = addr & W25QXX_WRITE_ADDR_MASK;
    while (size > 0) {
        if ((addr + size) > (pageaddr + W25QXX_WRITE_SIZE)) {
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
    ret = ERR_OK;

exit:
    return ret;
}

static int _w25qxx_erase(uint8_t cmd, uint32_t addr)
{
    uint8_t command[4] = {0};
    uint8_t ret = ERR_COMMON;

    if (ERR_OK != _w25qxx_disable_protection()) {
        goto exit;
    }

    command[0] = cmd;
    command[1] = (addr >> 16);
    command[2] = (addr >> 8);
    command[3] = (addr >> 0);
    w25qxx_spi_cs_low();
    ret = w25qxx_spi_transfer(command, NULL, sizeof(command)/sizeof(command[0]));
    w25qxx_spi_cs_high();
    if (ERR_OK == ret) {
        ret = w25qxx_wait_ready(W25X_PageErase_Timeout);
    }

exit:
    return ret;
}

uint8_t w25qxx_erase_page(uint32_t page)
{
    uint8_t ret = ERR_COMMON;

    if (ERR_OK != w25qxx_wait_ready(W25X_Operate_Timeout)) {
        goto exit;
    }
    ret = _w25qxx_erase(W25X_SectorErase, page * W25QXX_ERASE_SIZE);

exit:
    return ret;
}

uint8_t w25qxx_erase(uint32_t addr, uint32_t size)
{
    uint32_t end;
    uint8_t ret = ERR_COMMON;

    if ((addr & W25QXX_ERASE_ADDR_MASK ) != addr) {
        goto exit;
    }
    if (ERR_OK !=w25qxx_wait_ready(W25X_Operate_Timeout)) {
        goto exit;
    }

    end = (addr + size) & W25QXX_ERASE_ADDR_MASK;
    if (size == (size & W25QXX_ERASE_ADDR_MASK)) {
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

    w25qxx_spi_cs_low();
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
    w25qxx_spi_cs_high();
    return ret;
}

