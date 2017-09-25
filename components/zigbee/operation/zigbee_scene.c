#include "zcl_header.h"
#include "include/cmd_descriptor.h"
#include "iot_debug.h"


#define SCENE_OPT_TIMEOUT  (500)
//#define ZCL_SCENES_CLUSTER_ADD_SCENE_COMMAND_ID            0x00
//#define ZCL_SCENES_CLUSTER_REMOVE_SCENE_COMMAND_ID         0x02
//#define ZCL_SCENES_CLUSTER_REMOVE_ALL_SCENES_COMMAND_ID    0x03
//#define ZCL_SCENES_CLUSTER_STORE_SCENE_COMMAND_ID          0x04
void ZigbeeScene_AddRemoveStoreScene(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t command,uint16_t groupAddress, uint8_t scene, uint16_t transitionTime)
{
	cmdDesc->packType = PACK_TYPE_DEVICE_GROUP_SCENE_MANAGEMENT;
	cmdDesc->GroupSceneMgmt.clusterId = SCENES_CLUSTER_ID;
	cmdDesc->GroupSceneMgmt.addrMode = addrMode;
	cmdDesc->GroupSceneMgmt.address = address;
	cmdDesc->GroupSceneMgmt.endpoint = endpoint;
	cmdDesc->GroupSceneMgmt.commandId = command;
	if (command == ZCL_SCENES_CLUSTER_ADD_SCENE_COMMAND_ID ){
		cmdDesc->GroupSceneMgmt.GroupSceneCmd.addScene.groupId = groupAddress;
		cmdDesc->GroupSceneMgmt.GroupSceneCmd.addScene.sceneId = scene;
		cmdDesc->GroupSceneMgmt.GroupSceneCmd.addScene.transitionTime = transitionTime;
		cmdDesc->GroupSceneMgmt.GroupSceneCmd.addScene.name[0] = 0;
		cmdDesc->length = HEAD_SIZE_GROUP_SCENE_MGMT + sizeof(ZCL_AddScene_t);
	}else if(command == ZCL_SCENES_CLUSTER_REMOVE_SCENE_COMMAND_ID || command == ZCL_SCENES_CLUSTER_STORE_SCENE_COMMAND_ID){
		cmdDesc->GroupSceneMgmt.GroupSceneCmd.removeScene.groupId = groupAddress;
		cmdDesc->GroupSceneMgmt.GroupSceneCmd.removeScene.sceneId = scene;
		cmdDesc->length = HEAD_SIZE_GROUP_SCENE_MGMT + sizeof(ZCL_RemoveScene_t);
	}else if (command == ZCL_SCENES_CLUSTER_REMOVE_ALL_SCENES_COMMAND_ID){
		cmdDesc->GroupSceneMgmt.GroupSceneCmd.removeAllScenes.groupId = groupAddress;
		cmdDesc->length = HEAD_SIZE_GROUP_SCENE_MGMT + sizeof(ZCL_RemoveAllScenes_t);
	}
	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = SCENE_OPT_TIMEOUT;
}

void ZigbeeScene_EnhancedAddScene(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint16_t groupAddress, uint8_t scene, uint16_t transitionTime,
		uint8_t onoff, uint8_t level, uint16_t x, uint16_t y, uint16_t enhanceHue, uint8_t saturation)
{
	cmdDesc->packType = PACK_TYPE_DEVICE_GROUP_SCENE_MANAGEMENT;
	cmdDesc->GroupSceneMgmt.clusterId = SCENES_CLUSTER_ID;
	cmdDesc->GroupSceneMgmt.addrMode = addrMode;
	cmdDesc->GroupSceneMgmt.address = address;
	cmdDesc->GroupSceneMgmt.endpoint = endpoint;
	cmdDesc->GroupSceneMgmt.commandId = ZCL_SCENES_CLUSTER_ENHANCED_ADD_SCENE_COMMAND_ID;

	cmdDesc->GroupSceneMgmt.GroupSceneCmd.enhancedAddScene.groupId = groupAddress;
	cmdDesc->GroupSceneMgmt.GroupSceneCmd.enhancedAddScene.sceneId = scene;
	cmdDesc->GroupSceneMgmt.GroupSceneCmd.enhancedAddScene.transitionTime = transitionTime;
	cmdDesc->GroupSceneMgmt.GroupSceneCmd.enhancedAddScene.name[0] = 0;

	cmdDesc->GroupSceneMgmt.GroupSceneCmd.enhancedAddScene.onOffClusterExtFields.clusterId = ONOFF_CLUSTER_ID;
	cmdDesc->GroupSceneMgmt.GroupSceneCmd.enhancedAddScene.onOffClusterExtFields.length = sizeof(ZCL_OnOffClusterExtensionFieldSet_t) -3;
	cmdDesc->GroupSceneMgmt.GroupSceneCmd.enhancedAddScene.onOffClusterExtFields.onOffValue = onoff;

	cmdDesc->GroupSceneMgmt.GroupSceneCmd.enhancedAddScene.levelControlClusterExtFields.clusterId = LEVEL_CONTROL_CLUSTER_ID;
	cmdDesc->GroupSceneMgmt.GroupSceneCmd.enhancedAddScene.levelControlClusterExtFields.length = sizeof(ZCL_LevelControlClusterExtensionFieldSet_t) -3;
	cmdDesc->GroupSceneMgmt.GroupSceneCmd.enhancedAddScene.levelControlClusterExtFields.currentLevel = level;

	cmdDesc->GroupSceneMgmt.GroupSceneCmd.enhancedAddScene.colorControlClusterExtFields.clusterId = SCENES_CLUSTER_ID;
	cmdDesc->GroupSceneMgmt.GroupSceneCmd.enhancedAddScene.colorControlClusterExtFields.length = sizeof(ZCL_ColorControlClusterExtensionFieldSet_t) -3;
	cmdDesc->GroupSceneMgmt.GroupSceneCmd.enhancedAddScene.colorControlClusterExtFields.currentX = x;
	cmdDesc->GroupSceneMgmt.GroupSceneCmd.enhancedAddScene.colorControlClusterExtFields.currentY = y;
	cmdDesc->GroupSceneMgmt.GroupSceneCmd.enhancedAddScene.colorControlClusterExtFields.enhancedCurrentHue = enhanceHue;
	cmdDesc->GroupSceneMgmt.GroupSceneCmd.enhancedAddScene.colorControlClusterExtFields.currentSaturation = saturation;
	cmdDesc->GroupSceneMgmt.GroupSceneCmd.enhancedAddScene.colorControlClusterExtFields.colorLoopActive = 0;
	cmdDesc->length = HEAD_SIZE_GROUP_SCENE_MGMT + sizeof(ZCL_EnhancedAddScene_t);

	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = SCENE_OPT_TIMEOUT;
}

