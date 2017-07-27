#ifndef CRC_H
#define CRC_H

#include "stdint.h"

uint8_t FastCRC(uint8_t LastCRC, uint8_t newbyte);
uint8_t calculate_crc8(uint8_t *p,uint8_t len);






#endif
