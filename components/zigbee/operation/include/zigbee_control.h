/*
 * zigbee_control.h
 *
 * Created: 2015-07-20 11:53:36
 *  Author: matt.qian
 */ 


#ifndef ZIGBEE_CONTROL_H_
#define ZIGBEE_CONTROL_H_
#include "cmd_descriptor.h"

void zigbee_ctrl_on_off(appCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t commandId);

void zigbee_ctrl_move_to_level(appCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t commandId,uint8_t level, uint16_t transitionTime);
void zigbee_ctrl_move_level(appCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t commandId,uint8_t moveMode, uint8_t rate);
void zigbee_ctrl_step_level(appCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t commandId,uint8_t stepMode, uint8_t stepSize, uint16_t transitionTime);
void zigbee_ctrl_stop_level(appCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t commandId);

void zigbee_ctrl_move_to_hue_and_saturation(appCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t hue, uint8_t saturation, uint16_t transitionTime);
void zigbee_ctrl_enhanced_move_to_hue_and_saturation(appCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint16_t hue, uint8_t saturation, uint16_t transitionTime);
void zigbee_ctrl_move_hue(appCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t moveMode, uint8_t rate);
void zigbee_ctrl_enhanced_move_hue(appCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t moveMode, uint16_t rate);
void zigbee_ctrl_step_hue(appCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t stepMode, uint8_t stepSize, uint8_t transitionTime);
void zigbee_ctrl_enhanced_step_hue(appCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t stepMode, uint16_t stepSize, uint16_t transitionTime);
void zigbee_ctrl_move_to_color(appCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint16_t colorX, uint16_t colorY, uint16_t transitionTime);
void zigbee_ctrl_move_to_colorTemperature(appCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint16_t colorTemperature, uint16_t transitionTime);
void zigbee_ctrl_move_colorTemperature(appCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t  moveMode, uint16_t rate,uint16_t colorTemperatureMinimum,uint16_t colorTemperatureMaximum);
void zigbee_ctrl_step_colorTemperature(appCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t  stepMode, uint16_t stepSize,uint16_t transitionTime,uint16_t colorTemperatureMinimum,uint16_t colorTemperatureMaximum);
void zigbee_ctrl_recall_scene(appCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint16_t groupId, uint8_t sceneId);

#endif /* ZIGBEE_CONTROL_H_ */
