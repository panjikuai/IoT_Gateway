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

WiFiConfigParam_t g_wifi_param;
wifi_status_t g_wifi_status = WIFI_STATUS_WAIT;

TimerHandle_t system_timer = NULL;
QueueHandle_t wifi_param_set_queue = NULL;

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
    	g_wifi_status = WIFI_STATUS_WAIT;
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
    	g_wifi_status = WIFI_STATUS_CONNECTED_AP;
    	gpio_set_level(GPIO_NUM_2, 1);
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

    	g_wifi_status = WIFI_STATUS_GOT_IP;
        break;
	}
    case SYSTEM_EVENT_STA_DISCONNECTED:
    	IoT_DEBUG(WIFI_DBG | IoT_DBG_WARNING,("Disconnected \n"));
    	g_wifi_status = WIFI_STATUS_DISCONNECTED_AP;
        esp_wifi_connect();
        break;
    default:
        break;
    }
    return ESP_OK;
}


void smartconfig_airkiss_callback(uint8_t status, void *pdata)
{
	WiFiConfigParam_t *wifiParam = (WiFiConfigParam_t *)pdata;
	if (status == WIFI_STATUS_FOUND_CHANNEL){
		IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("Found AP channel:%d\r",wifiParam->channel));
	}else if (status == WIFI_STATUS_FINISHED){
		IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("ssid: %s, pwd: %s\r",wifiParam->ssid,wifiParam->pwd));
		xQueueSend( wifi_param_set_queue, wifiParam, 10 );
	}else if (status == WIFI_STATUS_TIMEOUT){
		IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("SmartConfig Timeout!\n"));
	}
	g_wifi_status = status;
}

void cloud_status_call_back(uint8_t status, void *info)
{

}

void wifi_Task(void *pvParameter)
{
	while(1){
		WiFiConfigParam_t param;
        if (xQueueReceive( wifi_param_set_queue , &param, 0 ) == pdTRUE){
        	wifi_set_wifi_param(&param);
        	esp_restart();
        }
		vTaskDelay(50/portTICK_PERIOD_MS);
		// IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("ssid: %s, pwd: %s\r",g_wifi_param.ssid,g_wifi_param.pwd));
	}
}

void system_timer_callback( TimerHandle_t xTimer )
{
	static uint8_t state = 0;

	if (g_wifi_status != WIFI_STATUS_CONNECTED_AP && g_wifi_status != WIFI_STATUS_GOT_IP){
		state = !state;
		gpio_set_level(GPIO_NUM_2, state);
	}else{
		//IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO, ("system timer\n") );
	}
}

void reload_button_pressed(void)
{
	WiFiConfigParam_t param;
	memset(&param, 0, sizeof(WiFiConfigParam_t));
	xQueueSend( wifi_param_set_queue, &param, 10 );
}

#define A2DP 1


void app_main(void)
{
	IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO, ("RAM left %d\n", esp_get_free_heap_size()) );
    ESP_ERROR_CHECK( nvs_flash_init() );

	Sound_Voice_Init();
	A2DP_Init();

    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

#ifdef _IOT_DEBUG_
	create_debug_semaphore();
#endif

    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    button_init(NULL, reload_button_pressed);
	
	system_timer = xTimerCreate("SYS_Timer", 500 / portTICK_PERIOD_MS, pdTRUE, 0, system_timer_callback );
	xTimerStart(system_timer,0);

    wifi_param_set_queue = xQueueCreate( 1, sizeof(WiFiConfigParam_t) );

    if (wifi_get_Wifi_param(&g_wifi_param) != ESP_OK){
		airkiss_start(smartconfig_airkiss_callback);
    }else{
    	wifi_connect_to_target_ap(&g_wifi_param);
		GatewayManager_Init();
		NetworkManager_Init();
    }

    xTaskCreate(&wifi_Task, "WIFI", 2048, NULL, tskIDLE_PRIORITY+1, NULL);
}


