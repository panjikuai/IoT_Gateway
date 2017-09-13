/*
 * group.h
 *
 * Created: 2015-04-16 11:18:35
 *  Author: matt.qian
 */ 


#ifndef GROUP_H_
#define GROUP_H_
#include "cmd_descriptor.h"

void zigbee_add_remove_group(AppCmdDescriptor_t *cmdDesc,uint8_t address,uint8_t endpoint,uint16_t groupAddress, uint8_t command);

#endif /* GROUP_H_ */
