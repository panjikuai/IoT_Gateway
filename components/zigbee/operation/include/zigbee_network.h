/*
 * zigbee_network.h
 *
 * Created: 2015-05-12 10:58:55
 *  Author: matt.qian
 */ 


#ifndef ZIGBEE_NETWORK_H_
#define ZIGBEE_NETWORK_H_

#include "cmd_descriptor.h"

void zigbee_network_change_phy_channel(AppCmdDescriptor_t *cmdDesc);
void zigbee_network_reset_gateway_to_fn(AppCmdDescriptor_t *cmdDesc);
void zigbee_network_classic_permit_join(AppCmdDescriptor_t *cmdDesc);
void zigbee_network_touchlink(AppCmdDescriptor_t *cmdDesc);
void zigbee_network_reset_device_to_fn(AppCmdDescriptor_t *cmdDesc,uint8_t address, uint8_t endpoint);

#endif /* ZIGBEE_NETWORK_H_ */
