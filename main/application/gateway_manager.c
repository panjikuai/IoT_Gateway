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

#include "jd_smart_config.h"
#include "zigbee_header.h"
#include "zigbee_cmd_service.h"
#include "cmd_descriptor.h"
#include "iot_debug.h"
#include "gateway_manager.h"
#include "network_manager.h"

extern int local_udp_socket;
extern uint8_t udp_send_Buff[SEND_MAX_BUFF_SIZE+ 20 ];
extern void get_ip_address(uint8_t *ip);
extern void get_wifi_mac_address(uint8_t *mac);

zigbee_ognz_info_t		g_zigbee_ognz_info;

QueueHandle_t zigbee_queue_cmd_in = NULL;
QueueHandle_t zigbee_queue_cmd_out = NULL;
QueueHandle_t zigbee_info_queue_query_start = NULL;
TimerHandle_t zigbee_info_query_timer = NULL;



zigbee_gateway_info_t *GatewayManager_GetGatewayInfo(void)
{
	return &g_zigbee_ognz_info.gateway_info;
}

void zigbee_ognz_info_init(void)
{
	memset((uint8_t *)&g_zigbee_ognz_info.gateway_info, 0, sizeof(zigbee_gateway_info_t));
	memset((uint8_t *)&g_zigbee_ognz_info.group_list, 	0, sizeof(zigbee_group_list_t));
	memset((uint8_t *)g_zigbee_ognz_info.group_info, 	0, sizeof(zigbee_group_info_t)*MAXIMUM_GROUPS_NUM);
	memset((uint8_t *)&g_zigbee_ognz_info.scene_list, 	0, sizeof(zigbee_scene_list_t));
	memset((uint8_t *)g_zigbee_ognz_info.scene_info, 	0, sizeof(zigbee_scene_info_t)*MAXIMUM_SCENES_NUM);
	memset((uint8_t *)&g_zigbee_ognz_info.device_list, 	0, sizeof(zigbee_device_list_t));
	memset((uint8_t *)g_zigbee_ognz_info.device_info, 	0, sizeof(zigbee_device_info_t)*MAXIMUM_DEVICES_NUM);
	g_zigbee_ognz_info.head = NULL;
}

void add_node_to_List(device_info_t *deviceInfo, uint32_t timeout)
{
	if(g_zigbee_ognz_info.head == NULL){
		g_zigbee_ognz_info.head = pvPortMalloc(MEM_ALIGN_SIZE(sizeof(zigbee_node_info_t)));
		memset(g_zigbee_ognz_info.head, 0x0, MEM_ALIGN_SIZE(sizeof(zigbee_node_info_t)));
		g_zigbee_ognz_info.head->status = STATUS_ONLINE;
		memcpy((uint8_t *)&g_zigbee_ognz_info.head->device_info,(uint8_t *)deviceInfo,sizeof(device_info_t));
		IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("New ADD: %02x, DID:%04x,EP:%02x\n",g_zigbee_ognz_info.head->device_info.address,g_zigbee_ognz_info.head->device_info.deviceId,g_zigbee_ognz_info.head->device_info.endpoint));
		g_zigbee_ognz_info.head->updateTimeout = timeout;
		g_zigbee_ognz_info.head->next = NULL;
	}else{
		zigbee_node_info_t *pNodePrv = g_zigbee_ognz_info.head;
		zigbee_node_info_t *pNodeCurr = g_zigbee_ognz_info.head->next;

		while(pNodeCurr){
			pNodePrv = pNodeCurr;
			pNodeCurr = pNodeCurr->next;
		}
		zigbee_node_info_t *node = pvPortMalloc(MEM_ALIGN_SIZE(sizeof(zigbee_node_info_t)));
		memset(node, 0x0, MEM_ALIGN_SIZE(sizeof(zigbee_node_info_t)));
		node->status = STATUS_ONLINE;
		memcpy((uint8_t *)&node->device_info,(uint8_t *)deviceInfo,sizeof(device_info_t));
		IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("New ADD: %02x, DID:%04x,EP:%02x\n",node->device_info.address,node->device_info.deviceId,node->device_info.endpoint));
		node->updateTimeout = timeout;
		node->next = NULL;
		pNodePrv->next = node;
	}
}

bool delete_node_from_List(device_info_t *deviceInfo)
{
	if (g_zigbee_ognz_info.head == NULL){
		return false;
	}
	if (g_zigbee_ognz_info.head->next == NULL){
		if(deviceInfo->address == g_zigbee_ognz_info.head->device_info.address && deviceInfo->endpoint == g_zigbee_ognz_info.head->device_info.endpoint){
			vPortFree(g_zigbee_ognz_info.head);
			g_zigbee_ognz_info.head = NULL;
			return true;
		}else{
			return false;
		}
	}

	zigbee_node_info_t *pNodePrv = g_zigbee_ognz_info.head;
	zigbee_node_info_t *pNodeCurr = g_zigbee_ognz_info.head->next;

	while(pNodeCurr){
		if(deviceInfo->address == pNodeCurr->device_info.address && deviceInfo->endpoint == pNodeCurr->device_info.endpoint){
			pNodePrv->next = pNodeCurr->next;
			vPortFree(pNodeCurr);
			return true;
		}
		pNodePrv = pNodeCurr;
		pNodeCurr = pNodeCurr->next;
	}
	return false;
}

zigbee_node_info_t *query_node_from_List(device_info_t *deviceInfo)
{
	zigbee_node_info_t *pNode = g_zigbee_ognz_info.head;

	while(pNode){
		if(deviceInfo->address == pNode->device_info.address && deviceInfo->endpoint == pNode->device_info.endpoint){
			return pNode;
		}else{
			pNode = pNode->next;
		}
	}
	return NULL;
}

zigbee_node_info_t *query_node_by_index(uint8_t index)
{
	zigbee_node_info_t *pNode = g_zigbee_ognz_info.head;

	uint8_t count = 0;
	while(pNode){
		if (count == index){
			return pNode;
		}else{
			count++;
			pNode = pNode->next;
		}
	}
	return NULL;
}

uint16_t calculate_group_info_size(zigbee_group_list_t *grouplist)
{
	uint16_t size = 0;
	for (uint8_t i = 0; i < MAXIMUM_GROUPS_NUM; i++){
		if(grouplist->list & (1ull << i)){
			size += sizeof(zigbee_group_info_t);
		}
	}
	return size;
}

uint16_t calculate_scene_info_size(zigbee_scene_list_t *scenelist)
{
	uint16_t size = 0;
	for (uint8_t i = 0; i < MAXIMUM_SCENES_NUM; i++){
		if(scenelist->list & (1ull << i)){
			size += sizeof(zigbee_scene_info_t);
		}
	}
	return size;
}

uint16_t calculate_device_info_size(zigbee_device_list_t *devicelist)
{
	uint16_t size = 0;
	uint64_t list = devicelist->deviceList[0];
	for (uint8_t i = 0; i < MAXIMUM_DEVICES_NUM; i++){
		if (i == 64){
			list = devicelist->deviceList[1];
		}
		if(list & (1ull << (i%64))){
			size += sizeof(uint8_t)+ g_zigbee_ognz_info.device_info[i].endpointNum * sizeof(endpoint_info_t);
		}
	}
	return size;
}

uint16_t calculate_device_state_size(zigbee_node_info_t *device_state)
{
	uint16_t size = 0;
	uint16_t deviceId;
	zigbee_node_info_t *pNext = device_state;

	while(pNext){
		size += sizeof(uint8_t);
		deviceId = pNext->device_info.deviceId;
		size += sizeof(device_info_t);
		if(deviceId == ZLL_ON_OFF_LIGHT_DEVICE_ID){
			size += sizeof(on_off_light_t);
		}else if (deviceId == ZLL_DIMMABLE_LIGHT_DEVICE_ID){
			size += sizeof(dimmable_light_t);
		}else if (deviceId == ZLL_TEMPERATURE_COLOR_LIGHT_DEVICE_ID){
			size += sizeof(color_temperature_light_t);
		}else if (deviceId == ZLL_COLOR_LIGHT_DEVICE_ID){
			size += sizeof(color_light_t);
		}else if (deviceId == ZLL_EXTENDED_COLOR_LIGHT_DEVICE_ID){
			size += sizeof(extended_color_light_t);
		}else{
			IoT_DEBUG(ZIGBEE_DBG | IoT_DBG_WARNING,("unknown deviceId = %04x\r\n",deviceId));
		}
		pNext= pNext->next;
	}

	return size;
}

//return buff length
uint16_t get_device_state_by_block_index(uint8_t *buff, uint8_t blockIndex)
{
	uint16_t size = 0;
	uint8_t deviceCount = 0;
	uint16_t deviceIndex = (uint16_t)blockIndex * 64;// 64 device state is 1 block
	uint16_t deviceQuantity = (uint16_t)(blockIndex + 1) * 64;// 64 device state is 1 block

	uint8_t *device_state_info_length = NULL;
	device_info_t 				*device_info;
	on_off_light_t 				*on_off_light;
	dimmable_light_t			*dimmable_light;
	color_temperature_light_t	*color_temperature_light;
	color_light_t				*color_light;
	extended_color_light_t		*extended_color_light;
	uint16_t deviceId;
	zigbee_node_info_t			*pNext;
	pNext = g_zigbee_ognz_info.head;

	uint16_t *start_endpoint_index = (uint16_t *)buff;
	*start_endpoint_index = blockIndex * 64;
	size +=sizeof(uint16_t);

	while(pNext){
		if (deviceCount >= deviceIndex){
			if (deviceCount < deviceQuantity){
				buff[size] = pNext->status;
				size += sizeof(uint8_t);
				device_info = (device_info_t *)(size + buff);
				memcpy(device_info, &pNext->device_info,sizeof(device_info_t));
				size += sizeof(device_info_t);
				
				//device state info length
				device_state_info_length = (uint8_t *)(size + buff);
				size += sizeof(uint8_t);

				deviceId = pNext->device_info.deviceId;
				if(deviceId == ZLL_ON_OFF_LIGHT_DEVICE_ID){
					*device_state_info_length = sizeof(on_off_light_t);
					on_off_light = (on_off_light_t *)(size + buff);
					memcpy(on_off_light, &pNext->on_off_light,sizeof(on_off_light_t));
					size += sizeof(on_off_light_t);
				}else if (deviceId == ZLL_DIMMABLE_LIGHT_DEVICE_ID){
					*device_state_info_length = sizeof(dimmable_light_t);
					dimmable_light = (dimmable_light_t *)(size + buff);
					memcpy(dimmable_light, &pNext->dimmable_light,sizeof(dimmable_light_t));
					size += sizeof(dimmable_light_t);
				}else if (deviceId == ZLL_TEMPERATURE_COLOR_LIGHT_DEVICE_ID){
					*device_state_info_length = sizeof(color_temperature_light_t);
					color_temperature_light = (color_temperature_light_t *)(size + buff);
					memcpy(color_temperature_light, &pNext->color_temperature_light,sizeof(color_temperature_light_t));
					size += sizeof(color_temperature_light_t);
				}else if (deviceId == ZLL_COLOR_LIGHT_DEVICE_ID){
					*device_state_info_length = sizeof(color_light_t);
					color_light = (color_light_t *)(size + buff);
					memcpy(color_light, &pNext->color_light,sizeof(color_light_t));
					size += sizeof(color_light_t);
				}else if (deviceId == ZLL_EXTENDED_COLOR_LIGHT_DEVICE_ID){
					*device_state_info_length = sizeof(extended_color_light_t);
					extended_color_light = (extended_color_light_t *)(size + buff);
					memcpy(extended_color_light, &pNext->extended_color_light,sizeof(extended_color_light_t));
					size += sizeof(extended_color_light_t);
				}else{
					*device_state_info_length = 0;
					IoT_DEBUG(ZIGBEE_DBG | IoT_DBG_WARNING,("unknown deviceId = %04x\r\n",deviceId));
				}
			}else{
				break;
			}
		}
		pNext = pNext->next;
		deviceCount++;
	}
	return size;
}

void process_gateway_connect_packet(network_message_t *msg)
{
	network_message_t *message =(network_message_t *)udp_send_Buff;
	memcpy((uint8_t *)&message->networkHeader, (uint8_t *)&msg->networkHeader, sizeof(network_header_t));
	message->networkHeader.messageType = NETWORK_MSG_TYPE_NON;
	message->networkHeader.packageLength = 4 + sizeof(GetNetworkConnectInfo_t);
	message->packTypeId = msg->packTypeId | CMD_RSP;
	message->command = CONN_CMD_QUERY_NET_INFO;
	message->contentLength = sizeof(GetNetworkConnectInfo_t);

	get_wifi_mac_address(message->GetNetworkConnectInfo.mac);
	get_ip_address(message->GetNetworkConnectInfo.ip);
	// uint8_t mac[6];
	// get_wifi_mac_address(mac);
	// for (uint8_t i = 0; i < 6; i++) message->GetNetworkConnectInfo.mac[i] = mac[i];
	// uint8_t ip[4];
	// get_ip_address(ip);
	// for (uint8_t i = 0; i < 4; i++) message->GetNetworkConnectInfo.ip[i] = ip[i];

	memcpy(&message->GetNetworkConnectInfo.zigbee_gateway_info,&g_zigbee_ognz_info.gateway_info, sizeof(zigbee_gateway_info_t));

	struct sockaddr sourceAddr= msg->sourceAddr;

	uint8_t *pdis = (uint8_t *)&sourceAddr;
	for (uint8_t i = 0; i < sizeof(struct sockaddr); i++){
		printf("%d ",pdis[i]);
	}
	printf("\n ");
	if(local_udp_socket >= 0){
		if (sendto(local_udp_socket,udp_send_Buff, NETWORK_MESSAGE_LENGTH + message->contentLength, 0, &sourceAddr, sizeof(struct sockaddr)) < 0){
			IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("sendto:ognz failed 66\r\n"));
			return false;
		}
	}else{
		IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("local_udp_socket 1 = -5\r\n"));
		return false;
	}
	return true;
}

bool process_ognz_struct_packet(network_message_t *msg)
{
	uint16_t size;
	zigbee_group_list_t   *zigbee_group_list;
	zigbee_group_info_t	  *zigbee_group_info;
	zigbee_scene_list_t	  *zigbee_scene_list;
	zigbee_scene_info_t	  *zigbee_scene_info;
	zigbee_device_list_t  *zigbee_device_list;

	network_message_t *message = (network_message_t *)udp_send_Buff;
	memcpy(&message->networkHeader,&msg->networkHeader, sizeof(network_header_t));

	message->networkHeader.messageType = NETWORK_MSG_TYPE_NON;
	message->networkHeader.sumFrames  = 1;
	message->networkHeader.currFrameNo = 1;
	
	message->packTypeId = msg->packTypeId | CMD_RSP; 
	message->command = msg->command;

	message->contentLength =  sizeof(GetOgnzStruct_t)
							+ sizeof(zigbee_group_list_t)  + calculate_group_info_size(&g_zigbee_ognz_info.group_list)
							+ sizeof(zigbee_scene_list_t)  + calculate_scene_info_size(&g_zigbee_ognz_info.scene_list)
							+ sizeof(zigbee_device_list_t);
	message->networkHeader.packageLength = message->contentLength + 4;
	message->GetOgnzStruct.blockIndex = msg->GetOgnzStruct.blockIndex;

	size = NETWORK_MESSAGE_LENGTH + sizeof(GetOgnzStruct_t);
	//fill group list
	zigbee_group_list = (zigbee_group_list_t *)(udp_send_Buff + size);
	memcpy(zigbee_group_list,&g_zigbee_ognz_info.group_list.list, sizeof(zigbee_group_list_t));
	// fill group info
	uint32_t groupList = g_zigbee_ognz_info.group_list.list;
	size += sizeof(zigbee_group_list_t);
	for (uint8_t i = 0; i < MAXIMUM_GROUPS_NUM ;i++){
		if(groupList & (1ull << i)){
			zigbee_group_info = (zigbee_group_info_t *)(size + udp_send_Buff);
			memcpy(zigbee_group_info,&g_zigbee_ognz_info.group_info[i], sizeof(zigbee_group_info_t));
			size += sizeof(zigbee_group_info_t);
		}
	}
	//fill scene list
	zigbee_scene_list = (zigbee_scene_list_t *)(size + udp_send_Buff);
	memcpy(zigbee_scene_list,&g_zigbee_ognz_info.scene_list.list, sizeof(zigbee_scene_list_t));
	size += sizeof(zigbee_scene_list_t);
	// fill scene info
	uint32_t sceneList = g_zigbee_ognz_info.scene_list.list;
	for (uint8_t i = 0; i < MAXIMUM_SCENES_NUM ;i++){
		if(sceneList & (1ull << i)){
			zigbee_scene_info = (zigbee_scene_info_t *)(size + udp_send_Buff);
			memcpy(zigbee_scene_info,&g_zigbee_ognz_info.scene_info[i], sizeof(zigbee_scene_info_t));
			size += sizeof(zigbee_scene_info_t);
		}
	}
	// fill device list
	zigbee_device_list = (zigbee_device_list_t *)(size + udp_send_Buff);
	memcpy(zigbee_device_list,&g_zigbee_ognz_info.device_list, sizeof(zigbee_device_list_t));
	size += sizeof(zigbee_device_list_t);

	struct sockaddr sourceAddr= msg->sourceAddr;


	uint8_t *pdis = (uint8_t *)&msg->sourceAddr;
	for (uint8_t i = 0; i < sizeof(struct sockaddr); i++){
		printf("%d ",pdis[i]);
	}
	printf("\n ");

	if(local_udp_socket >= 0){
		if (sendto(local_udp_socket,udp_send_Buff, size, 0, &sourceAddr, sizeof(struct sockaddr)) < 0){
			IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("sendto:ognz failed %d\r\n",size));
			return false;
		}
	}else{
		IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("local_udp_socket 1 = -1\r\n"));
		return false;
	}
	return true;
}

bool process_devices_info_packet(network_message_t *msg)
{
	//rand()%65536;
	uint16_t size;
	zigbee_device_list_t  *zigbee_device_list;
	zigbee_device_info_t  *zigbee_device_info;
	uint16_t *device_info_start_index;

	network_message_t *message = (network_message_t *)udp_send_Buff;
	memcpy(&message->networkHeader,&msg->networkHeader, sizeof(network_header_t));

	message->networkHeader.messageType = NETWORK_MSG_TYPE_NON;
	message->networkHeader.sumFrames  = 1;
	message->networkHeader.currFrameNo = 1;
	
	message->packTypeId |= CMD_RSP; 
	message->command = msg->command;
	message->GetDevicesInfo.blockIndex= msg->GetDevicesInfo.blockIndex;

	size = NETWORK_MESSAGE_LENGTH + sizeof(GetDevicesInfo_t);

	device_info_start_index = (uint16_t *)(size + udp_send_Buff);
	*device_info_start_index = msg->GetDevicesInfo.blockIndex *128;
	size +=sizeof(uint16_t);

	zigbee_device_list = (zigbee_device_list_t *)(size + udp_send_Buff);
	memcpy(zigbee_device_list,&g_zigbee_ognz_info.device_list, sizeof(zigbee_device_list_t));
	size += sizeof(zigbee_device_list_t);

	uint64_t devicelist = g_zigbee_ognz_info.device_list.deviceList[0];
	for (uint8_t i = 0; i < MAXIMUM_DEVICES_NUM; i++){
		if (i == 64){
			devicelist = g_zigbee_ognz_info.device_list.deviceList[1];
		}
		if (devicelist & (1ull << (i%64))){
			zigbee_device_info = (zigbee_device_info_t *)(size + udp_send_Buff);
			uint8_t deviceInfoLength =sizeof(uint16_t) + sizeof(uint8_t)+ g_zigbee_ognz_info.device_info[i].endpointNum * sizeof(endpoint_info_t);
			memcpy(zigbee_device_info,&g_zigbee_ognz_info.device_info[i], deviceInfoLength);
			size += deviceInfoLength;
		}
	}

	message->contentLength = size - NETWORK_MESSAGE_LENGTH;
	message->networkHeader.packageLength = message->contentLength + 4;

	struct sockaddr sourceAddr= msg->sourceAddr;

	uint8_t *pdis = (uint8_t *)&msg->sourceAddr;
	for (uint8_t i = 0; i < sizeof(struct sockaddr); i++){
		printf("%d ",pdis[i]);
	}
	printf("\n ");

	if(local_udp_socket >= 0){
		if (sendto(local_udp_socket,udp_send_Buff, size, 0, &sourceAddr, sizeof(struct sockaddr)) < 0){
			IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("sendto:device info failed %d\r\n",size));
			return false;
		}
	}else{
		IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("local_udp_socket 2= -1\r\n"));
		return false;
	}
	return true;
}

bool process_device_state_packet(network_message_t *msg)
{
	uint16_t size;

	network_message_t *message = (network_message_t *)udp_send_Buff;
	memcpy(&message->networkHeader,&msg->networkHeader, sizeof(network_header_t));
	message->networkHeader.messageType = NETWORK_MSG_TYPE_NON;
	message->networkHeader.sumFrames  = 1;
	message->networkHeader.currFrameNo = 1;
	
	message->packTypeId |= CMD_RSP; 
	message->command = msg->command;
	size = NETWORK_MESSAGE_LENGTH + 4;

	message->contentLength = get_device_state_by_block_index((udp_send_Buff + size), message->GetDevicesState.blockIndex);
	message->networkHeader.packageLength = message->contentLength + 4;
	size += message->contentLength;

	struct sockaddr sourceAddr= msg->sourceAddr;
	if(local_udp_socket >= 0){
		if (sendto(local_udp_socket,udp_send_Buff, size, 0, &sourceAddr, sizeof(struct sockaddr)) < 0){
			IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("sendto:device state failed\r\n"));
			return false;
		}
	}else{
		IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("local_udp_socket 3= -1\r\n"));
		return false;
	}
	return true;
}


void device_state_rsp_callback(Cmd_Response_t *resp, struct sockaddr addr,network_header_t header,uint8_t packTypeId, uint8_t command)
{
	if( resp->status != COMM_SUCCESS  || resp->responsePayload[3] != (PACK_TYPE_DEVICE_STATE_MANAGEMENT | 0x80) ){
		return;
	}
	DeviceStateMgmt_t *state = (DeviceStateMgmt_t *)(resp->responsePayload + HEAD_SIZE_GENERIC_HEADER + 1);
	if (state->addrMode != APS_SHORT_ADDRESS || state->commandId != ZCL_READ_ATTRIBUTES_COMMAND_ID){
		return;
	}

	device_info_t deviceInfo;
	deviceInfo.address = state->address;
	deviceInfo.endpoint = state->endpoint;
	zigbee_node_info_t *node = query_node_from_List(&deviceInfo);
	if (node == NULL){
		IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("node == NULL\n"));
		return;
	}

	gpio_set_level(GPIO_NUM_2, 0);

	uint8_t attributeCount = resp->responsePayload[HEAD_SIZE_GENERIC_HEADER + HEAD_SIZE_DEVICE_STATE];
	AttributeOprtRspStruct_t *attr;
	uint8_t size = HEAD_SIZE_GENERIC_HEADER + HEAD_SIZE_DEVICE_STATE + 1;
	uint8_t attrLength;
	if(state->clusterId == ONOFF_CLUSTER_ID){
		for (uint8_t i = 0; i < attributeCount; i++){
			attr = (AttributeOprtRspStruct_t *)(resp->responsePayload + size);
			if (attr->attributeId == ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID){
				node->on_off_light.onOff = attr->value8;
				return;
			}
			attrLength = zigbee_query_get_datetype_length(attr->type);
			if(attrLength == 255){//string
				attrLength = attr->payload[0]+1;
			}
			size += 2+1+1+attrLength;//uint16_t attributeId; uint8_t status; uint8_t type;
		}
	}else if (state->clusterId == LEVEL_CONTROL_CLUSTER_ID){
		for (uint8_t i = 0; i < attributeCount; i++){
			attr = (AttributeOprtRspStruct_t *)(resp->responsePayload + size);
			if (attr->attributeId == ZCL_LEVEL_CONTROL_CLUSTER_CURRENT_LEVEL_ATTRIBUTE_ID){
				node->dimmable_light.level = attr->value8;
				return;
			}
			attrLength = zigbee_query_get_datetype_length(attr->type);
			if(attrLength == 255){//string
				attrLength = attr->payload[0]+1;
			}
			size += 2+1+1+attrLength;//uint16_t attributeId; uint8_t status; uint8_t type;
		}
	}else if (state->clusterId == COLOR_CONTROL_CLUSTER_ID){
		for (uint8_t i = 0; i < attributeCount; i++){
			attr = (AttributeOprtRspStruct_t *)(resp->responsePayload + size);
			if (attr->attributeId == ZCL_ZLL_CLUSTER_CURRENT_SATURATION_SERVER_ATTRIBUTE_ID){
				node->extended_color_light.saturation = attr->value8;
			}else if (attr->attributeId == ZCL_ZLL_CLUSTER_ENHANCED_CURRENT_HUE_SERVER_ATTRIBUTE_ID){
				node->extended_color_light.hue = attr->value16;
			}else if (attr->attributeId == ZCL_ZLL_CLUSTER_COLOR_TEMPERATURE_SERVER_ATTRIBUTE_ID){
				if (node->device_info.deviceId == ZLL_TEMPERATURE_COLOR_LIGHT_DEVICE_ID ){
					node->color_temperature_light.color_temperature = attr->value16;
				}else{
					node->extended_color_light.color_temperature = attr->value16;
				}
			}
			attrLength = zigbee_query_get_datetype_length(attr->type);
			if(attrLength == 255){//string
				attrLength = attr->payload[0]+1;
			}
			size += 2+1+1+attrLength;//uint16_t attributeId; uint8_t status; uint8_t type;
		}
	}
}

bool isQuryableDevice(uint16_t deviceId)
{
	if(		deviceId == ZLL_ON_OFF_LIGHT_DEVICE_ID
		|| 	deviceId == ZLL_DIMMABLE_LIGHT_DEVICE_ID
		|| 	deviceId == ZLL_COLOR_LIGHT_DEVICE_ID
		|| 	deviceId == ZLL_EXTENDED_COLOR_LIGHT_DEVICE_ID
		|| 	deviceId == ZLL_TEMPERATURE_COLOR_LIGHT_DEVICE_ID){
		return true;
	}
	return false;
}

bool push_info_query_to_queue(appCmdDescriptor_t *cmdDesc,device_info_t *deviceInfo)
{
	int32_t queueAvailableCount = uxQueueSpacesAvailable(zigbee_queue_cmd_in);

	uint8_t result = false;
	switch(deviceInfo->deviceId){
	case ZLL_ON_OFF_LIGHT_DEVICE_ID:
		if (queueAvailableCount >= 2){
			zigbee_query_on_off(cmdDesc,deviceInfo->address,deviceInfo->endpoint);
			xQueueSend( zigbee_queue_cmd_in, cmdDesc, 0 );
			result = true;
		}
		break;
	case ZLL_DIMMABLE_LIGHT_DEVICE_ID:
		if (queueAvailableCount >= 3){
			zigbee_query_on_off(cmdDesc,deviceInfo->address,deviceInfo->endpoint);
			xQueueSend( zigbee_queue_cmd_in, cmdDesc, 0 );

			zigbee_query_level(cmdDesc,deviceInfo->address,deviceInfo->endpoint);
			xQueueSend( zigbee_queue_cmd_in, cmdDesc, 0 );
			result = true;
		}
		break;
	case ZLL_COLOR_LIGHT_DEVICE_ID:
		if (queueAvailableCount >= 4){
			zigbee_query_on_off(cmdDesc,deviceInfo->address,deviceInfo->endpoint);
			xQueueSend( zigbee_queue_cmd_in, cmdDesc, 0 );

			zigbee_query_level(cmdDesc,deviceInfo->address,deviceInfo->endpoint);
			xQueueSend( zigbee_queue_cmd_in, cmdDesc, 0 );

			zigbee_query_color(cmdDesc,deviceInfo->address,deviceInfo->endpoint);
			xQueueSend( zigbee_queue_cmd_in, cmdDesc, 0 );
			result = true;
		}
		break;
	case ZLL_EXTENDED_COLOR_LIGHT_DEVICE_ID:
		if (queueAvailableCount >= 4){
			zigbee_query_on_off(cmdDesc,deviceInfo->address,deviceInfo->endpoint);
			xQueueSend( zigbee_queue_cmd_in, cmdDesc, 0 );

			zigbee_query_level(cmdDesc,deviceInfo->address,deviceInfo->endpoint);
			xQueueSend( zigbee_queue_cmd_in, cmdDesc, 0 );

			zigbee_query_extended_color(cmdDesc,deviceInfo->address,deviceInfo->endpoint);
			xQueueSend( zigbee_queue_cmd_in, cmdDesc, 0 );
			result = true;
		}
		break;
	case ZLL_TEMPERATURE_COLOR_LIGHT_DEVICE_ID:
		if (queueAvailableCount >= 4){
			zigbee_query_on_off(cmdDesc,deviceInfo->address,deviceInfo->endpoint);
			xQueueSend( zigbee_queue_cmd_in, cmdDesc, 0 );

			zigbee_query_level(cmdDesc,deviceInfo->address,deviceInfo->endpoint);
			xQueueSend( zigbee_queue_cmd_in, cmdDesc, 0 );

			zigbee_query_colortemp(cmdDesc,deviceInfo->address,deviceInfo->endpoint);
			xQueueSend( zigbee_queue_cmd_in, cmdDesc, 0 );
			result = true;
		}
		break;
	default:
		result = false;
	}

	return result;
}

void zigbee_device_state_query(appCmdDescriptor_t *cmdDesc)
{
	static uint8_t nodeIndex = 0;
	zigbee_node_info_t *pNode = NULL;

	if (nodeIndex == 0){
		xTimerChangePeriod( zigbee_info_query_timer, DEVICE_INFO_QUERY_SHORT_INTERVAL/portTICK_PERIOD_MS, 0 );
	}
	pNode = query_node_by_index(nodeIndex);
	if(pNode != NULL){
		IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("Query device: %02x, DID:%04x,EP:%02x\n",pNode->device_info.address,pNode->device_info.deviceId,pNode->device_info.endpoint));
	}
	while(pNode){
		if(isQuryableDevice(pNode->device_info.deviceId)){
			cmdDesc->cmd_response_callback = device_state_rsp_callback;
			if(push_info_query_to_queue(cmdDesc,&pNode->device_info)){
				nodeIndex++;
			}
			return;
		}
		pNode = query_node_by_index(++nodeIndex);
	}
	nodeIndex = 0;
	xTimerChangePeriod( zigbee_info_query_timer, DEVICE_INFO_QUERY_INTERVAL/portTICK_PERIOD_MS, 0 );
}

void zigbee_cmd_callback(Cmd_Response_t *resp, struct sockaddr addr,network_header_t networkHeader,uint8_t packTypeId, uint8_t command)
{
	if (resp->responseLength <= (HEAD_SIZE_GENERIC_HEADER + 2)){
		return;
	}
	// need to feedback
	appCmdDescriptor_t *cmdDesc = (appCmdDescriptor_t *)resp->responsePayload;
	network_message_t message;
	memcpy(&message.networkHeader,&networkHeader,sizeof(network_header_t));
	message.networkHeader.messageType = NETWORK_MSG_TYPE_NON;
	message.networkHeader.sumFrames  = 1;
	message.networkHeader.currFrameNo = 1;
	
	message.packTypeId = packTypeId | CMD_RSP;
	message.command = command;
	message.contentLength = resp->responseLength - HEAD_SIZE_GENERIC_HEADER - 2;
	message.networkHeader.packageLength = message.contentLength + 4;
	memcpy(message.content,cmdDesc->subPayload,message.contentLength);

	message.sourceAddr = addr;
	// memcpy((uint8_t *)&message.sourceAddr,(uint8_t *)&addr,sizeof(struct sockaddr));
	xQueueSend(zigbee_queue_cmd_out, &message, 0 );

	if (packTypeId == MSG_TYPE_ID_CONFIG){
		if (command == CMD_CONFIG_ZGIBEE_DEVICE && resp->status == 0){
			// update group scene info
			info_query_t infoQuery;
			infoQuery.query_type = QUERY_OGNZ_STRUCTURE;
			xQueueSend( zigbee_info_queue_query_start, &infoQuery, 0);
		}else if (command == CMD_CONFIG_ZIGBEE_GATEWAY){
			// update device info
			
		}
	}
}

bool zigbee_ctrl_cmd_handle(appCmdDescriptor_t *cmdDesc,DeviceOprt_t *deviceCtrl)
{
	bool validCmd = true;
	if (deviceCtrl->clusterId == ONOFF_CLUSTER_ID){
		IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("ONOFF_CLUSTER_ID\n"));
		zigbee_ctrl_on_off(cmdDesc,deviceCtrl->addrMode,deviceCtrl->address,deviceCtrl->endpoint,deviceCtrl->commandId);
	}else if (deviceCtrl->clusterId == LEVEL_CONTROL_CLUSTER_ID){
		IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("LEVEL_CONTROL_CLUSTER_ID\n"));
		if ( deviceCtrl->commandId == ZCL_LEVEL_CONTROL_CLUSTER_MOVE_TO_LEVEL_COMMAND_ID || deviceCtrl->commandId == ZCL_LEVEL_CONTROL_CLUSTER_MOVE_TO_LEVEL_W_ONOFF_COMMAND_ID){
			zigbee_ctrl_move_to_level( cmdDesc,deviceCtrl->addrMode,deviceCtrl->address,deviceCtrl->endpoint,
					deviceCtrl->commandId,
					deviceCtrl->DeviceCmdOprt.moveToLevel.level,
					deviceCtrl->DeviceCmdOprt.moveToLevel.transitionTime);
		}else if (deviceCtrl->commandId == ZCL_LEVEL_CONTROL_CLUSTER_MOVE_COMMAND_ID || deviceCtrl->commandId == ZCL_LEVEL_CONTROL_CLUSTER_MOVE_W_ONOFF_COMMAND_ID){
			zigbee_ctrl_move_level(cmdDesc,deviceCtrl->addrMode,deviceCtrl->address,deviceCtrl->endpoint,
					deviceCtrl->commandId,
					deviceCtrl->DeviceCmdOprt.moveLevel.moveMode,
					deviceCtrl->DeviceCmdOprt.moveLevel.rate);
		}
	}else if (deviceCtrl->clusterId == COLOR_CONTROL_CLUSTER_ID){
		IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("COLOR_CONTROL_CLUSTER_ID\n"));
		if (deviceCtrl->commandId == ENHANCED_MOVE_TO_HUE_AND_SATURATION_COMMAND_ID){
			zigbee_ctrl_enhanced_move_to_hue_and_saturation(cmdDesc,deviceCtrl->addrMode,deviceCtrl->address,deviceCtrl->endpoint,
					deviceCtrl->DeviceCmdOprt.enhancedMoveToHueAndSaturation.enhancedHue,
					deviceCtrl->DeviceCmdOprt.enhancedMoveToHueAndSaturation.saturation,
					deviceCtrl->DeviceCmdOprt.enhancedMoveToHueAndSaturation.transitionTime);
		}else if (deviceCtrl->commandId == MOVE_TO_COLOR_TEMPERATURE_COMMAND_ID){
			zigbee_ctrl_move_to_colorTemperature(cmdDesc,deviceCtrl->addrMode,deviceCtrl->address,deviceCtrl->endpoint,
					deviceCtrl->DeviceCmdOprt.moveToColorTemperature.colorTemperature,
					deviceCtrl->DeviceCmdOprt.moveToColorTemperature.transitionTime);
		}
	}else if (deviceCtrl->clusterId == SCENES_CLUSTER_ID){
		zigbee_ctrl_recall_scene(cmdDesc,deviceCtrl->addrMode,deviceCtrl->address,deviceCtrl->endpoint,
					deviceCtrl->DeviceCmdOprt.recallScene.groupId, deviceCtrl->DeviceCmdOprt.recallScene.sceneId);
	}else{
		validCmd = false;
	}
	return validCmd;
}

bool zigbee_config_cmd_handle(appCmdDescriptor_t *cmdDesc,GroupSceneMgmt_t *groupSceneOpt)
{
	bool validCmd = true;
	if (groupSceneOpt->clusterId == GROUPS_CLUSTER_ID){
		if (   groupSceneOpt->commandId == ZCL_GROUPS_CLUSTER_ADD_GROUP_COMMAND_ID
			|| groupSceneOpt->commandId == ZCL_GROUPS_CLUSTER_ADD_GROUP_IF_IDENTIFYING_COMMAND_ID
			|| groupSceneOpt->commandId == ZCL_GROUPS_CLUSTER_REMOVE_GROUP_COMMAND_ID
			|| groupSceneOpt->commandId == ZCL_GROUPS_CLUSTER_REMOVE_ALL_GROUPS_COMMAND_ID){
			zigbee_add_remove_group(cmdDesc,groupSceneOpt->address,groupSceneOpt->endpoint,groupSceneOpt->GroupSceneCmd.addGroup.groupId, groupSceneOpt->commandId);
		}
	}else if(groupSceneOpt->clusterId == SCENES_CLUSTER_ID){
		if (   groupSceneOpt->commandId == ZCL_SCENES_CLUSTER_ADD_SCENE_COMMAND_ID
			|| groupSceneOpt->commandId == ZCL_SCENES_CLUSTER_STORE_SCENE_COMMAND_ID
			|| groupSceneOpt->commandId == ZCL_SCENES_CLUSTER_REMOVE_SCENE_COMMAND_ID
			|| groupSceneOpt->commandId == ZCL_SCENES_CLUSTER_REMOVE_ALL_SCENES_COMMAND_ID){
			zigbee_scene_add_remove_store_scene(cmdDesc,groupSceneOpt->addrMode,groupSceneOpt->address,groupSceneOpt->endpoint,groupSceneOpt->commandId,
					groupSceneOpt->GroupSceneCmd.addScene.groupId,
					groupSceneOpt->GroupSceneCmd.addScene.sceneId,
					groupSceneOpt->GroupSceneCmd.addScene.transitionTime);
		}else if (groupSceneOpt->commandId == ZCL_SCENES_CLUSTER_ENHANCED_ADD_SCENE_COMMAND_ID){
			zigbee_scene_enhanced_add_scene(cmdDesc,groupSceneOpt->addrMode,groupSceneOpt->address,groupSceneOpt->endpoint,groupSceneOpt->GroupSceneCmd.enhancedAddScene.groupId,
					groupSceneOpt->GroupSceneCmd.enhancedAddScene.sceneId, groupSceneOpt->GroupSceneCmd.enhancedAddScene.transitionTime,
					groupSceneOpt->GroupSceneCmd.enhancedAddScene.onOffClusterExtFields.onOffValue,
					groupSceneOpt->GroupSceneCmd.enhancedAddScene.levelControlClusterExtFields.currentLevel,
					groupSceneOpt->GroupSceneCmd.enhancedAddScene.colorControlClusterExtFields.currentX,
					groupSceneOpt->GroupSceneCmd.enhancedAddScene.colorControlClusterExtFields.currentY,
					groupSceneOpt->GroupSceneCmd.enhancedAddScene.colorControlClusterExtFields.enhancedCurrentHue,
					groupSceneOpt->GroupSceneCmd.enhancedAddScene.colorControlClusterExtFields.currentSaturation);
		}else{
			validCmd = false;
		}
	}else{
		validCmd = false;
	}
	return validCmd;
}

bool zigbee_network_cmd_handle(appCmdDescriptor_t *cmdDesc,NwkMgmt_t *nwkmgnt)
{
	bool validCmd = true;
	if (nwkmgnt->commandId == NWK_MGMT_RST_GATEWAY_TO_FN){
		zigbee_network_reset_gateway_to_fn(cmdDesc);
	}else if (nwkmgnt->commandId == NWK_MGMT_TOUCHLINK){
		zigbee_network_touchlink(cmdDesc);
	}else if (nwkmgnt->commandId == NWK_MGMT_CLASSIC_JOIN_PERMIT_JOIN){
		zigbee_network_classic_permit_join(cmdDesc);
	}else if (nwkmgnt->commandId == NWK_MGMT_CLASSIC_JOIN_RST_LIGHT_TO_FN){
		zigbee_network_reset_device_to_fn(cmdDesc,nwkmgnt->NwkMgmtClassicJoinRstTargetToFn.address, nwkmgnt->NwkMgmtClassicJoinRstTargetToFn.endpoint);
	}else{
		validCmd = false;
	}
	return validCmd;
}

void GatewayManager_HandleNetworkRequest(void *msg)
{
	network_message_t *message = (network_message_t *)msg;
	appCmdDescriptor_t cmdDesc;

	IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("GET: messageType:%02x,packTypeId:%02x\r\n",message->networkHeader.messageType, message->packTypeId));
	switch(message->packTypeId){
	case MSG_TYPE_ID_CONNECT:
		IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("get gateway network info\n"));
		process_gateway_connect_packet(message);
		break;
	case MSG_TYPE_ID_QUERY:
		IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("message->command :%02x\r\n",message->command));
    	if (message->command == CMD_QUERY_GET_OGNZ_STRUCTURE){
    		process_ognz_struct_packet(message);
    	}else if(message->command == CMD_QUERY_GET_DEVICES_INFO){
    		process_devices_info_packet(message);
    	}else if (message->command == CMD_QUERY_GET_DEVICES_STATE){
    		process_device_state_packet(message);
    	}
    	break;
	case MSG_TYPE_ID_CTRL:
		if(zigbee_ctrl_cmd_handle(&cmdDesc,&message->DeviceOprt)){
			memcpy(&cmdDesc.networkHeader,&message->networkHeader,sizeof(network_header_t));
			cmdDesc.packTypeId = message->packTypeId;
			cmdDesc.command = message->command;
			cmdDesc.sourceAddr = message->sourceAddr;
			cmdDesc.cmd_response_callback = zigbee_cmd_callback;
			xQueueSend( zigbee_queue_cmd_in, &cmdDesc, 0 );
		}
		break;
	case MSG_TYPE_ID_CONFIG:
		IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("message->command :%02x\r\n",message->command));
		if (message->command == CMD_CONFIG_ZGIBEE_DEVICE){
			if (zigbee_config_cmd_handle(&cmdDesc,&message->GroupSceneMgmt)){
				memcpy(&cmdDesc.networkHeader,&message->networkHeader,sizeof(network_header_t));
				cmdDesc.packTypeId = message->packTypeId;
				cmdDesc.command = message->command;
				cmdDesc.sourceAddr = message->sourceAddr;
				cmdDesc.cmd_response_callback = zigbee_cmd_callback;//
				xQueueSend( zigbee_queue_cmd_in, &cmdDesc, 0 );
			}
		}else if (message->command == CMD_CONFIG_ZIGBEE_GATEWAY){
			if (zigbee_network_cmd_handle(&cmdDesc,&message->NwkMgmt)){
				memcpy(&cmdDesc.networkHeader,&message->networkHeader,sizeof(network_header_t));
				cmdDesc.packTypeId = message->packTypeId;
				cmdDesc.command = message->command;
				cmdDesc.sourceAddr = message->sourceAddr;
				cmdDesc.cmd_response_callback = zigbee_cmd_callback;//
				xQueueSend( zigbee_queue_cmd_in, &cmdDesc, 0 );
			}
		}
		break;
	}
}

void gateway_info_rsp_callback(Cmd_Response_t *resp, struct sockaddr addr,network_header_t header,uint8_t packTypeId, uint8_t commandId)
{
	if( resp->status != COMM_SUCCESS ){
		return;
	}

	uint8_t command;
	if (resp->responsePayload[3] == (PACK_TYPE_GATEWAY_MANAGEMENT | 0x80) ){
		command = resp->responsePayload[4];
		if (command == GATEWAY_MGMT_GET_GATEWAY_NAME){
			GatewayMgmtGetNameRsp_t *rsp = (GatewayMgmtGetNameRsp_t *)(resp->responsePayload + HEAD_SIZE_GENERIC_HEADER + HEAD_SIZE_GATEWAY_MGMT);
			g_zigbee_ognz_info.gateway_info.channel 	= rsp->channel;
			g_zigbee_ognz_info.gateway_info.macAddress 	= rsp->macAddress;
			g_zigbee_ognz_info.gateway_info.panID 		= rsp->panID;
		}
	}else if(resp->responsePayload[3] == (PACK_TYPE_ORGANIZATION_DESCRIPTION | 0x80) ){
		command = resp->responsePayload[4];
		switch(command){
		case OGNZ_MGMT_GET_DEVICE_LIST:
		{
			OgnzMgmtReadDeviceListRsp_t *rsp = (OgnzMgmtReadDeviceListRsp_t *)(resp->responsePayload + HEAD_SIZE_GENERIC_HEADER + HEAD_SIZE_OGNZ_MGMT);
			g_zigbee_ognz_info.device_list.deviceList[0] =  rsp->deviceList[0];
			g_zigbee_ognz_info.device_list.deviceList[1] =  rsp->deviceList[1];
			break;
		}
		case OGNZ_MGMT_GET_GROUP_LIST:
		{
			OgnzMgmtReadGroupListRsp_t *rsp = (OgnzMgmtReadGroupListRsp_t *)(resp->responsePayload + HEAD_SIZE_GENERIC_HEADER + HEAD_SIZE_OGNZ_MGMT);
			g_zigbee_ognz_info.group_list.list =  rsp->list;
			break;
		}
		case OGNZ_MGMT_GET_SCENE_LIST:
		{
			OgnzMgmtReadSceneListRsp_t *rsp = (OgnzMgmtReadSceneListRsp_t *)(resp->responsePayload + HEAD_SIZE_GENERIC_HEADER + HEAD_SIZE_OGNZ_MGMT);
			g_zigbee_ognz_info.scene_list.list =  rsp->list;
			break;
		}
		case OGNZ_MGMT_GET_DEVICE_INFO:
		{
			OgnzMgmtQueryDeviceRsp_t *rsp = (OgnzMgmtQueryDeviceRsp_t *)(resp->responsePayload + HEAD_SIZE_GENERIC_HEADER + HEAD_SIZE_OGNZ_MGMT);
			device_info_t devInfo;

			if(rsp->address < MAXIMUM_DEVICES_NUM){
				g_zigbee_ognz_info.device_info[rsp->address].uuid = *((uint16_t *)rsp->name);
				g_zigbee_ognz_info.device_info[rsp->address].endpointNum =  rsp->deviceNum;
				for(uint8_t i = 0; i < rsp->deviceNum; i++){
					g_zigbee_ognz_info.device_info[rsp->address].endpoint[i].deviceId =  rsp->deviceInfo[i].deviceId;
					g_zigbee_ognz_info.device_info[rsp->address].endpoint[i].endpoint =  rsp->deviceInfo[i].endpoint;

					devInfo.address = rsp->address;
					devInfo.deviceId = rsp->deviceInfo[i].deviceId;
					devInfo.endpoint = rsp->deviceInfo[i].endpoint;

					IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("try to Get empty entry\n"));
					if(query_node_from_List(&devInfo) == NULL){
						IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("ADD: %02x, DID:%04x,EP:%02x\n",devInfo.address,devInfo.deviceId,devInfo.endpoint));
						add_node_to_List(&devInfo, 60*1000);
					}
				}
			}
			break;
		}
		case OGNZ_MGMT_GET_GROUP_INFO:
		case OGNZ_MGMT_GET_GROUP_INFO_EXT:
		{
			OgnzMgmtQueryGroupRsp_t *rsp = (OgnzMgmtQueryGroupRsp_t *)(resp->responsePayload + HEAD_SIZE_GENERIC_HEADER + HEAD_SIZE_OGNZ_MGMT);
			if(rsp->address < MAXIMUM_GROUPS_NUM){
				g_zigbee_ognz_info.group_info[rsp->address].deviceList[0] =  rsp->deviceList[0];
				g_zigbee_ognz_info.group_info[rsp->address].deviceList[1] =  rsp->deviceList[1];
			}
			break;
		}
		case OGNZ_MGMT_GET_SCENE_INFO:
		case OGNZ_MGMT_GET_SCENE_INFO_EXT:
		{
			OgnzMgmtQueryScenepRsp_t *rsp = (OgnzMgmtQueryScenepRsp_t *)(resp->responsePayload + HEAD_SIZE_GENERIC_HEADER + HEAD_SIZE_OGNZ_MGMT);
			if(rsp->address < MAXIMUM_SCENES_NUM){
				g_zigbee_ognz_info.scene_info[rsp->address].groupNo =  rsp->groupNo;
			}
			break;
		}
		default:
			break;
		}
	}
}

void gateway_manager_info_query(info_query_t infoQuery)
{
	//Read zigBee gateway info
	appCmdDescriptor_t cmdDesc;
	cmdDesc.cmd_response_callback = gateway_info_rsp_callback;

	//query gateway info
	if (infoQuery.query_type & QUERY_TYPE_GATEWAY){
		zigbee_gateway_query_gateway_info(&cmdDesc);
		zigbee_cmd_service_process_packet(&cmdDesc);
	}

	//query device list
	if (infoQuery.query_type & QUERY_TYPE_DEVICE_LIST){
		zigbee_ognz_query_device_group_scene_list(&cmdDesc,OGNZ_MGMT_GET_DEVICE_LIST);
		zigbee_cmd_service_process_packet(&cmdDesc);
	}

	//query group list
	if (infoQuery.query_type & QUERY_TYPE_GROUP_LIST){
		zigbee_ognz_query_device_group_scene_list(&cmdDesc,OGNZ_MGMT_GET_GROUP_LIST);
		zigbee_cmd_service_process_packet(&cmdDesc);
	}
	//query scene list
	if (infoQuery.query_type & QUERY_TYPE_SCENE_LIST){
		zigbee_ognz_query_device_group_scene_list(&cmdDesc,OGNZ_MGMT_GET_SCENE_LIST);
		zigbee_cmd_service_process_packet(&cmdDesc);
	}

	//query device info
	if (infoQuery.query_type & QUERY_TYPE_DEVICE_INFO){
		uint64_t devicelist;
		for(uint8_t i = 0; i < MAXIMUM_DEVICES_NUM; i++){
			devicelist = g_zigbee_ognz_info.device_list.deviceList[i/64];
			if( devicelist & ((uint64_t)1 << (i%64)) ){
				zigbee_ognz_query_device_group_scene_info(&cmdDesc,OGNZ_MGMT_GET_DEVICE_INFO,i);
				zigbee_cmd_service_process_packet(&cmdDesc);
			}
		}
	}

	//query group info
	if (infoQuery.query_type & QUERY_TYPE_GROUP_INFO){
		for(uint8_t i = 0; i < MAXIMUM_GROUPS_NUM; i++){
			if(g_zigbee_ognz_info.group_list.list & ((uint32_t)1 << (i%32)) ){
				zigbee_ognz_query_device_group_scene_info(&cmdDesc,OGNZ_MGMT_GET_GROUP_INFO,i);
				zigbee_cmd_service_process_packet(&cmdDesc);
			}
		}
	}

	//query scene info
	if (infoQuery.query_type & QUERY_TYPE_SCEN_INFO){
		for(uint8_t i = 0; i < MAXIMUM_SCENES_NUM; i++){
			if(g_zigbee_ognz_info.scene_list.list & ((uint32_t)1 << (i%32)) ){
				zigbee_ognz_query_device_group_scene_info(&cmdDesc,OGNZ_MGMT_GET_SCENE_INFO,i);
				zigbee_cmd_service_process_packet(&cmdDesc);
			}
		}
	}

	//query device state
	if (infoQuery.query_type & QUERY_TYPE_DEVICE_STATE){
		zigbee_device_state_query(&cmdDesc);
	}
}


void gateway_manager_task(void *pvParameter)
{
	struct sockaddr sourceAddr;
	info_query_t infoQuery;
	infoQuery.query_type = QUERY_OGNZ_STRUCTURE;
	gateway_manager_info_query(infoQuery);
	while(1){
		network_message_t cmdOutDesc;
		if (xQueueReceive(zigbee_queue_cmd_out, &cmdOutDesc, 0)){
			sourceAddr = cmdOutDesc.sourceAddr;
			if (local_udp_socket >= 0){
				if (sendto(local_udp_socket,(uint8_t *)&cmdOutDesc, cmdOutDesc.contentLength + NETWORK_MESSAGE_LENGTH, 0, &sourceAddr, sizeof(struct sockaddr)) < 0){
					IoT_DEBUG(LWIP_DBG | IoT_DBG_WARNING,("sendto:zigbee failed\r\n"));
				}
			}
		}
		appCmdDescriptor_t cmdInDesc;
		if(xQueueReceive(zigbee_queue_cmd_in, &cmdInDesc, 0)){
			gpio_set_level(GPIO_NUM_2, 1);
			zigbee_cmd_service_process_packet(&cmdInDesc);
		}

		if(xQueueReceive(zigbee_info_queue_query_start, &infoQuery, 0)){
			gateway_manager_info_query(infoQuery);
		}

		vTaskDelay(1/portTICK_PERIOD_MS);
	}
}

void device_info_query_callback( TimerHandle_t xTimer )
{
	info_query_t infoQuery;
	infoQuery.query_type = QUERY_TYPE_DEVICE_STATE,
	xQueueSend( zigbee_info_queue_query_start, &infoQuery, 0);
}




void GatewayManager_Init(void)
{
	zigbee_ognz_info_init();
	zigbee_cmd_service_init();
	zigbee_queue_cmd_in  = xQueueCreate( 6, sizeof(appCmdDescriptor_t) );
	zigbee_queue_cmd_out = xQueueCreate( 3, sizeof(network_message_t) );
	zigbee_info_queue_query_start = xQueueCreate( 1, sizeof(info_query_t) );
	zigbee_info_query_timer = xTimerCreate( "ZDSQ_timer", DEVICE_INFO_QUERY_INTERVAL/portTICK_PERIOD_MS, pdTRUE, 0, device_info_query_callback );
	xTimerStart(zigbee_info_query_timer,0);
	xTaskCreate(&gateway_manager_task, "GW_DEV_MT", 4096, NULL, tskIDLE_PRIORITY+3, NULL);
}






