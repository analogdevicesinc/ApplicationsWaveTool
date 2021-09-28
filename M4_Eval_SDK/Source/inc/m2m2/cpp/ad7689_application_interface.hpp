// #############################################################################
// THIS IS AN AUTOMATICALLY GENERATED FILE, DO NOT MODIFY IT!
// #############################################################################
#pragma once

#include "common_application_interface.hpp"
#include "common_sensor_interface.hpp"
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

enum M2M2_AD7689_APP_CMD_ENUM_t:uint8_t {
  _M2M2_AD7689_APP_CMD_LOWEST = 64,
  M2M2_AD7689_APP_CMD_WRITE_LCFG_REQ = 66,
  M2M2_AD7689_APP_CMD_WRITE_LCFG_RESP = 67,
  M2M2_AD7689_APP_CMD_READ_LCFG_REQ = 68,
  M2M2_AD7689_APP_CMD_READ_LCFG_RESP = 69,
  M2M2_AD7689_APP_CMD_SET_LCFG_REQ = 70,
  M2M2_AD7689_APP_CMD_SET_LCFG_RESP = 71,
};
static_assert(sizeof(M2M2_AD7689_APP_CMD_ENUM_t) == 1, "Enum 'M2M2_AD7689_APP_CMD_ENUM_t' has an incorrect size!");

enum M2M2_SENSOR_AD7689_NSAMPLES_ENUM_t:uint8_t {
  M2M2_SENSOR_AD7689_NSAMPLES = 6,
};
static_assert(sizeof(M2M2_SENSOR_AD7689_NSAMPLES_ENUM_t) == 1, "Enum 'M2M2_SENSOR_AD7689_NSAMPLES_ENUM_t' has an incorrect size!");

enum M2M2_SENSOR_AD7689_RAW_DATA_TYPES_ENUM_t:uint8_t {
  M2M2_SENSOR_AD7689_DATA = 0,
};
static_assert(sizeof(M2M2_SENSOR_AD7689_RAW_DATA_TYPES_ENUM_t) == 1, "Enum 'M2M2_SENSOR_AD7689_RAW_DATA_TYPES_ENUM_t' has an incorrect size!");

struct AD7689_app_lib_state_t {
  uint8_t  command; 
  uint8_t  status; 
  uint8_t  states[10]; 
};

struct AD7689_app_lcfg_op_t {
  uint8_t  field; 
  uint32_t  value; 
};

struct AD7689_app_lcfg_op_hdr_t {
  uint8_t  command; 
  uint8_t  status; 
  uint8_t  num_ops; 
  AD7689_app_lcfg_op_t  ops[1]; // NOTE: THIS FIELD IS INTENDED TO BE OF VARIABLE LENGTH! 
        // NOTE: Use offsetof(AD7689_app_lcfg_op_hdr_t, ops) instead of sizeof(AD7689_app_lcfg_op_hdr_t)
};

struct AD7689_app_set_lcfg_t {
  uint8_t  command; 
  uint8_t  status; 
  uint16_t  lcfgid; 
};

// Reset struct packing outside of this file
#pragma pack()
