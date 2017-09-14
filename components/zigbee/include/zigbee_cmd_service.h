#ifndef ZIGBEE_H
#define ZIGBEE_H

#include <stdint.h>
#include <stdbool.h>

#include "cmd_descriptor.h"

#define ZIGBEE_SEND_RETRY_TIMES				1
#define ZIGBEE_CMD_QUEUE_LEN				1
#define ZIGBEE_CMD_RESULT_QUEUE_LEN			1

void ZigbeeCmdService_Init(void);
bool ZigbeeCmdService_ProcessPacket(AppCmdDescriptor_t* pCmdDesc);
void ZigbeeCmdService_ResetModule(void);


#endif
