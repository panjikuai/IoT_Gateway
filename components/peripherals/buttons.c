#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"

#include "buttons.h"

#define ESP_INTR_FLAG_DEFAULT 0
#define BUTTON_SCAN_INTERVAL (50 / portTICK_PERIOD_MS)
#define BUTTON_SYSTEM_RELOAD_MAX (5000 / BUTTON_SCAN_INTERVAL)

#define BUTTON_MAX_TIME  0xFFFF

#define GPIO_RELOAD_BUTTON   	GPIO_NUM_21
#define GPIO_FUNCTION_BUTTON 	GPIO_NUM_15

#define BUTTON_NUM 2

TimerHandle_t buttonScanTimer = NULL;
uint32_t buttonScanCount[BUTTON_NUM] = {0,0};
ButtonShortPressCallback_t shortPressCallback = NULL;
ButtonLongPressCallback_t  longPressCallback = NULL;
void buttonScanTimerCallback( TimerHandle_t xTimer );

void gpio_isr_handler(void* arg)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (GPIO_RELOAD_BUTTON == (uint32_t)arg || GPIO_FUNCTION_BUTTON == (uint32_t)arg){ //34
		if (GPIO_RELOAD_BUTTON == (uint32_t)arg){
			buttonScanCount[BUTTON_RELOAD] = 0;
		}
		if (GPIO_FUNCTION_BUTTON == (uint32_t)arg){
			buttonScanCount[BUTTON_FUNC] = 0;
		}
		xTimerStartFromISR( buttonScanTimer, &xHigherPriorityTaskWoken );
	}
}

void buttonScanTimerCallback( TimerHandle_t xTimer )
{
	uint8_t KeyPressd[BUTTON_NUM] = {false,false};
	if (rtc_gpio_get_level(GPIO_RELOAD_BUTTON) == 0 ){
		KeyPressd[BUTTON_RELOAD] = true;
		if (buttonScanCount[BUTTON_RELOAD] != BUTTON_MAX_TIME){
			buttonScanCount[BUTTON_RELOAD]++;
			if (buttonScanCount[BUTTON_RELOAD] >= BUTTON_SYSTEM_RELOAD_MAX){
				buttonScanCount[BUTTON_RELOAD] = BUTTON_MAX_TIME;
				if (longPressCallback != NULL){
					longPressCallback(BUTTON_RELOAD);
				}
			}
		}
	}else{
		if (buttonScanCount[BUTTON_RELOAD] > 0 && buttonScanCount[BUTTON_RELOAD] < BUTTON_SYSTEM_RELOAD_MAX){
			if (shortPressCallback != NULL){
				shortPressCallback(BUTTON_RELOAD);
			}
		}
	}



	if (rtc_gpio_get_level(GPIO_FUNCTION_BUTTON) == 0 ){
		KeyPressd[BUTTON_FUNC] = true;
		if (buttonScanCount[BUTTON_FUNC] != BUTTON_MAX_TIME){
			buttonScanCount[BUTTON_FUNC]++;
			if (buttonScanCount[BUTTON_FUNC] >= BUTTON_SYSTEM_RELOAD_MAX){
				buttonScanCount[BUTTON_FUNC] = BUTTON_MAX_TIME;
				if (longPressCallback != NULL){
					longPressCallback(BUTTON_FUNC);
				}
			}
		}
	}else{
		if (buttonScanCount[BUTTON_FUNC] > 0 && buttonScanCount[BUTTON_FUNC] < BUTTON_SYSTEM_RELOAD_MAX){
			if (shortPressCallback != NULL){
				shortPressCallback(BUTTON_FUNC);
			}
		}
	}

	if (KeyPressd[BUTTON_RELOAD] == false && KeyPressd[BUTTON_FUNC] == false){
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
		.pin_bit_mask = ((uint64_t)1<<GPIO_FUNCTION_BUTTON) | ((uint64_t)1<<GPIO_RELOAD_BUTTON), //bit mask of the pins, use GPIO15 here
		.mode = GPIO_MODE_INPUT,					//set as input mode
		.pull_up_en = 1,							//enable pull-up mode
		.pull_down_en = 0,							//disable pull-down mode
	};
	gpio_config(&io_conf);

	gpio_set_intr_type(GPIO_FUNCTION_BUTTON, GPIO_INTR_NEGEDGE);				//change gpio interrupt type for one pin
	gpio_set_intr_type(GPIO_RELOAD_BUTTON, GPIO_INTR_NEGEDGE);					//change gpio interrupt type for one pin
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);							//install gpio isr service
	gpio_isr_handler_add(GPIO_FUNCTION_BUTTON, gpio_isr_handler, (void*) GPIO_FUNCTION_BUTTON); //hook isr handler for specific gpio pin Function Button
	gpio_isr_handler_add(GPIO_RELOAD_BUTTON,   gpio_isr_handler, (void*) GPIO_RELOAD_BUTTON); 	//hook isr handler for specific gpio pin Reload Button
	gpio_intr_enable(GPIO_FUNCTION_BUTTON);
	gpio_intr_enable(GPIO_RELOAD_BUTTON);
	
}


