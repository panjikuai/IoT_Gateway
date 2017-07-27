/***************************************************************************//**
  \file zclZllCommissioningCluster.h

  \brief
    The header file describes the ZLL Commissioning Cluster and its interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    04.07.11  A. Taradov - Created.
*******************************************************************************/

#ifndef _ZCLZLLCOMISSIONINGCLUSTER_H
#define _ZCLZLLCOMISSIONINGCLUSTER_H

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
 * \brief Commissioning Server Cluster attributes amount.
*/

#define ZCL_COMMISSIONING_CLUSTER_SERVER_ATTRIBUTES_AMOUNT 0

/**
 * \brief Commissioning Client Cluster attributes amount.
*/

#define ZCL_COMMISSIONING_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT 0

/**
 * \brief Commissioning Client Cluster commands amount.
*/

#define ZCL_COMMISSIONING_CLUSTER_COMMANDS_AMOUNT 5

/**
 * \brief Commissioning Server Cluster commands identifiers.
*/

#define ZCL_COMMISSIONING_CLUSTER_ENDPOINT_INFORMATION_COMMAND_ID 0x40
#define ZCL_COMMISSIONING_CLUSTER_GET_GROUP_IDENTIFIERS_RESPONSE_COMMAND_ID 0x41
#define ZCL_COMMISSIONING_CLUSTER_GET_ENDPOINT_LIST_RESPONSE_COMMAND_ID 0x42

/**
 * \brief Commissioning Client Cluster commands identifiers.
*/

#define ZCL_COMMISSIONING_CLUSTER_GET_GROUP_IDENTIFIERS_COMMAND_ID 0x41
#define ZCL_COMMISSIONING_CLUSTER_GET_ENDPOINT_LIST_COMMAND_ID 0x42


/******************************************************************************
                    Types section
******************************************************************************/

BEGIN_PACK

/**
 * \brief Get Group Identifiers command format
*/
typedef struct PACK
{
  uint8_t startIndex;
} ZCL_GetGroupIdentifiers_t;

/**
 * \brief Get Endpoint List command format
*/
typedef struct PACK
{
  uint8_t startIndex;
} ZCL_GetEndpointList_t;

/**
 * \brief Endpoint Information command format
*/
typedef struct PACK
{
  uint64_t ieeeAddress;
  uint16_t networkAddress;
  uint8_t  endpointId;
  uint16_t profileId;
  uint16_t deviceId;
  uint8_t  version;
} ZCL_EndpointInformation_t;

/**
 * \brief Group Information Record format
*/
typedef struct PACK
{
  uint16_t groupId;
  uint8_t  groupType;
} ZCL_GroupInformationRecord_t;

/**
 * \brief Endpoint Information Record format
*/
typedef struct PACK
{
  uint16_t networkAddress;
  uint8_t  endpointId;
  uint16_t profileId;
  uint16_t deviceId;
  uint8_t  version;
} ZCL_EndpointInformationRecord_t;

/**
 * \brief Get Group Identifiers Response command format
*/
typedef struct PACK
{
  uint8_t  total;
  uint8_t  startIndex;
  uint8_t  count;
  ZCL_GroupInformationRecord_t recordList[5];
} ZCL_GetGroupIdentifiersResponse_t;

/**
 * \brief Get Endpoint List Response command format
*/
typedef struct PACK
{
  uint8_t  total;
  uint8_t  startIndex;
  uint8_t  count;
  ZCL_EndpointInformationRecord_t recordList[1];
} ZCL_GetEndpointListResponse_t;

END_PACK


#endif // _ZCLZLLCOMISSIONINGCLUSTER_H

// eof zclZllCommissioningCluster.h
