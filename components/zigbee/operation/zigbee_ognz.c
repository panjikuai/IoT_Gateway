#include "zcl_header.h"
#include "include/cmd_descriptor.h"
#include "iot_debug.h"

#define INFO_QUERY_TIMEOUT  (50)

void zigbee_ognz_query_gateway_info(appCmdDescriptor_t *cmdDesc)
{
	cmdDesc->length = HEAD_SIZE_GATEWAY_MGMT;
	cmdDesc->packType = PACK_TYPE_GATEWAY_MANAGEMENT;
	cmdDesc->GatewayMgmt.commandId = GATEWAY_MGMT_GET_GATEWAY_NAME;
	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = INFO_QUERY_TIMEOUT;
}

void zigbee_ognz_query_device_group_scene_list(appCmdDescriptor_t *cmdDesc,uint8_t command)
{
	cmdDesc->length = HEAD_SIZE_OGNZ_MGMT;
	cmdDesc->packType = PACK_TYPE_ORGANIZATION_DESCRIPTION;
	cmdDesc->OgnzMgmt.commandId = command;
	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = INFO_QUERY_TIMEOUT;
}

// OGNZ_MGMT_GET_DEVICE_INFO        = 0x00,
// OGNZ_MGMT_GET_GROUP_INFO         = 0x01,
// OGNZ_MGMT_GET_SCENE_INFO         = 0x02,
void zigbee_ognz_query_device_group_scene_info(appCmdDescriptor_t *cmdDesc,uint8_t command,uint8_t address)
{
	cmdDesc->length = HEAD_SIZE_OGNZ_MGMT + sizeof(OgnzMgmtQuery_t);
	cmdDesc->packType = PACK_TYPE_ORGANIZATION_DESCRIPTION;
	cmdDesc->OgnzMgmt.commandId = command;
	cmdDesc->OgnzMgmt.OgnzMgmtQuery.address = address;
	cmdDesc->OgnzMgmt.OgnzMgmtQuery.endpoint = 0;
	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = INFO_QUERY_TIMEOUT;
}

void zigbee_ognz_query_zgp_device_list(appCmdDescriptor_t *cmdDesc)
{
	cmdDesc->length = HEAD_SIZE_OGNZ_MGMT;
	cmdDesc->packType = PACK_TYPE_ORGANIZATION_DESCRIPTION;
	cmdDesc->OgnzMgmt.commandId = OGNZ_MGMT_GET_ZGP_DEVICE_LIST;
	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = INFO_QUERY_TIMEOUT;
}

void zigbee_ognz_query_zgp_device_info(appCmdDescriptor_t *cmdDesc,uint8_t address)
{
	cmdDesc->length = HEAD_SIZE_OGNZ_MGMT + sizeof(OgnzMgmtZgpDeviceInfoQuery_t);
	cmdDesc->packType = PACK_TYPE_ORGANIZATION_DESCRIPTION;
	cmdDesc->OgnzMgmt.commandId = OGNZ_MGMT_GET_ZGP_DEVICE_INFO;
	cmdDesc->OgnzMgmt.OgnzMgmtZgpDeviceInfoQuery.address = address;
	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = INFO_QUERY_TIMEOUT;
}

