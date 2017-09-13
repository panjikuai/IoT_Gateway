#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H
#include "zigbee_header.h"
#include "zigbee_cmd_service.h"

#define LOCAL_UDP_PORT (5000)
#define NETWORK_MSG_SOF (0x55AA)
#define SOCKET_ADDR_MAX_SIZE				16

#define RX_MAX_BUFFER_SIZE					1200
#define SEND_MAX_BUFF_SIZE                  1200

typedef enum{
	NETWORK_MSG_TYPE_CON = 0,
	NETWORK_MSG_TYPE_NON,
	NETWORK_MSG_TYPE_ACK,
	NETWORK_MSG_TYPE_RST
}network_messageType_t;

typedef enum{
	MSG_TYPE_ID_CONNECT = 0,
	MSG_TYPE_ID_CTRL = 1,
	MSG_TYPE_ID_QUERY   = 2,
	MSG_TYPE_ID_CONFIG  = 3,
}messageTypeId_t;


typedef enum{
	CONN_CMD_QUERY_NET_INFO = 0,
}Connect_cmd_Id_t;


BEGIN_PACK

typedef struct{
	uint8_t  mac[6];//
	uint8_t  ip[4];
	zigbee_gateway_info_t zigbee_gateway_info;
}GetNetworkConnectInfo_t;

typedef struct{
	uint8_t blockIndex;//1 block = gw, groups scenes 128 devices/endpoints
}GetOgnzStruct_t;

typedef struct{
	uint8_t blockIndex;//1 block = 64 devices/endpoints
}GetDevicesInfo_t;

typedef struct{
	uint8_t blockIndex;//1 block = 64 devices/endpoints
}GetDevicesState_t;

typedef struct {
	network_header_t networkHeader;
	uint8_t 	packTypeId;
	uint8_t 	command;
	uint16_t    contentLength;
	union PACK{
		GetNetworkConnectInfo_t GetNetworkConnectInfo;
		GetOgnzStruct_t     GetOgnzStruct;
		GetDevicesInfo_t    GetDevicesInfo;
		GetDevicesState_t	GetDevicesState;

		GatewayMgmt_t 		GatewayMgmt;		//Gateway Management
		OgnzMgmt_t	  		OgnzMgmt;			//Organization Management
		NwkMgmt_t			NwkMgmt;			//Network Management
		GroupSceneMgmt_t	GroupSceneMgmt; 	//Group & Scene Management
		DeviceOprt_t		DeviceOprt;			//Device Operation（control）
		DeviceStateMgmt_t	DeviceStateMgmt;	//Device State Query
		SensorReportMgmt_t	SensorReportMgmt;	//Device State Report
		uint8_t content[MAX_PAYLOAD_SIZE -5];	//
	};
	struct sockaddr sourceAddr;
}NetworkMessage_t;

#define NETWORK_MESSAGE_LENGTH   (sizeof(network_header_t) + 4)

void NetworkManager_Init(void);

END_PACK







































#endif
