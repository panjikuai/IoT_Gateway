/*
 * scene.h
 *
 * Created: 2015-04-16 14:00:27
 *  Author: matt.qian
 */ 


#ifndef SCENE_H_
#define SCENE_H_

#include "cmd_descriptor.h"

void zigbee_scene_add_remove_store_scene(appCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint8_t command,uint16_t groupAddress, uint8_t scene,
										 uint16_t transitionTime);
void zigbee_scene_enhanced_add_scene(appCmdDescriptor_t *cmdDesc,APS_AddrMode_t addrMode,uint8_t address,uint8_t endpoint,uint16_t groupAddress, uint8_t scene, uint16_t transitionTime,
									 uint8_t onoff, uint8_t level, uint16_t x, uint16_t y, uint16_t enhanceHue, uint8_t saturation);


#endif /* SCENE_H_ */
