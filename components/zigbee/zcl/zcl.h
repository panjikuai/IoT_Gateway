/*
 * zcl.h
 *
 * Created: 2015-04-14 14:57:15
 *  Author: matt.qian
 */ 


#ifndef ZCL_H_
#define ZCL_H_

#include <stdint.h>
#include <stdbool.h>

#define CCPU_TO_LE16(x) (x)

/******************************************************************************
                           Definitions section
******************************************************************************/
#define PROFILE_ID_SMART_ENERGY  CCPU_TO_LE16(0x0109)
#define PROFILE_ID_CBA CCPU_TO_LE16(0x0105)
#define PROFILE_ID_HOME_AUTOMATION CCPU_TO_LE16(0x0104)
#define PROFILE_ID_LIGHT_LINK CCPU_TO_LE16(0xc05e)

/******************************************************************************
                           Types section
******************************************************************************/
typedef uint8_t  ZCL_ClusterType_t; // Not used in ZCL
typedef uint16_t ZCL_AttributeId_t; //!< The type for holding attribute IDs
typedef uint8_t  ZCL_CommandId_t; //!< The type for holding ZCL command IDs
typedef uint32_t ZCL_UTCTime_t; //!< The type for defining values of time used in various commands
typedef uint32_t ZCL_Date_t; // Not used in ZCL
typedef uint16_t ZCL_ReportTime_t; //!< The type for setting reporting intervals, in seconds

/***************************************************************************//**
\brief Enumerated status values used in ZCL
*******************************************************************************/
typedef enum
{
  //! Operation was successful.
  ZCL_SUCCESS_STATUS                      = 0x00,
  //!Operation was not successful
  ZCL_FAILURE_STATUS                      = 0x01,

  //0x02 - 0x7d - Reserved
  //! Invalid parameters have been provided
  ZCL_INVALID_ATTRIBUTE_VALUE_STATUS      = 0x02,
  //! Permission table error
  ZCL_TC_PERMISSION_TABLE_ERROR_STATUS    = 0x03,
  //! APS has failed to set the provided link key
  ZCL_APS_LINK_KEY_ERROR_STATUS           = 0x04,
  /*! Not all attributes were successfully written during write attribute request. */
  ZCL_WRITE_ATTRIBUTES_FAILURE_STATUS     = 0x0f,

  //! The sender of the command does not have authorization to carry out this command.
  ZCL_NOT_AUTHORIZED_STATUS               = 0x7e,
  //! A reserved field/subfield/bit contains a nonzero value
  ZCL_RESERVED_FIELD_NOT_ZERO_STATUS      = 0x7f,
  /*! The command appears to contain the wrong fields, as detected either by the
      presence of one or more invalid field entries or by there being missing fields.
      Command not carried out. Implementer has discretion as to whether to return
      this error or ::ZCL_INVALID_FIELD_STATUS. */
  ZCL_MALFORMED_COMMAND_STATUS            = 0x80,
  /*! The specified cluster command is not supported on the device.Command not
      carried out. */
  ZCL_UNSUP_CLUSTER_COMMAND_STATUS        = 0x81,
  /*! The specified general ZCL command is not supported on the device.*/
  ZCL_UNSUP_GENERAL_COMMAND_STATUS        = 0x82,
  /*! A manufacturer-specific unicast, cluster specific command was received
      with an unknown manufacturer code, or the manufacturer code was recognized but
      the command is not supported. */
  ZCL_UNSUP_MANUF_CLUSTER_COMMAND         = 0x83,
  /*! A manufacturer-specific unicast, ZCL specific command was received with an
      unknown manufacturer code, or manufacturer code was recognized but the command
      is not supported. */
  ZCL_UNSUP_MANUF_GENERAL_COMMAND_STATUS  = 0x84,
  /*! At least one field of the command contains an incorrect value, according
      to the specification the device is implemented to. */
  ZCL_INVALID_FIELD_STATUS                = 0x85,
  /*! The specified attribute does not exist on the device. */
  ZCL_UNSUPPORTED_ATTRIBUTE_STATUS        = 0x86,
  /*! Out of range error, or set to a reserved value. Attribute keeps its old
      value. */
  ZCL_INVALID_VALUE_STATUS                = 0x87,
  /*! Attempt to write a read-only attribute. */
  ZCL_READ_ONLY_STATUS                    = 0x88,
  /*! An operation (e.g. an attempt to create an entry in a table) failed due
      to an insufficient amount of free space available.*/
  ZCL_INSUFFICIENT_SPACE_STATUS           = 0x89,
  /*! An attempt to create an entry in a table failed due to a duplicate entry
      already being present in the table. */
  ZCL_DUPLICATE_EXISTS_STATUS             = 0x8a,
  /*! The requested information (e.g. table entry) be found. */
  ZCL_NOT_FOUND_STATUS                    = 0x8b,
  /*! Periodic reports cannot be issued for this attribute. */
  ZCL_UNREPORTABLE_ATTRIBUTE_STATUS       = 0x8c,
  /*! The data type given for an attribute is incorrect. Command not carried out. */
  ZCL_INVALID_DATA_TYPE_STATUS            = 0x8d,
  /*! The selector for an attribute is incorrect. */
  ZCL_INVALID_SELECTOR_STATUS             = 0x8e,
  /*! A request has been made to read an attribute that the requester is not
      authorized to read. No action taken. */
  ZCL_WRITE_ONLY_STATUS                   = 0x8f,
  /*! Setting the requested values would put the device in an inconsistent state
      on startup. No action taken. */
  ZCL_INCONSISTENT_STARTUP_STATE_STATUS   = 0x90,
  /*! An attempt has been made to write an attribute that is present but is
      defined using an out-of-band method and not over the air. */
  ZCL_DEFINED_OUT_OF_BAND_STATUS          = 0x91,
  /*! Failed case when a otau client or a otau server decides to abort the upgrade process. */
  ZCL_ABORT_STATUS                        = 0x95,
  /*! Invalid OTA upgrade image (ex. failed signature validation or signer information check or CRC check) */
  ZCL_INVALID_IMAGE_STATUS                = 0x96,
  /*! Server does not have data block available yet */
  ZCL_WAIT_FOR_DATA_STATUS                = 0x97,
  /*! No OTA upgrade image available for a particular client */
  ZCL_NO_IMAGE_AVAILABLE_STATUS           = 0x98,
  /*! The client still requires more OTA upgrade image files in order to successfully upgrade */
  ZCL_REQUIRE_MORE_IMAGE_STATUS           = 0x99,

  //0x97 - 0xbf - Reserved
  /* Request was successfully processed, but application is forcing to reply with DefaultResponse */
  ZCL_SUCCESS_WITH_DEFAULT_RESPONSE_STATUS = 0xAA,

  /*! An operation was unsuccessful due to a hardware failure. */
  ZCL_HARDWARE_FAILURE_STATUS             = 0xc0,
  /*! An operation was unsuccessful due to a software failure. */
  ZCL_SOFTWARE_FAILURE_STATUS             = 0xc1,
  /*! An error occurred during calibration. */
  ZCL_CALIBRATION_ERROR_STATUS            = 0xc2,

  //0xc3 - 0xff - Reserved

  /*! Indicates that a response command has not been received in time. */
  ZCL_NO_RESPONSE_ERROR_STATUS            = 0xc3,
  //ZCL_ATTRIBUTE_NOT_FOUND_STATUS,
  ZCL_BAD_FRAME_STATUS                    = 0xc4,
  //ZCL_WRONG_ATTRIBUTE_TYPE_STATUS,
  ZCL_WRONG_RESPONSE_LENGTH_STATUS        = 0xc5,
  /*! The last element has been extracted from the payload, or the payload size
      has been exceeded. Putting/getting payload elements must be finished. */
  ZCL_END_PAYLOAD_REACHED_STATUS          = 0xfd,
  ZCL_MAX_PAYLOAD_REACHED_STATUS          = 0xfe,
  /*! The request cannot be processed with parameters passed to the request. */
  ZCL_INVALID_PARAMETER_STATUS            = 0xff,

} ZCL_Status_t;

/********************************************************************************//**
\brief ZigBee data types identifiers

ZigBee devices, such as thermostats, lamps, etc., are defined in terms of the
attributes they contain, which can be written, read or reported using
ZCL commands. The following list defines the data types and formats that
can be used for these attributes. Note that individual clusters, which may use
different or new types, show valid values, ranges, and units for the attributes they
represent.
Each data type is allocated an 8-bit data type ID. The most significant 5 bits of this
ID are used to divide the types into 32 type classes, and the least significant 3 bits
specify a specific data type within this class.
***********************************************************************************/
typedef enum
{
  //Null
  ZCL_NO_DATA_TYPE_ID                       = 0x00,

  //General data
  ZCL_8BIT_DATA_TYPE_ID                     = 0x08,
  ZCL_16BIT_DATA_TYPE_ID                    = 0x09,
  ZCL_24BIT_DATA_TYPE_ID                    = 0x0a,
  ZCL_32BIT_DATA_TYPE_ID                    = 0x0b,
  ZCL_40BIT_DATA_TYPE_ID                    = 0x0c,
  ZCL_48BIT_DATA_TYPE_ID                    = 0x0d,
  ZCL_56BIT_DATA_TYPE_ID                    = 0x0e,
  ZCL_64BIT_DATA_TYPE_ID                    = 0x0f,

  //Logical
  ZCL_BOOLEAN_DATA_TYPE_ID                  = 0x10,

  //Bitmap
  ZCL_8BIT_BITMAP_DATA_TYPE_ID              = 0x18,
  ZCL_16BIT_BITMAP_DATA_TYPE_ID             = 0x19,
  ZCL_24BIT_BITMAP_DATA_TYPE_ID             = 0x1a,
  ZCL_32BIT_BITMAP_DATA_TYPE_ID             = 0x1b,
  ZCL_40BIT_BITMAP_DATA_TYPE_ID             = 0x1c,
  ZCL_48BIT_BITMAP_DATA_TYPE_ID             = 0x1d,
  ZCL_56BIT_BITMAP_DATA_TYPE_ID             = 0x1e,
  ZCL_64BIT_BITMAP_DATA_TYPE_ID             = 0x1f,

  //Unsigned integer
  ZCL_U8BIT_DATA_TYPE_ID                    = 0x20,
  ZCL_U16BIT_DATA_TYPE_ID                   = 0x21,
  ZCL_U24BIT_DATA_TYPE_ID                   = 0x22,
  ZCL_U32BIT_DATA_TYPE_ID                   = 0x23,
  ZCL_U40BIT_DATA_TYPE_ID                   = 0x24,
  ZCL_U48BIT_DATA_TYPE_ID                   = 0x25,
  ZCL_U56BIT_DATA_TYPE_ID                   = 0x26,
  ZCL_U64BIT_DATA_TYPE_ID                   = 0x27,

  //Signed integer
  ZCL_S8BIT_DATA_TYPE_ID                    = 0x28,
  ZCL_S16BIT_DATA_TYPE_ID                   = 0x29,
  ZCL_S24BIT_DATA_TYPE_ID                   = 0x2a,
  ZCL_S32BIT_DATA_TYPE_ID                   = 0x2b,
  ZCL_S40BIT_DATA_TYPE_ID                   = 0x2c,
  ZCL_S48BIT_DATA_TYPE_ID                   = 0x2d,
  ZCL_S56BIT_DATA_TYPE_ID                   = 0x2e,
  ZCL_S64BIT_DATA_TYPE_ID                   = 0x2f,

  //Enumeration
  ZCL_8BIT_ENUM_DATA_TYPE_ID                = 0x30,
  ZCL_16BIT_ENUM_DATA_TYPE_ID               = 0x31,

  //Floating point
  ZCL_FSEMI_PRECISION_DATA_TYPE_ID          = 0x38,
  ZCL_FSINGLE_PRECISION_DATA_TYPE_ID        = 0x39,
  ZCL_FDOUBLE_PRECISION_DATA_TYPE_ID        = 0x3a,

  //String
  ZCL_OCTET_STRING_DATA_TYPE_ID             = 0x41,
  ZCL_CHARACTER_STRING_DATA_TYPE_ID         = 0x42,
  ZCL_LONG_OCTET_STRING_DATA_TYPE_ID        = 0x43,
  ZCL_LONG_CHARACTER_STRING_DATA_TYPE_ID    = 0x44,

  //Ordered sequence
  ZCL_ARRAY_DATA_TYPE_ID                    = 0x48,
  ZCL_STRUCTURE_DATA_TYPE_ID                = 0x4c,

  //Collection
  ZCL_SET_DATA_TYPE_ID                      = 0x50,
  ZCL_BAG_DATA_TYPE_ID                      = 0x51,

  //Time
  ZCL_TIME_OF_DAY_DATA_TYPE_ID              = 0xe0,
  ZCL_DATE_DATA_TYPE_ID                     = 0xe1,
  ZCL_UTC_TIME_DATA_TYPE_ID                 = 0xe2,

  //Identifier
  ZCL_CLUSTER_ID_DATA_TYPE_ID               = 0xe8,
  ZCL_ATTRIBUTE_ID_DATA_TYPE_ID             = 0xe9,
  ZCL_BACNET_OID_DATA_TYPE_ID               = 0xea,

  //Misscellaneous
  ZCL_IEEE_ADDRESS_DATA_TYPE_ID             = 0xf0,
  ZCL_128BIT_SECURITY_KEY_DATA_TYPE_ID      = 0xf1,
} ZCL_AttributeType_t;

/***************************************************************************//**
\brief The list of general ZCL commands
*******************************************************************************/
typedef enum
{
  /*! Reading particular attributes' values from a remote device*/
  ZCL_READ_ATTRIBUTES_COMMAND_ID                            = 0x00,
  ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID                   = 0x01,

/*Write attributes*/
  /*! Modifying values of particular attributes on a remote device*/
  ZCL_WRITE_ATTRIBUTES_COMMAND_ID                           = 0x02,
  ZCL_WRITE_ATTRIBUTES_UNDIVIDED_COMMAND_ID                 = 0x03,
  ZCL_WRITE_ATTRIBUTES_RESPONSE_COMMAND_ID                  = 0x04,
  ZCL_WRITE_ATTRIBUTES_NO_RESPONSE_COMMAND_ID               = 0x05,

/*Configure reporting*/
  ZCL_CONFIGURE_REPORTING_COMMAND_ID                        = 0x06,
  ZCL_CONFIGURE_REPORTING_RESPONSE_COMMAND_ID               = 0x07,

/*Read reporting*/
  ZCL_READ_REPORTING_CONFIGURATION_COMMAND_ID               = 0x08,
  ZCL_READ_REPORTING_CONFIGURATION_RESPONSE_COMMAND_ID      = 0x09,

/*Report attributes*/
  /*! Reporting particular attributes to a remote device; that is,
  sending current attributes' values*/
  ZCL_REPORT_ATTRIBUTES_COMMAND_ID                          = 0x0a,

/*Default response*/
  ZCL_DEFAULT_RESPONSE_COMMAND_ID                           = 0x0b,

/*Discover attributes*/
  /*! Obtaining information (IDs and types) of attributes supported
  by a particular cluster on a remote device*/
  ZCL_DISCOVER_ATTRIBUTES_COMMAND_ID                        = 0x0c,
  ZCL_DISCOVER_ATTRIBUTES_RESPONSE_COMMAND_ID               = 0x0d,

/*Read attributes structured*/
  ZCL_READ_ATTRIBUTES_STRUCTURED_COMMAND_ID                 = 0x0e,

/*Write attributes structured*/
  ZCL_WRITE_ATTRIBUTES_STRUCTURED_COMMAND_ID                = 0x0f,
  ZCL_WRITE_ATTRIBUTES_STRUCTURED_RESPONSE_COMMAND_ID       = 0x10,

/*Discover commands received*/
  ZCL_DISCOVER_COMMANDS_RECEIVED_COMMAND_ID                 = 0x11,
  ZCL_DISCOVER_COMMANDS_RECEIVED_RESPONSE_COMMAND_ID        = 0x12,

/*Discover commands generated*/
  ZCL_DISCOVER_COMMANDS_GENERATED_COMMAND_ID                = 0x13,
  ZCL_DISCOVER_COMMANDS_GENERATED_RESPONSE_COMMAND_ID       = 0x14,

/*Discover Attributes extended*/
  ZCL_DISCOVER_ATTRIBUTES_EXTENDED_COMMAND_ID               = 0x15,
  ZCL_DISCOVER_ATTRIBUTES_EXTENDED_RESPONSE_COMMAND_ID      = 0x16

} ZCL_GeneralCommandId_t;

/***************************************************************************//**
\brief Type describing possible ZSE Device IDs
*******************************************************************************/
typedef enum
{
  /*! The Range Extender is a simple device that acts as a router for other devices. The
      Range Extender device shall not be a ZigBee end device. A product that
      implements the Range Extender device shall not implement any other devices
      defined in this profile. This device shall only be used if the product is not intended
      to have any other application, or if a private application is implemented that has
      not been addressed by this profile.*/
  ZSE_RANGE_EXTENDER_DEVICE_ID                          = 0x0008,
  /*! The Energy Service Portal connects the energy supply company communication
      network to the metering and energy management devices within the home. It
      routes messages to and from the relevant end points. It may be installed within a
      meter, thermostat, or In-Premise Display, or may be a standalone device, and it
      will contain another non-ZigBee communication module (e.g. power-line carrier,
      RF, GPRS, broadband Internet connection).*/
  ZSE_ENERGY_SERVICE_INTERFACE_DEVICE_ID                = 0x0500,
  /*!The Metering end device is a meter (electricity, gas, water, heat, etc.) that is fitted
      with a ZigBee device. Depending on what is being metered, the device may be
      capable of immediate (requested) reads or it will autonomously send readings
      periodically. A Metering end device may also be capable of communicating
      certain status indicators (e.g. battery low, tamper detected).*/
  ZSE_METERING_DEVICE_DEVICE_ID                         = 0x0501,
  /*! The In-Premise Display device will relay energy consumption data to the user by
      way of a graphical or text display. The display may or may not be an interactive
      device. At a minimum at least one of the following should be displayed: current
      energy usage, a history over selectable periods, pricing information, or text
      messages. As an interactive device, it can be used for returning simple messages
      for interpretation by the recipient.*/
  ZSE_IN_PREMISE_DISPLAY_DEVICE_ID                      = 0x0502,
  /*! The PCT device shall provide the capability to control the premise heating and
      cooling systems.*/
  ZSE_PROGRAMMABLE_COMMUNICATING_THERMOSTAT_DEVICE_ID   = 0x0503,
  /*! The Load Control device is capable of receiving Demand Response and Load
      Control events to manage consumption on a range of devices. Example devices
      are water heaters, exterior lighting, and pool pumps.*/
  ZSE_LOAD_CONTROL_DEVICE_DEVICE_ID                     = 0x0504,
  /*! Smart Appliance devices on the ZigBee network can participate in energy
      management activities. Examples of these are when Utilities initiate a demand
      response or pricing event, or the appliance actively informs customers via inhome
      displays of when or how energy is being used. In the latter case, scenarios
      include: Washer switching to cold water during periods of higher energy costs;
      Washer/Dryer/Oven/Hot Water Heater reporting cycle status;
      Over temperature conditions in Freezers and Refrigerators.*/
  ZSE_SMART_APPLIANCE_DEVICE_ID                         = 0x0505,
  /*! The Prepayment Terminal device will allow utility customers or other users (e.g.
      sub-metered tenants) to pay for consumption in discrete increments rather than
      establishing a traditional billing agreement. The Prepayment Terminal device will
      accept payment (e.g. credit card, code entry), display remaining balances, and
      alert the user of a balance approaching zero, and may perform some or all of the
      other functions described in In-Premise Display.*/
  ZSE_PREPAYMENT_TERMINAL_DEVICE_ID                     = 0x0506,
  /*! The Physical Device type will identify a supplemental (or sole) endpoint on which
      the clusters related to a physical product may reside. The endpoint shall not
      contain any cluster related to any individual logical SE device on the physical
      product. A product is allowed to have a Physical Device as its sole SE endpoint. A
      Physical Device must be capable of providing other SE device endpoints to be a
      certified SE product.*/
  ZSE_PHYSICAL_DEVICE_ID                                = 0x0507,
} ZSE_DeviceId_t;

/***************************************************************************//**
\brief Type describing possible HA device IDs
*******************************************************************************/
typedef enum
{
  HA_ON_OFF_SWITCH_DEVICE_ID                             = 0x0000,
  /*! The Remote Control device is capable of controlling and monitoring other
      devices. Typically the Remote Control device is a handheld, battery powered device, that
      can control devices (for example, turn a light on/off), monitor devices (for
      example, read the status of a temperature sensor) or do some user configuration
      (for example, change the setpoint of a thermostat or a light sensor). */
  HA_REMOTE_CONTROL_DEVICE_ID                           = 0x0006,
  /*! The Combined Interface device is capable of controlling and monitoring other
      devices. It is typically a mains-powered device like a personal computer. */
  HA_COMBINED_INTERFACE_ID                              = 0x0007,
  /*! The On/Off Light device is a light that can be switched on and off. */
  HA_ON_OFF_LIGHT_DEVICE_ID                             = 0x0100,
  /*! The Dimmable Light device is a light that can be switched on and off, and whose
      luminance level may be controlled. */
  HA_DIMMABLE_LIGHT_DEVICE_ID                           = 0x0101,
  /*! The On/Off Light Switch device can send on, off and toggle commands to devices
      (typically lights) to switch them on or off. */
  HA_ON_OFF_LIGHT_SWITCH_DEVICE_ID                      = 0x0103,
  /*! The Dimmer Switch device can send on, off and toggle commands to devices
      (typically lights) to switch them on or off, and can also control the level of a
      characteristic of such devices (typically the brightness of lights). */
  HA_DIMMER_SWITCH_DEVICE_ID                            = 0x0104,
  /*! The Light Sensor device reports the illuminance of an area. */
  HA_LIGHT_SENSOR_DEVICE_ID                             = 0x0106,
  /*! The Occupancy Sensor device reports the occupancy state of an area. */
  HA_OCCUPANCY_SENSOR_DEVICE_ID                         = 0x0107,
  HA_DOOR_MAGNET_DEVICE_ID_EXTEND						= 0x0108,
  /*! The Thermostat device can have either built-in or separate sensors for
      temperature, humidity or occupancy. It allows the desired temperature to be set
      either remotely or locally. The thermostat may send heating and/or cooling
      requirement notifications to a heating/cooling unit (for example, an indoor air
      handler) or may include a mechanism to control a heating or cooling unit directly. */
  HA_THERMOSTAT_DEVICE_ID                               = 0x0301,
  /*! Temperature Sensor Device Id */
  HA_TEMPERATURE_SENSOR_DEVICE_ID                       = 0x0302,
  /*! IAS Control and Indicating Equipment */
  HA_IAS_CIE_DEVICE_ID                                  = 0x0400,
  /*! IAS Ancillary Control Equipment */
  HA_IAS_ACE_DEVICE_ID                                  = 0x0401,
  /*! IAS Zone */
  HA_IAS_ZONE_DEVICE_ID                                 = 0x0402,
  /*! IAS Warning Device */
  HA_IAS_WD_DEVICE_ID                                   = 0x0403
} HA_DeviceId_t;

/***************************************************************************//**
\brief Type describing possible ZLL device IDs
*******************************************************************************/
typedef enum
{
  /*! On/Off Light */
  ZLL_ON_OFF_LIGHT_DEVICE_ID                            = 0x0000,
  /*! On/off plug-in unit */
  ZLL_ON_OFF_PLUGIN_UNIT_DEVICE_ID                      = 0x0010,
  /*! Dimmable light */
  ZLL_DIMMABLE_LIGHT_DEVICE_ID                          = 0x0100,
  /*! Dimmable plug-in unit */
  ZLL_DIMMABLE_PLUGIN_UNIT_DEVICE_ID                    = 0x0110,
  /*! Color light */
  ZLL_COLOR_LIGHT_DEVICE_ID                             = 0x0200,
  /*! Extended color light */
  ZLL_EXTENDED_COLOR_LIGHT_DEVICE_ID                    = 0x0210,
  /*! Temperature color light */
  ZLL_TEMPERATURE_COLOR_LIGHT_DEVICE_ID                 = 0x0220,
  /*! Color remote */
  ZLL_COLOR_REMOTE_DEVICE_ID                            = 0x0800,
  /*! Color scene remote */
  ZLL_COLOR_SCENE_REMOTE_DEVICE_ID                      = 0x0810,
  /*! Non-color remote */
  ZLL_NON_COLOR_REMOTE_DEVICE_ID                        = 0x0820,
  /*! Non-color scene remote */
  ZLL_NON_COLOR_SCENE_REMOTE_DEVICE_ID                  = 0x0830,
  /*! Control bridge */
  ZLL_CONTROL_BRIDGE_DEVICE_ID                          = 0x0840,
  /*! On/off sensor */
  ZLL_ON_OFF_SENSOR_DEVICE_ID                           = 0x0850,
} ZLL_DeviceId_t;

/*************************************************************************//**
  \brief ZCL events' IDs
*****************************************************************************/
typedef enum
{
  ZCL_ATTRIBUTE_READ_EVENT_ID,      //!< Attribute read event
  ZCL_ATTRIBUTE_WRITE_EVENT_ID,     //!< Attribute write event
} ZCL_EventId_t;

/*************************************************************************//**
  \brief ZCL notifications' IDs
*****************************************************************************/
typedef enum
{
  ZCL_APS_CONFIRM_ID,      //!< request was sent and transport acknowledgment was received
  ZCL_ZCL_RESPONSE_ID,     //!< zcl response was received
  ZCL_DEFAULT_RESPONSE_ID  //!< zcl default response was received
} ZCL_NotifyId_t;

/********************************************************************************//**
\brief Type describing payload element of read request command.

This type can be used to add one attribute entry to the read request payload.
***********************************************************************************/
#pragma pack(1)
typedef struct _ZCL_ReadAttributeReq_t
{
	uint8_t attribute_count;
	ZCL_AttributeId_t id; //!< Requested attribute id
} ZCL_ReadAttributeReq_t;

typedef struct _ZCL_ReadAttributeResp_t
{
	uint8_t result;
	uint8_t data_length;
	ZCL_AttributeId_t id; //!< Requested attribute id
	uint8_t status;
	uint8_t type;
	union{
		uint8_t on_off;
		uint8_t level;
		uint8_t hue;
		uint8_t saturation;
		uint16_t enhancedHue;
		uint16_t colorX;
		uint16_t colorY;
		uint16_t colorTemp;;
		uint16_t colorCapabilities;
	}data; 
} ZCL_ReadAttributeResp_t;

/********************************************************************************//**
\brief Type describing payload element of write request command.

This type can be used to add one attribute entry to the write request payload.
***********************************************************************************/
typedef struct _ZCL_WriteAttributeReq_t
{
	ZCL_AttributeId_t id;       //!< requested attribute id
	uint8_t           type;     //!< requested attribute type
	uint8_t           value[1]; //!< requested attribute value
} ZCL_WriteAttributeReq_t;

#pragma pack()
/*************************************************************************//**
 \brief ZCL Data Type Descriptor.
        Describes the Data Type length in bytes and Data Type kind (Analog or Descrete).
*****************************************************************************/
typedef struct
{
  uint16_t   length;                     //!<Length in bytes
  /*!Kind of Data Type. The #ZCL_DATA_TYPE_ANALOG_KIND, \n
   !#ZCL_DATA_TYPE_DISCRETE_KIND or \n
   !#ZCL_DATA_TYPE_COLLECTION_KIND should be used).*/
  uint8_t   kind;
} ZCL_DataTypeDescriptor_t;

/*************************************************************************//**
  \brief Clusters IDs' definitions. The list comprises all clusters supported in BitCloud.
*****************************************************************************/
enum
{
  /* General clusters defined by ZCL Specification */
  BASIC_CLUSTER_ID = CCPU_TO_LE16(0x0000),                            //!<Basic cluster Id
  POWER_CONFIGURATION_CLUSTER_ID = CCPU_TO_LE16(0x0001),              //!<Power configuration cluster Id
  IDENTIFY_CLUSTER_ID = CCPU_TO_LE16(0x0003),                         //!<Identify cluster Id
  GROUPS_CLUSTER_ID = CCPU_TO_LE16(0x0004),                           //!<Groups cluster Id
  SCENES_CLUSTER_ID = CCPU_TO_LE16(0x0005),                           //!<Scenes cluster Id
  ONOFF_CLUSTER_ID = CCPU_TO_LE16(0x0006),                            //!<OnOff cluster id
  ONOFF_SWITCH_CONFIGURATION_CLUSTER_ID = CCPU_TO_LE16(0x0007),       //!<OnOff Switch Configuration cluster id
  LEVEL_CONTROL_CLUSTER_ID = CCPU_TO_LE16(0x0008),                    //!<Level Control cluster id
  ALARMS_CLUSTER_ID = CCPU_TO_LE16(0x0009),                            //!<Alarm cluster id
  TIME_CLUSTER_ID = CCPU_TO_LE16(0x000a),                             //!<Time cluster Id
  OTAU_CLUSTER_ID = CCPU_TO_LE16(0x0019),                             //!<OTAU cluster Id
  THERMOSTAT_CLUSTER_ID = CCPU_TO_LE16(0x0201),                       //!<Thermostat cluster Id
  FAN_CONTROL_CLUSTER_ID = CCPU_TO_LE16(0x0202),                       //!<Fan control cluster Id
  THERMOSTAT_UI_CONF_CLUSTER_ID = CCPU_TO_LE16(0x0204),               //!<Thermostat ui conf cluster Id
  /* Lighting */
  COLOR_CONTROL_CLUSTER_ID = CCPU_TO_LE16(0x0300),                    //!<Color Control cluster id
  ILLUMINANCE_MEASUREMENT_CLUSTER_ID = CCPU_TO_LE16(0x0400),          //!<Illuminance Sensing cluster id  
  TEMPERATURE_MEASUREMENT_CLUSTER_ID = CCPU_TO_LE16(0x0402),          //!<Temperature measurement cluster id
  HUMIDITY_MEASUREMENT_CLUSTER_ID = CCPU_TO_LE16(0x0405),             //!<Humidity measurement cluster id
  OCCUPANCY_SENSING_CLUSTER_ID = CCPU_TO_LE16(0x0406),                //!<Occupancy Sensing cluster id
  /* Security & Safety */
  IAS_ZONE_CLUSTER_ID = CCPU_TO_LE16(0x0500),                         //!<IAS Zone Cluster id
  IAS_ACE_CLUSTER_ID = CCPU_TO_LE16(0x0501),                          //!<IAS ACE Cluster id
  GENERIC_TUNNEL_CLUSTER_ID = CCPU_TO_LE16(0x0600),                   //!<Generic tunnel cluster Id
  BACNET_PROTOCOL_TUNNEL_CLUSTER_ID = CCPU_TO_LE16(0x0601),           //!<BACnet protocol tunnel cluster Id
  /* Smart Energy Profile specific clusters */
  PRICE_CLUSTER_ID = CCPU_TO_LE16(0x0700),                            //!<Price cluster Id
  DEMAND_RESPONSE_AND_LOAD_CONTROL_CLUSTER_ID = CCPU_TO_LE16(0x0701), //!<Demand Response and Load Control cluster Id
  SIMPLE_METERING_CLUSTER_ID = CCPU_TO_LE16(0x0702),                  //!<Simple Metering cluster Id
  MESSAGE_CLUSTER_ID = CCPU_TO_LE16(0x0703),                          //!<Message Cluster Id
  ZCL_SE_TUNNEL_CLUSTER_ID = CCPU_TO_LE16(0x0704),                    //!<Smart Energy Tunneling (Complex Metering)
  ZCL_KEY_ESTABLISHMENT_CLUSTER_ID = CCPU_TO_LE16(0x0800),            //!<ZCL Key Establishment Cluster Id
  DIAGNOSTICS_CLUSTER_ID = CCPU_TO_LE16(0x0b05),                      //!<Diagnostics cluster Id
  /* Light Link Profile clusters */
  ZLL_COMMISSIONING_CLUSTER_ID = CCPU_TO_LE16(0x1000),                //!<ZLL Commissioning Cluster Id
  /* Manufacturer specific clusters */
  LINK_INFO_CLUSTER_ID = CCPU_TO_LE16(0xFF00)                         //!<Link Info cluster id
};

/**//**
 * \brief APS addressing modes
 *
 *  Defines APS addressing modes
 **/
typedef enum
{
  /** Indirect addressing mode via binding */
  APS_NO_ADDRESS = 0x00,
  /** Group addressing mode via multicasting */
  APS_GROUP_ADDRESS = 0x01,
  /** Unicast addressing mode, with a 16-bit network (short) address */
  APS_SHORT_ADDRESS = 0x02,
  /** Unicast addressing mode, with a 64-bit IEEE (extended) address. */
  APS_EXT_ADDRESS = 0x03
} APS_AddrMode_t;

typedef uint64_t ExtAddr_t;   ///< Extended address type declaration.
typedef uint16_t PanId_t;     ///< PAN identifier type declaration.
typedef uint16_t ShortAddr_t; ///< Short address type declaration.

#define PACK
#define PRAGMA(x) _Pragma(#x)
#define BEGIN_PACK PRAGMA(pack(1)) 
#define END_PACK   PRAGMA(pack())  

#endif /* ZCL_H_ */
