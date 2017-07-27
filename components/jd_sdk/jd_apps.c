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

#include "utils/packets.h"
#include "utils/nodeCache.h"

#include "storage.h"
#include "jd_apps.h"
#include "jd_device.h"
#include "iot_debug.h"

#define JD_PARAM_DIR 		"jd_dir"
#define JD_NET_WORK_FILE 	"parameter"

static jddevice_t app_param;
jddevice_t *jdDev = &app_param;

int tcp_client_socket = -1;
int udp_socket = -1;
uint8_t g_tcp_recv_buf[RX_BUFFER_SIZE];
uint8_t g_udp_recv_buf[RX_BUFFER_SIZE + SOCKET_ADDR_SIZE];

uint8_t g_tcp_sent_buf[TX_BUFFER_SIZE];
uint8_t g_udp_sent_buf[TX_BUFFER_SIZE];
uint8_t g_generic_buf[GENERIC_BUFFER_SIZE];
uint8_t g_dev_json_info[DEV_JSON_INFO_SIZE];
uint8_t g_hb_buf[HB_BUFFER_SIZE];

static uint8_t form_dev_json_info = 1;

void handle_cloud_request(uint8_t *cloud_packet, uint32_t packet_len);
void handle_local_request(uint8_t *local_packet, uint32_t packet_len);

jd_net_status_t net_status 		= NULL;
jd_do_recipe_t do_recipe 		= NULL;
jd_do_control_t do_control 		= NULL;
jd_do_snapshot_t do_snapshot 	= NULL;

QueueHandle_t jd_cloud_queue = NULL;

TimerHandle_t jd_data_upload_timer = NULL;

uint32_t g_server_time;
uint8_t gbCloudConnected = CLOUD_DISCONNECTED;
uint8_t tcp_data_sending = 0;

extern TimerHandle_t jd_heart_beat_timer;

void jd_apps_load_param(void)
{
    ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_STA, jdDev->mac));
    storage_getflash(JD_PARAM_DIR, JD_NET_WORK_FILE,jdDev, sizeof(jddevice_t));
    memcpy(jdDev->uuid, PRODUCT_UUID, sizeof(PRODUCT_UUID));
}

void jd_apps_store_param(void)
{
    storage_getflash(JD_PARAM_DIR, JD_NET_WORK_FILE,jdDev, sizeof(jddevice_t));
}

void jd_apps_generate_EccKey(void)
{
    if(jdDev->ecckeyConfigured != 1) {
        eccContexInit();
        jdDev->ecckeyConfigured = 1;
        storage_setflash(JD_PARAM_DIR, JD_NET_WORK_FILE,jdDev, sizeof(jddevice_t));
    }
}

void vTaskNetworkTcpEvent(void *pvParameters)
{
    int32_t lRetVal = -1;
    while(1){
        if (tcp_client_socket > 0){
            lRetVal = recv(tcp_client_socket, g_tcp_recv_buf, RX_BUFFER_SIZE, 0);
            if (lRetVal > 0){
//            	IoT_DEBUG(LWIP_DBG | IoT_DBG_INFO, ("tcp_client_socket: %d\r\n",lRetVal));
                handle_cloud_request(g_tcp_recv_buf, lRetVal);
            }else if (lRetVal == -1){
				close(tcp_client_socket);
            	tcp_client_socket = -1;
//            	IoT_DEBUG(LWIP_DBG | IoT_DBG_INFO, ("tcp_client_socket connect failed!"));
            }
        }
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

void vTaskNetworkUdpEvent(void *pvParameters)
{
	int32_t lRetVal = -1;
	socklen_t iAddrSize = sizeof(struct sockaddr_in);
	struct sockaddr_in sAddr={
		.sin_family = AF_INET,
		.sin_addr.s_addr = INADDR_ANY,
		.sin_port = htons(JD_UDP_PORT_1)
	};

	while(1){
		 if (udp_socket >= 0){
			 lRetVal = recvfrom(udp_socket, g_udp_recv_buf, RX_BUFFER_SIZE, 0,(struct sockaddr *)&sAddr, &iAddrSize);
			 if (lRetVal > 0){
				 //IoT_DEBUG(LWIP_DBG | IoT_DBG_INFO,("udp client:%s,got data:%d\n",inet_ntoa(sAddr.sin_addr),iAddrSize));
				 uint8_t *p = (uint8_t*)g_udp_recv_buf + RX_BUFFER_SIZE;
				 memcpy(p, (uint8_t*)&sAddr, sizeof(struct sockaddr));
				 handle_local_request(g_udp_recv_buf, lRetVal);
			 }else if (lRetVal == -1){
				 close(udp_socket);
				 udp_socket = -1;
				 IoT_DEBUG(LWIP_DBG | IoT_DBG_INFO, ("udp_socket connect failed!"));
			 }
		 }
		vTaskDelay(10/portTICK_PERIOD_MS);
	}
}


void jd_data_upload_timer_callback( TimerHandle_t xTimer )
{
	uint32_t msg = MSG_DATA_UPLOAD;
	xQueueSend( jd_cloud_queue, ( void * )&msg, 10 );
}


void jd_apps_init(jd_net_status_t jd_net_status,jd_do_recipe_t jd_do_recipe,jd_do_control_t jd_do_control, jd_do_snapshot_t jd_do_snapshot)
{
	net_status 		= jd_net_status;
	do_recipe 		= jd_do_recipe;
	do_control 		= jd_do_control;
	do_snapshot 	= jd_do_snapshot;
	jd_data_upload_timer = xTimerCreate( "upTimer", 2000/portTICK_PERIOD_MS, pdFALSE, 0, jd_data_upload_timer_callback );
}

static uint16_t form_scan_json_string(uint8_t *buf)
{
    uint16_t len;
    uint8_t *p = buf;
    uint8_t tbuf[64];
    *p = '{';
    *(p + 1) = '\0';
    sprintf((char *)tbuf, "\"mac\":\"%02X:%02X:%02X:%02X:%02X:%02X\",",
                    jdDev->mac[0], jdDev->mac[1], jdDev->mac[2], jdDev->mac[3], jdDev->mac[4], jdDev->mac[5]);
    strcat((char *)p, (char *)tbuf);
    sprintf((char *)tbuf, "\"productuuid\":\"%s\",", jdDev->uuid);
    strcat((char *)p, (char *)tbuf);
    sprintf((char *)tbuf, "\"lancon\":%d,", 1);
    strcat((char *)p, (char *)tbuf);
    sprintf((char *)tbuf, "\"trantype\":%d,", 1);
    strcat((char *)p, (char *)tbuf);
    if(jdDev->feedid[0] == 0x0 || jdDev->feedid[1] == 0x0){
        sprintf((char *)tbuf, "\"feedid\":\"\",");
        strcat((char *)p, (char *)tbuf);
    }
    else{
        sprintf((char *)tbuf, "\"feedid\":\"%s\",", jdDev->feedid);
        strcat((char *)p, (char *)tbuf);
        form_dev_json_info = 0;
    }
    sprintf((char *)tbuf, "\"devkey\":\"%s\",", jdDev->pubkeyS);
    strcat((char *)p, (char *)tbuf);

    sprintf((char *)tbuf, "\"devtype\":%d}", 0);
    strcat((char *)p, (char *)tbuf);

    len = strlen((char *)p);
    return len;
}


static int8_t parse_json_key(uint8_t *buff)
{
	char *p = NULL;
	uint8_t index = 0;
	const char *buf = (char *)buff;
	p = strstr(buf, "accesskey");
	if(p != NULL) {
		p = p + 12;
		memcpy(jdDev->accesskey, p, 32);
		jdDev->accesskey[32] = 0;
		p = NULL;
	}
	else {
		return 1;
	}
	p = strstr(buf, "feedid");
	if(p != NULL) {
		//M2M_INFO("find feedid\n");
		p = p + 9;
		while(*p != '"') {
			jdDev->feedid[index++] = *p++;
		}
		jdDev->feedid[index] = 0;
		p = NULL;
	}
	else {
		return 1;
	}
	p = strstr(buf, "localkey");
	if(p != NULL) {
		p = p + 11;
		memcpy(jdDev->localkey, p, 32);
		jdDev->localkey[32] = 0;
	}
	else {
		return 1;
	}
	jdDev->isConfigured = DEV_CLOUD_CONFIGURED;
	return 0;
}

void jd_apps_request_server_auth(void)
{
    int32_t len = 0;
    len = Server_Auth(g_hb_buf);
    if(tcp_client_socket >= 0) {
    	send(tcp_client_socket, g_hb_buf, len, 0);
    	IoT_DEBUG(JD_SMART_DBG | IoT_DBG_INFO,("Req auth..\n"));
    }
}

void jd_apps_send_heart_beat(void)
{
    int32_t len = Server_HB(g_hb_buf);
    if(tcp_client_socket >= 0) {
    	send(tcp_client_socket, g_hb_buf, len, 0);
    	IoT_DEBUG(JD_SMART_DBG | IoT_DBG_INFO,("heart beat.\n"));
    }
}

void jd_apps_data_upload(uint8_t snapshot)
{
    int32_t len = 0;
    dataupload_t *snap = (dataupload_t*)g_generic_buf;
    snap->timestamp = 10;

    if(snapshot == 1) {
		//length 0 means no payload in the get snapshot command
		if(do_snapshot((void*)&snap->data[0], 0) != JD_CONTROL_SUCCESS) {
		}
    }
    else {
		if(do_snapshot((void*)&snap->data[0], 0) != JD_CONTROL_SUCCESS) {

		}
    }
    //IOT_DEBUG(DBG_WIFI | IoT_DBG_INFO,("LED States upload to server: %d\n", snap->data[0]));
    len = serverPacketBuild(g_tcp_sent_buf, TX_BUFFER_SIZE, PT_UPLOAD, jdDev->sessionKey, (const uint8_t*)g_generic_buf, sizeof(lua_colorTemplight_ctrl_t) + sizeof(dataupload_t));
    if((tcp_client_socket >= 0) && (gbCloudConnected == CLOUD_CONNECTED)) {
            send(tcp_client_socket, g_tcp_sent_buf, len, 0);
            tcp_data_sending = 1;
            IoT_DEBUG(JD_SMART_DBG | IoT_DBG_INFO,("Data upload.\n"));
    }
}

void handle_local_request(uint8_t *local_packet, uint32_t packet_len)
{
    packetparam_t pParam = { 0 };
    uint16_t len;
    uint8_t data_to_server = 0;

    len = packetAnalyseV1(&pParam, local_packet, packet_len, g_generic_buf, GENERIC_BUFFER_SIZE);
    if(len < 1) {
//    	IoT_DEBUG(JOYLINK_DBG | IoT_DBG_INFO, ("UDP packet analyze error, packet len(%d)\n", packet_len));
        return;
    }

//    IoT_DEBUG(JOYLINK_DBG | IoT_DBG_INFO,("udp:type:%d,%s\r\n",pParam.type,g_generic_buf));
    switch (pParam.type){
    case PT_SCAN:
		if (pParam.version == 1){
			if(form_dev_json_info == 1) {
				form_scan_json_string(g_dev_json_info);
			}
//			IoT_DEBUG(JOYLINK_DBG | IoT_DBG_INFO,("sent to %s\r\n",g_dev_json_info));
			len = packetBuildV1(g_udp_sent_buf, TX_BUFFER_SIZE, ET_NOTHING, PT_SCAN, NULL, g_dev_json_info, strlen((char *)g_dev_json_info));
			data_to_server = 1;
		}else{
//			IoT_DEBUG(JOYLINK_DBG | IoT_DBG_INFO,("Old version\r\n"));
		}
        break;

    case PT_WRITE_ACCESSKEY:
        if (pParam.version == 1){
            const char *resp = "{\"code\":0,\"msg\":\"success!\"}";
//            IoT_DEBUG(JOYLINK_DBG | IoT_DBG_INFO,("Out: %s\n", resp));
            memset(g_udp_sent_buf, 0x0, TX_BUFFER_SIZE);
            if (packet_len != 0){
                packetAnalyseV1(NULL, local_packet, packet_len, g_generic_buf, GENERIC_BUFFER_SIZE);
            }
            if(parse_json_key((uint8_t*)g_generic_buf + TIMESTAMP_SIZE) == 0) {
                storage_setflash(JD_PARAM_DIR, JD_NET_WORK_FILE,jdDev, sizeof(jddevice_t));
            }
            len = packetBuildV1(g_udp_sent_buf, TX_BUFFER_SIZE, ET_NOTHING, PT_WRITE_ACCESSKEY, NULL, (uint8_t*)resp, strlen(resp));
            data_to_server = 1;
        }else{
//        	IoT_DEBUG(JOYLINK_DBG | IoT_DBG_INFO,("Write->Type:%d, Version:%d\n", pParam.type, pParam.version));
        }
        break;

    case PT_JSONCONTROL:
        if (pParam.version == 1){
//        	IoT_DEBUG(JOYLINK_DBG | IoT_DBG_INFO,("PT_JSONCONTROL"));
        }
        break;
    case PT_SCRIPTCONTROL:
        if (pParam.version == 1){
            control_t *pCtrl = (control_t*)g_generic_buf;
//            IoT_DEBUG(JOYLINK_DBG | IoT_DBG_INFO,("Local Control BizCode: %u, datalen: %u\n", pCtrl->biz_code, len));
            if(pCtrl->biz_code == CONTROL_REQUEST_CODE) {
                if(do_control((void*)&pCtrl->cmd[0], len - sizeof(control_t)) == JD_CONTROL_SUCCESS) {
                		xTimerStart(jd_data_upload_timer,0);
                        data_to_server = 1;
                }
                pCtrl->biz_code = CONTROL_RESP_CODE;
            }
            else if(pCtrl->biz_code == GET_SNAPSHOT_CODE) {
                if(do_snapshot((void*)&pCtrl->cmd[0], len - sizeof(control_t)) == JD_CONTROL_SUCCESS) {
                        data_to_server = 1;
                }
                pCtrl->biz_code = GET_SNAPSHOT_RESP_CODE;
            }
            len = sizeof (lua_colorTemplight_ctrl_t) + sizeof(control_t);
            len = packetBuildV1(g_udp_sent_buf, TX_BUFFER_SIZE, ET_PSKDYAES, PT_SCRIPTCONTROL, jdDev->localkey, g_generic_buf, len);
        }
        break;

    default:
        break;
    }

    if(data_to_server == 1) {
        if(udp_socket >= 0) {
            int16_t	s16Ret;
            uint8_t *pAddr = (uint8_t*)local_packet + RX_BUFFER_SIZE;
//            IoT_DEBUG(JOYLINK_DBG | IoT_DBG_INFO,("Addr:%08x\r\n",((struct sockaddr_in*)pAddr)->sin_addr.s_addr));
            s16Ret = sendto(udp_socket, g_udp_sent_buf, len, 0, (struct sockaddr*)pAddr, sizeof(struct sockaddr));
            if(s16Ret < 0) {
            	IoT_DEBUG(JD_SMART_DBG | IoT_DBG_WARNING,("sendto:jd failed\r\n"));
            }
        }
        data_to_server = 0;
    }
}

void handle_cloud_request(uint8_t *cloud_packet, uint32_t packet_len)
{
	packetparam_t pParam = { 0 };
	uint16_t len;
	uint8_t data_to_server = 0;
	uint8_t biz_code = 0;

	len = serverAnalyse(&pParam, cloud_packet, packet_len, g_generic_buf, GENERIC_BUFFER_SIZE);
	if(len < 1){
//		IOT_DEBUG(DBG_TCPEVENT | IoT_DBG_INFO,("TCP packet analyze error, packet len(%d)\n", packet_len));
		return;
	}
	switch (pParam.type){
	case PT_AUTH:
	{
		auth_resp_t *p = (auth_resp_t*)g_generic_buf;
		memcpy(jdDev->sessionKey, p->session_key, 32);
		jdDev->sessionKey[32] = 0;
		g_server_time = p->timestamp;
		uint32_t msg = MSG_HEART_BEAT;
		xQueueSend( jd_cloud_queue, ( void * )&msg, 10 );

		gbCloudConnected = CLOUD_CONNECTED;
	}
	break;
	case PT_BEAT:
	{
		heartbeat_resp_t *p = (heartbeat_resp_t*)g_generic_buf;
//		IOT_DEBUG(DBG_TCPEVENT | IoT_DBG_INFO,("Beat Resp===>Code=%u,Time:%u!\n", p->code, p->timestamp));
		g_server_time = p->timestamp;
//		hbLostCount = 0;
	}
	break;
	case PT_UPLOAD:
	{
		//dataupload_resp_t *p = (dataupload_resp_t*)g_generic_buf;
		//IOT_DEBUG(DBG_TCPEVENT | IoT_DBG_INFO,("Upload Resp===>Code=%u,Time:%u!\n", p->code, p->timestamp));
	}
	break;
	case PT_OTAU:
	break;
	case PT_OTAU_STATUS_REPORT:
	{
		g_server_time = *((uint32_t*)g_generic_buf);
	}
	break;
	case PT_SERVERCONTROL:
	{
		control_t *pCtrl = (control_t*)g_generic_buf;
//		IOT_DEBUG(DBG_CLOUD_API | IoT_DBG_INFO,("Server Control BizCode: %u, datalen: %u\n", pCtrl->biz_code, len));
		if (pCtrl->biz_code == CONTROL_REQUEST_CODE) {
			if(do_control(&pCtrl->cmd[0], len - sizeof(control_t)) == JD_CONTROL_SUCCESS) {
					data_to_server = 1;
					//start_upload_timer();
			}
			else {
					//debug_info_out(CMD_UART_TIMEOUT, NULL, 0);
			}
	//				IOT_DEBUG(DBG_TCPEVENT | IoT_DBG_INFO,("Light:%u, resp: %u, payload: %u\n", pCtrl->cmd[0], pCtrl->cmd[1], len - sizeof(control_t)));
			pCtrl->biz_code = CONTROL_RESP_CODE;
		}
		else if(pCtrl->biz_code == GET_SNAPSHOT_CODE) {
			if(do_snapshot(&pCtrl->cmd[0], len - sizeof(control_t)) == JD_CONTROL_SUCCESS) {
					data_to_server = 1;
			}
			else {
					//debug_info_out(CMD_UART_TIMEOUT, NULL, 0);
			}
			pCtrl->biz_code = GET_SNAPSHOT_RESP_CODE;
		}
		else if(pCtrl->biz_code == CLOUD_RECIPE_CODE) {
			if(do_recipe(&pCtrl->cmd[0], len - sizeof(control_t)) == JD_CONTROL_SUCCESS) {
					data_to_server = 1;
					//start_upload_timer();
			}
			else {
					//debug_info_out(CMD_UART_TIMEOUT, NULL, 0);
			}
			pCtrl->biz_code = CLOUD_RECIPE_RESP_CODE;
		}
		biz_code = pCtrl->biz_code;
		len = sizeof(lua_colorTemplight_ctrl_t) + sizeof(control_t);
		len = serverPacketBuild(g_tcp_sent_buf, TX_BUFFER_SIZE, PT_SERVERCONTROL, jdDev->sessionKey, g_generic_buf, len);
	}
	break;
	default:
//			IOT_DEBUG(DBG_WIFI | IoT_DBG_INFO,("Unknown Data type (%d) from Server\n", pParam.type));
	break;
	}
	if(data_to_server == 1) {
		if(tcp_client_socket >= 0) {
//			IOT_DEBUG(DBG_TCPEVENT | IoT_DBG_INFO,("tcp sent, %d\n", len));
			send(tcp_client_socket, g_tcp_sent_buf, len, 0);
			if(biz_code == CONTROL_RESP_CODE) {
				uint32_t msg = MSG_DATA_UPLOAD;
				xQueueSend( jd_cloud_queue, ( void * )&msg, 10 );
			}
		}
		data_to_server = 0;
	}
}























