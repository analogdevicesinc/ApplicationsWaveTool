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

enum M2M2_SENSOR_AD5940_COMMAND_ENUM_t:uint8_t {
  _M2M2_SENSOR_AD5940_COMMAND_LOWEST = 64,
  M2M2_SENSOR_AD5940_COMMAND_LOAD_CFG_REQ = 66,
  M2M2_SENSOR_AD5940_COMMAND_LOAD_CFG_RESP = 67,
  M2M2_SENSOR_AD5940_COMMAND_SET_ODR_REQ = 68,
  M2M2_SENSOR_AD5940_COMMAND_SET_ODR_RESP = 69,
  M2M2_SENSOR_AD5940_COMMAND_SET_WG_FREQ_REQ = 70,
  M2M2_SENSOR_AD5940_COMMAND_SET_WG_FREQ_RESP = 71,
};
static_assert(sizeof(M2M2_SENSOR_AD5940_COMMAND_ENUM_t) == 1, "Enum 'M2M2_SENSOR_AD5940_COMMAND_ENUM_t' has an incorrect size!");

enum M2M2_SENSOR_AD5940_NSAMPLES_ENUM_t:uint8_t {
  M2M2_SENSOR_AD5940_NSAMPLES_BCM = 4,
  M2M2_SENSOR_AD5940_NSAMPLES_EDA = 4,
  M2M2_SENSOR_AD5940_NSAMPLES_ECG = 2,
};
static_assert(sizeof(M2M2_SENSOR_AD5940_NSAMPLES_ENUM_t) == 1, "Enum 'M2M2_SENSOR_AD5940_NSAMPLES_ENUM_t' has an incorrect size!");

enum M2M2_SENSOR_AD5940_RAW_DATA_TYPES_ENUM_t:uint8_t {
  M2M2_SENSOR_RAW_DATA_AD5940_BCM = 0,
  M2M2_SENSOR_RAW_DATA_AD5940_EDA = 1,
  M2M2_SENSOR_RAW_DATA_AD5940_ECG = 2,
};
static_assert(sizeof(M2M2_SENSOR_AD5940_RAW_DATA_TYPES_ENUM_t) == 1, "Enum 'M2M2_SENSOR_AD5940_RAW_DATA_TYPES_ENUM_t' has an incorrect size!");

enum M2M2_SENSOR_AD5940_DATARATE_ENUM_t:uint8_t {
  M2M2_SENSOR_AD5940_DATARATE_25 = 0,
  M2M2_SENSOR_AD5940_DATARATE_50 = 1,
  M2M2_SENSOR_AD5940_DATARATE_100 = 2,
  M2M2_SENSOR_AD5940_DATARATE_200 = 3,
  M2M2_SENSOR_AD5940_DATARATE_500 = 4,
};
static_assert(sizeof(M2M2_SENSOR_AD5940_DATARATE_ENUM_t) == 1, "Enum 'M2M2_SENSOR_AD5940_DATARATE_ENUM_t' has an incorrect size!");

struct ad5940_data_header_t {
  uint8_t  command; 
  uint8_t  status; 
  uint16_t  sequence_num; 
  M2M2_SENSOR_AD5940_RAW_DATA_TYPES_ENUM_t  datatype; 
  uint32_t  timestamp; 
};

struct m2m2_sensor_ad5940_set_odr_resp_t {
  uint8_t  command; 
  uint8_t  status; 
  M2M2_SENSOR_AD5940_DATARATE_ENUM_t  datarate; 
};

struct m2m2_sensor_ad5940_set_wg_freq_resp_t {
  uint8_t  command; 
  uint8_t  status; 
  uint32_t  wg_freq;
};

// Reset struct packing outside of this file
#pragma pack()
