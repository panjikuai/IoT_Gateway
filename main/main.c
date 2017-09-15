#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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

WiFiConfigParam_t gWifiParam;
Wifi_status_t gWifiStatus = WIFI_STATUS_WAIT;

TimerHandle_t systemTimer = NULL;
QueueHandle_t wifiParamSetQueue = NULL;

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
    	// gpio_set_level(GPIO_NUM_2, 1);
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
	while(1){
		WiFiConfigParam_t param;
        if (xQueueReceive( wifiParamSetQueue , &param, 0 ) == pdTRUE){
        	WIFI_SetWifiParam(&param);
        	esp_restart();
        }
		vTaskDelay(50/portTICK_PERIOD_MS);
		// IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("ssid: %s, pwd: %s\r",gWifiParam.ssid,gWifiParam.pwd));
	}
}

void systemTimerCallback( TimerHandle_t xTimer )
{
	static uint8_t count = 0;
	static uint8_t state = 0;

	if (gWifiStatus != WIFI_STATUS_CONNECTED_AP && gWifiStatus != WIFI_STATUS_GOT_IP){
		state = !state;
		// gpio_set_level(GPIO_NUM_2, state);
	}else{
		//IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO, ("system timer\n") );
	}


	if (count == 0){
		count =1;
		LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP, 	LIGHT_RED, 	254, 254,100);
		LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_LEFT, 	LIGHT_GREEN,254, 254,100);
		LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_RIGHT, LIGHT_BLUE, 254, 254,100);
	}else if (count == 1){
		count = 2;
		LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP, 	LIGHT_GREEN,254, 254,100);
		LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_LEFT, 	LIGHT_BLUE, 254, 254,100);
		LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_RIGHT, LIGHT_RED,  254, 254,100);
	}else{
		count = 0;
		LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP, 	LIGHT_BLUE, 254, 254,100);
		LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_LEFT, 	LIGHT_RED,  254, 254,100);
		LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_RIGHT, LIGHT_GREEN,254, 254,100);
	}


}

void keyShortPressedHandle(ButtonValue_t key)
{

}

void keyLongPressedHandle(ButtonValue_t key)
{
	if (key == BUTTON_RELOAD){
		WiFiConfigParam_t param;
		memset(&param, 0, sizeof(WiFiConfigParam_t));
		xQueueSend( wifiParamSetQueue, &param, 10 );
	}
}

#define A2DP 1


void app_main(void)
{
	IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO, ("RAM left %d\n", esp_get_free_heap_size()) );
    ESP_ERROR_CHECK( nvs_flash_init() );

	

	SoundVoice_Init();
	A2DP_Init();

    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

#ifdef _IOT_DEBUG_
	DebugLog_Init();
#endif


	IoControl_Init();

	LedDisplay_Init();
	LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP, 	LIGHT_RED, 	254, 254,500);
	LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_LEFT, 	LIGHT_GREEN,254, 254,500);
	LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_RIGHT, LIGHT_BLUE, 254, 254,500);
    
    Button_KeyEventInit(keyShortPressedHandle, keyLongPressedHandle);
	
	systemTimer = xTimerCreate("SYS_Timer", 1000 / portTICK_PERIOD_MS, pdTRUE, 0, systemTimerCallback );
	xTimerStart(systemTimer,0);

    wifiParamSetQueue = xQueueCreate( 1, sizeof(WiFiConfigParam_t) );

    if (WIFI_GetWifiParam(&gWifiParam) != ESP_OK){
		Airkiss_start(smartConfig_callback);
    }else{
    	WIFI_ConnecToTargetAP(&gWifiParam);
		GatewayManager_Init();
		NetworkManager_Init();
	}

    xTaskCreate(&wifi_Task, "WIFI", 2048, NULL, tskIDLE_PRIORITY+1, NULL);
}


