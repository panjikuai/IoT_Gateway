#ifndef LIGHT_H
#define LIGHT_H

#include <stdint.h>

typedef struct _lua_colorTemplight_ctrl_
{
    uint8_t ctOnoffValid;
    uint8_t ctOnoffCtrl;
    uint8_t ctLevelValid;
    uint8_t ctLevelCtrl;
    uint8_t ctValueValid;
    uint8_t ctValueCtrlLo;
    uint8_t ctValueCtrlHi;
    uint8_t	ctTransTimeValid;
    uint8_t	ctTransTimeLo;
    uint8_t	ctTransTimeHi;
    uint8_t	errCode;
}lua_colorTemplight_ctrl_t;


int32_t apps_do_recipe(void *command, uint16_t len);
int32_t apps_do_ctrl(void *command, uint16_t len);
int32_t apps_do_snapshot(void *command, uint16_t len);

#endif
