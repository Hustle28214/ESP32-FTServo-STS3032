#ifndef _STS_ORDER_H
#define _STS_ORDER_H

#include <stdint.h>

// Status

#define STS_ERR_NO_REPLY 1;
#define STS_ERR_CRC_CMP 2;
#define STS_ERR_SLAVE_ID 3;
#define STS_ERR_BUFFER_LEN 4;


extern void ORDER_GET_END(uint8_t _End);
extern void ORDER_IS_BIG_END(uint8_t _End);
extern void ORDER_IS_LITTLE_END(uint8_t _End);
extern void ORDER_GET_LEVEL(void);
extern void ORDER_GET_SERVO_STATUS(void);
extern void ORDER_GET_SERVO_ERROR(void);
extern void ORDER_SET_LEVEL(uint8_t _Level);

// Transfrom the data structure


extern void DIGITS16_TO_DIGITS8(uint8_t *_dataL,uint8_t *_dataH, uint16_t _digits16);
extern void DIGITS8_TO_DIGITS16(uint8_t _dataL,uint8_t _dataH, uint8_t _digits8);



#endif