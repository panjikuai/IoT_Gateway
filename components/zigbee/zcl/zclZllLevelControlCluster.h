/***************************************************************************//**
  \file zclZllLevelControlCluster.h

  \brief
    Interface and types of Level Control Cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    22.06.10 D.Loskutnikov - Created.
*******************************************************************************/

#ifndef _ZCLZLLLEVELCONTROLCLUSTER_H
#define _ZCLZLLLEVELCONTROLCLUSTER_H

/*!
Attributes and commands for determining basic information about a device,
setting user device information such as location, enabling a device and resetting it
to factory defaults.
*/

/*******************************************************************************
                   Includes section
*******************************************************************************/

#include "zcl.h"

/*******************************************************************************
                   Define(s) section
*******************************************************************************/

#define ZLL_LEVEL_CONTROL_UP_DIRECTION    0x00
#define ZLL_LEVEL_CONTROL_DOWN_DIRECTION  0x01

/**
 * \brief Amount of Level Control cluster server's attributes
*/
#define ZCL_LEVEL_CONTROL_CLUSTER_SERVER_ATTRIBUTES_AMOUNT 2

/**
 * \brief Client doesn't have attributes.
*/
#define ZCL_LEVEL_CONTROL_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT 0

/**
 * \brief Level Control Cluster commands amount
*/

#define ZCL_LEVEL_CONTROL_CLUSTER_COMMANDS_AMOUNT 8

/**
 * \brief Level Control Cluster server's attributes identifiers
*/

#define ZCL_LEVEL_CONTROL_CLUSTER_CURRENT_LEVEL_ATTRIBUTE_ID            CCPU_TO_LE16(0x0000)
#define ZCL_LEVEL_CONTROL_CLUSTER_REMAINING_TIME_ATTRIBUTE_ID           CCPU_TO_LE16(0x0001)
#define ZCL_LEVEL_CONTROL_CLUSTER_ON_OFF_TRANSITION_TIME_ATTRIBUTE_ID   CCPU_TO_LE16(0x0010)
#define ZCL_LEVEL_CONTROL_CLUSTER_ON_LEVEL_ATTRIBUTE_ID                 CCPU_TO_LE16(0x0011)

/**
 * \brief On/Off Cluster client's command identifiers
*/

#define ZCL_LEVEL_CONTROL_CLUSTER_MOVE_TO_LEVEL_COMMAND_ID          0x00
#define ZCL_LEVEL_CONTROL_CLUSTER_MOVE_COMMAND_ID                   0x01
#define ZCL_LEVEL_CONTROL_CLUSTER_STEP_COMMAND_ID                   0x02
#define ZCL_LEVEL_CONTROL_CLUSTER_STOP_COMMAND_ID                   0x03
#define ZCL_LEVEL_CONTROL_CLUSTER_MOVE_TO_LEVEL_W_ONOFF_COMMAND_ID  0x04
#define ZCL_LEVEL_CONTROL_CLUSTER_MOVE_W_ONOFF_COMMAND_ID           0x05
#define ZCL_LEVEL_CONTROL_CLUSTER_STEP_W_ONOFF_COMMAND_ID           0x06
#define ZCL_LEVEL_CONTROL_CLUSTER_STOP_W_ONOFF_COMMAND_ID           0x07


BEGIN_PACK

/**
 * \brief Move To Level Command Payload format.
 */
typedef struct _ZCL_MoveToLevel_t
{
  uint8_t level;
  uint16_t transitionTime;
} ZCL_MoveToLevel_t;

/**
 * \brief Move Command Payload format.
 */
typedef struct _ZCL_Move_t
{
  uint8_t moveMode;
  uint8_t rate;
} ZCL_Move_t;

/**
 * \brief Step Command Payload format.
 */
typedef struct _ZCL_Step_t
{
  uint8_t stepMode;
  uint8_t stepSize;
  uint16_t transitionTime;
} ZCL_Step_t;


/**
 * \brief Level Control Cluster server's optional attributes
*/
typedef struct _ZCL_LevelControlClusterServerOptionalAttributes_t
{
  struct _onOffTransitionTime
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    uint16_t value;
    uint16_t minVal;
    uint16_t maxVal;
  } onOffTransitionTime;

  struct _onLevel
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t properties;
    uint8_t value;
    uint8_t minVal;
    uint8_t maxVal;
  } onLevel;
} ZCL_LevelControlClusterServerOptionalAttributes_t;

/**
 *   \brief Level Control Cluster extension field set
 */
typedef struct _ZCL_LevelControlClusterExtensionFieldSet_t
{
    uint16_t clusterId;
    uint8_t length;
    uint8_t currentLevel;
} ZCL_LevelControlClusterExtensionFieldSet_t;

END_PACK



#endif /* _ZCLZLLLEVELCONTROLCLUSTER_H */
