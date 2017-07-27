#ifndef WIFI_H
#define WIFI_H

#include "esp_err.h"

#define SSID_MAX_LEN 33
#define PWD_MAX_LEN  65

typedef struct _WiFiConfigParam_t{
  uint8_t ssid[SSID_MAX_LEN];
  uint8_t ssid_len;
  uint8_t pwd[PWD_MAX_LEN];
  uint8_t pwd_len;
  uint8_t randNumber;
  uint8_t channel;
}WiFiConfigParam_t;

esp_err_t wifi_get_Wifi_param(WiFiConfigParam_t *param);
esp_err_t wifi_set_wifi_param(WiFiConfigParam_t *param);
void wifi_connect_to_target_ap(WiFiConfigParam_t *param);































#endif
