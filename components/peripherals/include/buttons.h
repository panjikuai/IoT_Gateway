#ifndef BUTTONS_H
#define BUTTONS_H

typedef void(*button_short_press_callback_t)(void);
typedef void(*button_long_press_callback_t)(void);

void button_init(button_short_press_callback_t short_cb, button_long_press_callback_t long_cb);



#endif
