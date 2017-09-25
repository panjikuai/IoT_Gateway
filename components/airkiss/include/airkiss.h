#ifndef _AIRKISS_H_
#define _AIRKISS_H_

#include "esp_err.h"

typedef enum{
  AIRKISS_CHANNEL_LISTERNING= 0,
  AIRKISS_SSID_PWD_PICKUP,
  SMART_CFG_FINISHED,
}Airkiss_state_t;

typedef enum {
	AIRKISS_WIFI_CH_1 = 1,
	AIRKISS_WIFI_CH_2,
	AIRKISS_WIFI_CH_3,
	AIRKISS_WIFI_CH_4,
	AIRKISS_WIFI_CH_5,
	AIRKISS_WIFI_CH_6,
	AIRKISS_WIFI_CH_7,
	AIRKISS_WIFI_CH_8,
	AIRKISS_WIFI_CH_9,
	AIRKISS_WIFI_CH_10,
	AIRKISS_WIFI_CH_11,
	AIRKISS_WIFI_CH_12,
	AIRKISS_WIFI_CH_13
}AirkissChannel_t;

#define SSID_MAX_LEN 33
#define PWD_MAX_LEN  65

typedef struct{
  uint8_t ssid[SSID_MAX_LEN];
  uint8_t ssid_len;
  uint8_t pwd[PWD_MAX_LEN];
  uint8_t pwd_len;
  uint8_t randNumber;
  uint8_t channel;
}AirkissWiFiParam_t;

typedef void(*Airkiss_Callback_t)(uint8_t status, void *pdata);
void Airkiss_Start(Airkiss_Callback_t cb);































#endif
