#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>

/*Types definition*/
typedef unsigned char bool;
typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long dword;
typedef unsigned long dlong[2];
typedef void (*tIntFunc)(void);
typedef uint8_t TPE_ErrCode;

/* bit位结构体 */
#ifndef _UT_
#pragma MESSAGE DISABLE C1106 /* I am sure this bitfile type is supported */
#endif
typedef union{
    uint8_t Byte;
    struct{
        uint8_t Bit0  :1;
        uint8_t Bit1  :1;
        uint8_t Bit2  :1;
        uint8_t Bit3  :1; 
        uint8_t Bit4  :1;
        uint8_t Bit5  :1;
        uint8_t Bit6  :1;
        uint8_t Bit7  :1;  
    }Bits;
}BitsFlag;

/* 错误码定义 */
#define ERR_OK           0             /* OK */
#define ERR_SPEED        1             /* This device does not work in the active speed mode. */
#define ERR_RANGE        2             /* Parameter out of range. */
#define ERR_VALUE        3             /* Parameter of incorrect value. */
#define ERR_OVERFLOW     4             /* Timer overflow. */
#define ERR_MATH         5             /* Overflow during evaluation. */
#define ERR_ENABLED      6             /* Device is enabled. */
#define ERR_DISABLED     7             /* Device is disabled. */
#define ERR_BUSY         8             /* Device is busy. */
#define ERR_NOTAVAIL     9             /* Requested value or method not available. */
#define ERR_RXEMPTY      10            /* No data in receiver. */
#define ERR_TXFULL       11            /* Transmitter is full. */
#define ERR_BUSOFF       12            /* Bus not available. */
#define ERR_OVERRUN      13            /* Overrun error is detected. */
#define ERR_FRAMING      14            /* Framing error is detected. */
#define ERR_PARITY       15            /* Parity error is detected. */
#define ERR_NOISE        16            /* Noise error is detected. */
#define ERR_IDLE         17            /* Idle error is detected. */
#define ERR_FAULT        18            /* Fault error is detected. */
#define ERR_BREAK        19            /* Break char is received during communication. */
#define ERR_CRC          20            /* CRC error is detected. */
#define ERR_ARBITR       21            /* A node losts arbitration. This error occurs if two nodes start transmission at the same time. */
#define ERR_PROTECT      22            /* Protection error is detected. */
#define ERR_UNDERFLOW    23            /* Underflow error is detected. */
#define ERR_UNDERRUN     24            /* Underrun error is detected. */
#define ERR_COMMON       25            /* Common error of a device. */
#define ERR_LINSYNC      26            /* LIN synchronization error is detected. */

#define BOOL_TRUE          1
#define BOOL_FALSE         0

#define SWAP32(hl)  (((hl) >> 24) | (((hl) >>  8) & 0x0000ff00) | (((hl) <<  8) & 0x00ff0000) | ( (hl) << 24))
#define SWAP16(hs)  ((((hs) >> 8) & 0x00FF) | (((hs) << 8) & 0xFF00))

#endif

