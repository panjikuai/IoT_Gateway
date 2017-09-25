#include "zcl_header.h"
#include "include/cmd_descriptor.h"
#include "iot_debug.h"

#define DEVICE_QUERY_TIMEOUT  (500)

void ZigbeeQuery_OnOff(AppCmdDescriptor_t *cmdDesc,uint8_t address,uint8_t endpoint)
{
	cmdDesc->packType = PACK_TYPE_DEVICE_STATE_MANAGEMENT;
	cmdDesc->DeviceStateMgmt.clusterId = ONOFF_CLUSTER_ID;
	cmdDesc->DeviceStateMgmt.addrMode = APS_SHORT_ADDRESS;
	cmdDesc->DeviceStateMgmt.address = address;
	cmdDesc->DeviceStateMgmt.endpoint = endpoint;
	cmdDesc->DeviceStateMgmt.commandId = ZCL_READ_ATTRIBUTES_COMMAND_ID;
	cmdDesc->DeviceStateMgmt.ReadAttributeReq.attributeNum = 1;
	cmdDesc->DeviceStateMgmt.ReadAttributeReq.id[0] = ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID;

	cmdDesc->length = HEAD_SIZE_DEVICE_STATE + 1 + cmdDesc->DeviceStateMgmt.ReadAttributeReq.attributeNum *2;
	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_QUERY_TIMEOUT;
}

void ZigbeeQuery_Level(AppCmdDescriptor_t *cmdDesc,uint8_t address,uint8_t endpoint)
{
	cmdDesc->packType = PACK_TYPE_DEVICE_STATE_MANAGEMENT;
	cmdDesc->DeviceStateMgmt.clusterId = LEVEL_CONTROL_CLUSTER_ID;
	cmdDesc->DeviceStateMgmt.addrMode = APS_SHORT_ADDRESS;
	cmdDesc->DeviceStateMgmt.address = address;
	cmdDesc->DeviceStateMgmt.endpoint = endpoint;
	cmdDesc->DeviceStateMgmt.commandId = ZCL_READ_ATTRIBUTES_COMMAND_ID;
	cmdDesc->DeviceStateMgmt.ReadAttributeReq.attributeNum = 2;
	cmdDesc->DeviceStateMgmt.ReadAttributeReq.id[0] = ZCL_LEVEL_CONTROL_CLUSTER_CURRENT_LEVEL_ATTRIBUTE_ID;
	cmdDesc->DeviceStateMgmt.ReadAttributeReq.id[1] = ZCL_LEVEL_CONTROL_CLUSTER_REMAINING_TIME_ATTRIBUTE_ID;

	cmdDesc->length = HEAD_SIZE_DEVICE_STATE + 1 + cmdDesc->DeviceStateMgmt.ReadAttributeReq.attributeNum *2;
	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_QUERY_TIMEOUT;
}

void ZigbeeQuery_Color(AppCmdDescriptor_t *cmdDesc,uint8_t address,uint8_t endpoint)
{
	cmdDesc->packType = PACK_TYPE_DEVICE_STATE_MANAGEMENT;
	cmdDesc->DeviceStateMgmt.clusterId = COLOR_CONTROL_CLUSTER_ID;
	cmdDesc->DeviceStateMgmt.addrMode = APS_SHORT_ADDRESS;
	cmdDesc->DeviceStateMgmt.address = address;
	cmdDesc->DeviceStateMgmt.endpoint = endpoint;
	cmdDesc->DeviceStateMgmt.commandId = ZCL_READ_ATTRIBUTES_COMMAND_ID;
	cmdDesc->DeviceStateMgmt.ReadAttributeReq.attributeNum = 2;
	cmdDesc->DeviceStateMgmt.ReadAttributeReq.id[0] = ZCL_ZLL_CLUSTER_CURRENT_SATURATION_SERVER_ATTRIBUTE_ID;
	cmdDesc->DeviceStateMgmt.ReadAttributeReq.id[1] = ZCL_ZLL_CLUSTER_ENHANCED_CURRENT_HUE_SERVER_ATTRIBUTE_ID;

	cmdDesc->length = HEAD_SIZE_DEVICE_STATE + 1 + cmdDesc->DeviceStateMgmt.ReadAttributeReq.attributeNum *2;
	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_QUERY_TIMEOUT;
}

void ZigbeeQuery_ExtendedColor(AppCmdDescriptor_t *cmdDesc,uint8_t address,uint8_t endpoint)
{
	cmdDesc->packType = PACK_TYPE_DEVICE_STATE_MANAGEMENT;
	cmdDesc->DeviceStateMgmt.clusterId = COLOR_CONTROL_CLUSTER_ID;
	cmdDesc->DeviceStateMgmt.addrMode = APS_SHORT_ADDRESS;
	cmdDesc->DeviceStateMgmt.address = address;
	cmdDesc->DeviceStateMgmt.endpoint = endpoint;
	cmdDesc->DeviceStateMgmt.commandId = ZCL_READ_ATTRIBUTES_COMMAND_ID;
	cmdDesc->DeviceStateMgmt.ReadAttributeReq.attributeNum = 5;
	cmdDesc->DeviceStateMgmt.ReadAttributeReq.id[0] = ZCL_ZLL_CLUSTER_CURRENT_SATURATION_SERVER_ATTRIBUTE_ID;
	cmdDesc->DeviceStateMgmt.ReadAttributeReq.id[1] = ZCL_ZLL_CLUSTER_ENHANCED_CURRENT_HUE_SERVER_ATTRIBUTE_ID;
	cmdDesc->DeviceStateMgmt.ReadAttributeReq.id[2] = ZCL_ZLL_CLUSTER_COLOR_TEMPERATURE_SERVER_ATTRIBUTE_ID;
	cmdDesc->DeviceStateMgmt.ReadAttributeReq.id[3] = ZCL_ZLL_CLUSTER_CURRENT_X_SERVER_ATTRIBUTE_ID;
	cmdDesc->DeviceStateMgmt.ReadAttributeReq.id[4] = ZCL_ZLL_CLUSTER_CURRENT_Y_SERVER_ATTRIBUTE_ID;

	cmdDesc->length = HEAD_SIZE_DEVICE_STATE + 1 + cmdDesc->DeviceStateMgmt.ReadAttributeReq.attributeNum *2;
	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_QUERY_TIMEOUT;
}

void ZigbeeQuery_ColorTemperature(AppCmdDescriptor_t *cmdDesc,uint8_t address,uint8_t endpoint)
{
	cmdDesc->packType = PACK_TYPE_DEVICE_STATE_MANAGEMENT;
	cmdDesc->DeviceStateMgmt.clusterId = COLOR_CONTROL_CLUSTER_ID;
	cmdDesc->DeviceStateMgmt.addrMode = APS_SHORT_ADDRESS;
	cmdDesc->DeviceStateMgmt.address = address;
	cmdDesc->DeviceStateMgmt.endpoint = endpoint;
	cmdDesc->DeviceStateMgmt.commandId = ZCL_READ_ATTRIBUTES_COMMAND_ID;
	cmdDesc->DeviceStateMgmt.ReadAttributeReq.attributeNum = 3;
	cmdDesc->DeviceStateMgmt.ReadAttributeReq.id[0] = ZCL_ZLL_CLUSTER_COLOR_TEMPERATURE_SERVER_ATTRIBUTE_ID;
	cmdDesc->DeviceStateMgmt.ReadAttributeReq.id[1] = ZCL_ZLL_CLUSTER_COLOR_TEMP_PHYSICAL_MIN_SERVER_ATTRIBUTE_ID;
	cmdDesc->DeviceStateMgmt.ReadAttributeReq.id[2] = ZCL_ZLL_CLUSTER_COLOR_TEMP_PHYSICAL_MAX_SERVER_ATTRIBUTE_ID;

	cmdDesc->length = HEAD_SIZE_DEVICE_STATE + 1 + cmdDesc->DeviceStateMgmt.ReadAttributeReq.attributeNum *2;
	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_QUERY_TIMEOUT;
}


//void ZigbeeQuery_Color(AppCmdDescriptor_t *cmdDesc,uint8_t address,uint8_t endpoint)
//{
//	cmdDesc->packType = PACK_TYPE_DEVICE_STATE_MANAGEMENT;
//	cmdDesc->DeviceStateMgmt.clusterId = COLOR_CONTROL_CLUSTER_ID;
//	cmdDesc->DeviceStateMgmt.addrMode = APS_SHORT_ADDRESS;
//	cmdDesc->DeviceStateMgmt.address = address;
//	cmdDesc->DeviceStateMgmt.endpoint = endpoint;
//	cmdDesc->DeviceStateMgmt.commandId = ZCL_READ_ATTRIBUTES_COMMAND_ID;
//	cmdDesc->DeviceStateMgmt.ReadAttributeReq.attributeNum = 4;
//	cmdDesc->DeviceStateMgmt.ReadAttributeReq.id[0] = ZCL_ZLL_CLUSTER_COLOR_CAPABILITIES_SERVER_ATTRIBUTE_ID;
//	cmdDesc->DeviceStateMgmt.ReadAttributeReq.id[1] = ZCL_ZLL_CLUSTER_CURRENT_X_SERVER_ATTRIBUTE_ID;
//	cmdDesc->DeviceStateMgmt.ReadAttributeReq.id[2] = ZCL_ZLL_CLUSTER_CURRENT_Y_SERVER_ATTRIBUTE_ID;
//	cmdDesc->DeviceStateMgmt.ReadAttributeReq.id[3] = ZCL_ZLL_CLUSTER_REMAINING_TIME_SERVER_ATTRIBUTE_ID;
//
//	cmdDesc->length = HEAD_SIZE_DEVICE_STATE + 1 + cmdDesc->DeviceStateMgmt.ReadAttributeReq.attributeNum *2;
//	cmdDesc->cmd_response_callback = NULL;
//	cmdDesc->timeout = DEVICE_QUERY_TIMEOUT;
//}

#define OCTET_STRING_MAX_SIZE 255

uint8_t zigbee_query_get_datetype_length(uint8_t typeId)
{
	uint8_t length = 0;
	switch (typeId){
	case ZCL_NO_DATA_TYPE_ID:
	  break;
	//General data
	case ZCL_S8BIT_DATA_TYPE_ID:
	case ZCL_U8BIT_DATA_TYPE_ID:
	case ZCL_8BIT_DATA_TYPE_ID:
	case ZCL_BOOLEAN_DATA_TYPE_ID:
	case ZCL_8BIT_BITMAP_DATA_TYPE_ID:
	case ZCL_8BIT_ENUM_DATA_TYPE_ID:
	  length = 1;
	  break;
	case ZCL_U16BIT_DATA_TYPE_ID:
	case ZCL_S16BIT_DATA_TYPE_ID:
	case ZCL_16BIT_DATA_TYPE_ID:
	case ZCL_16BIT_BITMAP_DATA_TYPE_ID:
	case ZCL_16BIT_ENUM_DATA_TYPE_ID:
	case ZCL_CLUSTER_ID_DATA_TYPE_ID:
	case ZCL_ATTRIBUTE_ID_DATA_TYPE_ID:
	  length = 2;
	  break;
	case ZCL_U24BIT_DATA_TYPE_ID:
	case ZCL_S24BIT_DATA_TYPE_ID:
	case ZCL_24BIT_DATA_TYPE_ID:
	case ZCL_24BIT_BITMAP_DATA_TYPE_ID:
	  length = 3;
	  break;
	case ZCL_U32BIT_DATA_TYPE_ID:
	case ZCL_S32BIT_DATA_TYPE_ID:
	case ZCL_UTC_TIME_DATA_TYPE_ID:
	case ZCL_32BIT_DATA_TYPE_ID:
	case ZCL_32BIT_BITMAP_DATA_TYPE_ID:
	  length = 4;
	  break;
	case ZCL_U40BIT_DATA_TYPE_ID:
	case ZCL_S40BIT_DATA_TYPE_ID:
	case ZCL_40BIT_DATA_TYPE_ID:
	case ZCL_40BIT_BITMAP_DATA_TYPE_ID:
	  length = 5;
	  break;
	case ZCL_U48BIT_DATA_TYPE_ID:
	case ZCL_S48BIT_DATA_TYPE_ID:
	case ZCL_48BIT_DATA_TYPE_ID:
	case ZCL_48BIT_BITMAP_DATA_TYPE_ID:
	  length = 6;
	  break;
	case ZCL_U56BIT_DATA_TYPE_ID:
	case ZCL_S56BIT_DATA_TYPE_ID:
	case ZCL_56BIT_DATA_TYPE_ID:
	case ZCL_56BIT_BITMAP_DATA_TYPE_ID:
	  length = 7;
	  break;
	case ZCL_U64BIT_DATA_TYPE_ID:
	case ZCL_S64BIT_DATA_TYPE_ID:
	case ZCL_64BIT_DATA_TYPE_ID:
	case ZCL_64BIT_BITMAP_DATA_TYPE_ID:
	case ZCL_IEEE_ADDRESS_DATA_TYPE_ID:
	  length = 8;
	  break;
	case ZCL_128BIT_SECURITY_KEY_DATA_TYPE_ID:
	  length = 16;
	  break;
	case ZCL_OCTET_STRING_DATA_TYPE_ID:
	case ZCL_CHARACTER_STRING_DATA_TYPE_ID:
		length = OCTET_STRING_MAX_SIZE;
	  break;
	default:
	  length = 0;
	  break;
	}
	return length;
}


