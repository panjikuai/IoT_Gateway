#ifndef STORAGE_H
#define STORAGE_H

#include "esp_err.h"

esp_err_t storage_getflash(const char* dir, const char* file,void *out_value, int len);
esp_err_t storage_setflash(const char* dir, const char* file,void *in_value, int len);


#endif
