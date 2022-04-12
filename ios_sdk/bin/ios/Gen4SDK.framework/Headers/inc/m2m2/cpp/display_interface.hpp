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


enum M2M2_DISPLAY_APP_CMD_ENUM_t:uint8_t {
  _M2M2_DISPLAY_APP_CMD_LOWEST = 64,
  M2M2_DISPLAY_APP_CMD_SET_DISPLAY_REQ = 66,
  M2M2_DISPLAY_APP_CMD_SET_DISPLAY_RESP = 67,
  M2M2_DISPLAY_APP_CMD_BACKLIGHT_CNTRL_REQ = 68,
  M2M2_DISPLAY_APP_CMD_BACKLIGHT_CNTRL_RESP = 69,
  M2M2_DISPLAY_APP_CMD_KEY_TEST_REQ = 70,
  M2M2_DISPLAY_APP_CMD_KEY_TEST_RESP = 71,
  M2M2_DISPLAY_APP_CMD_KEY_STREAM_DATA = 72,
};
static_assert(sizeof(M2M2_DISPLAY_APP_CMD_ENUM_t) == 1, "Enum 'M2M2_DISPLAY_APP_CMD_ENUM_t' has an incorrect size!");

enum M2M2_DISPLAY_SET_COMMAND_ENUM_t:uint8_t {
  M2M2_DISPLAY_SET_WHITE = 0,
  M2M2_DISPLAY_SET_BLACK = 1,
  M2M2_DISPLAY_SET_RED = 2,
  M2M2_DISPLAY_SET_GREEN = 3,
  M2M2_DISPLAY_SET_BLUE = 4,
};
static_assert(sizeof(M2M2_DISPLAY_SET_COMMAND_ENUM_t) == 1, "Enum 'M2M2_DISPLAY_SET_COMMAND_ENUM_t' has an incorrect size!");

enum M2M2_BACKLIGHT_CNTRL_COMMAND_ENUM_t:uint8_t {
  M2M2_BACKLIGHT_CNTRL_OFF = 0,
  M2M2_BACKLIGHT_CNTRL_ON = 1,
};
static_assert(sizeof(M2M2_BACKLIGHT_CNTRL_COMMAND_ENUM_t) == 1, "Enum 'M2M2_BACKLIGHT_CNTRL_COMMAND_ENUM_t' has an incorrect size!");

enum M2M2_KEY_TEST_COMMAND_ENUM_t:uint8_t {
  M2M2_KEY_TEST_SELECT_BUTTON = 0,
  M2M2_KEY_TEST_NAVIGATE_BUTTON = 1,
};
static_assert(sizeof(M2M2_KEY_TEST_COMMAND_ENUM_t) == 1, "Enum 'M2M2_KEY_TEST_COMMAND_ENUM_t' has an incorrect size!");

struct m2m2_display_set_command_t {
  uint8_t  command; 
  uint8_t  status; 
  uint8_t  colour; 
};

struct m2m2_backlight_cntrl_command_t {
  uint8_t  command; 
  uint8_t  status; 
  uint8_t  control; 
};

struct m2m2_key_test_command_t {
  uint8_t  command; 
  uint8_t  status; 
  uint8_t  enable; 
};

struct m2m2_pm_sys_key_test_data_t {
  uint8_t  command; 
  uint8_t  status; 
  uint8_t  key_value; 
};

// Reset struct packing outside of this file
#pragma pack()
