#include "KF32A156.h"
#include "ChipMessageApi.h"
#include "hw_flash.h"
#include "common.h"

extern unsigned int __text_end__;
extern unsigned int __indata_end__;
extern unsigned int __indata_start__;

uint8_t *FlashCodeGetDrvAddr(void)
{
    return (uint8_t *)&__indata_start__;
}

int FlashCodePreInit(void)
{
/* TODO */
    return 0;
}


/* 从flash加载flash驱动 */
int FlashCodeLoadDriver(void)
{
    uint32_t *s = (unsigned int*)&__text_end__;
    uint32_t *begin = (unsigned int*)&__indata_start__;
    uint32_t *end = (unsigned int*)&__indata_end__;

    while(begin < end)
        *begin++ = *s++;

    return 0;
}

int FlashCodeRead(uint32_t addr, uint8_t *buffer, uint32_t size)
{
    uint32_t i = 0;

    for (i = 0; i < size; i++) {
        buffer[i] = *(uint8_t *)addr++;
    }

    return 0;
}

int FlashCodeErase(uint32_t addr, uint32_t size)
{
    uint32_t FlashRetVal = 0;
    uint32_t IntState = 0;
#if 0
    /* 首地址未对齐 */
    if (0 != addr & FLASH_CODE_ERASE_ADDR_MASK){
        return ERR_RANGE;
    }
    /* 结束地址未对齐 */
    if (0 != (addr + size) & FLASH_CODE_ERASE_ADDR_MASK){
        return ERR_RANGE;
    }
#endif
    IntState = (INT_CTL0 & INT_CTL0_AIE);
    SFR_CLR_BIT_ASM(INT_CTL0, INT_CTL0_AIE_POS);
    FlashRetVal = __FLASH_Erase__RAM__(Function_Parameter_Validate, addr, size);
    if (IntState != 0) {
        SFR_SET_BIT_ASM(INT_CTL0, INT_CTL0_AIE_POS);
    }

    return FlashRetVal;
}

int FlashCodeWrite(uint32_t addr, uint8_t *buffer, uint32_t size)
{
    uint32_t FlashRetVal = 0;
    uint32_t IntState = 0;

    /* 首地址未对齐 */
    if (0 != addr & FLASH_CODE_WRITE_ADDR_MASK){
        return ERR_RANGE;
    }
    /* 结束地址未对齐 */
    if (0 != (addr + size) & FLASH_CODE_WRITE_ADDR_MASK){
        return ERR_RANGE;
    }

    IntState = (INT_CTL0 & INT_CTL0_AIE);
    SFR_CLR_BIT_ASM(INT_CTL0, INT_CTL0_AIE_POS);
    FlashRetVal = __FLASH_Program_NBytes__RAM__(Function_Parameter_Validate, addr, size, (void *)buffer);
    if (IntState != 0) {
        SFR_SET_BIT_ASM(INT_CTL0, INT_CTL0_AIE_POS);
    }

    return FlashRetVal;
}
#if 0
uint32_t FlashCFGUserWrite(uint32_t addr, uint8_t *buffer,uint32_t size)
{
    uint32_t FlashRetVal = 0;
    uint32_t IntState = 0;

    IntState = (INT_CTL0 & INT_CTL0_AIE);
    SFR_CLR_BIT_ASM(INT_CTL0, INT_CTL0_AIE_POS);
    FlashRetVal = __FLASHCFGUSER_Program__(Function_Parameter_Validate, addr, size, buffer);
    if (IntState != 0) {
        SFR_SET_BIT_ASM(INT_CTL0, INT_CTL0_AIE_POS);
    }

    return FlashRetVal;
}

uint32_t FlashCFGUserRead(uint32_t addr, uint8_t *buffer,uint32_t size)
{
    uint32_t FlashRetVal = 0;
    uint32_t IntState = 0;

    IntState = (INT_CTL0 & INT_CTL0_AIE);
    SFR_CLR_BIT_ASM(INT_CTL0, INT_CTL0_AIE_POS);
    FlashRetVal = __FLASHCFGUSER_Read__(addr, size, buffer);
    if (IntState != 0) {
        SFR_SET_BIT_ASM(INT_CTL0, INT_CTL0_AIE_POS);
    }

    return FlashRetVal;
}
#endif
