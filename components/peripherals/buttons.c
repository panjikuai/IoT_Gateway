#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"


#include "buttons.h"

#define ESP_INTR_FLAG_DEFAULT 0
#define BUTTON_SCAN_INTERVAL (100 / portTICK_PERIOD_MS)
#define BUTTON_SYSTEM_RELOAD_MAX (5000 / BUTTON_SCAN_INTERVAL)

TimerHandle_t button_scan_timer = NULL;
static uint32_t button_scan_count = 0;

button_short_press_callback_t short_press_callback = NULL;
button_long_press_callback_t  long_press_callback = NULL;

void button_scan_timer_callback( TimerHandle_t xTimer );


void gpio_isr_handler(void* arg)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (GPIO_NUM_34 == (uint32_t)arg){
		button_scan_count = 0;
		xTimerStartFromISR( button_scan_timer, &xHigherPriorityTaskWoken );
	}
}

void button_scan_timer_callback( TimerHandle_t xTimer )
{
	if (rtc_gpio_get_level(GPIO_NUM_34) == 0){
		button_scan_count++;
		if (button_scan_count >= BUTTON_SYSTEM_RELOAD_MAX){
			button_scan_count = BUTTON_SYSTEM_RELOAD_MAX;
			if (long_press_callback != NULL){
				xTimerStop(button_scan_timer,0);
				long_press_callback();
			}
		}
	}else{
		if (button_scan_count < BUTTON_SYSTEM_RELOAD_MAX){
			button_scan_count = 0;
			if (short_press_callback != NULL){
				short_press_callback();
			}
		}
		xTimerStop(button_scan_timer,0);
	}
}

void button_init(button_short_press_callback_t short_cb, button_long_press_callback_t long_cb)
{
	gpio_config_t io_conf;

	short_press_callback = short_cb;
	long_press_callback  = long_cb;

	io_conf.intr_type = GPIO_INTR_NEGEDGE;	//interrupt of rising edge
	io_conf.pin_bit_mask = ((uint64_t)1<<GPIO_NUM_34); 	//bit mask of the pins, use GPIO34 here
	io_conf.mode = GPIO_MODE_INPUT;				//set as input mode
	io_conf.pull_up_en = 1;						//enable pull-up mode
	io_conf.pull_down_en = 0;					//disable pull-down mode
	gpio_config(&io_conf);

    gpio_set_intr_type(GPIO_NUM_34, GPIO_INTR_NEGEDGE);//change gpio interrupt type for one pin
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);	//install gpio isr service
    gpio_isr_handler_add(GPIO_NUM_34, gpio_isr_handler, (void*) GPIO_NUM_34); //hook isr handler for specific gpio pin
    gpio_intr_enable(GPIO_NUM_34);

    button_scan_timer = xTimerCreate( "btnTimer", BUTTON_SCAN_INTERVAL, pdTRUE, 0, button_scan_timer_callback );
}


