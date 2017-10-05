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
#include "driver/gpio.h"

#include "gateway_manager.h"
#include "network_manager.h"
#include "jd_smart_config.h"
#include "iot_debug.h"


extern Wifi_status_t gWifiStatus;
int local_udp_socket = -1;
uint8_t udp_recv_buf[RX_MAX_BUFFER_SIZE + SOCKET_ADDR_MAX_SIZE];
uint8_t udp_send_Buff[SEND_MAX_BUFF_SIZE+ 20 ];
QueueHandle_t networkInQueue = NULL;

TimerHandle_t deviceInfoReportTimer = NULL;
extern void get_ip_address(uint8_t *ip);
extern void get_wifi_mac_address(uint8_t *mac);

bool isVaildDataPackageRequest(NetworkHeader_t *header)
{
	if (header->sof != NETWORK_MSG_SOF || header->messageType > NETWORK_MSG_TYPE_RST){
		return false;
	}
	return true;
}

bool isSyncDataPackageRequest(NetworkHeader_t *header)
{
	if (header->messageType != NETWORK_MSG_TYPE_CON){
		return false;
	}
	return true;
}

bool sendAcknowledge(NetworkHeader_t *header,struct sockaddr *addr)
{
	memcpy(udp_send_Buff, (uint8_t *)header, sizeof(NetworkHeader_t));
	NetworkMessage_t *message =(NetworkMessage_t *)udp_send_Buff;
	message->networkHeader.messageType = NETWORK_MSG_TYPE_ACK;
	message->networkHeader.packageLength = 0;

	if (sendto(local_udp_socket,udp_send_Buff, sizeof(NetworkHeader_t), 0, addr, sizeof(struct sockaddr)) < 0){
		IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("sendto:ack failed\r\n"));
		return false;
	}
	return true;
}


void handleUdpRequest(uint8_t *buff, int32_t length)
{
	NetworkMessage_t *message= (NetworkMessage_t *)buff;
	struct sockaddr sourceAddr;
	uint8_t *pAddr = buff + RX_MAX_BUFFER_SIZE;
	memcpy((uint8_t*)&sourceAddr, pAddr, sizeof(struct sockaddr));

	uint16_t packageLength = message->contentLength + NETWORK_MESSAGE_LENGTH;
	while(1){
		if (packageLength > length || isVaildDataPackageRequest(&message->networkHeader) == false){
			return;
		}

		if (isSyncDataPackageRequest(&message->networkHeader) == true){
			IoT_DEBUG(LWIP_DBG | IoT_DBG_INFO,("send ACK\n"));
			if (sendAcknowledge(&message->networkHeader,(struct sockaddr*)pAddr)== false){// Send ACK
				IoT_DEBUG(LWIP_DBG | IoT_DBG_INFO,("send ACK error!\n"));
				return;
			}
		}
		message->sourceAddr = sourceAddr;

		xQueueSend(networkInQueue, message, 0);// Add buff to Queue
		if (length > packageLength){
			message = (NetworkMessage_t *)(buff + packageLength);
			packageLength += message->contentLength + NETWORK_MESSAGE_LENGTH;
		}else{
			return;
		}
	}
}


void networkCommTask(void *pvParameter)
{
	int32_t lRetVal = -1;
	socklen_t iAddrSize = sizeof(struct sockaddr_in);
	struct sockaddr_in sAddr={
		.sin_family = AF_INET,
		.sin_addr.s_addr = INADDR_ANY,
		.sin_port = htons(LOCAL_UDP_PORT)
	};
	while(1){
		 if (local_udp_socket >= 0){
			 lRetVal = recvfrom(local_udp_socket, udp_recv_buf, RX_MAX_BUFFER_SIZE, 0,(struct sockaddr *)&sAddr, &iAddrSize);
			 if (lRetVal > 0){
				 //IoT_DEBUG(LWIP_DBG | IoT_DBG_INFO,("udp client:%s,got data:%d\n",inet_ntoa(sAddr.sin_addr),lRetVal));
				uint8_t *pdis = (uint8_t *)&sAddr.sin_addr.s_addr;
				IoT_DEBUG(LWIP_DBG | IoT_DBG_INFO,("udp client:%d.%d.%d.%d port:%d len:%d\n",pdis[0],pdis[1],pdis[2],pdis[3],htons(sAddr.sin_port),lRetVal));
				// printf("RECV:");
				// for(uint16_t i = 0; i < lRetVal;i++){
				// 	printf("%02x ",udp_recv_buf[i]);
				// }
				// printf("\n");
				// pdis[3] = 255;
				// if (sendto(local_udp_socket,udp_recv_buf, lRetVal, 0, (struct sockaddr *)&sAddr, sizeof(struct sockaddr)) < 0){
				// 	IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("sendto:ack failed\r\n"));
				// }else{
				// 	IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("sendto:ack\n"));
				// }

				pdis[3] = 255;
				uint8_t *p = udp_recv_buf + RX_MAX_BUFFER_SIZE;
				memcpy(p, (uint8_t*)&sAddr, sizeof(struct sockaddr));
				handleUdpRequest(udp_recv_buf, lRetVal);
			 }else if (lRetVal == -1){
				 close(local_udp_socket);
				 local_udp_socket = -1;
				 IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("local_udp_socket 5= -1\r\n"));
			 }
		 }
		 vTaskDelay(2/portTICK_PERIOD_MS);
	}
}

void networkMaintainTask(void *pvParameter)
{
	while(1){

		if (gWifiStatus == WIFI_STATUS_GOT_IP && local_udp_socket == -1){
			struct sockaddr_in server_addr;
			memset(&server_addr, 0, sizeof(server_addr));
			server_addr.sin_family = AF_INET;
			server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			server_addr.sin_port = htons(LOCAL_UDP_PORT);
			local_udp_socket = socket(AF_INET,SOCK_DGRAM,0);
			if( bind(local_udp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))< 0){
				close(local_udp_socket);
				local_udp_socket = -1;
			}
		}

		NetworkMessage_t message;
		if(xQueueReceive(networkInQueue, &message, 0)){
			GatewayManager_HandleNetworkRequest((void *)&message);
		}
		vTaskDelay(5/portTICK_PERIOD_MS);
		
	}

}


void loadReportInfo(NetworkMessage_t *message)
{
	struct sockaddr_in sAddr={
		.sin_family = AF_INET,
		.sin_addr.s_addr = INADDR_ANY,
		.sin_port = htons(LOCAL_UDP_PORT + 1)
	};

	message->networkHeader.sof = NETWORK_MSG_SOF;
	message->networkHeader.magicCode = rand();
	message->networkHeader.messageType = NETWORK_MSG_TYPE_NON;
	message->networkHeader.sumFrames = 1;
	message->networkHeader.currFrameNo = 1;
	message->networkHeader.encrypted = 0;
	message->networkHeader.packageLength = 4;

	message->packTypeId = MSG_TYPE_ID_CONNECT;
	message->command = CONN_CMD_QUERY_NET_INFO;
	message->contentLength = 0;

	uint8_t ip[4];
	get_ip_address(ip);
	for (uint8_t i = 0; i < 4; i++) message->GetNetworkConnectInfo.ip[i] = ip[i];

	ip[3] = 255;
	sAddr.sin_addr.s_addr =  *((uint32_t *)ip);
	struct sockaddr sourceAddr;
	memcpy((uint8_t*)&sourceAddr, (uint8_t *)&sAddr, sizeof(struct sockaddr));
	sourceAddr.sa_len = sizeof(struct sockaddr);
	message->sourceAddr = sourceAddr;

}


void deviceInfoReportTimerCallback( TimerHandle_t xTimer )
{
	static uint8_t reportCount = 0;
	NetworkMessage_t message;
	if (gWifiStatus == WIFI_STATUS_GOT_IP && local_udp_socket != -1){
		loadReportInfo(&message);
		xQueueSend(networkInQueue, &message, 0);// Add buff to Queue
		reportCount++;
		if (reportCount >= 3){
			xTimerStop(deviceInfoReportTimer,0);
			reportCount = 0;
		}
	}
}

void NetworkManager_Init(void)
{
	deviceInfoReportTimer = xTimerCreate("ReportTimer", 1000 / portTICK_PERIOD_MS, pdTRUE, 0, deviceInfoReportTimerCallback );
	xTimerStart(deviceInfoReportTimer,0);

	networkInQueue = xQueueCreate( 5, sizeof(NetworkMessage_t) );
	xTaskCreate(&networkMaintainTask, "LOCAL_UDP", 4096, NULL, tskIDLE_PRIORITY+3, NULL);
	xTaskCreate(&networkCommTask,  	  "UDP_SOCK", 4096, NULL, tskIDLE_PRIORITY+3, NULL);
}

