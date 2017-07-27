#include "zcl_header.h"
#include "include/cmd_descriptor.h"
#include "iot_debug.h"

#define GW_INFO_QUERY_TIMEOUT  (50)

void zigbee_gateway_query_gateway_info(appCmdDescriptor_t *cmdDesc)
{
	cmdDesc->length = HEAD_SIZE_GATEWAY_MGMT;
	cmdDesc->packType = PACK_TYPE_GATEWAY_MANAGEMENT;
	cmdDesc->GatewayMgmt.commandId = GATEWAY_MGMT_GET_GATEWAY_NAME;
	cmdDesc->timeout = GW_INFO_QUERY_TIMEOUT;
	//cmdDesc->cmd_response_callback = NULL;
}


