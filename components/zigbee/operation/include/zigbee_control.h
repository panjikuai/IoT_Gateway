/*
 * zigbee_control.h
 *
 * Created: 2015-07-20 11:53:36
 *  Author: matt.qian
 */ 


#ifndef ZIGBEE_CONTROL_H_
#define ZIGBEE_CONTROL_H_
#include "cmd_descriptor.h"

void ZigbeeCtrl_OnOff(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t commandId);

void ZigbeeCtrl_MoveToLevel(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t commandId,uint8_t level, uint16_t transitionTime);
void ZigbeeCtrl_MoveLevel(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t commandId,uint8_t moveMode, uint8_t rate);
void ZigbeeCtrl_StepLevel(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t commandId,uint8_t stepMode, uint8_t stepSize, uint16_t transitionTime);
void ZigbeeCtrl_StopLevel(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t commandId);

void ZigbeeCtrl_MoveToHueAndSaturation(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t hue, uint8_t saturation, uint16_t transitionTime);
void ZigbeeCtrl_EnhancedMoveToHueAndSaturation(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint16_t hue, uint8_t saturation, uint16_t transitionTime);
void ZigbeeCtrl_MoveHue(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t moveMode, uint8_t rate);
void ZigbeeCtrl_EnhancedMoveHue(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t moveMode, uint16_t rate);
void ZigbeeCtrl_StepHue(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t stepMode, uint8_t stepSize, uint8_t transitionTime);
void ZigbeeCtrl_EnhancedStepHue(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t stepMode, uint16_t stepSize, uint16_t transitionTime);
void ZigbeeCtrl_MoveToColor(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint16_t colorX, uint16_t colorY, uint16_t transitionTime);
void ZigbeeCtrl_MoveToColorTemperature(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint16_t colorTemperature, uint16_t transitionTime);
void ZigbeeCtrl_MoveColorTemperature(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t  moveMode, uint16_t rate,uint16_t colorTemperatureMinimum,uint16_t colorTemperatureMaximum);
void ZigbeeCtrl_StepColorTemperature(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t  stepMode, uint16_t stepSize,uint16_t transitionTime,uint16_t colorTemperatureMinimum,uint16_t colorTemperatureMaximum);
void ZigbeeCtrl_RecallScene(AppCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint16_t groupId, uint8_t sceneId);

#endif /* ZIGBEE_CONTROL_H_ */
