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

#define EUTTON_EVENT_TYPE_SHORT_PRESS 0
#define EUTTON_EVENT_TYPE_LONG_PRESS  1

typedef struct{
	uint16_t type; 	//long press or short press;
	uint16_t keyValue; 
}ButtonHandleEvent_t;

extern QueueHandle_t soundVoideEventQueue;

WiFiConfigParam_t gWifiParam;
Wifi_status_t gWifiStatus = WIFI_STATUS_WAIT;

TimerHandle_t systemTimer = NULL;
QueueHandle_t wifiParamSetQueue = NULL;
QueueHandle_t buttonHandleQueue = NULL;

uint8_t ipaddr[4];
void get_ip_address(uint8_t *ip)
{
	memcpy(ip,ipaddr,4);
}

void get_wifi_mac_address(uint8_t *mac)
{
	esp_efuse_mac_get_default(mac);
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
    case SYSTEM_EVENT_STA_GOT_IP:{
        system_event_sta_got_ip_t *got_ip = &event->event_info.got_ip;
		ipaddr[0] = ip4_addr1_16(&got_ip->ip_info.ip);
		ipaddr[1] = ip4_addr2_16(&got_ip->ip_info.ip);
		ipaddr[2] = ip4_addr3_16(&got_ip->ip_info.ip);
		ipaddr[3] = ip4_addr4_16(&got_ip->ip_info.ip);
		IoT_DEBUG(WIFI_DBG | IoT_DBG_INFO,("Got IP Addr:%d.%d.%d.%d\n",ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]));
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


void SmartConfig_Callback(uint8_t status, void *pdata)
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

void wifi_Task(void *pvParameter)
{
	ButtonHandleEvent_t event;
	WiFiConfigParam_t 	param;
	while(1){
        if (xQueueReceive( wifiParamSetQueue , &param, 0 ) == pdTRUE){
			WIFI_SetWifiParam(&param);
			IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("ssid: %s, pwd: %s\r",param.ssid,param.pwd));
			vTaskDelay(100/portTICK_PERIOD_MS);
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
				uint32_t soundListIndex;
				if (event.keyValue == BUTTON_FUNC){
					soundListIndex = 1;
					xQueueSend( soundVoideEventQueue, &soundListIndex, 10/portTICK_PERIOD_MS );
				}else if (event.keyValue == BUTTON_RELOAD){
					soundListIndex = 0;
					xQueueSend( soundVoideEventQueue, &soundListIndex, 10/portTICK_PERIOD_MS );
				}
			}
		}
		vTaskDelay(50/portTICK_PERIOD_MS);
	}
}

void systemTimerCallback( TimerHandle_t xTimer )
{
	if (gWifiStatus == WIFI_STATUS_DISCONNECTED_AP ){
		
	}else{
		//IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO, ("system timer\n") );
	}
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

	DebugLog_Init();
	IoControl_Init();
	LedDisplay_Init();
	Button_KeyEventInit(keyShortPressedHandle, keyLongPressedHandle);
	SoundVoice_Init();
	// A2DP_Init();

	wifiParamSetQueue = xQueueCreate( 1, sizeof(WiFiConfigParam_t) );
	buttonHandleQueue = xQueueCreate( 1, sizeof(ButtonHandleEvent_t) );

    if (WIFI_GetWifiParam(&gWifiParam) != ESP_OK){
		Airkiss_Start(SmartConfig_Callback);
    }else{
		IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("ssid: %s, pwd: %s\r",gWifiParam.ssid,gWifiParam.pwd));
    	WIFI_ConnecToTargetAP(&gWifiParam);
		GatewayManager_Init();
		NetworkManager_Init();
	}

	systemTimer = xTimerCreate("SYS_Timer", 2000 / portTICK_PERIOD_MS, pdTRUE, 0, systemTimerCallback );
	xTimerStart(systemTimer,0);

	xTaskCreate(&wifi_Task, "WIFI", 2048, NULL, tskIDLE_PRIORITY+1, NULL);
	
}


