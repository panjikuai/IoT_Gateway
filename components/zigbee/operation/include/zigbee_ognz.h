#ifndef ZIGBEE_OGNZ_H_
#define ZIGBEE_OGNZ_H_

#include "cmd_descriptor.h"

void ZigbeeOgnz_QueryDeviceGroupSceneList(AppCmdDescriptor_t *cmdDesc,uint8_t command);
void ZigbeeOgnz_QueryDeviceGroupSceneInfo(AppCmdDescriptor_t *cmdDesc,uint8_t command,uint8_t address);
void ZigbeeOgnz_QueryZgpDeviceList(AppCmdDescriptor_t *cmdDesc);
void ZigbeeOgnz_QueryZgpDeviceInfo(AppCmdDescriptor_t *cmdDesc,uint8_t address);

#endif
