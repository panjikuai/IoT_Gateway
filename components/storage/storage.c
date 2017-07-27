#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"



esp_err_t storage_getflash(const char* dir, const char* file,void *out_value, int len)
{
	esp_err_t err;
	nvs_handle handle;
	size_t nvs_size = len;

	ESP_ERROR_CHECK( nvs_open(dir, NVS_READWRITE, &handle) );
	err =  nvs_get_blob(handle, file, out_value, &nvs_size);
	nvs_close(handle);
	return err;
}


esp_err_t storage_setflash(const char* dir, const char* file,void *in_value, int len)
{
	esp_err_t err;
	nvs_handle handle;

	ESP_ERROR_CHECK( nvs_open(dir, NVS_READWRITE, &handle) );
	err = nvs_set_blob(handle, file, in_value, len);
	ESP_ERROR_CHECK(nvs_commit(handle));
	nvs_close(handle);
	return err;
}
