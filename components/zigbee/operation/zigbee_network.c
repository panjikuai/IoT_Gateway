#include "zcl_header.h"
#include "include/cmd_descriptor.h"
#include "iot_debug.h"

#define NETWORK_OPT_TIMEOUT  (500)

void zigbee_network_change_phy_channel(AppCmdDescriptor_t *cmdDesc)
{
	cmdDesc->length = HEAD_SIZE_NWK_MGMT;
	cmdDesc->packType = PACK_TYPE_DEVICE_NETWORK_MANAGEMENT;
	cmdDesc->NwkMgmt.commandId = NWK_MGMT_SET_PHY_CHANNEL;
	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = NETWORK_OPT_TIMEOUT;
}

void zigbee_network_reset_gateway_to_fn(AppCmdDescriptor_t *cmdDesc)
{
	cmdDesc->length = HEAD_SIZE_NWK_MGMT;
	cmdDesc->packType = PACK_TYPE_DEVICE_NETWORK_MANAGEMENT;
	cmdDesc->NwkMgmt.commandId = NWK_MGMT_RST_GATEWAY_TO_FN;
	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = NETWORK_OPT_TIMEOUT;
}


void zigbee_network_classic_permit_join(AppCmdDescriptor_t *cmdDesc)
{
	cmdDesc->length = HEAD_SIZE_NWK_MGMT + sizeof(NwkMgmtClassicJoinPermitJoin_t);
	cmdDesc->packType = PACK_TYPE_DEVICE_NETWORK_MANAGEMENT;
	cmdDesc->NwkMgmt.commandId = NWK_MGMT_CLASSIC_JOIN_PERMIT_JOIN;
	cmdDesc->NwkMgmt.NwkMgmtClassicJoinPermitJoin.enableMacJoin = 0;
	cmdDesc->NwkMgmt.NwkMgmtClassicJoinPermitJoin.macAddress = 0;
	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = NETWORK_OPT_TIMEOUT;
}

void zigbee_network_touchlink(AppCmdDescriptor_t *cmdDesc)
{
	cmdDesc->length = HEAD_SIZE_NWK_MGMT;
	cmdDesc->packType = PACK_TYPE_DEVICE_NETWORK_MANAGEMENT;
	cmdDesc->NwkMgmt.commandId = NWK_MGMT_TOUCHLINK;
	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = NETWORK_OPT_TIMEOUT*15;
}

void zigbee_network_reset_device_to_fn(AppCmdDescriptor_t *cmdDesc,uint8_t address, uint8_t endpoint)
{
	cmdDesc->length = HEAD_SIZE_NWK_MGMT + sizeof(NwkMgmtClassicJoinRstTargetToFn_t);
	cmdDesc->packType = PACK_TYPE_DEVICE_NETWORK_MANAGEMENT;
	cmdDesc->NwkMgmt.commandId = NWK_MGMT_CLASSIC_JOIN_RST_LIGHT_TO_FN;
	cmdDesc->NwkMgmt.NwkMgmtClassicJoinRstTargetToFn.address = address;
	cmdDesc->NwkMgmt.NwkMgmtClassicJoinRstTargetToFn.endpoint = endpoint;

	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = NETWORK_OPT_TIMEOUT;
}

