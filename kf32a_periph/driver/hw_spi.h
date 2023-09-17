#ifndef _HW_SPI_H_
#define _HW_SPI_H_

uint8_t HwSpiInit(uint8_t ch, uint32_t baud);
uint8_t hwSpiTransfer(uint8_t ch, uint8_t *send, uint8_t *recv, uint32_t size);

#endif

