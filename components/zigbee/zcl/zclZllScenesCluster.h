/***************************************************************************//**
  \file zclScenesCluster.h

  \brief
    The header file describes the ZCL Scenes Cluster and its interface

    The file describes the types and interface of the ZCL Scenes Cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    12.02.09 D. Kasyanov - Created.
*******************************************************************************/

#ifndef _ZCLSCENESCLUSTER_H
#define _ZCLSCENESCLUSTER_H

/*!
Attributes and commands for determining basic information about a device,
setting user device information such as location, enabling a device and resetting it
to factory defaults.
*/

/*******************************************************************************
                   Includes section
*******************************************************************************/

#include "zcl.h"
#include "zclZllOnOffCluster.h"
#include "zclZllLevelControlCluster.h"
#include "zclZllColorControlCluster.h"

/*******************************************************************************
                   Define(s) section
*******************************************************************************/

/**
 * \brief Scenes Cluster scene name support bit
*/

#define ZCL_SCENES_CLUSTER_NAME_SUPPORT_FLAG  0x80

#define ZCL_SCENES_CLUSTER_COPY_ALL_SCENES 0x01

/**
 * \brief Scenes Cluster server attributes amount
*/

#define ZCL_SCENES_CLUSTER_SERVER_ATTRIBUTES_AMOUNT 5

/**
 * \brief Scenes Cluster client attributes amount. Clinet doesn't have attributes.
*/

#define ZCL_SCENES_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT 0

/**
 * \brief Scenes Cluster commands amount
*/

#define ZCL_SCENES_CLUSTER_COMMANDS_AMOUNT 19

/**
 * \brief Scenes Cluster server's attributes identifiers
*/

#define ZCL_SCENES_CLUSTER_SCENE_COUNT_SERVER_ATTRIBUTE_ID    CCPU_TO_LE16(0x0000)
#define ZCL_SCENES_CLUSTER_CURRENT_SCENE_SERVER_ATTRIBUTE_ID  CCPU_TO_LE16(0x0001)
#define ZCL_SCENES_CLUSTER_CURRENT_GROUP_SERVER_ATTRIBUTE_ID  CCPU_TO_LE16(0x0002)
#define ZCL_SCENES_CLUSTER_SCENE_VALID_SERVER_ATTRIBUTE_ID    CCPU_TO_LE16(0x0003)
#define ZCL_SCENES_CLUSTER_NAME_SUPPORT_SERVER_ATTRIBUTE_ID   CCPU_TO_LE16(0x0004)

/**
 * \brief Scenes Cluster client's command identifiers
*/

#define ZCL_SCENES_CLUSTER_ADD_SCENE_COMMAND_ID            0x00
#define ZCL_SCENES_CLUSTER_VIEW_SCENE_COMMAND_ID           0x01
#define ZCL_SCENES_CLUSTER_REMOVE_SCENE_COMMAND_ID         0x02
#define ZCL_SCENES_CLUSTER_REMOVE_ALL_SCENES_COMMAND_ID    0x03
#define ZCL_SCENES_CLUSTER_STORE_SCENE_COMMAND_ID          0x04
#define ZCL_SCENES_CLUSTER_RECALL_SCENE_COMMAND_ID         0x05
#define ZCL_SCENES_CLUSTER_GET_SCENE_MEMBERSHIP_COMMAND_ID 0x06
#define ZCL_SCENES_CLUSTER_ENHANCED_ADD_SCENE_COMMAND_ID   0x40
#define ZCL_SCENES_CLUSTER_ENHANCED_VIEW_SCENE_COMMAND_ID  0x41
#define ZCL_SCENES_CLUSTER_COPY_SCENE_COMMAND_ID           0x42

/**
 * \brief Scenes Cluster servers's command identifiers
*/

#define ZCL_SCENES_CLUSTER_ADD_SCENE_RESPONSE_COMMAND_ID         0x00
#define ZCL_SCENES_CLUSTER_VIEW_SCENE_RESPONSE_COMMAND_ID        0x01
#define ZCL_SCENES_CLUSTER_REMOVE_SCENE_RESPONSE_COMMAND_ID      0x02
#define ZCL_SCENES_CLUSTER_REMOVE_ALL_SCENES_RESPONSE_COMMAND_ID 0x03
#define ZCL_SCENES_CLUSTER_STORE_SCENE_RESPONSE_COMMAND_ID       0x04
#define ZCL_SCENES_CLUSTER_GET_SCENE_MEMBERSHIP_RESPONSE_COMMAND_ID 0x06
#define ZCL_SCENES_CLUSTER_ENHANCED_ADD_SCENE_RESPONSE_COMMAND_ID 0x40
#define ZCL_SCENES_CLUSTER_ENHANCED_VIEW_SCENE_RESPONSE_COMMAND_ID 0x41
#define ZCL_SCENES_CLUSTER_COPY_SCENE_RESPONSE_COMMAND_ID        0x42

/******************************************************************************
                    Types section
******************************************************************************/
BEGIN_PACK

/**
 * \brief Add Scene Command Payload format.
 */

typedef struct _ZCL_AddScene_t
{
  uint16_t groupId;
  uint8_t sceneId;
  uint16_t transitionTime;
  uint8_t name[1];
} ZCL_AddScene_t;

/**
 * \brief View Scene Command Payload format.
 */

typedef struct _ZCL_ViewScene_t
{
  uint16_t groupId;
  uint8_t sceneId;
} ZCL_ViewScene_t;

/**
 * \brief Remove Scene Command Payload format.
 */

typedef struct _ZCL_RemoveScene_t
{
  uint16_t groupId;
  uint8_t sceneId;
} ZCL_RemoveScene_t;

/**
 * \brief Remove All Scenes Command Payload format.
 */

typedef struct _ZCL_RemoveAllScenes_t
{
  uint16_t groupId;
} ZCL_RemoveAllScenes_t;

/**
 * \brief Store Scene Command Payload format.
 */

typedef struct _ZCL_StoreScene_t
{
  uint16_t groupId;
  uint8_t sceneId;
} ZCL_StoreScene_t;

/**
 * \brief Recall Scene Command Payload format.
 */

typedef struct _ZCL_RecallScene_t
{
  uint16_t groupId;
  uint8_t sceneId;
} ZCL_RecallScene_t;

/**
 * \brief Get Scene Membership Command Payload format.
 */

typedef struct _ZCL_GetSceneMembership_t
{
  uint16_t groupId;
} ZCL_GetSceneMembership_t;

/**
 * \brief Enhanced Add Scene Command Payload format.
 */

typedef struct _ZCL_EnhancedAddScene_t
{
  uint16_t groupId;
  uint8_t sceneId;
  uint16_t transitionTime;
  uint8_t name[1];
  ZCL_OnOffClusterExtensionFieldSet_t onOffClusterExtFields;
  ZCL_LevelControlClusterExtensionFieldSet_t levelControlClusterExtFields;
  ZCL_ColorControlClusterExtensionFieldSet_t colorControlClusterExtFields;
} ZCL_EnhancedAddScene_t;

/**
 * \brief Enhanced View Scene Command Payload format.
 */

typedef struct _ZCL_EnhancedViewScene_t
{
  uint16_t groupId;
  uint8_t sceneId;
} ZCL_EnhancedViewScene_t;

/**
 * \brief Copy Scene Command Payload format.
 */

typedef struct _ZCL_CopyScene_t
{
  uint8_t  mode;
  uint16_t groupIdFrom;
  uint8_t  sceneIdFrom;
  uint16_t groupIdTo;
  uint8_t  sceneIdTo;
} ZCL_CopyScene_t;

/**
 * \brief Add Scene Response Command Payload format.
 */

typedef struct _ZCL_AddSceneResponse_t
{
  uint8_t status;
  uint16_t groupId;
  uint8_t sceneId;
} ZCL_AddSceneResponse_t;

/**
 * \brief View Scene Response Command Payload format.
 */

typedef struct _ZCL_ViewSceneResponse_t
{
  uint8_t status;
  uint16_t groupId;
  uint8_t sceneId;
  uint16_t transitionTime;
  uint8_t nameAndExtField[1]; // string sceneName, extension field.
} ZCL_ViewSceneResponse_t;

/**
 * \brief Remove Scene Command Payload format.
 */

typedef struct _ZCL_RemoveSceneResponse_t
{
  uint8_t status;
  uint16_t groupId;
  uint8_t sceneId;
} ZCL_RemoveSceneResponse_t;

/**
 * \brief Remove All Scenes Response Command Payload format.
 */

typedef struct _ZCL_RemoveAllScenesResponse_t
{
  uint8_t status;
  uint16_t groupId;
} ZCL_RemoveAllScenesResponse_t;

/**
 * \brief Store Scene Response Command Payload format.
 */

typedef struct _ZCL_StoreSceneResponse_t
{
  uint8_t status;
  uint16_t groupId;
  uint8_t sceneId;
} ZCL_StoreSceneResponse_t;

/**
 * \brief Get Scene Membership Response Command Payload format.
 */

typedef struct _ZCL_GetSceneMembershipResponse_t
{
  uint8_t status;
  uint8_t capacity;
  uint16_t groupId;
  uint8_t sceneCount;
  uint8_t sceneList[16];
} ZCL_GetSceneMembershipResponse_t;

/**
 * \brief Enhanced Add Scene Response Command Payload format.
 */

typedef struct _ZCL_EnhancedAddSceneResponse_t
{
  uint8_t status;
  uint16_t groupId;
  uint8_t sceneId;
} ZCL_EnhancedAddSceneResponse_t;

/**
 * \brief Enhanced View Scene Response Command Payload format.
 */

typedef struct _ZCL_EnhancedViewSceneResponse_t
{
  uint8_t status;
  uint16_t groupId;
  uint8_t sceneId;
  uint16_t transitionTime;
  uint8_t name[1];
  ZCL_OnOffClusterExtensionFieldSet_t onOffClusterExtFields;
  ZCL_LevelControlClusterExtensionFieldSet_t levelControlClusterExtFields;
  ZCL_ColorControlClusterExtensionFieldSet_t colorControlClusterExtFields;
} ZCL_EnhancedViewSceneResponse_t;

/**
 * \brief Copy Scene Response Command Payload format.
 */

typedef struct _ZCL_CopySceneResponse_t
{
  uint8_t  status;
  uint16_t groupIdFrom;
  uint8_t  sceneIdFrom;
} ZCL_CopySceneResponse_t;

/**
 * \brief Extension Fields Set format
 */

typedef struct _ZCL_ExtensionFieldSets_t
{
  uint16_t clusterId;
  uint8_t length;
} ZCL_ExtensionFieldSets_t;



END_PACK


#endif /* _ZCLSCENESCLUSTER_H */

