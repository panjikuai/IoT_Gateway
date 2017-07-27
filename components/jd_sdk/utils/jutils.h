#ifndef _UTILS_H
#define _UTILS_H
#include <stdint.h>



int byte2hexstr(const uint8_t *pBytes, int srcLen, uint8_t *pDstStr, int dstLen);
int hexStr2bytes(const char *hexStr, uint8_t *buf, int bufLen);



#endif /* utils.h */