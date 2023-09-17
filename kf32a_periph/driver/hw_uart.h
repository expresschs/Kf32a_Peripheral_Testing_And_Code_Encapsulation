#ifndef _HW_UART_H_
#define _HW_UART_H_

uint8_t UartInit(uint8_t ch, uint32_t baud);
uint8_t UartSend(uint8_t ch, uint8_t *data, uint16_t len);
uint8_t UartRecv(uint8_t ch, uint8_t *data, uint16_t *len);
uint8_t UartDeinit(uint8_t ch);

#endif

