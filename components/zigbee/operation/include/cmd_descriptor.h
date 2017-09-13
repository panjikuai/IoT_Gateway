/*
 * cmd_descriptor.h
 *
 * Created: 2015-04-20 15:51:21
 *  Author: matt.qian
 */ 


#ifndef CMD_DESCRIPTOR_H_
#define CMD_DESCRIPTOR_H_

#include "zcl_header.h"
#include <lwip/sockets.h>

#define PACK

#define MAX_DATAPACK_SIZE 				38
#define SIMPLE_DESC_CLUSTER_ID_MAX_NUM  20

#define HEAD_SIZE_GENERIC_HEADER        3
#define HEAD_SIZE_GATEWAY_MGMT          2
#define HEAD_SIZE_OGNZ_MGMT             2
#define HEAD_SIZE_NWK_MGMT              4
#define HEAD_SIZE_GROUP_SCENE_MGMT      9
#define HEAD_SIZE_DEVICE_OPT            9
#define HEAD_SIZE_DEVICE_STATE          9

#define HEAD_SIZE_UPGRADE               4
#define HEAD_SIZE_SENROR_REPORT_STATE           (9 +2) //Add deviceId

#define MAX_PAYLOAD_SIZE 			 (HEAD_SIZE_UPGRADE+ 58 + 20)// 58 Maximum block size is ZCL_OtauImageBlockRespA_t
#define READ_ATTRIBUTES_PAYLOAD_SIZE (MAX_PAYLOAD_SIZE - HEAD_SIZE_DEVICE_STATE -2)
#define MAX_SUPPORTED_ENDPOINTS 32

#define EEPROM_NAME_SIZE        8
#define MAX_ENDPOINT_NUM        3

typedef enum PACK{
    COMM_STATE_MACHINE_SOF        = 0,
    COMM_STATE_MACHINE_SECURIZTY  = 1,
    COMM_STATE_MACHINE_LENGTH     = 2,
    COMM_STATE_MACHINE_DATAPACK   = 3,
    COMM_STATE_MACHINE_PEC        = 4
}CommStateMachine_t;

typedef enum PACK{
    COMM_SOF_DATA        = 0xAA
}CommSof_t;

typedef enum PACK{
    COMM_SECURIZTY_PLAIN        = 0,
    COMM_SECURIZTY_ENCRIPT      = 1,
    COMM_SECURIZTY_RESERVED     = 0xF0
}CommSecurityMode_t;

typedef enum PACK{
    COMM_SUCCESS        = 0x00,
    COMM_FALSE          = 0xFF
}CommResult_t;

typedef enum PACK {
    PACK_TYPE_GATEWAY_MANAGEMENT                = 0x00,    //Gateway Description
    PACK_TYPE_ORGANIZATION_DESCRIPTION          = 0x01,    //Device,Group Scene Information Description
    PACK_TYPE_DEVICE_NETWORK_MANAGEMENT         = 0x02,    //Commissioning
    PACK_TYPE_DEVICE_GROUP_SCENE_MANAGEMENT     = 0x03,    //Group and Scene Edit
    PACK_TYPE_DEVICE_CONTROL                    = 0x04,    //Control Command
    PACK_TYPE_DEVICE_STATE_MANAGEMENT           = 0x05,    //Attributes Read/Write
    PACK_TYPE_UPGRADE_MANAGEMENT                = 0x06,    //Gateway or Device Upgrade
    PACK_TYPE_SENSOR_REPORT_MANAGEMENT          = 0x10,    //Sensor Attributes Report
    PACK_TYPE_ZGP_SENSOR_REPORT_MANAGEMENT      = 0x11,    //ZGP Sensor Attributes Report
    PACK_TYPE_MANUFACTURE_TEST_MANAGEMENT       = 0x6E,    //Manufacturing Test
    PACK_TYPE_DEBUG                             = 0x6F,    //debug Info
    //0x40-0x7F reserved
}PackType_t;

typedef enum PACK{
    GATEWAY_MGMT_GET_GATEWAY_NAME          = 0x00,
    GATEWAY_MGMT_SET_GATEWAY_NAME          = 0x20,
}GatewayMgmtCmdProfile_t;

typedef enum PACK{
    OGNZ_MGMT_GET_DEVICE_INFO        = 0x00,
    OGNZ_MGMT_GET_GROUP_INFO         = 0x01,
    OGNZ_MGMT_GET_SCENE_INFO         = 0x02,

    OGNZ_MGMT_GET_GROUP_INFO_EXT     = 0x03,
    OGNZ_MGMT_GET_SCENE_INFO_EXT     = 0x04,

    OGNZ_MGMT_GET_DEVICE_LIST        = 0x10,
    OGNZ_MGMT_GET_GROUP_LIST         = 0x11,
    OGNZ_MGMT_GET_SCENE_LIST         = 0x12,
    OGNZ_MGMT_GET_DEVICES_OF_GROUP   = 0x13,
    OGNZ_MGMT_GET_GROUP_NO_OF_SCENE  = 0x14,

    OGNZ_MGMT_SET_DEVICE_INFO        = 0x20,
    OGNZ_MGMT_SET_GROUP_INFO         = 0x21,
    OGNZ_MGMT_SET_SCENE_INFO         = 0x22,

    OGNZ_MGMT_GET_ZGP_DEVICE_LIST    = 0x30,
    OGNZ_MGMT_GET_ZGP_DEVICE_INFO    = 0x31
}OgnzMgmtCmdProfile_t;

typedef enum PACK{
    NWK_MGMT_RST_GATEWAY_TO_FN                = 0x00,

    NWK_MGMT_ZGP_COMMISSIONING                = 0x05,
    NWK_MGMT_ZGP_REMOVE_DEVICE_FROM_SINK_TABLE= 0x08,
    NWK_MGMT_ZGP_DEVICE_NETWORK_CHANGE_IND    = 0x09,
    NWK_MGMT_ZGP_SERVER_CONFIGURATION_CMD     = 0x0A,
    NWK_MGMT_ZGP_CTRL                         = 0x0B,
    NWK_MGMT_ZGP_PARAMETER_REQ                = 0x0C,

    NWK_MGMT_TOUCHLINK                        = 0x10,
    NWK_MGMT_TOUCHLINK_RST_LIGHT_TO_FN        = 0x11,

    NWK_MGMT_SECURITYLINK                     = 0x20,
    NWK_MGMT_SECURITYLINK_RST_LIGHT_TO_FN     = 0x21,

    NWK_MGMT_REQ_DEVICE_LEAVE_NETWORK         = 0x2A,

    NWK_MGMT_FREELINK                         = 0x30,
    NWK_MGMT_FREELINK_RST_LIGHT_TO_FN         = 0x31,

    NWK_MGMT_CLASSIC_JOIN_CREATE_NETWORK      = 0x40,
    NWK_MGMT_CLASSIC_JOIN_PERMIT_JOIN         = 0x41,
    NWK_MGMT_CLASSIC_JOIN_RST_LIGHT_TO_FN     = 0x42,
    NWK_MGMT_DEVICE_LEAVE_NETWORK_IND         = 0x43,

    NWK_MGMT_DEVICE_ENDPOINTS                 = 0x44,
    NWK_MGMT_GET_DEVICE_SIMP_DESC             = 0x45,
    NWK_MGMT_GET_NODE_DESC                    = 0x46,

    NWK_MGMT_GET_DEVICE_ENDPOINT_INFO         = 0x48,
    NWK_MGMT_GET_GROUP_IDENTIFER_REQ          = 0x49,
    NWK_MGMT_GET_ENDPOINT_LIST_REQ            = 0x4A,

    NWK_MGMT_SET_PHY_CHANNEL                  = 0x50,
    NWK_MGMT_GET_PHY_CHANNEL                  = 0x51,

    NWK_MGMT_RADIO_TEST                       = 0xEE,
}NetworkCmdProfile_t;

typedef enum _ZGP_ApplicationId_t
{
  ZGP_SRC_APPID       =  0x00,
  ZGP_LPED_APPID      =  0x01,
  ZGP_IEEE_ADDR_APPID =  0x02
} ZGP_ApplicationId_t;

#define CM_MAX_READ_ATTRIBUTE_COUNT 20

BEGIN_PACK

typedef struct
{
	uint8_t          status;          //!< status of command  //CommResult_t
	uint8_t          responseLength;  //!< length of response payload
	uint8_t          *responsePayload; //!< pointer to response payload. Response payload can be parsed by ZCL_GetNextElement() function
} Cmd_Response_t;
//*****************************************************************//
typedef struct PACK{
    uint8_t result;
    uint8_t factoryNew;
    uint16_t panID;
    uint8_t channel;
    uint64_t macAddress;
    uint8_t nameLength;
    uint8_t name[EEPROM_NAME_SIZE];
}GatewayMgmtGetNameRsp_t;

typedef struct PACK{
    uint8_t nameLength;
    uint8_t name[EEPROM_NAME_SIZE];
}GatewayMgmtSetName_t;

typedef struct PACK
{
    uint8_t result;
}GatewayMgmtSetNameRsp_t;
typedef struct{
	uint8_t commandId;
	union PACK{
		GatewayMgmtGetNameRsp_t         GatewayMgmtGetNameRsp;
		GatewayMgmtSetName_t            GatewayMgmtSetName;
		GatewayMgmtSetNameRsp_t         GatewayMgmtSetNameRsp;
	};
}GatewayMgmt_t;
//*****************************************************************//
typedef struct PACK{
    uint8_t address;
    uint8_t endpoint;
}OgnzMgmtQuery_t;

typedef struct PACK
{
    uint16_t deviceId;
    uint8_t endpoint;
}QueryDeviceInfo_t;

typedef struct PACK{
    uint8_t address;
    uint8_t endpoint;
    uint8_t result;
    uint64_t macAddress;
    uint16_t profileId;
    uint8_t nameLength;
    uint8_t name[6];
    uint8_t deviceNum;
    QueryDeviceInfo_t deviceInfo[MAX_ENDPOINT_NUM];
//    uint8_t result;
}OgnzMgmtQueryDeviceRsp_t;

typedef struct PACK{
    uint8_t address;
    uint8_t result;
    uint64_t deviceList[2];
    uint8_t nameLength;
    uint8_t name[EEPROM_NAME_SIZE];
}OgnzMgmtQueryGroupRsp_t;

typedef struct PACK{
    uint8_t address;
    uint8_t result;
    uint8_t groupNo;
    uint8_t nameLength;
    uint8_t name[EEPROM_NAME_SIZE];
}OgnzMgmtQueryScenepRsp_t;

typedef struct PACK{
    uint8_t address;
    uint8_t endpoint;
    uint8_t nameLength;
    uint8_t name[EEPROM_NAME_SIZE];
}OgnzMgmtWrite_t;

typedef struct PACK{
    uint8_t address;
    uint8_t endpoint;
    uint8_t result;
}OgnzMgmtWriteRsp_t;

typedef struct PACK{
    uint8_t result;
    uint64_t deviceList[2];
}OgnzMgmtReadDeviceListRsp_t;

typedef struct PACK{
    uint8_t result;
    uint32_t zgpDeviceList;
}OgnzMgmtReadZgpDeviceListRsp_t;

typedef struct PACK{
    uint8_t address;
}OgnzMgmtZgpDeviceInfoQuery_t;

typedef struct PACK{
    uint8_t address;
    uint8_t result;
    uint16_t deviceId;
    uint8_t  endpoint;
    ZGP_ApplicationId_t appId;
    union{
      uint32_t srcId;
      ExtAddr_t deviceAddress;
    };
}OgnzMgmtZgpDeviceInfoQueryRsp_t;

typedef struct PACK{
    uint8_t result;
    uint32_t list;
}OgnzMgmtReadGroupListRsp_t;

typedef struct PACK{
    uint8_t result;
    uint32_t list;
}OgnzMgmtReadSceneListRsp_t;

typedef struct PACK{
    uint8_t groupIndex;
}OgnzMgmtReadDevicesOfGroup_t;

typedef struct PACK{
    uint8_t result;
    uint8_t groupIndex;
    uint64_t deviceList[2];
}OgnzMgmtReadDevicesOfGroupRsp_t;

typedef struct PACK{
    uint8_t sceneIndex;
}OgnzMgmtReadGroupNoOfScene_t;

typedef struct PACK{
    uint8_t result;
    uint8_t sceneIndex;
    uint8_t groupNo;
}OgnzMgmtReadGroupNoOfSceneRsp_t;

typedef struct{
	uint8_t commandId;
	union PACK{
		OgnzMgmtQuery_t                     OgnzMgmtQuery;
		OgnzMgmtQueryDeviceRsp_t            OgnzMgmtQueryDeviceRsp;
		OgnzMgmtQueryGroupRsp_t             OgnzMgmtQueryGroupRsp;
		OgnzMgmtQueryScenepRsp_t            OgnzMgmtQuerySceneRsp;

		OgnzMgmtWrite_t                     OgnzMgmtWrite;
		OgnzMgmtWriteRsp_t                  OgnzMgmtWriteRsp;

		OgnzMgmtReadDeviceListRsp_t         OgnzMgmtReadDeviceListRsp;
		OgnzMgmtReadGroupListRsp_t          OgnzMgmtReadGroupListRsp;
		OgnzMgmtReadSceneListRsp_t          OgnzMgmtReadSceneListRsp;

		OgnzMgmtReadZgpDeviceListRsp_t      OgnzMgmtReadZgpDeviceListRsp;
		OgnzMgmtZgpDeviceInfoQuery_t        OgnzMgmtZgpDeviceInfoQuery;
		OgnzMgmtZgpDeviceInfoQueryRsp_t     OgnzMgmtZgpDeviceInfoQueryRsp;
	};
}OgnzMgmt_t;
//********************************************************************//
typedef struct PACK{
    uint8_t  option;
}NwkMgmtZgpCommissioning_t;

typedef struct PACK{
    uint8_t  option;
    uint8_t result;
}NwkMgmtZgpCommissioningRsp_t;

typedef struct PACK{
    uint64_t securityNumber;
}NwkMgmtSecuritylink_t;

typedef struct PACK{
    uint64_t securityNumber;
}NwkMgmtSecuritylinkRstTargetToFn_t;

typedef struct PACK{
    uint8_t address;
}NwkMgmtReqDeviceLeaveNetwork_t;

typedef struct PACK{
    uint8_t address;
    uint8_t result;
}NwkMgmtReqDeviceLeaveNetworkReq_t;

typedef struct PACK{
    uint8_t address;
}NwkMgmtGetDeviceEndpoints_t;

typedef struct PACK{
    uint8_t address;
    uint8_t result;
    uint8_t endpointNum;
    uint8_t endpoint[MAX_SUPPORTED_ENDPOINTS];
}NwkMgmtGetDeviceEndpointsRsp_t;

typedef struct PACK{
    uint8_t address;
}NwkMgmtGetNodeDesc_t;

typedef struct PACK{
    uint8_t address;
    uint8_t result;
    uint16_t nodeDescriptor;    //N_Zdp_NodeDescriptor_t
    uint8_t  macCapabilityFlags;//N_Zdp_MacCapability_t
    uint16_t manufacturerCode;
}NwkMgmtGetNodeDescRsp_t;

typedef struct PACK{
    uint8_t address;
    uint8_t endpoint;
}NwkMgmtGetDeviceSimpDescriptor_t;

typedef struct PACK{
    uint8_t  addrMode;
    uint8_t  address;
    uint8_t  endpoint;
    // Commissioning cluster
    ZCL_GetGroupIdentifiers_t         getGroupIdentifiers;
    ZCL_GetEndpointList_t             getEndpointList;
    ZCL_EndpointInformation_t         endpointInformation;
    ZCL_GetGroupIdentifiersResponse_t getGroupIdentifiersResp;
    ZCL_GetEndpointListResponse_t     getEndpointListResp;
}NwkMgmtCommissioningDescriptor_t;

typedef struct PACK{
    uint8_t  address;
    uint8_t endpoint;
    uint8_t result;
    uint64_t macAddress;
    uint16_t deviceId;
    uint8_t inClusterNum;
    uint16_t clusterId[SIMPLE_DESC_CLUSTER_ID_MAX_NUM];
}NwkMgmtGetDeviceSimpDescriptorRsp_t;

typedef struct PACK{
    uint8_t  enableMacJoin;
    uint64_t macAddress;
}NwkMgmtClassicJoinPermitJoin_t;

typedef struct PACK{
    uint8_t address;
    uint8_t endpoint;
}NwkMgmtClassicJoinRstTargetToFn_t;

typedef struct PACK{
    uint8_t address;
    uint8_t result;
}NwkMgmtDeviceLeaveNetworkInd_t;

typedef struct PACK{
    uint8_t result;
    uint8_t channel;
}NwkMgmtGetPhyChannelRsp_t;

typedef struct PACK{
    uint8_t result;
}NwkMgmtLinkRsp_t;

typedef struct PACK{
    uint8_t result;
    uint8_t  address;
    uint64_t macAddress;
    uint8_t deviceNum;
    QueryDeviceInfo_t DeviceInfo[MAX_ENDPOINT_NUM];
}NwkMgmtTouchLinkRsp_t;

typedef struct{
	uint16_t profileId;
	uint8_t commandId;
	union PACK{
		NwkMgmtZgpCommissioning_t           NwkMgmtZgpCommissioning;
		NwkMgmtZgpCommissioningRsp_t        NwkMgmtZgpCommissioningRsp;
		NwkMgmtSecuritylink_t               NwkMgmtSecuritylink;
		NwkMgmtSecuritylinkRstTargetToFn_t  NwkMgmtSecuritylinkRstTargetToFn;
		NwkMgmtReqDeviceLeaveNetwork_t      NwkMgmtReqDeviceLeaveNetwork;
		NwkMgmtReqDeviceLeaveNetworkReq_t   NwkMgmtReqDeviceLeaveNetworkReq;
		NwkMgmtGetDeviceEndpoints_t         NwkMgmtGetDeviceEndpoints;
		NwkMgmtGetDeviceEndpointsRsp_t      NwkMgmtGetDeviceEndpointsRsp;
		NwkMgmtGetDeviceSimpDescriptor_t    NwkMgmtGetDeviceSimpDescriptor;
		NwkMgmtGetDeviceSimpDescriptorRsp_t NwkMgmtGetDeviceSimpDescriptorRsp;
		NwkMgmtGetNodeDesc_t                NwkMgmtGetNodeDesc;
		NwkMgmtGetNodeDescRsp_t             NwkMgmtGetNodeDescRsp;
		NwkMgmtCommissioningDescriptor_t    NwkMgmtCommissioningDescriptor;
		NwkMgmtClassicJoinPermitJoin_t      NwkMgmtClassicJoinPermitJoin;
		NwkMgmtClassicJoinRstTargetToFn_t   NwkMgmtClassicJoinRstTargetToFn;
		NwkMgmtDeviceLeaveNetworkInd_t      NwkMgmtDeviceLeaveNetworkInd;
		NwkMgmtGetPhyChannelRsp_t           NwkMgmtGetPhyChannelRsp;
		NwkMgmtLinkRsp_t                    NwkMgmtLinkRsp;
		NwkMgmtTouchLinkRsp_t               NwkMgmtTouchLinkRsp;
	};
}NwkMgmt_t;

//********************************************************************//
typedef union PACK{
    // Groups cluster
    ZCL_AddGroup_t                   addGroup;
    ZCL_ViewGroup_t                  viewGroup;
    ZCL_GetGroupMembershipExt_t      getGroupMembership;
    ZCL_RemoveGroup_t                removeGroup;
    ZCL_AddGroupIfIdentifying_t      addGroupIfIdentifying;
    ZCL_AddGroupResponse_t           addGroupResp;
    ZCL_ViewGroupResponse_t          viewGroupResp;
    ZCL_GetGroupMembershipResponse_t getGroupMembershipResp;
    ZCL_RemoveGroupResponse_t        removeGroupResp;

    // Scenes cluster
    ZCL_AddScene_t                   addScene;
    ZCL_ViewScene_t                  viewScene;
    ZCL_RemoveScene_t                removeScene;
    ZCL_RemoveAllScenes_t            removeAllScenes;
    ZCL_StoreScene_t                 storeScene;

    ZCL_GetSceneMembership_t         getSceneMembership;
    ZCL_EnhancedAddScene_t           enhancedAddScene;
    ZCL_EnhancedViewScene_t          enhancedViewScene;
    ZCL_CopyScene_t                  copyScene;
    ZCL_AddSceneResponse_t           addSceneResp;
    ZCL_EnhancedViewSceneResponse_t  enhancedViewSceneResp;
    ZCL_RemoveSceneResponse_t        removeSceneResp;
    ZCL_RemoveAllScenesResponse_t    removeAllScenesResp;
    ZCL_StoreSceneResponse_t         storeSceneResp;
    ZCL_GetSceneMembershipResponse_t getSceneMembershipResp;
    ZCL_CopySceneResponse_t          copySceneResp;

}GroupSceneCmd_t;

typedef union PACK{
    uint8_t result;
}GroupSceneCmdDefaultRsp_t;

typedef struct{
	uint16_t profileId;
	uint16_t clusterId;
	uint8_t  addrMode;
	uint8_t  address;
	uint8_t  endpoint;
	uint8_t commandId;
	union PACK{
		  GroupSceneCmd_t               GroupSceneCmd;
		  GroupSceneCmdDefaultRsp_t     GroupSceneCmdDefaultRsp;
		  uint8_t payload[MAX_PAYLOAD_SIZE - HEAD_SIZE_GROUP_SCENE_MGMT];
	};
}GroupSceneMgmt_t;

//********************************************************************//
typedef struct PACK
{
  uint8_t boundDevicesAmount;
} ZCL_ReadyToTransmit_t;

typedef union PACK{
    uint8_t     percentage;
    uint16_t    value;
    // On/Off cluster
    ZCL_OffWithEffect_t   offWithEffect;
    ZCL_OnWithTimedOff_t  onWithTimedOff;

    // Level Control cluster
    ZCL_MoveToLevel_t     moveToLevel;
    ZCL_Move_t            moveLevel;
    ZCL_Step_t            stepLevel;

    // Color Control cluster
    ZCL_ZllMoveToHueCommand_t         moveToHue;
    ZCL_ZllMoveHueCommand_t           moveHue;
    ZCL_ZllStepHueCommand_t           stepHue;
    ZCL_ZllMoveToSaturationCommand_t  moveToSaturation;
    ZCL_ZllMoveSaturationCommand_t    moveSaturation;
    ZCL_ZllStepSaturationCommand_t    stepSaturation;
    ZCL_ZllMoveToHueAndSaturationCommand_t moveToHueAndSaturation;
    ZCL_ZllMoveToColorCommand_t       moveToColor;
    ZCL_ZllMoveColorCommand_t         moveColor;
    ZCL_ZllStepColorCommand_t         stepColor;
    ZCL_ZllEnhancedMoveToHueCommand_t enhancedMoveToHue;
    ZCL_ZllEnhancedMoveHueCommand_t   enhancedMoveHue;
    ZCL_ZllEnhancedStepHueCommand_t   enhancedStepHue;
    ZCL_ZllEnhancedMoveToHueAndSaturationCommand_t enhancedMoveToHueAndSaturation;
    ZCL_ZllColorLoopSetCommand_t      colorLoopSet;
    ZCL_ZllMoveToColorTemperatureCommand_t moveToColorTemperature;
    ZCL_ZllMoveColorTemperatureCommand_t moveColorTemperature;
    ZCL_ZllStepColorTemperatureCommand_t stepColorTemperature;

    ZCL_RecallScene_t                recallScene;
    // Identify cluster
    ZCL_Identify_t              identify;
    ZCL_TriggerEffect_t         triggerEffect;
    ZCL_IdentifyQueryResponse_t identifyQueryResp;

    // Commissioning cluster
    ZCL_GetGroupIdentifiers_t         getGroupIdentifiers;
    ZCL_GetEndpointList_t             getEndpointList;
    ZCL_EndpointInformation_t         endpointInformation;
    ZCL_GetGroupIdentifiersResponse_t getGroupIdentifiersResp;
    ZCL_GetEndpointListResponse_t     getEndpointListResp;

    // Link Info Cluster
    ZCL_ReadyToTransmit_t readyToTransmit;
    // Read attribute request
    ZCL_ReadAttributeReq_t    readAttribute;
    ZCL_WriteAttributeReq_t   writeAttribute;
    uint8_t writeAttributesUndivided[sizeof(ZCL_WriteAttributeReq_t) * 2 - 2 + sizeof(uint32_t) * 2];
    uint8_t payload[20];
}DeviceCmdOprt_t;

typedef struct PACK{
    uint8_t result;
}DeviceCmdOprtRsp_t;

typedef struct{
	uint16_t profileId;
	uint16_t clusterId;
	uint8_t  addrMode;
	uint8_t  address;
	uint8_t  endpoint;
	uint8_t commandId;
	union PACK{
		DeviceCmdOprt_t         DeviceCmdOprt;
		DeviceCmdOprtRsp_t      DeviceCmdOprtRsp;
	};
}DeviceOprt_t;
//********************************************************************//
typedef struct PACK{
  uint8_t attributeNum;
  ZCL_AttributeId_t id[CM_MAX_READ_ATTRIBUTE_COUNT]; //!< Requested attribute id
} ReadAttributeReq_t;

typedef struct PACK{
    uint8_t result;
    uint8_t dataLength;
    uint8_t payload[READ_ATTRIBUTES_PAYLOAD_SIZE];
}AttributeOprtRsp_t;

typedef struct PACK{
    uint16_t attributeId;
    uint8_t status;
    uint8_t type;
    union{
    	uint8_t  value8;
    	uint16_t value16;
    	uint32_t value32;
    	uint64_t value64;
    	struct{//string
    		uint8_t length;
    		uint8_t payload[20];
    	};
    };
}AttributeOprtRspStruct_t;


typedef struct PACK{
    uint8_t attributeNum;
    uint8_t value[MAX_PAYLOAD_SIZE - HEAD_SIZE_DEVICE_STATE -1];
}WriteAttributeReq_t;

typedef struct PACK{
    uint8_t result;
}DeviceStateOprtRsp_t;

typedef struct{
	uint16_t profileId;
	uint16_t clusterId;
	uint8_t  addrMode;
	uint8_t  address;
	uint8_t  endpoint;
	uint8_t commandId;
	union PACK{
		ReadAttributeReq_t         ReadAttributeReq;
		WriteAttributeReq_t        WriteAttributeReq;
		AttributeOprtRsp_t         AttributeOprtRsp;
	};
}DeviceStateMgmt_t;
//********************************************************************//

typedef struct{
	uint16_t profileId;
	uint16_t clusterId;
	uint8_t  addrMode;
	uint8_t  address;
	uint8_t  endpoint;
	uint16_t deviceId;//New Added
	uint8_t  length;
	uint8_t  payload[MAX_PAYLOAD_SIZE - HEAD_SIZE_SENROR_REPORT_STATE];
}SensorReportMgmt_t;
//********************************************************************//

typedef struct {
	uint16_t    sof;
	uint16_t    messageId;
	uint8_t     messageType;
	uint8_t 	sumFrames;
	uint8_t 	currFrameNo;
    uint8_t     encrypted;
    uint16_t    packageLength;
}network_header_t;

typedef struct PACK{
	union{
		struct{
			uint8_t sof;
			uint8_t encrypt;
			uint8_t length;
			uint8_t packType;
			union PACK{
				GatewayMgmt_t 		GatewayMgmt;	//Gateway Management
				OgnzMgmt_t	  		OgnzMgmt;		//Organization Management
				NwkMgmt_t			NwkMgmt;		//Network Management
				GroupSceneMgmt_t	GroupSceneMgmt; //Group & Scene Management
				DeviceOprt_t		DeviceOprt;		//Device Operation（control）
				DeviceStateMgmt_t	DeviceStateMgmt;//Device State Query
				SensorReportMgmt_t	SensorReportMgmt;//Device State Report
				uint8_t subPayload[MAX_PAYLOAD_SIZE - 5];
			};
		};
		uint8_t payload[MAX_PAYLOAD_SIZE];
	};
	uint16_t timeout;
	struct sockaddr sourceAddr;//for UDP
    network_header_t    networkHeader;
	uint8_t 	        packTypeId;
	uint8_t 	        command;
	void (*cmd_response_callback)(Cmd_Response_t *resp, struct sockaddr sourceAddr,network_header_t networkHeader,uint8_t packTypeId, uint8_t command);
}AppCmdDescriptor_t;

END_PACK
#endif /* CMD_DESCRIPTOR_H_ */
