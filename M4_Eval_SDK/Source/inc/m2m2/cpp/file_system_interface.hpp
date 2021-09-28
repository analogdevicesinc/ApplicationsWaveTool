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

enum M2M2_FILE_SYS_CMD_ENUM_t:uint8_t {
  __M2M2_FILE_SYS_CMD_LOWEST = 64,
  M2M2_FILE_SYS_CMD_MOUNT_REQ = 66,
  M2M2_FILE_SYS_CMD_MOUNT_RESP = 67,
  M2M2_FILE_SYS_CMD_FORMAT_REQ = 70,
  M2M2_FILE_SYS_CMD_FORMAT_RESP = 71,
  M2M2_FILE_SYS_CMD_LS_REQ = 72,
  M2M2_FILE_SYS_CMD_LS_RESP = 73,
  M2M2_FILE_SYS_CMD_GET_REQ = 76,
  M2M2_FILE_SYS_CMD_GET_RESP = 77,
  M2M2_FILE_SYS_CMD_VOL_INFO_REQ = 78,
  M2M2_FILE_SYS_CMD_VOL_INFO_RESP = 79,
  M2M2_FILE_SYS_CMD_GET_FS_STATUS_REQ = 80,
  M2M2_FILE_SYS_CMD_GET_FS_STATUS_RESP = 81,
  M2M2_FILE_SYS_CMD_GET_FS_STREAM_SUB_STATUS_REQ = 82,
  M2M2_FILE_SYS_CMD_GET_FS_STREAM_SUB_STATUS_RESP = 83,
  M2M2_FILE_SYS_CMD_STOP_FS_STREAM_REQ = 84,
  M2M2_FILE_SYS_CMD_STOP_FS_STREAM_RESP = 85,
  M2M2_FILE_SYS_CMD_GET_FS_DEBUG_INFO_REQ = 86,
  M2M2_FILE_SYS_CMD_GET_FS_DEBUG_INFO_RESP = 87,
  M2M2_FILE_SYS_CMD_TEST_LOG_REQ = 88,
  M2M2_FILE_SYS_CMD_TEST_LOG_RESP = 89,
  M2M2_FILE_SYS_CMD_FORCE_STOP_LOG_REQ = 90,
  M2M2_FILE_SYS_CMD_FORCE_STOP_LOG_RESP = 91,
  M2M2_FILE_SYS_CMD_SET_KEY_VALUE_PAIR_REQ = 100,
  M2M2_FILE_SYS_CMD_SET_KEY_VALUE_PAIR_RESP = 101,
  M2M2_FILE_SYS_CMD_DCFG_START_LOG_REQ = 102,
  M2M2_FILE_SYS_CMD_DCFG_START_LOG_RESP = 103,
  M2M2_FILE_SYS_CMD_DCFG_STOP_LOG_REQ = 104,
  M2M2_FILE_SYS_CMD_DCFG_STOP_LOG_RESP = 105,
  M2M2_FILE_SYS_CMD_LOG_USER_CONFIG_DATA_REQ = 106,
  M2M2_FILE_SYS_CMD_LOG_USER_CONFIG_DATA_RESP = 107,
  M2M2_FILE_SYS_CMD_FIND_CONFIG_FILE_REQ = 108,
  M2M2_FILE_SYS_CMD_FIND_CONFIG_FILE_RESP = 109,
  M2M2_FILE_SYS_CMD_DELETE_CONFIG_FILE_REQ = 110,
  M2M2_FILE_SYS_CMD_DELETE_CONFIG_FILE_RESP = 111,
  M2M2_FILE_SYS_CMD_GET_NUMBER_OF_FILE_REQ = 112,
  M2M2_FILE_SYS_CMD_GET_NUMBER_OF_FILE_RESP = 113,
  M2M2_FILE_SYS_CMD_CHUNK_RETRANSMIT_REQ = 114,
  M2M2_FILE_SYS_CMD_CHUNK_RETRANSMIT_RESP = 115,
};
static_assert(sizeof(M2M2_FILE_SYS_CMD_ENUM_t) == 1, "Enum 'M2M2_FILE_SYS_CMD_ENUM_t' has an incorrect size!");

enum M2M2_FILE_SYS_STATUS_ENUM_t:uint8_t {
  __M2M2_FILE_SYS_ERR_LOWEST = 64,
  M2M2_FILE_SYS_STATUS_OK = 65,
  M2M2_FILE_SYS_STATUS_ERROR = 66,
  M2M2_FILE_SYS_END_OF_FILE = 67,
  M2M2_FILE_SYS_END_OF_DIR = 68,
  M2M2_FILE_SYS_ERR_INVALID = 69,
  M2M2_FILE_SYS_ERR_ARGS = 70,
  M2M2_FILE_SYS_ERR_FORMAT = 71,
  M2M2_FILE_SYS_ERR_MEMORY_FULL = 72,
  M2M2_FILE_SYS_ERR_LOG_FORCE_STOPPED = 73,
  M2M2_FILE_SYS_ERR_MAX_FILE_COUNT = 74,
  M2M2_FILE_SYS_CONFIG_FILE_FOUND = 75,
  M2M2_FILE_SYS_CONFIG_FILE_NOT_FOUND = 76,
  M2M2_FILE_SYS_ERR_CONFIG_FILE_POSITION = 77,
  M2M2_FILE_SYS_STATUS_LOGGING_IN_PROGRESS = 78,
  M2M2_FILE_SYS_STATUS_LOGGING_ERROR = 79,
  M2M2_FILE_SYS_STATUS_LOGGING_NOT_STARTED = 80,
  M2M2_FILE_SYS_ERR_BATTERY_LOW = 81,
  M2M2_FILE_SYS_ERR_POWER_STATE_SHUTDOWN = 82,
  M2M2_FILE_SYS_CONFIG_FILE_ERR_POSITION = 83,
  M2M2_FILE_SYS_ERR_NOT_CHKD = 255,
};
static_assert(sizeof(M2M2_FILE_SYS_STATUS_ENUM_t) == 1, "Enum 'M2M2_FILE_SYS_STATUS_ENUM_t' has an incorrect size!");

enum FILE_TYPE_ENUM_t:uint8_t {
  M2M2_FILE_SYS_IS_DIR = 0,
  M2M2_FILE_SYS_IS_FILE = 1,
  M2M2_FILE_SYS_INVALID_TYPE = 255,
};
static_assert(sizeof(FILE_TYPE_ENUM_t) == 1, "Enum 'FILE_TYPE_ENUM_t' has an incorrect size!");

enum FILE_SYS_STREAM_SUBS_STATE_ENUM_t:uint8_t {
  M2M2_FILE_SYS_UNSUBSCRIBED = 0,
  M2M2_FILE_SYS_SUBSCRIBED = 1,
  M2M2_FILE_SYS_SUBS_INVALID = 255,
};
static_assert(sizeof(FILE_SYS_STREAM_SUBS_STATE_ENUM_t) == 1, "Enum 'FILE_SYS_STREAM_SUBS_STATE_ENUM_t' has an incorrect size!");

enum FILE_STOP_LOGGING_t:uint8_t {
  M2M2_FILE_SYS_STOP_LOGGING = 0,
  M2M2_FILE_SYS_MEMORY_FULL = 1,
  M2M2_FILE_SYS_BATTERY_LOW = 2,
  M2M2_FILE_SYS_POWER_STATE_SHUTDOWN = 3,
  M2M2_FILE_SYS_STOP_LOGGING_INVALID = 255,
};
static_assert(sizeof(FILE_STOP_LOGGING_t) == 1, "Enum 'FILE_STOP_LOGGING_t' has an incorrect size!");

struct m2m2_file_sys_cmd_t {
  uint8_t  command; 
  uint8_t  status; 
};

struct m2m2_file_sys_ls_req_t {
  uint8_t  command; 
  uint8_t  status; 
  uint8_t  dir_path[1]; // NOTE: THIS FIELD IS INTENDED TO BE OF VARIABLE LENGTH! 
        // NOTE: Use offsetof(m2m2_file_sys_ls_req_t, dir_path) instead of sizeof(m2m2_file_sys_ls_req_t)
};

struct m2m2_file_sys_ls_resp_t {
  uint8_t  command; 
  uint8_t  status; 
  uint8_t  full_file_name[40]; 
  FILE_TYPE_ENUM_t  filetype; 
  uint32_t  filesize; 
};

struct m2m2_file_sys_get_req_t {
  uint8_t  command; 
  uint8_t  status; 
  uint8_t  file_name[1]; // NOTE: THIS FIELD IS INTENDED TO BE OF VARIABLE LENGTH! 
        // NOTE: Use offsetof(m2m2_file_sys_get_req_t, file_name) instead of sizeof(m2m2_file_sys_get_req_t)
};

struct m2m2_file_sys_get_resp_t {
  uint8_t  command; 
  uint8_t  status; 
  uint16_t  len_stream; 
  uint8_t  byte_stream[512]; 
  uint16_t  crc16; 
};

struct m2m2_file_sys_ref_hr {
  uint8_t  command; 
  uint8_t  status; 
  uint16_t  refhr; 
  uint16_t  year; 
  uint8_t  month; 
  uint8_t  day; 
  uint8_t  hour; 
  uint8_t  minute; 
  uint8_t  second; 
  uint32_t  TZ_sec; 
};

struct m2m2_file_sys_set_key_value_pair_req_t {
  uint8_t  command; 
  uint8_t  status; 
  uint8_t  valueID[16]; 
};

struct m2m2_file_sys_set_key_value_pair_resp_t {
  uint8_t  command; 
  uint8_t  status; 
  uint16_t  keyID; 
  uint8_t  valueID[16]; 
  uint16_t  year; 
  uint8_t  month; 
  uint8_t  day; 
  uint8_t  hour; 
  uint8_t  minute; 
  uint8_t  second; 
  uint32_t  TZ_sec; 
};

struct m2m2_file_sys_vol_info_resp_t {
  uint8_t  command; 
  uint8_t  status; 
  uint32_t  totalmemory; 
  uint32_t  usedmemory; 
  uint16_t  freememory; 
};

struct m2m2_file_sys_get_subs_status_resp_t {
  uint8_t  command; 
  uint8_t  status; 
  M2M2_ADDR_ENUM_t  stream; 
  FILE_SYS_STREAM_SUBS_STATE_ENUM_t  subs_state; 
};

struct m2m2_file_sys_debug_info_req_t {
  uint8_t  command; 
  uint8_t  status; 
  M2M2_ADDR_ENUM_t  stream; 
};

struct m2m2_file_sys_debug_info_resp_t {
  uint8_t  command; 
  uint8_t  status; 
  M2M2_ADDR_ENUM_t  stream; 
  uint32_t  packets_received; 
  uint32_t  packets_missed; 
};

struct m2m2_file_sys_user_config_data {
  uint8_t  command; 
  uint8_t  status; 
  uint16_t  len_configstream; 
  uint8_t  byte_configstream[70]; 
};

struct m2m2_file_sys_user_cfg_summary_pkt_t {
  uint8_t  command; 
  uint8_t  status; 
  uint16_t  start_cmd_len; 
  uint16_t  start_cmd_cnt; 
  uint16_t  stop_cmd_len; 
  uint16_t  stop_cmd_cnt; 
  uint16_t  crc16; 
};

struct m2m2_file_sys_get_file_count_pkt_t {
  uint8_t  command; 
  uint8_t  status; 
  uint16_t  file_count; 
};

struct m2m2_file_sys_pkt_retransmit_req_t {
  uint8_t  command; 
  uint8_t  status; 
  uint8_t  Roll_over; 
  uint16_t  chunk_number; 
  uint8_t  file_name[1]; // NOTE: THIS FIELD IS INTENDED TO BE OF VARIABLE LENGTH! 
        // NOTE: Use offsetof(m2m2_file_sys_pkt_retransmit_req_t, file_name) instead of sizeof(m2m2_file_sys_pkt_retransmit_req_t)
};

// Reset struct packing outside of this file
#pragma pack()
