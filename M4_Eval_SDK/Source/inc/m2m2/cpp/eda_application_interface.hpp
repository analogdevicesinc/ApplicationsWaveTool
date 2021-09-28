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

enum M2M2_EDA_APP_CMD_ENUM_t:uint8_t {
  _M2M2_EDA_APP_CMD_LOWEST = 64,
  M2M2_EDA_APP_CMD_DYNAMIC_SCALE_REQ = 66,
  M2M2_EDA_APP_CMD_DYNAMIC_SCALE_RESP = 67,
  M2M2_EDA_APP_CMD_SET_DATA_RATE_REQ = 68,
  M2M2_EDA_APP_CMD_SET_DATA_RATE_RESP = 69,
  M2M2_EDA_APP_CMD_SET_DFT_NUM_REQ = 70,
  M2M2_EDA_APP_CMD_SET_DFT_NUM_RESP = 71,
};
static_assert(sizeof(M2M2_EDA_APP_CMD_ENUM_t) == 1, "Enum 'M2M2_EDA_APP_CMD_ENUM_t' has an incorrect size!");

enum M2M2_SENSOR_EDA_NSAMPLES_ENUM_t:uint8_t {
  M2M2_SENSOR_EDA_NSAMPLES = 6,
};
static_assert(sizeof(M2M2_SENSOR_EDA_NSAMPLES_ENUM_t) == 1, "Enum 'M2M2_SENSOR_EDA_NSAMPLES_ENUM_t' has an incorrect size!");

enum M2M2_SENSOR_EDA_RAW_DATA_TYPES_ENUM_t:uint8_t {
  M2M2_SENSOR_EDA_DATA = 0,
};
static_assert(sizeof(M2M2_SENSOR_EDA_RAW_DATA_TYPES_ENUM_t) == 1, "Enum 'M2M2_SENSOR_EDA_RAW_DATA_TYPES_ENUM_t' has an incorrect size!");

struct eda_app_lib_state_t {
  uint8_t  command; 
  uint8_t  status; 
  uint8_t  states[10]; 
};

struct eda_app_lcfg_op_t {
  uint8_t  field; 
  uint32_t  value; 
};

struct eda_app_lcfg_op_hdr_t {
  uint8_t  command; 
  uint8_t  status; 
  uint8_t  num_ops; 
  eda_app_lcfg_op_t  ops[1]; // NOTE: THIS FIELD IS INTENDED TO BE OF VARIABLE LENGTH! 
        // NOTE: Use offsetof(eda_app_lcfg_op_hdr_t, ops) instead of sizeof(eda_app_lcfg_op_hdr_t)
};

struct eda_data_set_t {
  uint32_t  timestamp; 
  int16_t  realdata; 
  int16_t  imgdata; 
};

struct eda_app_stream_t {
  uint8_t  command; 
  uint8_t  status; 
  uint16_t  sequence_num; 
  M2M2_SENSOR_EDA_RAW_DATA_TYPES_ENUM_t  datatype; 
  eda_data_set_t  eda_data[6]; 
};

struct eda_app_dynamic_scale_t {
  uint8_t  command; 
  uint8_t  status; 
  uint8_t  dscale; 
};

struct eda_app_set_data_rate_t {
  uint8_t  command; 
  uint8_t  status; 
  uint16_t  datarate; 
};

struct eda_app_set_dft_num_t {
  uint8_t  command; 
  uint8_t  status; 
  uint8_t  dftnum; 
};

// Reset struct packing outside of this file
#pragma pack()