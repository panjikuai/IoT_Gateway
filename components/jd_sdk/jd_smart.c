#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

#include "jd_smart.h"
#include "jd_apps.h"
#include "jd_smart_config.h"
#include "iot_debug.h"


extern int tcp_client_socket;
extern int udp_socket;
extern QueueHandle_t jd_cloud_queue;



TimerHandle_t jd_heart_beat_timer = NULL;

int32_t g_remote_server_ip = 0;
uint32_t dns_is_connecting = false;

extern Wifi_status_t gWifiStatus;

void jd_smart_task(void *pvParameter);


void jd_smart_start(jd_net_status_t net_status,jd_do_recipe_t do_recipe,jd_do_control_t do_control, jd_do_snapshot_t do_snapshot)
{
	jd_apps_init(net_status, do_recipe, do_control,  do_snapshot);
	xTaskCreate(&jd_smart_task, "JD_SMT", 2048, NULL, tskIDLE_PRIORITY+2, NULL);
}

void jd_heart_beat_timer_callback( TimerHandle_t xTimer )
{
	uint32_t msg = MSG_HEART_BEAT;
	xQueueSend( jd_cloud_queue, ( void * )&msg, 10 );
}

void dns_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg)
{
//	IoT_DEBUG((LWIP_DBG | IoT_DBG_INFO),("JD IP:%s\n", inet_ntoa(*ipaddr)));
	g_remote_server_ip = (uint32_t)(ipaddr->u_addr.ip4.addr);
	dns_is_connecting = false;
}

void jd_smart_task(void *pvParameter)
{
	ip_addr_t addr;
	struct sockaddr_in server_addr;
	uint32_t jdMessage;

    jd_apps_load_param();
    jd_apps_generate_EccKey();
    dns_init();

    jd_cloud_queue = xQueueCreate( 1, sizeof(uint32_t) );
    jd_heart_beat_timer = xTimerCreate( "hbTimer", 20000/portTICK_PERIOD_MS, pdTRUE, 0, jd_heart_beat_timer_callback );

    xTaskCreate(&vTaskNetworkTcpEvent, "JD_TCP", 2048, NULL, tskIDLE_PRIORITY+3, NULL);
    xTaskCreate(&vTaskNetworkUdpEvent, "JD_UDP", 2048, NULL, tskIDLE_PRIORITY+3, NULL);
    while (1) {

        if (gWifiStatus == WIFI_STATUS_GOT_IP){
			if(g_remote_server_ip == 0 && dns_is_connecting == false){
				dns_is_connecting = true;
				dns_gethostbyname((char*)(MAIN_HOST_NAME), &addr,dns_callback, NULL);
			}
        }

        if ( gWifiStatus == WIFI_STATUS_GOT_IP){
        	if (udp_socket == -1){
				memset(&server_addr, 0, sizeof(server_addr));
				server_addr.sin_family = AF_INET;
				server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
				server_addr.sin_port = htons(JD_UDP_PORT_1);
				udp_socket = socket(AF_INET,SOCK_DGRAM,0);
				if( bind(udp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))< 0){
					close(udp_socket);
					udp_socket = -1;
					IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("udp bind error!\n"));
				}
        	}
        }
        if ( gWifiStatus == WIFI_STATUS_GOT_IP && g_remote_server_ip != 0){
			if (tcp_client_socket == -1){
				tcp_client_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
				memset(&server_addr, 0, sizeof(server_addr));
				server_addr.sin_family = AF_INET;
				server_addr.sin_addr.s_addr = g_remote_server_ip;
				server_addr.sin_port = htons(MAIN_HOST_PORT);
				if( connect(tcp_client_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0){
					close(tcp_client_socket);
					tcp_client_socket = -1;
					IoT_DEBUG(LWIP_DBG | IoT_DBG_INFO,("connect failed!\r\n"));
					xTimerStop(jd_heart_beat_timer,0);
				}else{
					IoT_DEBUG(LWIP_DBG | IoT_DBG_INFO,("connected to JD server\n"));
					xTimerStart(jd_heart_beat_timer,0);
					uint32_t msg = MSG_AUTH;
					xQueueSend( jd_cloud_queue, ( void * )&msg, 10 );
				}
			}
        }
        if (gWifiStatus != WIFI_STATUS_GOT_IP){
        	if (udp_socket != -1){
				close(udp_socket);
				udp_socket = -1;
        	}
        	if (tcp_client_socket != -1){
				close(tcp_client_socket);
				tcp_client_socket = -1;
        	}
        }

        if (xQueueReceive( jd_cloud_queue, &jdMessage, 0 ) == pdTRUE){
        	switch(jdMessage){
        	case MSG_AUTH:
        		jd_apps_request_server_auth();
        		break;
        	case MSG_HEART_BEAT:
        		jd_apps_send_heart_beat();
        		break;
        	case MSG_DATA_UPLOAD:
        		jd_apps_data_upload(0);
        		break;
        	}
        }

        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}
