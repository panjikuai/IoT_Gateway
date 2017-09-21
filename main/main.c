#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "math.h"  

#include <lwip/sockets.h>
#include "lwip/dns.h"
#include "lwip/inet.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"

#include "wifi.h"
#include "jd_device.h"
#include "jd_smart_config.h"
#include "jd_smart.h"

#include "airkiss.h"

#include "buttons.h"
#include "zigbee_cmd_service.h"
#include "iot_debug.h"
#include "gateway_manager.h"
#include "network_manager.h"
#include "sound_voice.h"

#include "a2dp.h"

#include "ledDisplay.h"
#include "ioControl.h"

#include "fft.h"

#define FFT_N  64

extern QueueHandle_t soundVoideEventQueue;

QueueHandle_t fftHandleQueue = NULL;

complex fft_result[FFT_N];

#define F_INDEX  26

uint8_t lightColorIndex = 0;

void fft_convert(const uint8_t *data, uint32_t len)
{
	if (len < FFT_N*2){
		return;
	}
	uint16_t *buff =  (uint16_t *)data;
	// uint32_t sum = 0;
	// for (uint16_t i = 0; i < len/2; i++){
	// 	sum +=buff[i];
	// }

	// sum = sum/(len/2);

	// float level = (float)sum/65535.0;
	// level = level*level*2;
	
	// level = level > 1? 1:level;
	// IoT_DEBUG(GENERIC_DBG | IoT_DBG_INFO,("level: %f\r\n",level));

	// LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP, LIGHT_BLUE, 254, (uint8_t)(level*254),1);

	
	uint16_t step = len/FFT_N;
	step = step - (step%2);
	// fill fft input buff
	for (uint16_t i = 0; i < FFT_N; i++){
		fft_result[i].real = (float)buff[i*step];
		fft_result[i].imag = 0;
	}

	uint16_t event;
	xQueueSend( fftHandleQueue, &event, 0 );

}

WiFiConfigParam_t gWifiParam;
Wifi_status_t gWifiStatus = WIFI_STATUS_WAIT;

TimerHandle_t systemTimer = NULL;
QueueHandle_t wifiParamSetQueue = NULL;

#define EUTTON_EVENT_TYPE_SHORT_PRESS 0
#define EUTTON_EVENT_TYPE_LONG_PRESS  1


typedef struct{
	uint16_t type; 	//long press or short press;
	uint16_t keyValue; 
}ButtonHandleEvent_t;

QueueHandle_t buttonHandleQueue = NULL;


uint8_t ipaddr[4];

void get_ip_address(uint8_t *ip)
{
	memcpy(ip,ipaddr,4);
}

void get_wifi_mac_address(uint8_t *mac)
{
	esp_efuse_mac_get_default(mac);
	// esp_efuse_read_mac(mac);
}

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
    	gWifiStatus = WIFI_STATUS_WAIT;
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
    	gWifiStatus = WIFI_STATUS_CONNECTED_AP;
    	break;
    case SYSTEM_EVENT_STA_GOT_IP:
	{
    	IoT_DEBUG(WIFI_DBG | IoT_DBG_INFO,("got IP address\n"));
        system_event_sta_got_ip_t *got_ip = &event->event_info.got_ip;
        // IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO, ("SYSTEM_EVENT_STA_GOTIP, ip:" IPSTR ", mask:" IPSTR ", gw:" IPSTR "\n",
		// IP2STR(&got_ip->ip_info.ip),
		// IP2STR(&got_ip->ip_info.netmask),
		// IP2STR(&got_ip->ip_info.gw)));
		ipaddr[0] = ip4_addr1_16(&got_ip->ip_info.ip);
		ipaddr[1] = ip4_addr2_16(&got_ip->ip_info.ip);
		ipaddr[2] = ip4_addr3_16(&got_ip->ip_info.ip);
		ipaddr[3] = ip4_addr4_16(&got_ip->ip_info.ip);

    	gWifiStatus = WIFI_STATUS_GOT_IP;
        break;
	}
    case SYSTEM_EVENT_STA_DISCONNECTED:
    	IoT_DEBUG(WIFI_DBG | IoT_DBG_WARNING,("Disconnected \n"));
    	gWifiStatus = WIFI_STATUS_DISCONNECTED_AP;
        esp_wifi_connect();
        break;
    default:
        break;
    }
    return ESP_OK;
}


void smartConfig_callback(uint8_t status, void *pdata)
{
	WiFiConfigParam_t *wifiParam = (WiFiConfigParam_t *)pdata;
	if (status == WIFI_STATUS_FOUND_CHANNEL){
		IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("Found AP channel:%d\r",wifiParam->channel));
	}else if (status == WIFI_STATUS_FINISHED){
		IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("ssid: %s, pwd: %s\r",wifiParam->ssid,wifiParam->pwd));
		xQueueSend( wifiParamSetQueue, wifiParam, 10 );
	}else if (status == WIFI_STATUS_TIMEOUT){
		IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("SmartConfig Timeout!\n"));
	}
	gWifiStatus = status;
}

void cloud_status_call_back(uint8_t status, void *info)
{

}

void wifi_Task(void *pvParameter)
{
	uint32_t ledStatus = 0;

	ButtonHandleEvent_t event;
	WiFiConfigParam_t 	param;
	while(1){
		
        if (xQueueReceive( wifiParamSetQueue , &param, 0 ) == pdTRUE){
        	WIFI_SetWifiParam(&param);
        	esp_restart();
		}
		
		if (xQueueReceive( buttonHandleQueue , &event, 0 ) == pdTRUE){
			IoT_DEBUG(GENERIC_DBG | IoT_DBG_INFO,("Event:type:%d, key:%d\r\n",event.type,event.keyValue));
			if (event.type == EUTTON_EVENT_TYPE_LONG_PRESS){
				if (event.keyValue == BUTTON_RELOAD){
					memset(&param, 0, sizeof(WiFiConfigParam_t));
					xQueueSend( wifiParamSetQueue, &param, 0 );
				}
			}else{
				if (event.keyValue == BUTTON_FUNC){
					if (ledStatus == 0){
						ledStatus = 1;
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP, 	LIGHT_RED, 	254, 254,100);
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_LEFT, 	LIGHT_RED,	254, 254,100);
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_RIGHT, LIGHT_RED, 	254, 254,100);
					}else if (ledStatus == 1){
						ledStatus = 2;
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP, 	LIGHT_GREEN,254, 254,100);
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_LEFT, 	LIGHT_GREEN,254, 254,100);
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_RIGHT, LIGHT_GREEN,254, 254,100);
					}else if (ledStatus == 2){
						ledStatus = 3;
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP, 	LIGHT_BLUE, 254, 254,100);
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_LEFT, 	LIGHT_BLUE, 254, 254,100);
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_RIGHT, LIGHT_BLUE, 254, 254,100);
					}else if (ledStatus == 3){
						ledStatus = 4;
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP, 	65536*1/6, 254, 254,100);
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_LEFT, 	65536*1/6, 254, 254,100);
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_RIGHT, 65536*1/6, 254, 254,100);
					}else if (ledStatus == 4){
						ledStatus = 5;
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP, 	65536*3/6, 254, 254,100);
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_LEFT, 	65536*3/6, 254, 254,100);
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_RIGHT, 65536*3/6, 254, 254,100);
					}else if (ledStatus == 5){
						ledStatus = 6;
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP, 	65536*5/6, 254, 254,100);
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_LEFT, 	65536*5/6, 254, 254,100);
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_RIGHT, 65536*5/6, 254, 254,100);
					}else{
						ledStatus = 0;
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP, 	0, 0, 254,100);
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_LEFT, 	0, 0, 254,100);
						LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_RIGHT, 0, 0, 254,100);
					}
				}else if (event.keyValue == BUTTON_RELOAD){
					uint32_t soundIndex = 0;
					xQueueSend( soundVoideEventQueue, &soundIndex, 10/portTICK_PERIOD_MS );
				}
			}
		}

		uint16_t p;
		if (xQueueReceive( fftHandleQueue , &p, 0 ) == pdTRUE){
			fft(FFT_N,fft_result);
			float level_low = 0;
			float level_medium = 0;
			float level_high = 0;
			for (uint16_t i = 0; i< (FFT_N/2);i++){
				if (i < (FFT_N/6)){
					level_low += sqrt( fft_result[i].real * fft_result[i].real+fft_result[i].imag * fft_result[i].imag);
				}else if (i < (FFT_N/3)){
					level_medium += sqrt( fft_result[i].real * fft_result[i].real+fft_result[i].imag * fft_result[i].imag);
				}else{
					level_high += sqrt( fft_result[i].real * fft_result[i].real+fft_result[i].imag * fft_result[i].imag);
				}
			}

			level_low = level_low/(FFT_N/6);
			level_medium = level_medium/(FFT_N/6);
			level_high = level_high/((FFT_N/2)-FFT_N/3);

			//level = level/(FFT_N/2-3);
			IoT_DEBUG(GENERIC_DBG | IoT_DBG_INFO,("freq: %f, level_high: %f\r\n",44.1*F_INDEX/FFT_N,level_high));

			uint16_t color = LIGHT_BLUE;
			if (lightColorIndex == 0){
				color = LIGHT_BLUE;
			}else if (lightColorIndex == 1){
				color = LIGHT_GREEN;
			}else{
				color = LIGHT_RED;
			}
			LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP, color, 254, (uint8_t)(level_high*254/65535),20);
			LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_LEFT, color, 254, (uint8_t)(level_low*254/65535),20);
			LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_RIGHT, color, 254, (uint8_t)(level_medium*254/65535),20);
		}


		vTaskDelay(5/portTICK_PERIOD_MS);
		// IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("ssid: %s, pwd: %s\r",gWifiParam.ssid,gWifiParam.pwd));
	}
}

void systemTimerCallback( TimerHandle_t xTimer )
{
	// static uint8_t count = 0;
	static uint8_t state = 0;

	if (gWifiStatus != WIFI_STATUS_CONNECTED_AP && gWifiStatus != WIFI_STATUS_GOT_IP){
		state = !state;
	}else{
		//IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO, ("system timer\n") );
	}
	uint32_t randN = rand();
	lightColorIndex = (uint8_t)(randN % 3);
	IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO, ("lightColorIndex:%d,randN:%d\n",lightColorIndex,randN) );
}

void keyShortPressedHandle(ButtonValue_t key)
{
	ButtonHandleEvent_t event={
		.type = EUTTON_EVENT_TYPE_SHORT_PRESS,
		.keyValue = key
	};
	xQueueSend( buttonHandleQueue, &event, 0 );
}

void keyLongPressedHandle(ButtonValue_t key)
{
	ButtonHandleEvent_t event={
		.type = EUTTON_EVENT_TYPE_LONG_PRESS,
		.keyValue = key
	};
	xQueueSend( buttonHandleQueue, &event, 0 );
}

#define A2DP 1


void app_main(void)
{
	IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO, ("RAM left %d\n", esp_get_free_heap_size()) );
    ESP_ERROR_CHECK( nvs_flash_init() );

    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

#ifdef _IOT_DEBUG_
	DebugLog_Init();
#endif
	IoControl_Init();

	LedDisplay_Init();
	LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP, 	LIGHT_RED, 	254, 0,500);
	LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_LEFT, 	LIGHT_GREEN,254, 0,500);
	LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_RIGHT, LIGHT_BLUE, 254, 0,500);
    
	Button_KeyEventInit(keyShortPressedHandle, keyLongPressedHandle);

	systemTimer = xTimerCreate("SYS_Timer", 5000 / portTICK_PERIOD_MS, pdTRUE, 0, systemTimerCallback );
	xTimerStart(systemTimer,0);
	
	SoundVoice_Init();
	// A2DP_Init();

	wifiParamSetQueue = xQueueCreate( 1, sizeof(WiFiConfigParam_t) );
	buttonHandleQueue = xQueueCreate( 1, sizeof(ButtonHandleEvent_t) );

	fftHandleQueue  = xQueueCreate( 1, sizeof(uint16_t) );

    if (WIFI_GetWifiParam(&gWifiParam) != ESP_OK){
		Airkiss_start(smartConfig_callback);
    }else{
    	WIFI_ConnecToTargetAP(&gWifiParam);
		GatewayManager_Init();
		NetworkManager_Init();
	}

    xTaskCreate(&wifi_Task, "WIFI", 2048, NULL, tskIDLE_PRIORITY+1, NULL);
}


