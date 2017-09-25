#include <stdio.h>
#include <stddef.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "iot_debug.h"

xSemaphoreHandle xsem_dbg = NULL;

void DebugLog_Init(void)
{
	vSemaphoreCreateBinary(xsem_dbg);
}

