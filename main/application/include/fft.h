#ifndef __FFT_H__  
#define __FFT_H__  
  
#include <stdint.h>
#include <stdbool.h>

typedef struct complex{ //复数类型  
    float real;       //实部  
    float imag;       //虚部  
}complex; 

void FFT_DataInput(const uint8_t *data, uint32_t len);
uint32_t FFT_Execute(int N,complex f[]);

#endif

