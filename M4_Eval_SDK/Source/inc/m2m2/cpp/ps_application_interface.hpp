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

enum M2M2_PS_SYS_PWR_STATE_ENUM_t:uint8_t {
  M2M2_PS_SYS_PWR_STATE_ACTIVE = 0,
  M2M2_PS_SYS_PWR_STATE_FLEXI = 1,
  M2M2_PS_SYS_PWR_STATE_HIBERNATE = 2,
  M2M2_PS_SYS_PWR_STATE_SHUTDOWN = 3,
};
static_assert(sizeof(M2M2_PS_SYS_PWR_STATE_ENUM_t) == 1, "Enum 'M2M2_PS_SYS_PWR_STATE_ENUM_t' has an incorrect size!");

enum M2M2_PS_SYS_COMMAND_ENUM_t:uint8_t {
  __M2M2_PS_SYS_COMMAND_LOWEST = 64,
  M2M2_PS_SYS_COMMAND_SET_DATE_TIME_REQ = 66,
  M2M2_PS_SYS_COMMAND_SET_DATE_TIME_RESP = 67,
  M2M2_PS_SYS_COMMAND_SET_PWR_STATE_REQ = 68,
  M2M2_PS_SYS_COMMAND_SET_PWR_STATE_RESP = 69,
  M2M2_PS_SYS_COMMAND_GET_PS_APPS_INFO_REQ = 72,
  M2M2_PS_SYS_COMMAND_GET_PS_APPS_INFO_RESP = 73,
  M2M2_PS_SYS_COMMAND_ACTIVATE_TOUCH_SENSOR_REQ = 74,
  M2M2_PS_SYS_COMMAND_ACTIVATE_TOUCH_SENSOR_RESP = 75,
  M2M2_PS_SYS_COMMAND_DEACTIVATE_TOUCH_SENSOR_REQ = 76,
  M2M2_PS_SYS_COMMAND_DEACTIVATE_TOUCH_SENSOR_RESP = 77,
  M2M2_PS_SYS_COMMAND_GET_BOARD_INFO_REQ = 78,
  M2M2_PS_SYS_COMMAND_GET_BOARD_INFO_RESP = 79,
  M2M2_PS_SYS_COMMAND_SYSTEM_RESET_REQ = 80,
  M2M2_PS_SYS_COMMAND_SYSTEM_RESET_RESP = 81,
};
static_assert(sizeof(M2M2_PS_SYS_COMMAND_ENUM_t) == 1, "Enum 'M2M2_PS_SYS_COMMAND_ENUM_t' has an incorrect size!");

enum ADI_PS_BOARD_TYPE_t:uint8_t {
  ADI_PS_BOARD_TYPE_UNKNOWN = 0,
  ADI_PS_BOARD_TYPE_ADPD107_WATCH = 1,
  ADI_PS_BOARD_TYPE_ADPD107_CHEST_STRAP = 2,
  ADI_PS_BOARD_TYPE_ADPD185_WATCH = 3,
  ADI_PS_BOARD_TYPE_ADPD188_WATCH = 4,
};
static_assert(sizeof(ADI_PS_BOARD_TYPE_t) == 1, "Enum 'ADI_PS_BOARD_TYPE_t' has an incorrect size!");

enum M2M2_PS_SYS_STATUS_ENUM_t:uint8_t {
  __M2M2_PS_SYS_STATUS_LOWEST = 64,
  M2M2_PS_SYS_STATUS_OK = 65,
  M2M2_PS_SYS_STATUS_ERR_ARGS = 66,
  M2M2_PS_SYS_STATUS_ERR_NOT_CHKD = 255,
};
static_assert(sizeof(M2M2_PS_SYS_STATUS_ENUM_t) == 1, "Enum 'M2M2_PS_SYS_STATUS_ENUM_t' has an incorrect size!");

struct m2m2_ps_sys_cmd_t {
  uint8_t  command; 
  uint8_t  status; 
};

struct m2m2_ps_sys_pwr_state_t {
  uint8_t  command; 
  uint8_t  status; 
  M2M2_PS_SYS_PWR_STATE_ENUM_t  state; 
};

struct m2m2_ps_sys_date_time_req_t {
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

struct m2m2_ps_sys_board_info_req_t {
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
  ADI_PS_BOARD_TYPE_t  board_type; 
};

struct m2m2_ps_sys_sensor_app_status {
  M2M2_ADDR_ENUM_t  sensor_app_id; 
  uint8_t  num_subscribers; 
  uint8_t  num_start_reqs; 
};

struct m2m2_ps_sys_sensor_apps_info_req_t {
  uint8_t  command; 
  uint8_t  status; 
  uint16_t  num_sensor_apps; 
  m2m2_ps_sys_sensor_app_status  app_info[15]; 
};

// Reset struct packing outside of this file
#pragma pack()
