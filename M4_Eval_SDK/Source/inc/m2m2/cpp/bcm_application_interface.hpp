// #############################################################################
// THIS IS AN AUTOMATICALLY GENERATED FILE, DO NOT MODIFY IT!
// #############################################################################
#pragma once

#include "common_application_interface.hpp"
#include "common_sensor_interface.hpp"
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

enum M2M2_BCM_APP_CMD_ENUM_t:uint8_t {
  _M2M2_BCM_APP_CMD_LOWEST = 64,
  M2M2_BCM_APP_CMD_SWEEP_FREQ_ENABLE_REQ = 66,
  M2M2_BCM_APP_CMD_SWEEP_FREQ_ENABLE_RESP = 67,
  M2M2_BCM_APP_CMD_SWEEP_FREQ_DISABLE_REQ = 68,
  M2M2_BCM_APP_CMD_SWEEP_FREQ_DISABLE_RESP = 69,
  M2M2_BCM_APP_CMD_SET_DFT_NUM_REQ = 70,
  M2M2_BCM_APP_CMD_SET_DFT_NUM_RESP = 71,
};
static_assert(sizeof(M2M2_BCM_APP_CMD_ENUM_t) == 1, "Enum 'M2M2_BCM_APP_CMD_ENUM_t' has an incorrect size!");

enum M2M2_SENSOR_BCM_NSAMPLES_ENUM_t:uint8_t {
  M2M2_SENSOR_BCM_NSAMPLES = 4,
};
static_assert(sizeof(M2M2_SENSOR_BCM_NSAMPLES_ENUM_t) == 1, "Enum 'M2M2_SENSOR_BCM_NSAMPLES_ENUM_t' has an incorrect size!");

enum M2M2_SENSOR_BCM_RAW_DATA_TYPES_ENUM_t:uint8_t {
  M2M2_SENSOR_BCM_DATA = 0,
};
static_assert(sizeof(M2M2_SENSOR_BCM_RAW_DATA_TYPES_ENUM_t) == 1, "Enum 'M2M2_SENSOR_BCM_RAW_DATA_TYPES_ENUM_t' has an incorrect size!");

enum M2M2_SENSOR_BCM_SWEEP_FREQ_INDEX_ENUM_t:uint8_t {
  M2M2_SENSOR_BCM_FREQ_50KHZ = 255,
  M2M2_SENSOR_BCM_FREQ_1000HZ = 0,
  M2M2_SENSOR_BCM_FREQ_3760HZ = 1,
  M2M2_SENSOR_BCM_FREQ_14140HZ = 2,
  M2M2_SENSOR_BCM_FREQ_53180HZ = 3,
  M2M2_SENSOR_BCM_FREQ_200KHZ = 4,
};
static_assert(sizeof(M2M2_SENSOR_BCM_SWEEP_FREQ_INDEX_ENUM_t) == 1, "Enum 'M2M2_SENSOR_BCM_SWEEP_FREQ_INDEX_ENUM_t' has an incorrect size!");

struct bcm_app_lib_state_t {
  uint8_t  command; 
  uint8_t  status; 
  uint8_t  states[10]; 
};

struct bcm_app_lcfg_op_t {
  uint8_t  field; 
  uint32_t  value; 
};

struct bcm_app_lcfg_op_hdr_t {
  uint8_t  command; 
  uint8_t  status; 
  uint8_t  num_ops; 
  bcm_app_lcfg_op_t  ops[1]; // NOTE: THIS FIELD IS INTENDED TO BE OF VARIABLE LENGTH! 
        // NOTE: Use offsetof(bcm_app_lcfg_op_hdr_t, ops) instead of sizeof(bcm_app_lcfg_op_hdr_t)
};

struct bcm_data_set_t {
  uint32_t  timestamp; 
  int32_t  real; 
  int32_t  img; 
  M2M2_SENSOR_BCM_SWEEP_FREQ_INDEX_ENUM_t  freq_index; 
};

struct bcm_app_stream_t {
  uint8_t  command; 
  uint8_t  status; 
  uint16_t  sequence_num; 
  M2M2_SENSOR_BCM_RAW_DATA_TYPES_ENUM_t  datatype; 
  bcm_data_set_t  bcm_data[4]; 
};

struct m2m2_bcm_app_sweep_freq_resp_t {
  uint8_t  command; 
  uint8_t  status; 
};

struct m2m2_sensor_ad5940_set_dft_num_resp_t {
  uint8_t  command; 
  uint8_t  status; 
  uint32_t  dft_num; 
};

// Reset struct packing outside of this file
#pragma pack()
