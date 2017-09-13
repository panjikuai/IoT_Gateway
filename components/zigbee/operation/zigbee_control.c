#include "zcl_header.h"
#include "include/cmd_descriptor.h"
#include "iot_debug.h"

#define DEVICE_OPT_TIMEOUT  (500)

//#define ZCL_ONOFF_CLUSTER_OFF_COMMAND_ID 0x00
//#define ZCL_ONOFF_CLUSTER_ON_COMMAND_ID 0x01
//#define ZCL_ONOFF_CLUSTER_TOGGLE_COMMAND_ID 0x02
void zigbee_ctrl_on_off(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t commandId)
{
	cmdDesc->length = HEAD_SIZE_DEVICE_OPT;
	cmdDesc->packType = PACK_TYPE_DEVICE_CONTROL;
	cmdDesc->DeviceOprt.profileId = PROFILE_ID_LIGHT_LINK;
	cmdDesc->DeviceOprt.clusterId = ONOFF_CLUSTER_ID;
	cmdDesc->DeviceOprt.addrMode = addrMode;
	cmdDesc->DeviceOprt.address = address;
	cmdDesc->DeviceOprt.endpoint = endpoint;
	cmdDesc->DeviceOprt.commandId = commandId;

	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_OPT_TIMEOUT;
}

//#define ZCL_LEVEL_CONTROL_CLUSTER_MOVE_TO_LEVEL_COMMAND_ID          0x00
//#define ZCL_LEVEL_CONTROL_CLUSTER_MOVE_TO_LEVEL_W_ONOFF_COMMAND_ID  0x04
void zigbee_ctrl_move_to_level(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t commandId,uint8_t level, uint16_t transitionTime)
{
	cmdDesc->length = HEAD_SIZE_DEVICE_OPT + sizeof(ZCL_MoveToLevel_t);
	cmdDesc->packType = PACK_TYPE_DEVICE_CONTROL;
	cmdDesc->DeviceOprt.profileId = PROFILE_ID_LIGHT_LINK;
	cmdDesc->DeviceOprt.clusterId = LEVEL_CONTROL_CLUSTER_ID;
	cmdDesc->DeviceOprt.addrMode = addrMode;
	cmdDesc->DeviceOprt.address = address;
	cmdDesc->DeviceOprt.endpoint = endpoint;
	cmdDesc->DeviceOprt.commandId = commandId;
	cmdDesc->DeviceOprt.DeviceCmdOprt.moveToLevel.level = level;
	cmdDesc->DeviceOprt.DeviceCmdOprt.moveToLevel.transitionTime = transitionTime;

	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_OPT_TIMEOUT;
}

//#define ZCL_LEVEL_CONTROL_CLUSTER_MOVE_COMMAND_ID                   0x01
//#define ZCL_LEVEL_CONTROL_CLUSTER_MOVE_W_ONOFF_COMMAND_ID           0x05
void zigbee_ctrl_move_level(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t commandId,uint8_t moveMode, uint8_t rate)
{
	cmdDesc->length = HEAD_SIZE_DEVICE_OPT + sizeof(ZCL_Move_t);
	cmdDesc->packType = PACK_TYPE_DEVICE_CONTROL;
	cmdDesc->DeviceOprt.profileId = PROFILE_ID_LIGHT_LINK;
	cmdDesc->DeviceOprt.clusterId = LEVEL_CONTROL_CLUSTER_ID;
	cmdDesc->DeviceOprt.addrMode = addrMode;
	cmdDesc->DeviceOprt.address = address;
	cmdDesc->DeviceOprt.endpoint = endpoint;
	cmdDesc->DeviceOprt.commandId = commandId;
	cmdDesc->DeviceOprt.DeviceCmdOprt.moveLevel.moveMode = moveMode;
	cmdDesc->DeviceOprt.DeviceCmdOprt.moveLevel.rate = rate;

	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_OPT_TIMEOUT;
}

//#define ZCL_LEVEL_CONTROL_CLUSTER_STEP_COMMAND_ID                   0x02
//#define ZCL_LEVEL_CONTROL_CLUSTER_STEP_W_ONOFF_COMMAND_ID           0x06
void zigbee_ctrl_step_level(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t commandId,uint8_t stepMode, uint8_t stepSize, uint16_t transitionTime)
{
	cmdDesc->length = HEAD_SIZE_DEVICE_OPT + sizeof(ZCL_Step_t);
	cmdDesc->packType = PACK_TYPE_DEVICE_CONTROL;
	cmdDesc->DeviceOprt.profileId = PROFILE_ID_LIGHT_LINK;
	cmdDesc->DeviceOprt.clusterId = LEVEL_CONTROL_CLUSTER_ID;
	cmdDesc->DeviceOprt.addrMode = addrMode;
	cmdDesc->DeviceOprt.address = address;
	cmdDesc->DeviceOprt.endpoint = endpoint;
	cmdDesc->DeviceOprt.commandId = commandId;
	cmdDesc->DeviceOprt.DeviceCmdOprt.stepLevel.stepMode = stepMode;
	cmdDesc->DeviceOprt.DeviceCmdOprt.stepLevel.stepSize = stepSize;
	cmdDesc->DeviceOprt.DeviceCmdOprt.stepLevel.transitionTime = transitionTime;

	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_OPT_TIMEOUT;
}

//#define ZCL_LEVEL_CONTROL_CLUSTER_STOP_COMMAND_ID                   0x03
//#define ZCL_LEVEL_CONTROL_CLUSTER_STOP_W_ONOFF_COMMAND_ID           0x07
void zigbee_ctrl_stop_level(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t commandId)
{
	cmdDesc->length = HEAD_SIZE_DEVICE_OPT;
	cmdDesc->packType = PACK_TYPE_DEVICE_CONTROL;
	cmdDesc->DeviceOprt.profileId = PROFILE_ID_LIGHT_LINK;
	cmdDesc->DeviceOprt.clusterId = LEVEL_CONTROL_CLUSTER_ID;
	cmdDesc->DeviceOprt.addrMode = addrMode;
	cmdDesc->DeviceOprt.address = address;
	cmdDesc->DeviceOprt.endpoint = endpoint;
	cmdDesc->DeviceOprt.commandId = commandId;

	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_OPT_TIMEOUT;
}

void zigbee_ctrl_move_to_hue_and_saturation(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t hue, uint8_t saturation, uint16_t transitionTime)
{
	cmdDesc->length = HEAD_SIZE_DEVICE_OPT + sizeof(ZCL_ZllMoveToHueAndSaturationCommand_t);
	cmdDesc->packType = PACK_TYPE_DEVICE_CONTROL;
	cmdDesc->DeviceOprt.profileId = PROFILE_ID_LIGHT_LINK;
	cmdDesc->DeviceOprt.clusterId = COLOR_CONTROL_CLUSTER_ID;
	cmdDesc->DeviceOprt.addrMode = addrMode;
	cmdDesc->DeviceOprt.address = address;
	cmdDesc->DeviceOprt.endpoint = endpoint;
	cmdDesc->DeviceOprt.commandId = MOVE_TO_HUE_AND_SATURATION_COMMAND_ID;

	cmdDesc->DeviceOprt.DeviceCmdOprt.moveToHueAndSaturation.hue = hue;
	cmdDesc->DeviceOprt.DeviceCmdOprt.moveToHueAndSaturation.saturation = saturation;
	cmdDesc->DeviceOprt.DeviceCmdOprt.moveToHueAndSaturation.transitionTime = transitionTime;

	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_OPT_TIMEOUT;
}

void zigbee_ctrl_enhanced_move_to_hue_and_saturation(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint16_t enhancedHue, uint8_t saturation, uint16_t transitionTime)
{
	cmdDesc->length = HEAD_SIZE_DEVICE_OPT + sizeof(ZCL_ZllEnhancedMoveToHueAndSaturationCommand_t);
	cmdDesc->packType = PACK_TYPE_DEVICE_CONTROL;
	cmdDesc->DeviceOprt.profileId = PROFILE_ID_LIGHT_LINK;
	cmdDesc->DeviceOprt.clusterId = COLOR_CONTROL_CLUSTER_ID;
	cmdDesc->DeviceOprt.addrMode = addrMode;
	cmdDesc->DeviceOprt.address = address;
	cmdDesc->DeviceOprt.endpoint = endpoint;
	cmdDesc->DeviceOprt.commandId = ENHANCED_MOVE_TO_HUE_AND_SATURATION_COMMAND_ID;

	cmdDesc->DeviceOprt.DeviceCmdOprt.enhancedMoveToHueAndSaturation.enhancedHue = enhancedHue;
	cmdDesc->DeviceOprt.DeviceCmdOprt.enhancedMoveToHueAndSaturation.saturation = saturation;
	cmdDesc->DeviceOprt.DeviceCmdOprt.enhancedMoveToHueAndSaturation.transitionTime = transitionTime;

	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_OPT_TIMEOUT;
}


void zigbee_ctrl_move_hue(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t moveMode, uint8_t rate)
{
	cmdDesc->length = HEAD_SIZE_DEVICE_OPT + sizeof(ZCL_ZllMoveHueCommand_t);
	cmdDesc->packType = PACK_TYPE_DEVICE_CONTROL;
	cmdDesc->DeviceOprt.profileId = PROFILE_ID_LIGHT_LINK;
	cmdDesc->DeviceOprt.clusterId = COLOR_CONTROL_CLUSTER_ID;
	cmdDesc->DeviceOprt.addrMode = addrMode;
	cmdDesc->DeviceOprt.address = address;
	cmdDesc->DeviceOprt.endpoint = endpoint;
	cmdDesc->DeviceOprt.commandId = MOVE_HUE_COMMAND_ID;

	cmdDesc->DeviceOprt.DeviceCmdOprt.moveHue.moveMode = moveMode;
	cmdDesc->DeviceOprt.DeviceCmdOprt.moveHue.rate = rate;

	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_OPT_TIMEOUT;
}

void zigbee_ctrl_enhanced_move_hue(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t moveMode, uint16_t rate)
{
	cmdDesc->length = HEAD_SIZE_DEVICE_OPT + sizeof(ZCL_ZllEnhancedMoveHueCommand_t);
	cmdDesc->packType = PACK_TYPE_DEVICE_CONTROL;
	cmdDesc->DeviceOprt.profileId = PROFILE_ID_LIGHT_LINK;
	cmdDesc->DeviceOprt.clusterId = COLOR_CONTROL_CLUSTER_ID;
	cmdDesc->DeviceOprt.addrMode = addrMode;
	cmdDesc->DeviceOprt.address = address;
	cmdDesc->DeviceOprt.endpoint = endpoint;
	cmdDesc->DeviceOprt.commandId = ENHANCED_MOVE_HUE_COMMAND_ID;

	cmdDesc->DeviceOprt.DeviceCmdOprt.enhancedMoveHue.moveMode = moveMode;
	cmdDesc->DeviceOprt.DeviceCmdOprt.enhancedMoveHue.rate = rate;

	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_OPT_TIMEOUT;
}


void zigbee_ctrl_step_hue(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t stepMode, uint8_t stepSize, uint8_t transitionTime)
{
	cmdDesc->length = HEAD_SIZE_DEVICE_OPT + sizeof(ZCL_ZllStepHueCommand_t);
	cmdDesc->packType = PACK_TYPE_DEVICE_CONTROL;
	cmdDesc->DeviceOprt.profileId = PROFILE_ID_LIGHT_LINK;
	cmdDesc->DeviceOprt.clusterId = COLOR_CONTROL_CLUSTER_ID;
	cmdDesc->DeviceOprt.addrMode = addrMode;
	cmdDesc->DeviceOprt.address = address;
	cmdDesc->DeviceOprt.endpoint = endpoint;
	cmdDesc->DeviceOprt.commandId = STEP_HUE_COMMAND_ID;

	cmdDesc->DeviceOprt.DeviceCmdOprt.stepHue.stepMode = stepMode;
	cmdDesc->DeviceOprt.DeviceCmdOprt.stepHue.stepSize = stepSize;
	cmdDesc->DeviceOprt.DeviceCmdOprt.stepHue.transitionTime = transitionTime;

	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_OPT_TIMEOUT;
}

void zigbee_ctrl_enhanced_step_hue(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t stepMode, uint16_t stepSize, uint16_t transitionTime)
{
	cmdDesc->length = HEAD_SIZE_DEVICE_OPT + sizeof(ZCL_ZllEnhancedStepHueCommand_t);
	cmdDesc->packType = PACK_TYPE_DEVICE_CONTROL;
	cmdDesc->DeviceOprt.profileId = PROFILE_ID_LIGHT_LINK;
	cmdDesc->DeviceOprt.clusterId = COLOR_CONTROL_CLUSTER_ID;
	cmdDesc->DeviceOprt.addrMode = addrMode;
	cmdDesc->DeviceOprt.address = address;
	cmdDesc->DeviceOprt.endpoint = endpoint;
	cmdDesc->DeviceOprt.commandId = ENHANCED_STEP_HUE_COMMAND_ID;

	cmdDesc->DeviceOprt.DeviceCmdOprt.enhancedStepHue.stepMode = stepMode;
	cmdDesc->DeviceOprt.DeviceCmdOprt.enhancedStepHue.stepSize = stepSize;
	cmdDesc->DeviceOprt.DeviceCmdOprt.enhancedStepHue.transitionTime = transitionTime;

	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_OPT_TIMEOUT;
}

void zigbee_ctrl_move_to_color(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint16_t colorX, uint16_t colorY, uint16_t transitionTime)
{
	cmdDesc->length = HEAD_SIZE_DEVICE_OPT + sizeof(ZCL_ZllMoveToColorCommand_t);
	cmdDesc->packType = PACK_TYPE_DEVICE_CONTROL;
	cmdDesc->DeviceOprt.profileId = PROFILE_ID_LIGHT_LINK;
	cmdDesc->DeviceOprt.clusterId = COLOR_CONTROL_CLUSTER_ID;
	cmdDesc->DeviceOprt.addrMode = addrMode;
	cmdDesc->DeviceOprt.address = address;
	cmdDesc->DeviceOprt.endpoint = endpoint;
	cmdDesc->DeviceOprt.commandId = MOVE_TO_COLOR_COMMAND_ID;

	cmdDesc->DeviceOprt.DeviceCmdOprt.moveToColor.colorX = colorX;
	cmdDesc->DeviceOprt.DeviceCmdOprt.moveToColor.colorY = colorY;
	cmdDesc->DeviceOprt.DeviceCmdOprt.moveToColor.transitionTime = transitionTime;

	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_OPT_TIMEOUT;
}

void zigbee_ctrl_move_to_colorTemperature(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint16_t colorTemperature, uint16_t transitionTime)
{
	cmdDesc->length = HEAD_SIZE_DEVICE_OPT + sizeof(ZCL_ZllMoveToColorTemperatureCommand_t);
	cmdDesc->packType = PACK_TYPE_DEVICE_CONTROL;
	cmdDesc->DeviceOprt.profileId = PROFILE_ID_LIGHT_LINK;
	cmdDesc->DeviceOprt.clusterId = COLOR_CONTROL_CLUSTER_ID;
	cmdDesc->DeviceOprt.addrMode = addrMode;
	cmdDesc->DeviceOprt.address = address;
	cmdDesc->DeviceOprt.endpoint = endpoint;
	cmdDesc->DeviceOprt.commandId = MOVE_TO_COLOR_TEMPERATURE_COMMAND_ID;

	cmdDesc->DeviceOprt.DeviceCmdOprt.moveToColorTemperature.colorTemperature = colorTemperature;
	cmdDesc->DeviceOprt.DeviceCmdOprt.moveToColorTemperature.transitionTime = transitionTime;

	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_OPT_TIMEOUT;
}

void zigbee_ctrl_move_colorTemperature(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t  moveMode, uint16_t rate,uint16_t colorTemperatureMinimum,uint16_t colorTemperatureMaximum)
{
	cmdDesc->length = HEAD_SIZE_DEVICE_OPT + sizeof(ZCL_ZllMoveColorTemperatureCommand_t);
	cmdDesc->packType = PACK_TYPE_DEVICE_CONTROL;
	cmdDesc->DeviceOprt.profileId = PROFILE_ID_LIGHT_LINK;
	cmdDesc->DeviceOprt.clusterId = COLOR_CONTROL_CLUSTER_ID;
	cmdDesc->DeviceOprt.addrMode = addrMode;
	cmdDesc->DeviceOprt.address = address;
	cmdDesc->DeviceOprt.endpoint = endpoint;
	cmdDesc->DeviceOprt.commandId = MOVE_COLOR_TEMPERATURE_COMMAND_ID;

	cmdDesc->DeviceOprt.DeviceCmdOprt.moveColorTemperature.moveMode = moveMode;
	cmdDesc->DeviceOprt.DeviceCmdOprt.moveColorTemperature.rate = rate;
	cmdDesc->DeviceOprt.DeviceCmdOprt.moveColorTemperature.colorTemperatureMinimum = colorTemperatureMinimum;
	cmdDesc->DeviceOprt.DeviceCmdOprt.moveColorTemperature.colorTemperatureMaximum = colorTemperatureMaximum;

	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_OPT_TIMEOUT;
}

void zigbee_ctrl_step_colorTemperature(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t  stepMode, uint16_t stepSize,uint16_t transitionTime,uint16_t colorTemperatureMinimum,uint16_t colorTemperatureMaximum)
{
	cmdDesc->length = HEAD_SIZE_DEVICE_OPT + sizeof(ZCL_ZllStepColorTemperatureCommand_t);
	cmdDesc->packType = PACK_TYPE_DEVICE_CONTROL;
	cmdDesc->DeviceOprt.profileId = PROFILE_ID_LIGHT_LINK;
	cmdDesc->DeviceOprt.clusterId = COLOR_CONTROL_CLUSTER_ID;
	cmdDesc->DeviceOprt.addrMode = addrMode;
	cmdDesc->DeviceOprt.address = address;
	cmdDesc->DeviceOprt.endpoint = endpoint;
	cmdDesc->DeviceOprt.commandId = MOVE_COLOR_TEMPERATURE_COMMAND_ID;

	cmdDesc->DeviceOprt.DeviceCmdOprt.stepColorTemperature.stepMode = stepMode;
	cmdDesc->DeviceOprt.DeviceCmdOprt.stepColorTemperature.stepSize = stepSize;
	cmdDesc->DeviceOprt.DeviceCmdOprt.stepColorTemperature.transitionTime = transitionTime;
	cmdDesc->DeviceOprt.DeviceCmdOprt.stepColorTemperature.colorTemperatureMinimum = colorTemperatureMinimum;
	cmdDesc->DeviceOprt.DeviceCmdOprt.stepColorTemperature.colorTemperatureMaximum = colorTemperatureMaximum;

	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_OPT_TIMEOUT;
}

void zigbee_ctrl_recall_scene(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint16_t  groupId, uint8_t sceneId)
{
	cmdDesc->length = HEAD_SIZE_DEVICE_OPT + sizeof(ZCL_RecallScene_t);
	cmdDesc->packType = PACK_TYPE_DEVICE_CONTROL;
	cmdDesc->DeviceOprt.profileId = PROFILE_ID_LIGHT_LINK;
	cmdDesc->DeviceOprt.clusterId = SCENES_CLUSTER_ID;
	cmdDesc->DeviceOprt.addrMode = addrMode;
	cmdDesc->DeviceOprt.address = address;
	cmdDesc->DeviceOprt.endpoint = endpoint;
	cmdDesc->DeviceOprt.commandId = ZCL_SCENES_CLUSTER_RECALL_SCENE_COMMAND_ID;

	cmdDesc->DeviceOprt.DeviceCmdOprt.recallScene.groupId = groupId;
	cmdDesc->DeviceOprt.DeviceCmdOprt.recallScene.sceneId = sceneId;

	//cmdDesc->cmd_response_callback = NULL;
	cmdDesc->timeout = DEVICE_OPT_TIMEOUT;
}
