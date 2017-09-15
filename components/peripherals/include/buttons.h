#ifndef BUTTONS_H
#define BUTTONS_H

typedef enum {
    BUTTON_RELOAD   = 0,
    BUTTON_FUNC     = 1
}ButtonValue_t;

typedef void(*ButtonShortPressCallback_t)(ButtonValue_t value);
typedef void(*ButtonLongPressCallback_t)(ButtonValue_t value);

void Button_KeyEventInit(ButtonShortPressCallback_t short_cb, ButtonLongPressCallback_t long_cb);



#endif
