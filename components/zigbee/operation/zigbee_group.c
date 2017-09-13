#include "zcl_header.h"
#include "include/cmd_descriptor.h"
#include "iot_debug.h"

#define GROUP_OPT_TIMEOUT  (500)
//ZCL_GROUPS_CLUSTER_ADD_GROUP_COMMAND_ID;
//ZCL_GROUPS_CLUSTER_REMOVE_GROUP_COMMAND_ID;
//ZCL_GROUPS_CLUSTER_REMOVE_ALL_GROUPS_COMMAND_ID "groupAddress is invalid"
void ZigbeeGroup_AddRemoveGroup(AppCmdDescriptor_t *cmdDesc,uint8_t address,uint8_t endpoint,uint16_t groupAddress, uint8_t command)
{
	cmdDesc->packType = PACK_TYPE_DEVICE_GROUP_SCENE_MANAGEMENT;
	cmdDesc->GroupSceneMgmt.clusterId = GROUPS_CLUSTER_ID;
	cmdDesc->GroupSceneMgmt.addrMode = APS_SHORT_ADDRESS;
	cmdDesc->GroupSceneMgmt.address = address;
	cmdDesc->GroupSceneMgmt.endpoint = endpoint;
	cmdDesc->GroupSceneMgmt.commandId = command;
	if (command == ZCL_GROUPS_CLUSTER_ADD_GROUP_COMMAND_ID || command == ZCL_GROUPS_CLUSTER_ADD_GROUP_IF_IDENTIFYING_COMMAND_ID){
		cmdDesc->GroupSceneMgmt.GroupSceneCmd.addGroup.groupId = groupAddress;
		cmdDesc->GroupSceneMgmt.GroupSceneCmd.addGroup.groupName[0] = 0;
		cmdDesc->length = HEAD_SIZE_GROUP_SCENE_MGMT + sizeof(ZCL_AddGroup_t);
	}else if(command == ZCL_GROUPS_CLUSTER_REMOVE_GROUP_COMMAND_ID){
		cmdDesc->GroupSceneMgmt.GroupSceneCmd.removeGroup.groupId = groupAddress;
		cmdDesc->length = HEAD_SIZE_GROUP_SCENE_MGMT + sizeof(ZCL_RemoveGroup_t);
	}else if (command == ZCL_GROUPS_CLUSTER_REMOVE_ALL_GROUPS_COMMAND_ID){
		cmdDesc->length = HEAD_SIZE_GROUP_SCENE_MGMT;
	}
	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = GROUP_OPT_TIMEOUT;
}






