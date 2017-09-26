#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nvs.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"

#include "storage.h"
#include "wifi.h"


#define SMART_CONFIG_DIR 		"WiFi"
#define SMART_CONFIG_FILE_WIFI 	"parameter"

esp_err_t WIFI_GetWifiParam(WiFiConfigParam_t *param)
{
	esp_err_t err;
	err = storage_getflash(SMART_CONFIG_DIR, SMART_CONFIG_FILE_WIFI,param, sizeof(WiFiConfigParam_t));
	if(ESP_OK == err){
		if (   param->ssid_len ==0 || param->ssid_len > SSID_MAX_LEN
			|| param->pwd_len  ==0 || param->pwd_len > PWD_MAX_LEN){
			err = ESP_ERR_NVS_NOT_FOUND;
		}
	}
	return err;
}

esp_err_t WIFI_SetWifiParam(WiFiConfigParam_t *param)
{
	esp_err_t err;
	err = storage_setflash(SMART_CONFIG_DIR, SMART_CONFIG_FILE_WIFI,param, sizeof(WiFiConfigParam_t));
	return err;
}

void WIFI_ConnecToTargetAP(WiFiConfigParam_t *param)
{
    wifi_config_t sta_config;
    memset(&sta_config,0,sizeof(wifi_config_t));
    memcpy(sta_config.sta.ssid, param->ssid, param->ssid_len);
    memcpy(sta_config.sta.password, param->pwd, param->pwd_len);
    sta_config.sta.bssid_set = false;
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK(esp_wifi_start());
}
