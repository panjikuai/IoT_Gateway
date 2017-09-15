#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"

#include "buttons.h"

#define ESP_INTR_FLAG_DEFAULT 0
#define BUTTON_SCAN_INTERVAL (100 / portTICK_PERIOD_MS)
#define BUTTON_SYSTEM_RELOAD_MAX (5000 / BUTTON_SCAN_INTERVAL)

TimerHandle_t buttonScanTimer = NULL;
uint32_t buttonScanCount = 0;
ButtonShortPressCallback_t shortPressCallback = NULL;
ButtonLongPressCallback_t  longPressCallback = NULL;
void buttonScanTimerCallback( TimerHandle_t xTimer );

void gpio_isr_handler(void* arg)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (GPIO_NUM_21 == (uint32_t)arg){ //34
		buttonScanCount = 0;
		xTimerStartFromISR( buttonScanTimer, &xHigherPriorityTaskWoken );
	}
}

void buttonScanTimerCallback( TimerHandle_t xTimer )
{
	if (rtc_gpio_get_level(GPIO_NUM_21) == 0){ //Reload Button
		buttonScanCount++;
		if (buttonScanCount >= BUTTON_SYSTEM_RELOAD_MAX){
			buttonScanCount = BUTTON_SYSTEM_RELOAD_MAX;
			if (longPressCallback != NULL){
				xTimerStop(buttonScanTimer,0);
				longPressCallback(BUTTON_RELOAD);
			}
		}
	}else{
		if (buttonScanCount < BUTTON_SYSTEM_RELOAD_MAX && buttonScanCount > 0){
			buttonScanCount = 0;
			if (shortPressCallback != NULL){
				shortPressCallback(BUTTON_RELOAD);
			}
		}
		xTimerStop(buttonScanTimer,0);
	}
}

void Button_KeyEventInit(ButtonShortPressCallback_t short_cb, ButtonLongPressCallback_t long_cb)
{
	shortPressCallback = short_cb;
	longPressCallback  = long_cb;

	if (buttonScanTimer == NULL){
		buttonScanTimer = xTimerCreate( "btnTimer", BUTTON_SCAN_INTERVAL, pdTRUE, 0, buttonScanTimerCallback );
	}
	
	gpio_config_t io_conf = {
		.intr_type = GPIO_INTR_NEGEDGE,				//interrupt of rising edge
		.pin_bit_mask = ((uint64_t)1<<GPIO_NUM_15) | ((uint64_t)1<<GPIO_NUM_21), //bit mask of the pins, use GPIO15 here
		.mode = GPIO_MODE_INPUT,					//set as input mode
		.pull_up_en = 1,							//enable pull-up mode
		.pull_down_en = 0,							//disable pull-down mode
	};
	gpio_config(&io_conf);

	gpio_set_intr_type(GPIO_NUM_15, GPIO_INTR_NEGEDGE);							//change gpio interrupt type for one pin
	gpio_set_intr_type(GPIO_NUM_21, GPIO_INTR_NEGEDGE);							//change gpio interrupt type for one pin
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);							//install gpio isr service
	gpio_isr_handler_add(GPIO_NUM_15, gpio_isr_handler, (void*) GPIO_NUM_15); 	//hook isr handler for specific gpio pin Function Button
	gpio_isr_handler_add(GPIO_NUM_21, gpio_isr_handler, (void*) GPIO_NUM_21); 	//hook isr handler for specific gpio pin Reload Button
	gpio_intr_enable(GPIO_NUM_15);
	gpio_intr_enable(GPIO_NUM_21);
	
}


