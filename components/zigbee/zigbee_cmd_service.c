#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "crc.h"

#include "queue_uart.h"

#include "zigbee_header.h"
#include "zigbee_cmd_service.h"
#include "iot_debug.h"

//typedef enum PACK{
//    COMM_STATE_MACHINE_SOF        = 0,
//    COMM_STATE_MACHINE_SECURIZTY  = 1,
//    COMM_STATE_MACHINE_LENGTH     = 2,
//    COMM_STATE_MACHINE_DATAPACK   = 3,
//    COMM_STATE_MACHINE_PEC        = 4
//}CommStateMachine_t;
//
//CommStateMachine_t commStateMachine = COMM_STATE_MACHINE_SOF;

QueueHandle_t uart_queue;
uint8_t uart_rx_buff[RX_BUFF_SIZE];

xQueueHandle zigbee_cmd_queue = NULL;
xQueueHandle zigbee_cmd_result_queue = NULL;

TimerHandle_t recv_timeout_timer = NULL;

void zigbee_data_recv_task(void *pvParameter);


bool check_packet_pec(uint8_t *packet_buf, uint8_t packet_size) //sensor report packet
{
	if(packet_buf[packet_size-1] != calculate_crc8(packet_buf, packet_size-1)){
		IoT_DEBUG(ZIGBEE_DBG | IoT_DBG_WARNING, ("Invalid CRC\r\n"));
		return false;
	}

	return true;
}

bool check_report_packet(uint8_t *packet_buf, uint8_t packet_size) //sensor report packet
{
	if (packet_buf == NULL){
		return false;
	}

	if(   packet_buf[3] == (PACK_TYPE_ZGP_SENSOR_REPORT_MANAGEMENT | 0x80)
	   || packet_buf[3] == (PACK_TYPE_SENSOR_REPORT_MANAGEMENT     | 0x80) ){
		return true;
	}

	return false;
}

/* check header, length, crc of received packet */
bool check_response_packet(AppCmdDescriptor_t *pCmdDesc,Cmd_Response_t *rsp)
{
	rsp->status = COMM_FALSE;
	bool isResponsePack = true;
	if(rsp->responsePayload == NULL) {
		return false;
	}

	if (pCmdDesc->packType != (rsp->responsePayload[3] & 0x7F)){
		return false;
	}

	switch(pCmdDesc->packType){
	case PACK_TYPE_GATEWAY_MANAGEMENT:
		if (pCmdDesc->GatewayMgmt.commandId != rsp->responsePayload[4]){
			isResponsePack = false;
		}
		rsp->status = rsp->responsePayload[5];
		break;
	case PACK_TYPE_ORGANIZATION_DESCRIPTION:
		if (pCmdDesc->OgnzMgmt.commandId != rsp->responsePayload[4]){
			isResponsePack = false;
		}
		if (   pCmdDesc->OgnzMgmt.commandId == OGNZ_MGMT_GET_DEVICE_LIST
			|| pCmdDesc->OgnzMgmt.commandId == OGNZ_MGMT_GET_GROUP_LIST
			|| pCmdDesc->OgnzMgmt.commandId == OGNZ_MGMT_GET_SCENE_LIST
			|| pCmdDesc->OgnzMgmt.commandId == OGNZ_MGMT_GET_ZGP_DEVICE_LIST){
			rsp->status = rsp->responsePayload[5];
		}else if(  pCmdDesc->OgnzMgmt.commandId == OGNZ_MGMT_GET_DEVICE_INFO){
			rsp->status = rsp->responsePayload[7];
		}else if(pCmdDesc->OgnzMgmt.commandId == OGNZ_MGMT_GET_GROUP_INFO || pCmdDesc->OgnzMgmt.commandId == OGNZ_MGMT_GET_SCENE_INFO){
			rsp->status = rsp->responsePayload[6];
		}
		break;
	case PACK_TYPE_DEVICE_NETWORK_MANAGEMENT:
		if (pCmdDesc->NwkMgmt.commandId != rsp->responsePayload[6]){
			isResponsePack = false;
		}
		if (  (pCmdDesc->NwkMgmt.commandId == NWK_MGMT_RST_GATEWAY_TO_FN)
			||(pCmdDesc->NwkMgmt.commandId == NWK_MGMT_TOUCHLINK)
			||(pCmdDesc->NwkMgmt.commandId == NWK_MGMT_CLASSIC_JOIN_PERMIT_JOIN)
			||(pCmdDesc->NwkMgmt.commandId == NWK_MGMT_CLASSIC_JOIN_RST_LIGHT_TO_FN)
			||(pCmdDesc->NwkMgmt.commandId == NWK_MGMT_SET_PHY_CHANNEL)){
			rsp->status = rsp->responsePayload[7];
		}else if (pCmdDesc->NwkMgmt.commandId == NWK_MGMT_ZGP_COMMISSIONING){
			rsp->status = rsp->responsePayload[8];
		}
		break;
	case PACK_TYPE_DEVICE_GROUP_SCENE_MANAGEMENT:
	case PACK_TYPE_DEVICE_CONTROL:
	case PACK_TYPE_DEVICE_STATE_MANAGEMENT:
		if (  ( pCmdDesc->GroupSceneMgmt.clusterId != (uint16_t)rsp->responsePayload[6] +((uint16_t)rsp->responsePayload[7] << 8) )
			||( pCmdDesc->GroupSceneMgmt.addrMode  != rsp->responsePayload[8])
			||( pCmdDesc->GroupSceneMgmt.address   != rsp->responsePayload[9])
			||( pCmdDesc->GroupSceneMgmt.endpoint  != rsp->responsePayload[10])
			||( pCmdDesc->GroupSceneMgmt.commandId != rsp->responsePayload[11])){
			isResponsePack = false;
		}
		rsp->status = rsp->responsePayload[12];
		break;
	default:
		isResponsePack = false;
		break;
	}

	return isResponsePack;
}

//void recv_timeout_callback( TimerHandle_t xTimer )
//{
//	commStateMachine = COMM_STATE_MACHINE_SOF;
//}

void zigbee_data_recv_task(void *pvParameter)
{
	uart_event_t event;
	int32_t  rx_length;
	Cmd_Response_t resp;
	AppCmdDescriptor_t *pCmdDesc;
	uint8_t *pbuf = NULL;
	uint8_t pkt_len;

//	recv_timeout_timer = xTimerCreate( "recv_timeout_timer", 50/portTICK_PERIOD_MS, pdFALSE, 0, recv_timeout_callback );
//	xTimerStart(recv_timeout_timer,0);

	while(1){
		if (xQueueReceive(uart_queue, (void * )&event, (portTickType)portMAX_DELAY)){
			if (event.type == UART_DATA || event.type == UART_BUFFER_FULL || event.type == UART_FIFO_OVF){
				rx_length = QueueUart_ReadPacket(uart_rx_buff);
				pbuf = uart_rx_buff;
				pkt_len = pbuf[2];

				while(rx_length > 0){
					if (pbuf[0] != COMM_SOF_DATA)	break;
					if(pkt_len > rx_length -4)		break;
					resp.responseLength  = pkt_len + 4;
					resp.responsePayload = pbuf;
					if(false == check_packet_pec(resp.responsePayload, resp.responseLength) ){
						IoT_DEBUG(ZIGBEE_DBG | IoT_DBG_INFO, ("UART packet PEC error!\r\n"));
						break;
					}
					if (check_report_packet(resp.responsePayload, resp.responseLength)){
						IoT_DEBUG(ZIGBEE_DBG | IoT_DBG_INFO, ("Sensor report packet.\r\n"));
						//Handler report info
					}else{
						if(xQueuePeek(zigbee_cmd_queue,(void*)&pCmdDesc, 0)){
							if(check_response_packet(pCmdDesc,&resp)){
								if(xQueueReceive(zigbee_cmd_queue,(void*)&pCmdDesc, 0)){
									if (pCmdDesc->cmd_response_callback){
										pCmdDesc->cmd_response_callback(&resp,pCmdDesc->sourceAddr,pCmdDesc->networkHeader, pCmdDesc->packTypeId,pCmdDesc->command);
									}
									xQueueSend(zigbee_cmd_result_queue, &resp.status, 10);
								}else{
									//report command or time out response
									IoT_DEBUG(ZIGBEE_DBG | IoT_DBG_INFO, ("report command or time out response\r\n"));
								}
							}
						}else{
							IoT_DEBUG(ZIGBEE_DBG | IoT_DBG_INFO, ("no command or time out response\r\n"));
						}
					}
					rx_length = rx_length - pkt_len -4;
					pbuf = pbuf + pkt_len + 4;
				}
			}
		}
	}
}

/* generate zigbee packet */
uint8_t generate_packet(AppCmdDescriptor_t* pCmdDesc)
{
	uint8_t length = HEAD_SIZE_GENERIC_HEADER + pCmdDesc->length + 1;
	if (length > MAX_PAYLOAD_SIZE){
		IoT_DEBUG(ZIGBEE_DBG | IoT_DBG_WARNING,("length error:%d\n",length));
		return 0;
	}
	pCmdDesc->sof 		= COMM_SOF_DATA;
	pCmdDesc->encrypt 	= COMM_SECURIZTY_PLAIN;
	pCmdDesc->payload[length-1] = calculate_crc8(pCmdDesc->payload,length -1);
	return length;
}


/* send packet to zigbee serial send task and wait for result */
bool ZigbeeCmdService_ProcessPacket(AppCmdDescriptor_t* pCmdDesc)
{
	uint8_t packet_len;
	uint8_t result;
	uint8_t retry_cnt=0;
	AppCmdDescriptor_t *pCmd;
	Cmd_Response_t resp;
	packet_len = generate_packet(pCmdDesc);
	if (packet_len == 0){
		return false;
	}

	while(retry_cnt < ZIGBEE_SEND_RETRY_TIMES){
		xQueueSend(zigbee_cmd_queue, &pCmdDesc,10);
		if(QueueUart_SendPacket(pCmdDesc->payload, packet_len)){
			if(xQueueReceive(zigbee_cmd_result_queue, &result, pCmdDesc->timeout)){
				if(result == COMM_SUCCESS){
					return true;
				}
			}else {
				xQueueReceive(zigbee_cmd_queue, (void*)&pCmd, 0);//To remove the command from the queue
				resp.status = COMM_FALSE;
				resp.responseLength = pCmd->length + 4;
				resp.responsePayload =pCmd->payload;
				if (pCmd->cmd_response_callback){
					pCmd->cmd_response_callback(&resp,pCmd->sourceAddr, pCmdDesc->networkHeader,pCmd->packTypeId,pCmd->command);
				}
				IoT_DEBUG(ZIGBEE_DBG | IoT_DBG_WARNING,("UART received timeout!\n"));
			}
		}else {
			xQueueReceive(zigbee_cmd_queue, (void*)&pCmd, 0);//To remove the command from the queue
			resp.status = COMM_FALSE;
			resp.responseLength = pCmd->length + 4;
			resp.responsePayload =pCmd->payload;
			if (pCmd->cmd_response_callback){
				pCmd->cmd_response_callback(&resp,pCmd->sourceAddr, pCmdDesc->networkHeader,pCmd->packTypeId,pCmd->command);
			}
			IoT_DEBUG(ZIGBEE_DBG | IoT_DBG_WARNING,("UART sent error!\n"));
		}
		retry_cnt++;
	}
	return false;
}

void ZigbeeCmdService_Init(void)
{
	zigbee_cmd_queue = xQueueCreate(ZIGBEE_CMD_QUEUE_LEN, sizeof(void *));
	zigbee_cmd_result_queue = xQueueCreate(ZIGBEE_CMD_RESULT_QUEUE_LEN, sizeof(uint8_t));
	QueueUart_Init(&uart_queue);
	xTaskCreate(&zigbee_data_recv_task, "ZB_RECV", 2048, NULL, tskIDLE_PRIORITY+4, NULL);
}


void ZigbeeCmdService_ResetModule(void)
{
	gpio_set_level(GPIO_NUM_21, 0);
	vTaskDelay(10/portTICK_PERIOD_MS);
	gpio_set_level(GPIO_NUM_21, 1);
	vTaskDelay(500/portTICK_PERIOD_MS);
}


