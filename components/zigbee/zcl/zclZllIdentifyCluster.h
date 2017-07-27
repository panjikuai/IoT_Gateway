/***************************************************************************//**
  \file zclZllIdentifyCluster.h

  \brief
    The header file describes the ZLL Identify Cluster and its interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    04.07.11  A. Taradov - Created.
*******************************************************************************/

#ifndef _ZCLZLLIDENTIFYCLUSTER_H
#define _ZCLZLLIDENTIFYCLUSTER_H

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

/**
 * \brief Identify Server Cluster attributes amount.
*/

#define ZCL_IDENTIFY_CLUSTER_SERVER_ATTRIBUTES_AMOUNT 1

/**
 * \brief Identify Client Cluster attributes amount.
*/

#define ZCL_IDENTIFY_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT 0

/**
 * \brief Identify Client Cluster commands amount.
*/

#define ZCL_IDENTIFY_CLUSTER_COMMANDS_AMOUNT 4

/**
 * \brief Identify Server Cluster attributes identifiers.
*/

#define ZCL_IDENTIFY_CLUSTER_IDENTIFY_TIME_ATTRIBUTE_ID  CCPU_TO_LE16(0x0000)

/**
 * \brief Identify Server Cluster commands identifiers.
*/

#define ZCL_IDENTIFY_CLUSTER_IDENTIFY_QUERY_RESPONSE_COMMAND_ID 0x00

/**
 * \brief Identify Client Cluster commands identifiers.
*/

#define ZCL_IDENTIFY_CLUSTER_IDENTIFY_COMMAND_ID 0x00
#define ZCL_IDENTIFY_CLUSTER_IDENTIFY_QUERY_COMMAND_ID 0x01
#define ZCL_IDENTIFY_CLUSTER_TRIGGER_EFFECT_COMMAND_ID 0x40


/******************************************************************************
                    Types section
******************************************************************************/

/**
 * \brief Possible values for the Effect Identifier field.
*/
enum
{
  ZCL_EFFECT_IDENTIFIER_BLINK = 0x00,
  ZCL_EFFECT_IDENTIFIER_BREATHE = 0x01,
  ZCL_EFFECT_IDENTIFIER_OKAY = 0x02,
  ZCL_EFFECT_IDENTIFIER_CHANNEL_CHANGE = 0x0b,
  ZCL_EFFECT_IDENTIFIER_FINISH_EFFECT = 0xfe,
  ZCL_EFFECT_IDENTIFIER_STOP_EFFECT = 0xff,
};

/**
 * \brief Possible values for the Effect Variant field.
*/
enum
{
  ZCL_EFFECT_VARIANT_DEFAULT = 0x00,
};

BEGIN_PACK

/**
 * \brief Identify Command Payload format.
*/
typedef struct
{
  uint16_t identifyTime;
} ZCL_Identify_t;

/**
 * \brief Trigger Effect Command Payload format.
*/
typedef struct
{
  uint8_t effectIdentifier;
  uint8_t effectVariant;
} ZCL_TriggerEffect_t;

/**
 * \brief Identify Query Response Payload format.
*/
typedef struct
{
  uint16_t timeout;
} ZCL_IdentifyQueryResponse_t;

typedef struct
{
  struct PACK
  {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint16_t            value;
  } identifyTime;
} ZCL_IdentifyClusterAttributes_t;

END_PACK


#endif // _ZCLZLLIDENTIFYCLUSTER_H

// eof zclZllIdentifyCluster.h
