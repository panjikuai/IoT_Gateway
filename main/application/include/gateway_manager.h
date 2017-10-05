#ifndef GATEWAY_MGNT_H
#define GATEWAY_MGNT_H

#include "zigbee_header.h"
#include "zigbee_cmd_service.h"

#define  MAXIMUM_DEVICES_NUM  	128
#define  MAXIMUM_GROUPS_NUM   	32
#define  MAXIMUM_SCENES_NUM   	32

#define DEVICE_INFO_QUERY_SHORT_INTERVAL  (50)
#define DEVICE_INFO_QUERY_INTERVAL  	  (20000)

#define MEM_ALIGNMENT			4
#define MEM_ALIGN_SIZE(size) (((size) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1))

#define CMD_RSP (0x80)

typedef enum{
	CMD_QUERY_GET_OGNZ_STRUCTURE = 0,
	CMD_QUERY_GET_DEVICES_INFO,
	CMD_QUERY_GET_DEVICES_STATE,
}cmd_query_t;

//DeviceOprt_t
typedef enum{
	CMD_CTRL_GATEWAY = 0,
	CMD_CTRL_DEVICES,
}cmd_control_t;

typedef enum{
	CMD_CONFIG_ZIGBEE_GATEWAY = 0,
	CMD_CONFIG_ZGIBEE_DEVICE,
	CMD_CONFIG_IFTTT,
}cmd_config_t;

BEGIN_PACK

typedef struct{
	uint8_t command;
	struct{
		uint8_t message[32];
	}find_request;
	struct{
		uint8_t mac[6];
		uint8_t ipAddr[4];
	}find_response;
}Server_finding_t;

//////////////////////////////////////////////////////////////
typedef struct{//11
    uint16_t panID;
    uint8_t channel;
    uint64_t macAddress;
}zigbee_gateway_info_t;

typedef struct{//16
	uint64_t deviceList[2];
}zigbee_device_list_t;


typedef struct{
	uint8_t  endpoint;
	uint16_t deviceId;
}endpoint_info_t;

typedef struct{//1 + 3*3 = 10 *128 = 1280
	uint16_t uuid;
    uint8_t  endpointNum;
    endpoint_info_t endpoint[3];
}zigbee_device_info_t;

typedef struct{//4
	uint32_t list;
}zigbee_group_list_t;

typedef struct{//16*32 = 512
	uint64_t deviceList[2];
}zigbee_group_info_t;

typedef struct{//4
	uint32_t list;
}zigbee_scene_list_t;

typedef struct{//1*32 = 32
	uint8_t groupNo;
}zigbee_scene_info_t;
///////////////////////////////////////////////////////////////
typedef struct{//1
	uint8_t onOff;
}on_off_light_t;

typedef struct{//2
	uint8_t onOff;
	uint8_t level;
}dimmable_light_t;

typedef struct{//4
	uint8_t onOff;
	uint8_t level;
	uint16_t color_temperature;
}color_temperature_light_t;

typedef struct{//5
	uint8_t onOff;
	uint8_t level;
	uint8_t saturation;
	uint16_t hue;
}color_light_t;

typedef struct{//7*128*3
	uint8_t onOff;
	uint8_t level;
	uint8_t saturation;
	uint16_t hue;
	uint16_t color_temperature;
}extended_color_light_t;

typedef struct{
	uint8_t address;
	uint8_t  endpoint;
	uint16_t deviceId;
}device_info_t;

#define STATUS_ONLINE  0
#define STATUS_OFFLINE 1

struct zigbee_node_info{

	uint8_t  status;//0: online, 1: offline
	device_info_t	device_info;
	union{
		on_off_light_t 				on_off_light;
		dimmable_light_t			dimmable_light;
		color_temperature_light_t	color_temperature_light;
		color_light_t				color_light;
		extended_color_light_t		extended_color_light;
	};
	uint32_t updateTimeout;
	struct zigbee_node_info *next;
} ;

typedef struct zigbee_node_info  zigbee_node_info_t;
///////////////////////////////////////////////////////////////
typedef struct{
	zigbee_gateway_info_t 		gateway_info;
	zigbee_group_list_t			group_list;
	zigbee_group_info_t			group_info[MAXIMUM_GROUPS_NUM];
	zigbee_scene_list_t			scene_list;
	zigbee_scene_info_t			scene_info[MAXIMUM_SCENES_NUM];
	zigbee_device_list_t		device_list;
	zigbee_device_info_t		device_info[MAXIMUM_DEVICES_NUM];
	zigbee_node_info_t     		*head;
}zigbee_ognz_info_t;

#define QUERY_TYPE_GATEWAY  	0x01
#define QUERY_TYPE_GROUP_LIST 	0x02
#define QUERY_TYPE_GROUP_INFO	0x04
#define QUERY_TYPE_SCENE_LIST	0x08
#define QUERY_TYPE_SCEN_INFO	0x10
#define QUERY_TYPE_DEVICE_LIST	0x20
#define QUERY_TYPE_DEVICE_INFO	0x40
#define QUERY_TYPE_DEVICE_STATE 0x80

#define QUERY_TYPE_ALL  		0xFF
#define QUERY_OGNZ_STRUCTURE    0x7F

#define QUERY_INFO_ALL

typedef struct{
	uint8_t query_type;
	union{
		uint32_t groupIndex;
		uint32_t scene_index;
		uint64_t device_index[2];
	};
}InfoQuery_t;

// ZGP device trigger
typedef struct{
	uint8_t pack_type;
	uint16_t profileId;
	uint16_t clusterId;
	uint8_t appid;
	union{
		uint32_t source_appid;
		uint64_t ieeeAddr;
	};
	uint16_t deviceId;
	uint8_t command;
}linkage_trigger_t;

// Action only support scenes
typedef struct{
    uint8_t  packType;
    uint16_t profileId;
    uint16_t clusterId;
    uint8_t  addrMode;
    uint8_t  address;
    uint8_t  endpoint;
    uint8_t commandId;
    ZCL_RecallScene_t   recallScene;
}linkage_action_t;


END_PACK

void GatewayManager_Init(void);
void GatewayManager_HandleNetworkRequest(void *message);
zigbee_gateway_info_t *GatewayManager_GetGatewayInfo(void);

#endif
