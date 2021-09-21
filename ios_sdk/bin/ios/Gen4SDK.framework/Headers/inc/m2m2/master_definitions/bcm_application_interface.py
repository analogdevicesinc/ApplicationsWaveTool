#!/usr/bin/env python3

from ctypes import *

import common_application_interface

import common_sensor_interface

M2M2_BCM_APP_CMD_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("_M2M2_BCM_APP_CMD_LOWEST",                    0x40),
    ("M2M2_BCM_APP_CMD_SWEEP_FREQ_ENABLE_REQ",      0x42),
    ("M2M2_BCM_APP_CMD_SWEEP_FREQ_ENABLE_RESP",     0x43),
    ("M2M2_BCM_APP_CMD_SWEEP_FREQ_DISABLE_REQ",     0x44),
    ("M2M2_BCM_APP_CMD_SWEEP_FREQ_DISABLE_RESP",    0x45),
    ("M2M2_BCM_APP_CMD_SET_DFT_NUM_REQ",            0x46),
    ("M2M2_BCM_APP_CMD_SET_DFT_NUM_RESP",           0x47),
    ]
}

M2M2_SENSOR_BCM_NSAMPLES_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("M2M2_SENSOR_BCM_NSAMPLES",        0x4),
    ]
}

M2M2_SENSOR_BCM_RAW_DATA_TYPES_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("M2M2_SENSOR_BCM_DATA",   0x00),
    ]
}

M2M2_SENSOR_BCM_SWEEP_FREQ_INDEX_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("M2M2_SENSOR_BCM_FREQ_50KHZ",     0xFF),
    ("M2M2_SENSOR_BCM_FREQ_1000HZ",    0x00),
    ("M2M2_SENSOR_BCM_FREQ_3760HZ",    0x01),
    ("M2M2_SENSOR_BCM_FREQ_14140HZ",   0x02),
    ("M2M2_SENSOR_BCM_FREQ_53180HZ",   0x03),
    ("M2M2_SENSOR_BCM_FREQ_200KHZ",    0x04),
    ]
}

bcm_app_lib_state_t = {
  "struct_fields": [
    {
    "name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"states",
    "length":10,
    "type":c_uint8},
  ]
}

bcm_app_lcfg_op_t = {
  "struct_fields": [
    {"name":"field",
    "type":c_uint8},
    {"name":"value",
    "type":c_uint32},
  ]
}

bcm_app_lcfg_op_hdr_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"num_ops",
    "type":c_uint8},
    {"name":"ops",
    "length":0,
    "type":bcm_app_lcfg_op_t},
  ]
}

bcm_data_set_t = {
  "struct_fields": [
    {"name": "timestamp",
    "type":c_uint32},
    {"name":"real",
    "type":c_int32},
    {"name":"img",
    "type":c_int32},
    {"name":"freq_index",
    "type":M2M2_SENSOR_BCM_SWEEP_FREQ_INDEX_ENUM_t},
  ]
}

bcm_app_stream_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_data_stream_hdr_t},
    {"name":"datatype",
    "type":M2M2_SENSOR_BCM_RAW_DATA_TYPES_ENUM_t},
    {"name":"bcm_data",
    "length":4,
    "type":bcm_data_set_t},
  ]
}

m2m2_bcm_app_sweep_freq_resp_t = {
  "struct_fields": [
    {"name":"command",
    "type":c_uint8},
    {"name":"status",
    "type":c_uint8},
  ]
}

m2m2_sensor_ad5940_set_dft_num_resp_t = {
  "struct_fields": [
    {"name":"command",
    "type":c_uint8},
    {"name":"status",
    "type":c_uint8},
    {"name":"dft_num",
    "type":c_uint32},
  ]
}
