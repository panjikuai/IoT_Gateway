#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "driver/ledc.h"

#include "ledDisplay.h"


#define PWM_BIT_NUM  LEDC_TIMER_13_BIT

#if PWM_BIT_NUM == LEDC_TIMER_10_BIT
    #define PWM_FULL_SCALE    1023
#elif PWM_BIT_NUM == LEDC_TIMER_11_BIT
    #define PWM_FULL_SCALE    2047
#elif PWM_BIT_NUM == LEDC_TIMER_12_BIT
    #define PWM_FULL_SCALE    4095
#elif PWM_BIT_NUM == LEDC_TIMER_13_BIT
    #define PWM_FULL_SCALE    8191
#elif PWM_BIT_NUM == LEDC_TIMER_14_BIT
    #define PWM_FULL_SCALE    16383
#else
    #define PWM_FULL_SCALE    32767
#endif


#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_UP_R_GPIO       (2)
#define LEDC_HS_UP_R_CHANNEL    LEDC_CHANNEL_0
#define LEDC_HS_UP_G_GPIO       (4)
#define LEDC_HS_UP_G_CHANNEL    LEDC_CHANNEL_1
#define LEDC_HS_UP_B_GPIO       (16)
#define LEDC_HS_UP_B_CHANNEL    LEDC_CHANNEL_2

#define LEDC_HS_LEFT_R_GPIO     (32)//(34)
#define LEDC_HS_LEFT_R_CHANNEL  LEDC_CHANNEL_3
#define LEDC_HS_LEFT_G_GPIO     (27)//(39)
#define LEDC_HS_LEFT_G_CHANNEL  LEDC_CHANNEL_4
#define LEDC_HS_LEFT_B_GPIO     (13)//(36)
#define LEDC_HS_LEFT_B_CHANNEL  LEDC_CHANNEL_5

#define LEDC_HS_RIGHT_R_GPIO    (5)
#define LEDC_HS_RIGHT_R_CHANNEL LEDC_CHANNEL_6
#define LEDC_HS_RIGHT_G_GPIO    (18)
#define LEDC_HS_RIGHT_G_CHANNEL LEDC_CHANNEL_7

#define LEDC_HS_TIMER1          LEDC_TIMER_1
#define LEDC_HS_RIGHT_B_GPIO    (19)
#define LEDC_HS_RIGHT_B_CHANNEL LEDC_CHANNEL_0

typedef struct {
    int channel;
    int io;
    int mode;
    int timer_idx;
}Ledc_info_t;

#define LED_CHANNEL_NUM 9

Ledc_info_t ledcChannel[LED_CHANNEL_NUM] = {
    //UP leds
    {
        .channel   = LEDC_HS_UP_R_CHANNEL,
        .io        = LEDC_HS_UP_R_GPIO,
        .mode      = LEDC_HS_MODE,
        .timer_idx = LEDC_HS_TIMER
    },
    {
        .channel   = LEDC_HS_UP_G_CHANNEL,
        .io        = LEDC_HS_UP_G_GPIO,
        .mode      = LEDC_HS_MODE,
        .timer_idx = LEDC_HS_TIMER
    },
    {
        .channel   = LEDC_HS_UP_B_CHANNEL,
        .io        = LEDC_HS_UP_B_GPIO,
        .mode      = LEDC_HS_MODE,
        .timer_idx = LEDC_HS_TIMER
    },
    //Left leds
    {
        .channel   = LEDC_HS_LEFT_R_CHANNEL,
        .io        = LEDC_HS_LEFT_R_GPIO,
        .mode      = LEDC_HS_MODE,
        .timer_idx = LEDC_HS_TIMER
    },
    {
        .channel   = LEDC_HS_LEFT_G_CHANNEL,
        .io        = LEDC_HS_LEFT_G_GPIO,
        .mode      = LEDC_HS_MODE,
        .timer_idx = LEDC_HS_TIMER
    },
    {
        .channel   = LEDC_HS_LEFT_B_CHANNEL,
        .io        = LEDC_HS_LEFT_B_GPIO,
        .mode      = LEDC_HS_MODE,
        .timer_idx = LEDC_HS_TIMER
    },
    //Right leds
    {
        .channel   = LEDC_HS_RIGHT_R_CHANNEL,
        .io        = LEDC_HS_RIGHT_R_GPIO,
        .mode      = LEDC_HS_MODE,
        .timer_idx = LEDC_HS_TIMER
    },
    {
        .channel   = LEDC_HS_RIGHT_G_CHANNEL,
        .io        = LEDC_HS_RIGHT_G_GPIO,
        .mode      = LEDC_HS_MODE,
        .timer_idx = LEDC_HS_TIMER
    },
    {
        .channel   = LEDC_HS_RIGHT_B_CHANNEL,
        .io        = LEDC_HS_RIGHT_B_GPIO,
        .mode      = LEDC_HS_MODE,
        .timer_idx = LEDC_HS_TIMER1
    },

};

void LedDisplay_Init(void)
{
    ledc_timer_config_t ledc_timer = {
        .bit_num = PWM_BIT_NUM,         //set timer counter bit number
        .freq_hz = 1000,                //set frequency of pwm
        .speed_mode = LEDC_HS_MODE,     //timer mode,
        .timer_num = LEDC_HS_TIMER      //timer index
    };
    //timer0
    ledc_timer_config(&ledc_timer);
    //timer1
    ledc_timer.timer_num = LEDC_HS_TIMER1;
    ledc_timer_config(&ledc_timer);

    for (uint32_t i = 0; i < LED_CHANNEL_NUM; i++){
        ledc_channel_config_t ledc_channel = {
            .channel = ledcChannel[i].channel,      //set LEDC channel 0
            .duty = 0,                              //set the duty for initialization.(duty range is 0 ~ ((2**bit_num)-1)
            .gpio_num = ledcChannel[i].io,          //GPIO number
            .intr_type = LEDC_INTR_FADE_END,        //GPIO INTR TYPE, as an example, we enable fade_end interrupt here.
            .speed_mode = ledcChannel[i].mode,      //set LEDC mode, from ledc_mode_t
            //set LEDC timer source, if different channel use one timer,
            //the frequency and bit_num of these channels should be the same
            .timer_sel = ledcChannel[i].timer_idx,
        };
        //set the configuration
        ledc_channel_config(&ledc_channel);
    }
    ledc_fade_func_install(0);//initialize fade service.

    // for (uint32_t i = 0; i < LED_CHANNEL_NUM; i++) {
    //     ledc_set_fade_with_time(ledcChannel[i].mode, ledcChannel[i].channel, 4000, 500);
    //     ledc_fade_start(ledcChannel[i].mode, ledcChannel[i].channel, LEDC_FADE_NO_WAIT);
    // }

}

// ledc_set_duty(ledc_ch[ch].mode, ledc_ch[ch].channel, 2000);
// ledc_update_duty(ledc_ch[ch].mode, ledc_ch[ch].channel);


void LedDisplay_MoveToHueAndSaturationLevel(LightChannel_t lightChannel, uint16_t hue, uint8_t saturation, uint8_t level,uint16_t transitionTime)
{
    if (lightChannel >= LIGHT_CHANNEL_MAX){
        return;
    }

	float r=0,g=0,b=0;
	float f,p,q,t;
	float hue_f,saturation_f,level_f;
	
	hue_f		 = hue/65536.0;
	saturation_f = (saturation&0xFE)/254.0;
	level_f		 = (level&0xFE)/254.0;
	
	float hi_f = hue_f * 6.0f;
	uint8_t hi = (uint8_t)hi_f;
	
	f = hi_f - hi;
	p = level_f *(1 - saturation_f);
	q = level_f *(1 - saturation_f * f );
	t = level_f *(1 - saturation_f * (1-f) );
	
	if (hi == 0){
		r = level_f;
		g = t;
		b = p;
	}else if (hi == 1){
		r = q;
		g = level_f;
		b = p;
	}else if (hi == 2){
		r = p;
		g = level_f;
		b = t;
	}else if (hi == 3){
		r = p;
		g = q;
		b = level_f;
	}else if (hi == 4){
		r = t;
		g = p;
		b = level_f;
	}else if (hi == 5){
		r = level_f;
		g = p;
		b = q;
    }
    
    uint8_t ledIdex = lightChannel*3;

    ledc_set_fade_with_time(ledcChannel[ledIdex].mode, ledcChannel[ledIdex].channel, (int)(r*PWM_FULL_SCALE), transitionTime);
    ledc_fade_start(ledcChannel[ledIdex].mode, ledcChannel[ledIdex].channel, LEDC_FADE_NO_WAIT);

    ledIdex++;
    ledc_set_fade_with_time(ledcChannel[ledIdex].mode, ledcChannel[ledIdex].channel, (int)(g*PWM_FULL_SCALE), transitionTime);
    ledc_fade_start(ledcChannel[ledIdex].mode, ledcChannel[ledIdex].channel, LEDC_FADE_NO_WAIT);

    ledIdex++;
    ledc_set_fade_with_time(ledcChannel[ledIdex].mode, ledcChannel[ledIdex].channel, (int)(b*PWM_FULL_SCALE), transitionTime);
    ledc_fade_start(ledcChannel[ledIdex].mode, ledcChannel[ledIdex].channel, LEDC_FADE_NO_WAIT);

}




































