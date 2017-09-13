/*
 * zigbee_network.h
 *
 * Created: 2015-05-12 10:58:55
 *  Author: matt.qian
 */ 


#ifndef ZIGBEE_NETWORK_H_
#define ZIGBEE_NETWORK_H_

#include "cmd_descriptor.h"

void ZigbeeNetwork_ChangePhyChannel(AppCmdDescriptor_t *cmdDesc);
void ZigbeeNetwork_ResetGatewayToFn(AppCmdDescriptor_t *cmdDesc);
void ZigbeeNetwork_ClassicPermitJoin(AppCmdDescriptor_t *cmdDesc);
void ZigbeeNetwork_Touchlink(AppCmdDescriptor_t *cmdDesc);
void ZigbeeNetwork_ResetDeviceToFn(AppCmdDescriptor_t *cmdDesc,uint8_t address, uint8_t endpoint);

#endif /* ZIGBEE_NETWORK_H_ */
