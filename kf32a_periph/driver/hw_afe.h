#ifndef __HW_AFE_H
#define __HW_AFE_H

#define AFE_IC_NUM        (6U)

void AfeInit(void);
void AfePoll(void);
uint8_t AfeCellVoltageGet(uint8_t ch, uint16_t *voltage);
uint8_t AfeGpioVoltageGet(uint8_t ch, uint16_t *voltage);
uint8_t AfeSetDischarge(uint8_t ch);
uint8_t AfeClearDischarge(uint8_t ch);
uint8_t AfeGetDischarge(uint8_t ch, uint8_t *onoff);
uint8_t AfePecErrGet(void);

#endif

