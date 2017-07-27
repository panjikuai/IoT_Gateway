/*
 * zclZllBasicCluster.h
 *
 * Created: 2015-04-14 16:41:49
 *  Author: matt.qian
 */ 


#ifndef ZCLZLLBASICCLUSTER_H_
#define ZCLZLLBASICCLUSTER_H_

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

/***************************************************************************//**
  \brief ZCL Basic Cluster server side commands amount
*******************************************************************************/
#define ZCL_BASIC_CLUSTER_SERVER_ATTRIBUTES_AMOUNT        9
//The ZCLVersion and PowerSource attributes

/***************************************************************************//**
  \brief ZCL Basic Cluster derver side commands amount
*******************************************************************************/
#define ZCL_BASIC_CLUSTER_SERVER_COMMANDS_AMOUNT          0
//There are no any commands at the server side

/***************************************************************************//**
  \brief ZCL Basic Cluster client side commands amount
*******************************************************************************/
#define ZCL_BASIC_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT        0
//There are no any attributes at the client side

/***************************************************************************//**
  \brief ZCL Basic Cluster derver side commands amount
*******************************************************************************/
#define ZCL_BASIC_CLUSTER_CLIENT_COMMANDS_AMOUNT          0
//There are no any commands at the client side


//Atribute Ids of Basic Device Information Attribute Set at the server side
//!ZCL Basic Cluster server side ZCLVersion attribute id
#define ZCL_BASIC_CLUSTER_SERVER_ZCL_VERSION_ATTRIBUTE_ID               CCPU_TO_LE16(0x0000)
//!ZCL Basic Cluster server side ApplicationVersion attribute id
#define ZCL_BASIC_CLUSTER_SERVER_APPLICATION_VERSION_ATTRIBUTE_ID       CCPU_TO_LE16(0x0001)
//!ZCL Basic Cluster server side StackVersion attribute id
#define ZCL_BASIC_CLUSTER_SERVER_STACK_VERSION_ATTRIBUTE_ID             CCPU_TO_LE16(0x0002)
//!ZCL Basic Cluster server side HWVersion attribute id
#define ZCL_BASIC_CLUSTER_SERVER_HW_VERSION_ATTRIBUTE_ID                CCPU_TO_LE16(0x0003)
//!ZCL Basic Cluster server side ManufacturerName attribute id
#define ZCL_BASIC_CLUSTER_SERVER_MANUFACTURER_NAME_ATTRIBUTE_ID         CCPU_TO_LE16(0x0004)
//!ZCL Basic Cluster server side ModelIdentifier attribute id
#define ZCL_BASIC_CLUSTER_SERVER_MODEL_IDENTIFIER_ATTRIBUTE_ID          CCPU_TO_LE16(0x0005)
//!ZCL Basic Cluster server side DateCode attribute id
#define ZCL_BASIC_CLUSTER_SERVER_DATE_CODE_ATTRIBUTE_ID                 CCPU_TO_LE16(0x0006)
//!ZCL Basic Cluster server side PowerSource attribute id
#define ZCL_BASIC_CLUSTER_SERVER_POWER_SOURCE_ATTRIBUTE_ID              CCPU_TO_LE16(0x0007)

//Atribute Ids of Basic Device Settings Attribute Set at the server side
//!ZCL Basic Cluster server side LocationDescription attribute id
#define ZCL_BASIC_CLUSTER_SERVER_LOCATION_DESCRIPTION_ATTRIBUTE_ID      CCPU_TO_LE16(0x0010)
//!ZCL Basic Cluster server side PhysicalEnvironment attribute id
#define ZCL_BASIC_CLUSTER_SERVER_PHYSICAL_ENVIRONMENT_ATTRIBUTE_ID      CCPU_TO_LE16(0x0011)
//!ZCL Basic Cluster server side DeviceEnabled attribute id
#define ZCL_BASIC_CLUSTER_SERVER_DEVICE_ENABLED_ATTRIBUTE_ID            CCPU_TO_LE16(0x0012)
//!ZCL Basic Cluster server side AlarmMask attribute id
#define ZCL_BASIC_CLUSTER_SERVER_ALARM_MASK_ATTRIBUTE_ID                CCPU_TO_LE16(0x0013)

//!ZLL Basic Cluster server side SWBuildID attribute id
#define ZCL_BASIC_CLUSTER_SERVER_SW_BUILD_ID_ATTRIBUTE_ID               CCPU_TO_LE16(0x4000)

//!ZCL Basic Cluster server side ResetToFactoryDeafaults commabd id
#define ZCL_BASIC_CLUSTER_SERVER_RESET_TO_FACTORY_DEFAULTS_COMMAND_ID   0x00


BEGIN_PACK
/***************************************************************************//**
  \brief
    ZCL Basic Cluster server side attributes
*******************************************************************************/
typedef struct PACK
{
  //!Basic Device Information attribute set (Id == 0x00)

  /*!ZCL Version attribute (Read only, M)
  The ZCLVersion attribute is 8-bits in length and specifies the version number of
  the ZigBee Cluster Library that all clusters on this endpoint conform to. For the
  initial version of the ZCL, this attribute shall be set to 0x01.
  */
  struct PACK
  {
    ZCL_AttributeId_t   id;           //!<Attribute Id (0x0000, #ZCL_BASIC_CLUSTER_SERVER_ZCL_VERSION_ATTRIBUTE_ID)
    uint8_t             type;         //!<Attribute type (unsigned 8-bit integer, #ZCL_U8BIT_DATA_TYPE_ID)
    uint8_t             properties;   //!<Attribute properties bitmask
    uint8_t             value;        //!<Attribute value (default 0x00)
  } zclVersion;

  /*!TBD
  */
  struct PACK
  {
    ZCL_AttributeId_t   id;           //!<Attribute Id (0x0001, #ZCL_BASIC_CLUSTER_SERVER_APPLICATION_VERSION_ATTRIBUTE_ID)
    uint8_t             type;         //!<Attribute type (unsigned 8-bit integer, #ZCL_U8BIT_DATA_TYPE_ID)
    uint8_t             properties;   //!<Attribute properties bitmask
    uint8_t             value;        //!<Attribute value (default 0x00)
  } applicationVersion;

  /*!TBD
  */
  struct PACK
  {
    ZCL_AttributeId_t   id;           //!<Attribute Id (0x0002, #ZCL_BASIC_CLUSTER_SERVER_STACK_VERSION_ATTRIBUTE_ID)
    uint8_t             type;         //!<Attribute type (unsigned 8-bit integer, #ZCL_U8BIT_DATA_TYPE_ID)
    uint8_t             properties;   //!<Attribute properties bitmask
    uint8_t             value;        //!<Attribute value (default 0x00)
  } stackVersion;

  /*!TBD
  */
  struct PACK
  {
    ZCL_AttributeId_t   id;           //!<Attribute Id (0x0003, #ZCL_BASIC_CLUSTER_SERVER_HW_VERSION_ATTRIBUTE_ID)
    uint8_t             type;         //!<Attribute type (unsigned 8-bit integer, #ZCL_U8BIT_DATA_TYPE_ID)
    uint8_t             properties;   //!<Attribute properties bitmask
    uint8_t             value;        //!<Attribute value (default 0x00)
  } hwVersion;

  /*!TBD
  */
  struct PACK
  {
    ZCL_AttributeId_t   id;           //!<Attribute Id (0x0004, #ZCL_BASIC_CLUSTER_SERVER_MANUFACTURER_NAME_ATTRIBUTE_ID)
    uint8_t             type;         //!<Attribute type (character string, #ZCL_CHARACTER_STRING_DATA_TYPE_ID)
    uint8_t             properties;   //!<Attribute properties bitmask
    uint8_t             value[8];     //!<Attribute value
  } manufacturerName;

  /*!TBD
  */
  struct PACK
  {
    ZCL_AttributeId_t   id;           //!<Attribute Id (0x0005, #ZCL_BASIC_CLUSTER_SERVER_MODEL_IDENTIFIER_ATTRIBUTE_ID)
    uint8_t             type;         //!<Attribute type (character string, #ZCL_CHARACTER_STRING_DATA_TYPE_ID)
    uint8_t             properties;   //!<Attribute properties bitmask
    uint8_t             value[8];     //!<Attribute value
  } modelIdentifier;

  /*!TBD
  */
  struct PACK
  {
    ZCL_AttributeId_t   id;           //!<Attribute Id (0x0006, #ZCL_BASIC_CLUSTER_SERVER_DATE_CODE_ATTRIBUTE_ID)
    uint8_t             type;         //!<Attribute type (character string, #ZCL_CHARACTER_STRING_DATA_TYPE_ID)
    uint8_t             properties;   //!<Attribute properties bitmask
    uint8_t             value[10];     //!<Attribute value
  } dateCode;

  /*!Power Source attribute (Read only, M)
  The PowerSource attribute is 8-bits in length and specifies the source(s) of power
  available to the device. Bits b0 of this attribute represent the primary power
  source of the device and bit b7 indicates whether the device has a secondary power
  source in the form of a battery backup.

  Bits b0 of this attribute shall be set to one of the non-reserved values:
  0x00        - Unknown
  0x01        - Mains (single phase)
  0x02        - Mains (3 phase)
  0x03        - Battery
  0x04        - DC source
  0x05        - Emergency mains constantly powered
  0x06        - Emergency mains and transfer switch
  0x07-0x7f   - Reserved

  Bit b7 of this attribute shall be set to 1 if the device has a secondary power source
  in the form of a battery backup. Otherwise, bit b7 shall be set to 0.
  */
  struct PACK
  {
    ZCL_AttributeId_t   id;           //!<Attribute Id (0x0007, #ZCL_BASIC_CLUSTER_SERVER_POWER_SOURCE_ATTRIBUTE_ID)
    uint8_t             type;         //!<Attribute type (8-bit enumeration, #ZCL_8BIT_ENUM_DATA_TYPE_ID)
    uint8_t             properties;   //!<Attribute properties bitmask
    uint8_t             value;        //!<Attribute value (default 0x00)
  } powerSource;


  /*!TBD
  */
  struct PACK
  {
    ZCL_AttributeId_t   id;           //!<Attribute Id (0x4000, #ZCL_BASIC_CLUSTER_SERVER_SW_BUILD_ID_ATTRIBUTE_ID)
    uint8_t             type;         //!<Attribute (character string, #ZCL_CHARACTER_STRING_DATA_TYPE_ID)
    uint8_t             properties;   //!<Attribute properties bitmask
    uint8_t             value[9];     //!<Attribute value
  } swBuildId;

  //!Basic Device Settings attribute set (Id == 0x01)
} ZCL_BasicClusterServerAttributes_t;

END_PACK



#endif /* ZCLZLLBASICCLUSTER_H_ */
