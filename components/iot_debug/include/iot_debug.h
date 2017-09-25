#ifndef _IOT_DEBUG_H
#define _IOT_DEBUG_H

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define _IOT_DEBUG_

/* Debug level */
#define IoT_DBG_INFO			0x01u
#define IoT_DBG_WARNING			0x02u
#define IoT_DBG_SERIOUS			0x03u
#define IoT_DBG_LEVEL_MASK		0x0fu

/** Flag for IoT_DEBUG to enable the debug message */
#define IoT_DBG_ON            0x80u
/** Flag for IoT_DEBUG to disable the debug message */
#define IoT_DBG_OFF           0x00u


/* Components debug switch */
#define GENERIC_DBG					IoT_DBG_ON
#define WIFI_DBG					IoT_DBG_ON
#define SMART_CONFIG_DBG			IoT_DBG_ON
#define LWIP_DBG					IoT_DBG_ON
#define JD_SMART_DBG				IoT_DBG_ON
#define ZIGBEE_DBG					IoT_DBG_ON


#define PARAM_STORE_DBG				IoT_DBG_ON
#define OTAU_DBG					IoT_DBG_ON
#define DOWNLOAD_DBG				IoT_DBG_ON
#define CLOUD_API_DBG				IoT_DBG_ON
#define JSON_DBG					IoT_DBG_ON
#define POLARSSL_DBG				IoT_DBG_ON
#define SSL_DBG						IoT_DBG_ON
#define SERIAL_DBG					IoT_DBG_ON
#define UPGRADE_DBG					IoT_DBG_ON
#define ZIGBEE_DBG					IoT_DBG_ON
#define IoT_DBG_MIN_LEVEL			IoT_DBG_INFO

#ifdef _IOT_DEBUG_

extern xSemaphoreHandle xsem_dbg;

#define IoT_DEBUG(dbg_level, message) do { \
	if((((dbg_level) & IoT_DBG_LEVEL_MASK) >= IoT_DBG_MIN_LEVEL) && \
	    ((dbg_level) & IoT_DBG_ON)) { \
			if(xsem_dbg != NULL) { \
				xSemaphoreTake(xsem_dbg, portMAX_DELAY); \
				printf("(%5d)[%s]", xTaskGetTickCount(),pcTaskGetTaskName(NULL)); \
				printf message; \
				xSemaphoreGive(xsem_dbg); \
			} \
			else { \
				printf message; \
			} \
	} \
}while(0)

#else

#define IoT_DEBUG(dbg_level, message)

#endif


void DebugLog_Init(void);




#endif
