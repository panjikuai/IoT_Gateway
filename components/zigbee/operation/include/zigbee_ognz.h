#ifndef ZIGBEE_OGNZ_H_
#define ZIGBEE_OGNZ_H_

#include "cmd_descriptor.h"

void zigbee_ognz_query_device_group_scene_list(appCmdDescriptor_t *cmdDesc,uint8_t command);
void zigbee_ognz_query_device_group_scene_info(appCmdDescriptor_t *cmdDesc,uint8_t command,uint8_t address);
void zigbee_ognz_query_zgp_device_list(appCmdDescriptor_t *cmdDesc);
void zigbee_ognz_query_zgp_device_info(appCmdDescriptor_t *cmdDesc,uint8_t address);

#endif
