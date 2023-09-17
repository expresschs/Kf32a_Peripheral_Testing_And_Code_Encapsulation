#ifndef _HW_FLASH_H_
#define _HW_FLASH_H_

///////////////////////////////////////////////////////////////////////////
// Code flash
///////////////////////////////////////////////////////////////////////////
//擦除地址1K字节对齐
#define FLASH_CODE_ERASE_SHIFT      10
#define FLASH_CODE_ERASE_SIZE       (0x01 << FLASH_CODE_ERASE_SHIFT)
#define FLASH_CODE_ERASE_ADDR_MASK  ((~0UL) << FLASH_CODE_ERASE_SHIFT)

//写入地址8字节对齐
#define FLASH_CODE_WRITE_SHIFT      3
#define FLASH_CODE_WRITE_SIZE       (0x01 << FLASH_CODE_WRITE_SHIFT)
#define FLASH_CODE_WRITE_ADDR_MASK  ((~0UL) << FLASH_CODE_WRITE_SHIFT)

/* 0块表示驱动，1块表示应用 */
#define FLASH_CODE_BLOCK_MAX		1

/* 获取驱动程序下载地址 */
uint8_t *FlashCodeGetDrvAddr(void);
/* 驱动下载前调用 */
int FlashCodePreInit(void);
/* 从flash加载flash驱动 */
int FlashCodeLoadDriver(void);
/* 驱动下载后调用 */
int FlashCodePostInit(void);
int FlashCodeRead(uint32_t addr, uint8_t *buffer, uint32_t size);
int FlashCodeWrite(uint32_t addr, uint8_t *buffer, uint32_t size);
int FlashCodeErase(uint32_t addr, uint32_t size);

///////////////////////////////////////////////////////////////////////////
// Data flash
///////////////////////////////////////////////////////////////////////////
//擦除地址64字节对齐
#define FLASH_DATA_ERASE_SHIFT      6
#define FLASH_DATA_ERASE_SIZE       (0x01 << FLASH_DATA_ERASE_SHIFT)
#define FLASH_DATA_ERASE_ADDR_MASK  ((~0UL) << FLASH_DATA_ERASE_SHIFT)

//写入地址4字节对齐
#define FLASH_DATA_WRITE_SHIFT      2
#define FLASH_DATA_WRITE_SIZE       (0x01 << FLASH_DATA_WRITE_SHIFT)
#define FLASH_DATA_WRITE_ADDR_MASK  ((~0UL) << FLASH_DATA_WRITE_SHIFT)

//读取地址4字节对齐
#define FLASH_DATA_READ_SHIFT       2
#define FLASH_DATA_READ_SIZE        (0x01 << FLASH_DATA_READ_SHIFT)
#define FLASH_DATA_READ_ADDR_MASK   ((~0UL) << FLASH_DATA_READ_SHIFT)

int FlashDataInit(void);
int FlashDataRead(uint32_t addr, uint8_t *buffer, uint32_t size);
int FlashDataWrite(uint32_t addr, uint8_t *buffer, uint32_t size);
int FlashDataErase(uint32_t addr, uint32_t size);

#if 0
int FlashCFGUserWrite(uint32_t addr, uint8_t *buffer,uint32_t size);
int FlashCFGUserRead(uint32_t addr, uint8_t *buffer,uint32_t size);
#endif

#endif



