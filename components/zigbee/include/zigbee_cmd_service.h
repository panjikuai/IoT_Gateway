#ifndef ZIGBEE_H
#define ZIGBEE_H

#include <stdint.h>
#include <stdbool.h>

#include "cmd_descriptor.h"

#define ZIGBEE_SEND_RETRY_TIMES				1
#define ZIGBEE_CMD_QUEUE_LEN				1
#define ZIGBEE_CMD_RESULT_QUEUE_LEN			1

void zigbee_cmd_service_init(void);
bool zigbee_cmd_service_process_packet(appCmdDescriptor_t* pCmdDesc);



#endif
