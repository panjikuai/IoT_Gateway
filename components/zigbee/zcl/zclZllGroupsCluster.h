/***************************************************************************//**
  \file zclZllGroupsCluster.h

  \brief
    The header file describes the ZCL ZLL Groups Cluster and its interface

    The file describes the types and interface of the ZCL Groups Cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    12.02.09 D. Kasyanov - Created.
*******************************************************************************/

#ifndef _ZCLZLLGROUPSCLUSTER_H
#define _ZCLZLLGROUPSCLUSTER_H

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
 * \brief Groups Cluster group name support bit
*/

#define GROUP_NAME_SUPPORT_FLAG  0x80

/**
 * \brief Groups Cluster server attributes amount
*/

#define ZCL_GROUPS_CLUSTER_SERVER_ATTRIBUTES_AMOUNT 1

/**
 * \brief Groups Cluster client attributes amount. Clinet doesn't have attributes.
*/

#define ZCL_GROUPS_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT 0

/**
 * \brief Groups Cluster commands amount
*/

#define ZCL_GROUPS_CLUSTER_COMMANDS_AMOUNT 10

/**
 * \brief Groups Cluster server's attributes identifiers
*/

#define ZCL_GROUPS_CLUSTER_NAME_SUPPORT_SERVER_ATTRIBUTE_ID  CCPU_TO_LE16(0x0000)

#define CS_GROUP_TABLE_SIZE 10

/**
 * \brief Groups Cluster client's command identifiers
*/

#define ZCL_GROUPS_CLUSTER_ADD_GROUP_COMMAND_ID 0x00
#define ZCL_GROUPS_CLUSTER_VIEW_GROUP_COMMAND_ID 0x01
#define ZCL_GROUPS_CLUSTER_GET_GROUP_MEMBERSHIP_COMMAND_ID 0x02
#define ZCL_GROUPS_CLUSTER_REMOVE_GROUP_COMMAND_ID 0x03
#define ZCL_GROUPS_CLUSTER_REMOVE_ALL_GROUPS_COMMAND_ID 0x04
#define ZCL_GROUPS_CLUSTER_ADD_GROUP_IF_IDENTIFYING_COMMAND_ID 0x05

/**
 * \brief Groups Cluster servers's command identifiers
*/

#define ZCL_GROUPS_CLUSTER_ADD_GROUP_RESPONSE_COMMAND_ID 0x00
#define ZCL_GROUPS_CLUSTER_VIEW_GROUP_RESPONSE_COMMAND_ID 0x01
#define ZCL_GROUPS_CLUSTER_GET_GROUP_MEMBERSHIP_RESPONSE_COMMAND_ID 0x02
#define ZCL_GROUPS_CLUSTER_REMOVE_GROUP_RESPONSE_COMMAND_ID 0x03


/******************************************************************************
                    Types section
 ******************************************************************************/
BEGIN_PACK

/**
 * \brief Add Group Command Payload format.
 */

typedef struct _ZCL_AddGroup_t
{
  uint16_t groupId;
  uint8_t groupName[1];
} ZCL_AddGroup_t;

/**
 * \brief View Group Command Payload format.
 */

typedef struct _ZCL_ViewGroup_t
{
  uint16_t groupId;
} ZCL_ViewGroup_t;

/**
 * \brief Get Group Membership Command Payload format.
 */

typedef struct _ZCL_GetGroupMembership_t
{
  uint8_t groupCount;
  uint16_t groupList[1];
} ZCL_GetGroupMembership_t;

typedef struct PACK
{
  uint8_t groupCount;
  uint16_t groupList[CS_GROUP_TABLE_SIZE];
} ZCL_GetGroupMembershipExt_t;

/**
 * \brief Remove Group Command Payload format.
 */

typedef struct _ZCL_RemoveGroup_t
{
  uint16_t groupId;
} ZCL_RemoveGroup_t;

/**
 * \brief Add Group if identifying Command Payload format.
 */

typedef struct _ZCL_AddGroupIfIdentifying_t
{
  uint16_t groupId;
  uint8_t groupName[1];
} ZCL_AddGroupIfIdentifying_t;

/**
 * \brief Add Group Response Command Payload format.
 */

typedef struct _ZCL_AddGroupResponse_t
{
  uint8_t status;
  uint16_t groupId;
} ZCL_AddGroupResponse_t;

/**
 * \brief View Group Response Command Payload format.
 */

typedef struct _ZCL_ViewGroupResponse_t
{
  uint8_t status;
  uint16_t groupId;
  uint8_t groupName[1];
} ZCL_ViewGroupResponse_t;

/**
 * \brief Get Group Membership Response Payload format.
 */

typedef struct _ZCL_GetGroupMembershipResponse_t
{
  uint8_t capacity;
  uint8_t groupCount;
  uint16_t groupList[CS_GROUP_TABLE_SIZE];
} ZCL_GetGroupMembershipResponse_t;

/**
 * \brief Remove Group Command Response Payload format.
 */

typedef struct _ZCL_RemoveGroupResponse_t
{
  uint8_t status;
  uint16_t groupId;
} ZCL_RemoveGroupResponse_t;


END_PACK



#endif /* _ZCLZLLGROUPSCLUSTER_H */

