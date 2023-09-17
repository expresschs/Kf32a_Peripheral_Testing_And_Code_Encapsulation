#ifndef _HW_LIN_H_
#define _HW_LIN_H_

#define RLIN_OK      0
#define RLIN_ERROR   1
#define RLIN_NO_MSG  2
#define RLIN_READY   3
#define RLIN_BUSY    4

typedef enum{
    RLIN_RECEIVE = 0,
    RLIN_SEND
}LinMode;

/* 校验方式 */
typedef enum{
    RLIN_CLASSIC = 0,
    RLIN_ENHANCED
}LinCheckSum;

typedef struct{
    uint8_t id;
    uint8_t data[8];
    uint8_t len;
}LinMsg;

void LinInit(void);
uint8_t LinSend(LinMsg *msg, LinMode mode, LinCheckSum checkSum);
uint8_t LinRecieve(LinMsg *msg);
void LinGotoSleep(void);
void LinWakeUp(void);

#endif

