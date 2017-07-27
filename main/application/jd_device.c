#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue_uart.h"
#include "jd_device.h"
#include "crc.h"
#include "zigbee_header.h"
#include "zigbee_cmd_service.h"
#include "iot_debug.h"

#define JD_CONTROL_SUCCESS             	    0
#define JD_CONTROL_FAILURE             		2

uint8_t level = 1;
uint16_t ctValue = 3000;
uint8_t onOff = 0;
uint16_t transTimeValue = 0;

int32_t apps_do_recipe(void *command, uint16_t len)
{
    lua_colorTemplight_ctrl_t *light = (lua_colorTemplight_ctrl_t *)command;

    light->ctOnoffValid = 1;
    light->ctOnoffCtrl = onOff;

    light->ctLevelValid = 1;
    light->ctLevelCtrl = level;

    light->ctValueCtrlLo = (uint8_t)ctValue;
    light->ctValueCtrlHi = (uint8_t)(ctValue>>8);

    light->ctTransTimeLo = (uint8_t)transTimeValue;
    light->ctTransTimeHi = (uint8_t)(transTimeValue >>8);

    light->errCode = JD_CONTROL_SUCCESS;
    IoT_DEBUG(JD_SMART_DBG | IoT_DBG_INFO,("do_recipe...\n"));

    return JD_CONTROL_SUCCESS;
}

int32_t apps_do_ctrl(void *command, uint16_t len)
{
	appCmdDescriptor_t cmdDesc;

    uint16_t transTime = 0;
    if(len == 0) {
        return JD_CONTROL_FAILURE;
    }

    lua_colorTemplight_ctrl_t *light = (lua_colorTemplight_ctrl_t *)command;
    light->errCode = JD_CONTROL_SUCCESS;

    if (light->ctTransTimeValid){
        transTime = light->ctTransTimeHi;
        transTime = (transTime << 8) + light->ctTransTimeLo;
    }

    if (light->ctOnoffValid){
    	onOff = light->ctOnoffCtrl;
        zigbee_ctrl_on_off(&cmdDesc,APS_SHORT_ADDRESS,0,0,onOff);
        IoT_DEBUG(JD_SMART_DBG | IoT_DBG_INFO,("do_ctrl onOff: %d\n",onOff));

    }else if(light->ctLevelValid){
    	level = light->ctLevelCtrl;
        zigbee_ctrl_move_to_level(&cmdDesc,APS_SHORT_ADDRESS,0,0,ZCL_LEVEL_CONTROL_CLUSTER_MOVE_TO_LEVEL_COMMAND_ID,level, 5);
        IoT_DEBUG(JD_SMART_DBG | IoT_DBG_INFO,("do_ctrl level: %d\n",level));

    }else if (light->ctValueValid){
        ctValue = ((uint16_t)(light->ctValueCtrlHi)<< 8) + light->ctValueCtrlLo;
        zigbee_ctrl_move_to_colorTemperature(&cmdDesc,APS_SHORT_ADDRESS,0,0,(uint16_t)(1000000/ctValue), 5);
        IoT_DEBUG(JD_SMART_DBG | IoT_DBG_INFO,("do_ctrl colorT: %d\n",ctValue));
    }

    if(light->ctOnoffValid || light->ctLevelValid || light->ctValueValid){
    	zigbee_cmd_service_process_packet(&cmdDesc);
    }

    return JD_CONTROL_SUCCESS;
}

int32_t apps_do_snapshot(void *command, uint16_t len)
{
    lua_colorTemplight_ctrl_t *light = (lua_colorTemplight_ctrl_t *)command;

    light->ctOnoffValid = 1;
    light->ctOnoffCtrl = onOff;

    light->ctLevelValid = 1;
    light->ctLevelCtrl = level;

    light->ctValueValid = 1;
    light->ctValueCtrlLo = (uint8_t)ctValue;
    light->ctValueCtrlHi = (uint8_t)(ctValue>>8);

    light->ctTransTimeValid = 1;
    light->ctTransTimeLo = (uint8_t)transTimeValue;
    light->ctTransTimeHi = (uint8_t)(transTimeValue >>8);

    light->errCode = JD_CONTROL_SUCCESS;
    IoT_DEBUG(JD_SMART_DBG | IoT_DBG_INFO,("do_snapshot...\n"));

    return JD_CONTROL_SUCCESS;
}
