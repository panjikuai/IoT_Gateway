#ifndef INDICATOR_H
#define INDICATOR_H


typedef enum{
    LIGHT_CHANNEL_UP = 0,
    LIGHT_CHANNEL_LEFT,
    LIGHT_CHANNEL_RIGHT,
    LIGHT_CHANNEL_MAX
}LightChannel_t;


#define LIGHT_RED   0
#define LIGHT_GREEN (65536/3)
#define LIGHT_BLUE  (65536*2/3)

void LedDisplay_Init(void);
void LedDisplay_MoveToHueAndSaturationLevel(LightChannel_t lightChannel, uint16_t hue, uint8_t saturation, uint8_t level,uint16_t transitionTime);















#endif

