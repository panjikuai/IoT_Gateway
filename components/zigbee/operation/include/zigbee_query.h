/*
 * zigbee_info_query.h
 *
 * Created: 2015-05-11 16:13:30
 *  Author: matt.qian
 */ 

#include <stdint.h>

#ifndef ZIGBEE_INFO_QUERY_H_
#define ZIGBEE_INFO_QUERY_H_

#include "cmd_descriptor.h"

void ZigbeeQuery_OnOff(AppCmdDescriptor_t *cmdDesc,uint8_t address,uint8_t endpoint);
void ZigbeeQuery_Level(AppCmdDescriptor_t *cmdDesc,uint8_t address,uint8_t endpoint);
void ZigbeeQuery_Color(AppCmdDescriptor_t *cmdDesc,uint8_t address,uint8_t endpoint);
void ZigbeeQuery_ExtendedColor(AppCmdDescriptor_t *cmdDesc,uint8_t address,uint8_t endpoint);
void ZigbeeQuery_ColorTemperature(AppCmdDescriptor_t *cmdDesc,uint8_t address,uint8_t endpoint);
uint8_t zigbee_query_get_datetype_length(uint8_t typeId);

#endif /* ZIGBEE_INFO_QUERY_H_ */
