/***************************************************************************//**
  \file zclZllOnOffCluster.h

  \brief
    The header file describes the ZLL On/Off Cluster and its interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    21.03.11 A. Taradov - Created.
*******************************************************************************/

#ifndef _ZCLZLLONOFFCLUSTER_H
#define	_ZCLZLLONOFFCLUSTER_H

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

#define ZCL_ON_OFF_CLUSTER_ACCEPT_ONLY_WHEN_ON 0x01

/**
 * \brief On/Off Cluster server attributes amount
*/

#define ZCL_ONOFF_CLUSTER_SERVER_ATTRIBUTES_AMOUNT 4

/**
 * \brief On/Off Cluster client attributes amount. Clinet doesn't have attributes.
*/

#define ZCL_ONOFF_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT 0

/**
 * \brief On/Off Cluster commands amount
*/

#define ZCL_ONOFF_CLUSTER_COMMANDS_AMOUNT 6

/**
 * \brief On/Off Cluster server's attributes identifiers
*/

#define ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID  CCPU_TO_LE16(0x0000)
#define ZCL_ONOFF_CLUSTER_GLOBAL_SCENE_CONTROL_SERVER_ATTRIBUTE_ID  CCPU_TO_LE16(0x4000)
#define ZCL_ONOFF_CLUSTER_ON_TIME_SERVER_ATTRIBUTE_ID  CCPU_TO_LE16(0x4001)
#define ZCL_ONOFF_CLUSTER_OFF_WAIT_TIME_SERVER_ATTRIBUTE_ID  CCPU_TO_LE16(0x4002)

/**
 * \brief On/Off Cluster client's command identifiers
*/

#define ZCL_ONOFF_CLUSTER_OFF_COMMAND_ID 0x00
#define ZCL_ONOFF_CLUSTER_ON_COMMAND_ID 0x01
#define ZCL_ONOFF_CLUSTER_TOGGLE_COMMAND_ID 0x02
#define ZCL_ONOFF_CLUSTER_OFF_WITH_EFFECT_COMMAND_ID 0x40
#define ZCL_ONOFF_CLUSTER_ON_WITH_RECALL_GLOBAL_SCENE_COMMAND_ID 0x41
#define ZCL_ONOFF_CLUSTER_ON_WITH_TIMED_OFF_COMMAND_ID 0x42


/******************************************************************************
                    Types section
******************************************************************************/

//BEGIN_PACK
#pragma pack(1) 

/**
 *   \brief On/Off Cluster extension field set
 */
typedef struct _ZCL_OnOffClusterExtensionFieldSet_t
{
    uint16_t clusterId;
    uint8_t length;
    bool onOffValue;
} ZCL_OnOffClusterExtensionFieldSet_t;


typedef struct _ZCL_OffWithEffect_t
{
  uint8_t effectIdentifier;
  uint8_t effectVariant;
} ZCL_OffWithEffect_t;

typedef struct _ZCL_OnWithTimedOff_t
{
  uint8_t onOffControl;
  uint16_t onTime;
  uint16_t offWaitTime;
} ZCL_OnWithTimedOff_t;

//END_PACK
#pragma pack() 



#endif	/* _ZCLZLLONOFFCLUSTER_H */

