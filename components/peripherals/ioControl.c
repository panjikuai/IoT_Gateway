#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "driver/uart.h"
#include "driver/gpio.h"


void IoControl_Init(void)
{
    gpio_set_direction(GPIO_NUM_21, GPIO_MODE_OUTPUT);

    // Reset zigbee module
	gpio_set_level(GPIO_NUM_21, 0);
	vTaskDelay(10/portTICK_PERIOD_MS);
	gpio_set_level(GPIO_NUM_21, 1);
	vTaskDelay(500/portTICK_PERIOD_MS);
}
