#include "stdint.h"

const uint8_t crctable[16] =  {0,0x07,0x0E,0x09, 0x1c,0x1b,0x12,0x15, 0x38,0x3F,0x36,0x31, 0x24,0x23,0x2A,0x2D};
const uint8_t crctable2[16] = {0,0x70,0xE0,0x90, 0xC1,0xB1,0x21,0x51, 0x83,0xF3,0x63,0x13, 0x42,0x32,0xA2,0xD2};

uint8_t FastCRC(uint8_t LastCRC, uint8_t newbyte)
{
	uint8_t index;
	index = newbyte;
	index ^= LastCRC;
	index >>= 4;
	LastCRC &= 0x0F;
	LastCRC ^= crctable2[index];
	index = LastCRC;
	index ^= newbyte;
	index &= 0x0F;
	LastCRC &= 0xF0;
	LastCRC ^= crctable[index];
	return(LastCRC);
}

/* calculate crc8 */
uint8_t calculate_crc8(uint8_t *p,uint8_t len)
{
	uint8_t crc_value = 0;
	uint8_t i;
	
	for(i=0;i<len;i++){
		crc_value = FastCRC(crc_value,p[i]);
	}
	
	return crc_value;
}
