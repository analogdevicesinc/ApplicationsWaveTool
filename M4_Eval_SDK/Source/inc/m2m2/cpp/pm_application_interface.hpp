// #############################################################################
// THIS IS AN AUTOMATICALLY GENERATED FILE, DO NOT MODIFY IT!
// #############################################################################
#pragma once

#include "common_application_interface.hpp"
#include "m2m2_core.hpp"
#include <stdint.h>


/* Explicitly enforce struct packing so that the nested structs and unions are laid out
    as expected. */
#if defined __CC_ARM || defined __IAR_SYSTEMS_ICC__ || __clang__ || defined _MSC_VER || defined __GNUC__
// DELIBERATELY BLANK
#else
#error "WARNING! Your compiler might not support '#pragma pack(1)'! \
  You must add an equivalent compiler directive to the file generator!"
#endif  // defined __CC_ARM || defined __IAR_SYSTEMS_ICC__ || __clang__ || defined _MSC_VER || defined __GNUC__
#pragma pack(1)

enum M2M2_PM_SYS_COMMAND_ENUM_t:uint8_t {
  __M2M2_PM_SYS_COMMAND_LOWEST = 64,
  M2M2_PM_SYS_COMMAND_SET_DATE_TIME_REQ = 66,
  M2M2_PM_SYS_COMMAND_SET_DATE_TIME_RESP = 67,
  M2M2_PM_SYS_COMMAND_GET_BAT_INFO_REQ = 68,
  M2M2_PM_SYS_COMMAND_GET_BAT_INFO_RESP = 69,
  M2M2_PM_SYS_COMMAND_SET_BAT_THR_REQ = 70,
  M2M2_PM_SYS_COMMAND_SET_BAT_THR_RESP = 71,
  M2M2_PM_SYS_COMMAND_SET_PWR_STATE_REQ = 72,
  M2M2_PM_SYS_COMMAND_SET_PWR_STATE_RESP = 73,
  M2M2_PM_SYS_COMMAND_GET_INFO_REQ = 74,
  M2M2_PM_SYS_COMMAND_GET_INFO_RESP = 75,
  M2M2_PM_SYS_COMMAND_ENABLE_BAT_CHARGE_REQ = 76,
  M2M2_PM_SYS_COMMAND_ENABLE_BAT_CHARGE_RESP = 77,
  M2M2_PM_SYS_COMMAND_DISABLE_BAT_CHARGE_REQ = 78,
  M2M2_PM_SYS_COMMAND_DISABLE_BAT_CHARGE_RESP = 79,
  M2M2_PM_SYS_COMMAND_USB_PWR_REQ = 80,
  M2M2_PM_SYS_COMMAND_USB_PWR_RESP = 81,
  M2M2_PM_SYS_COMMAND_GET_DATE_TIME_REQ = 82,
  M2M2_PM_SYS_COMMAND_GET_DATE_TIME_RESP = 83,
  M2M2_PM_SYS_COMMAND_GET_BOARD_INFO_REQ = 84,
  M2M2_PM_SYS_COMMAND_GET_BOARD_INFO_RESP = 85,
  M2M2_PM_SYS_COMMAND_THERMISTOR_STATE_CHANGE_REQ = 86,
  M2M2_PM_SYS_COMMAND_THERMISTOR_STATE_CHANGE_RESP = 87,
  M2M2_PM_SYS_COMMAND_GET_MCU_VERSION_REQ = 88,
  M2M2_PM_SYS_COMMAND_GET_MCU_VERSION_RESP = 89,
  M2M2_PM_SYS_COMMAND_BOOST_REQ = 90,
  M2M2_PM_SYS_COMMAND_BOOST_RESP = 91,
  M2M2_PM_SYS_COMMAND_ADP5258_WRITE_RDAC_REQ = 92,
  M2M2_PM_SYS_COMMAND_ADP5258_WRITE_RDAC_RESP = 93,
  M2M2_PM_SYS_COMMAND_BLUETOOTH_REQ = 94,
  M2M2_PM_SYS_COMMAND_BLUETOOTH_RESP = 95,
  M2M2_PM_SYS_COMMAND_ENABLE_USER_CONFIG_LOG_REQ = 96,
  M2M2_PM_SYS_COMMAND_ENABLE_USER_CONFIG_LOG_RESP = 97,
  M2M2_PM_SYS_COMMAND_DISABLE_USER_CONFIG_LOG_REQ = 98,
  M2M2_PM_SYS_COMMAND_DISABLE_USER_CONFIG_LOG_RESP = 99,
  M2M2_PM_SYS_COMMAND_STORE_TO_EEPROM_REQ = 100,
  M2M2_PM_SYS_COMMAND_STORE_TO_EEPROM_RESP = 101,
  M2M2_PS_SYS_COMMAND_GET_LOW_TOUCH_LOGGING_STATUS_REQ = 102,
  M2M2_PS_SYS_COMMAND_GET_LOW_TOUCH_LOGGING_STATUS_RESP = 103,
  M2M2_PM_SYS_COMMAND_WRITE_EEPROM_REQ = 104,
  M2M2_PM_SYS_COMMAND_WRITE_EEPROM_RESP = 105,
  M2M2_PM_SYS_COMMAND_READ_EEPROM_REQ = 106,
  M2M2_PM_SYS_COMMAND_READ_EEPROM_RESP = 107,
  M2M2_PM_SYS_COMMAND_AD5110_WRITE_RDAC_REQ = 108,
  M2M2_PM_SYS_COMMAND_AD5110_WRITE_RDAC_RESP = 109,
  M2M2_PM_SYS_COMMAND_AD5110_READ_RDAC_REQ = 110,
  M2M2_PM_SYS_COMMAND_AD5110_READ_RDAC_RESP = 111,
  M2M2_PM_SYS_COMMAND_ADP5258_READ_RDAC_REQ = 112,
  M2M2_PM_SYS_COMMAND_ADP5258_READ_RDAC_RESP = 113,
  M2M2_PM_SYS_COMMAND_ADP5350_OVP_SETTING_REQ = 114,
  M2M2_PM_SYS_COMMAND_ADP5350_OVP_SETTING_RESP = 115,
  M2M2_PM_SYS_COMMAND_AD5110_Store_to_EEPROM_REQ = 116,
  M2M2_PM_SYS_COMMAND_AD5110_Store_to_EEPROM_RESP = 117,
  M2M2_PM_SYS_COMMAND_SYSTEM_RESET_REQ = 118,
  M2M2_PM_SYS_COMMAND_SYSTEM_RESET_RESP = 119,
  M2M2_PM_SYS_COMMAND_DUT_POWER_ON_REQ = 120,
  M2M2_PM_SYS_COMMAND_DUT_POWER_ON_RESP = 121,
  M2M2_PM_SYS_COMMAND_DUT_POWER_OFF_REQ = 122,
  M2M2_PM_SYS_COMMAND_DUT_POWER_OFF_RESP = 123,
};
static_assert(sizeof(M2M2_PM_SYS_COMMAND_ENUM_t) == 1, "Enum 'M2M2_PM_SYS_COMMAND_ENUM_t' has an incorrect size!");

enum M2M2_PM_SYS_STATUS_ENUM_t:uint8_t {
  __M2M2_PM_SYS_STATUS_LOWEST = 64,
  M2M2_PM_SYS_STATUS_OK = 65,
  M2M2_PM_SYS_STATUS_ERR_ARGS = 66,
  M2M2_PM_SYS_STATUS_LOW_TOUCH_LOGGING_ALREADY_STARTED = 67,
  M2M2_PM_SYS_STATUS_CONFIG_FILE_NOT_FOUND = 68,
  M2M2_PM_SYS_STATUS_CONFIG_FILE_READ_ERR = 69,
  M2M2_PM_SYS_STATUS_ENABLE_USER_CONFIG_LOG_FAILED = 70,
  M2M2_PM_SYS_STATUS_USER_CONFIG_LOG_ENABLED = 71,
  M2M2_PM_SYS_STATUS_DISABLE_USER_CONFIG_LOG_FAILED = 72,
  M2M2_PM_SYS_STATUS_USER_CONFIG_LOG_DISABLED = 73,
  M2M2_PM_SYS_STATUS_LOG_STOPPED_THROUGH_BUTTON_A = 74,
  M2M2_PM_SYS_STATUS_LOW_TOUCH_LOGGING_IN_PROGRESS = 75,
  M2M2_PM_SYS_STATUS_LOW_TOUCH_LOGGING_NOT_STARTED = 76,
  M2M2_PM_SYS_STATUS_LOW_TOUCH_MAX_FILE_ERR = 77,
  M2M2_PM_SYS_STATUS_LOW_TOUCH_MEMORY_FULL_ERR = 78,
  M2M2_PM_SYS_STATUS_ERR_NOT_CHKD = 255,
};
static_assert(sizeof(M2M2_PM_SYS_STATUS_ENUM_t) == 1, "Enum 'M2M2_PM_SYS_STATUS_ENUM_t' has an incorrect size!");

enum M2M2_PM_SYS_BAT_STATE_ENUM_t:uint8_t {
  M2M2_PM_SYS_BAT_STATE_NOT_AVAIL = 0,
  M2M2_PM_SYS_BAT_STATE_NOT_CHARGING = 1,
  M2M2_PM_SYS_BAT_STATE_CHARGING = 2,
  M2M2_PM_SYS_BAT_STATE_COMPLETE = 3,
  M2M2_PM_SYS_BAT_STATE_DETECTION = 4,
  M2M2_PM_SYS_BAT_STATE_CHARGE_ERR = 5,
};
static_assert(sizeof(M2M2_PM_SYS_BAT_STATE_ENUM_t) == 1, "Enum 'M2M2_PM_SYS_BAT_STATE_ENUM_t' has an incorrect size!");

enum M2M2_PM_SYS_USB_PWR_ENUM_t:uint8_t {
  M2M2_PM_SYS_USB_PWR_ENABLE = 0,
  M2M2_PM_SYS_USB_PWR_DISABLE = 1,
};
static_assert(sizeof(M2M2_PM_SYS_USB_PWR_ENUM_t) == 1, "Enum 'M2M2_PM_SYS_USB_PWR_ENUM_t' has an incorrect size!");

enum M2M2_PM_SYS_THERMISTOR_STATE_ENUM_t:uint8_t {
  M2M2_PM_SYS_THERMISTOR_ENABLE = 0,
  M2M2_PM_SYS_THERMISTOR_DISABLE = 1,
};
static_assert(sizeof(M2M2_PM_SYS_THERMISTOR_STATE_ENUM_t) == 1, "Enum 'M2M2_PM_SYS_THERMISTOR_STATE_ENUM_t' has an incorrect size!");

enum M2M2_PM_SYS_USB_PWR_ACTION_ENUM_t:uint8_t {
  M2M2_PM_SYS_USB_PWR_SET = 0,
  M2M2_PM_SYS_USB_PWR_GET = 1,
};
static_assert(sizeof(M2M2_PM_SYS_USB_PWR_ACTION_ENUM_t) == 1, "Enum 'M2M2_PM_SYS_USB_PWR_ACTION_ENUM_t' has an incorrect size!");

enum M2M2_PM_SYS_PWR_STATE_ENUM_t:uint8_t {
  M2M2_PM_SYS_PWR_STATE_ACTIVE = 0,
  M2M2_PM_SYS_PWR_STATE_FLEXI = 1,
  M2M2_PM_SYS_PWR_STATE_HIBERNATE = 2,
  M2M2_PM_SYS_PWR_STATE_SHUTDOWN = 3,
};
static_assert(sizeof(M2M2_PM_SYS_PWR_STATE_ENUM_t) == 1, "Enum 'M2M2_PM_SYS_PWR_STATE_ENUM_t' has an incorrect size!");

enum M2M2_PM_SYS_MCU_TYPE_ENUM_t:uint8_t {
  M2M2_PM_SYS_MCU_INVALID = 0,
  M2M2_PM_SYS_MCU_M3 = 1,
  M2M2_PM_SYS_MCU_M4 = 2,
};
static_assert(sizeof(M2M2_PM_SYS_MCU_TYPE_ENUM_t) == 1, "Enum 'M2M2_PM_SYS_MCU_TYPE_ENUM_t' has an incorrect size!");

enum M2M2_PM_SYS_OVP_ENUM_t:uint8_t {
  M2M2_PM_SYS_OVP_18_5V = 0,
  M2M2_PM_SYS_OVP_15V = 1,
  M2M2_PM_SYS_OVP_10V = 2,
  M2M2_PM_SYS_OVP_5_6V = 3,
};
static_assert(sizeof(M2M2_PM_SYS_OVP_ENUM_t) == 1, "Enum 'M2M2_PM_SYS_OVP_ENUM_t' has an incorrect size!");

enum M2M2_PM_SYS_BOOST_ENUM_t:uint8_t {
  M2M2_PM_SYS_BOOST_ENABLE = 0,
  M2M2_PM_SYS_BOOST_DISABLE = 1,
};
static_assert(sizeof(M2M2_PM_SYS_BOOST_ENUM_t) == 1, "Enum 'M2M2_PM_SYS_BOOST_ENUM_t' has an incorrect size!");

enum ADI_PM_BOARD_TYPE_t:uint8_t {
  ADI_PM_BOARD_TYPE_UNKNOWN = 0,
  ADI_PM_BOARD_TYPE_ADPD107_WATCH = 1,
  ADI_PM_BOARD_TYPE_ADPD107_CHEST_STRAP = 2,
  ADI_PM_BOARD_TYPE_ADPD185_WATCH = 3,
  ADI_PM_BOARD_TYPE_ADPD188_WATCH = 4,
};
static_assert(sizeof(ADI_PM_BOARD_TYPE_t) == 1, "Enum 'ADI_PM_BOARD_TYPE_t' has an incorrect size!");

struct m2m2_pm_sys_cmd_t {
  uint8_t  command; 
  uint8_t  status; 
};

struct m2m2_pm_sys_pwr_state_t {
  uint8_t  command; 
  uint8_t  status; 
  M2M2_PM_SYS_PWR_STATE_ENUM_t  state; 
};

struct m2m2_pm_sys_info_t {
  uint8_t  command; 
  uint8_t  status; 
  uint16_t  version; 
  uint8_t  mac_addr[6]; 
  uint32_t  device_id; 
  uint32_t  model_number; 
  uint16_t  hw_id; 
  uint16_t  bom_id; 
  uint8_t  batch_id; 
  uint32_t  date; 
  ADI_PM_BOARD_TYPE_t  board_type; 
};

struct m2m2_pm_sys_date_time_req_t {
  uint8_t  command; 
  uint8_t  status; 
  uint16_t  year; 
  uint8_t  month; 
  uint8_t  day; 
  uint8_t  hour; 
  uint8_t  minute; 
  uint8_t  second; 
  uint32_t  TZ_sec; 
};

struct m2m2_pm_sys_bat_info_resp_t {
  uint8_t  command; 
  uint8_t  status; 
  uint32_t  timestamp; 
  M2M2_PM_SYS_BAT_STATE_ENUM_t  bat_chrg_stat; 
  uint8_t  bat_lvl; 
  uint16_t  bat_mv; 
  uint16_t  bat_temp; 
};

struct m2m2_pm_sys_bat_thr_req_t {
  uint8_t  command; 
  uint8_t  status; 
  uint8_t  bat_level_low; 
  uint8_t  bat_level_critical; 
};

struct m2m2_bat_crit_info_resp_t {
  uint8_t  command; 
  uint8_t  status; 
  uint16_t  bat_mv; 
};

struct m2m2_pm_sys_usb_pwr_cmd_t {
  uint8_t  command; 
  uint8_t  status; 
  M2M2_PM_SYS_USB_PWR_ENUM_t  enable; 
  M2M2_PM_SYS_USB_PWR_ACTION_ENUM_t  action; 
};

struct m2m2_pm_sys_bat_thermistor_cmd_t {
  uint8_t  command; 
  uint8_t  status; 
  M2M2_PM_SYS_THERMISTOR_STATE_ENUM_t  enable; 
};

struct m2m2_pm_sys_mcu_version_t {
  uint8_t  command; 
  uint8_t  status; 
  M2M2_PM_SYS_MCU_TYPE_ENUM_t  mcu; 
};

struct m2m2_pm_sys_boost_cmd_t {
  uint8_t  command; 
  uint8_t  status; 
  M2M2_PM_SYS_BOOST_ENUM_t  enable; 
  M2M2_PM_SYS_OVP_ENUM_t  ovp; 
};

struct m2m2_pm_sys_powerboost_cmd_t {
  uint8_t  command; 
  uint8_t  status; 
  M2M2_PM_SYS_BOOST_ENUM_t  enable; 
  uint8_t  ovp; 
};

struct m2m2_pm_sys_eeprom_cmd_t {
  uint8_t  command; 
  uint8_t  status; 
  uint8_t  size; 
  uint8_t  byte_stream[16]; 
};

// Reset struct packing outside of this file
#pragma pack()
