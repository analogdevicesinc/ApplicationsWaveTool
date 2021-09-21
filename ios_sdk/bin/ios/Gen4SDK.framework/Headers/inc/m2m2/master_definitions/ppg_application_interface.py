#!/usr/bin/env python3

from ctypes import *

import m2m2_core

import common_sensor_interface

import common_application_interface

M2M2_PPG_APP_CMD_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("_M2M2_PPG_APP_CMD_LOWEST",                        0x40),
    ("M2M2_PPG_APP_CMD_GET_LAST_STATES_REQ",            0x42),
    ("M2M2_PPG_APP_CMD_GET_LAST_STATES_RESP",           0x43),
    ("M2M2_PPG_APP_CMD_GET_STATES_INFO_REQ",            0x44),
    ("M2M2_PPG_APP_CMD_GET_STATES_INFO_RESP",           0x45),
    ("M2M2_PPG_APP_CMD_GET_ALGO_VENDOR_VERSION_REQ",    0x46),
    ("M2M2_PPG_APP_CMD_GET_ALGO_VENDOR_VERSION_RESP",   0x47),
    ("M2M2_PPG_APP_CMD_SYNC_DATA_REQ",                  0x48),
    ("M2M2_PPG_APP_CMD_SYNC_DATA_RESP",                 0x49),
    ("M2M2_PPG_APP_CMD_DEBUG_DATA_REQ",                 0x4A),
    ("M2M2_PPG_APP_CMD_DEBUG_DATA_RESP",                0x4B),
    ("M2M2_PPG_APP_CMD_GET_CTRVALUE_REQ",               0x4C),
    ("M2M2_PPG_APP_CMD_GET_CTRVALUE_RESP",              0x4D),
    ("M2M2_PPG_APP_CMD_GET_SMETRICS_REQ",               0x4E),
    ("M2M2_PPG_APP_CMD_GET_SMETRICS_RESP",              0x4F),
    ]
}

M2M2_PPG_APP_SYNC_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("M2M2_PPG_APP_SOFTWARE_SYNC",  0x00),
    ("M2M2_PPG_APP_HARDWARE_SYNC",  0x03),
    ]
}

M2M2_SENSOR_PPG_SYNC_NSAMPLES_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("M2M2_SENSOR_PPG_SYNC_NSAMPLES",   0x03),
    ]
}

M2M2_SENSOR_HRV_NSAMPLES_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("M2M2_SENSOR_HRV_NSAMPLES",    0x4),
    ]
}

M2M2_SENSOR_PPG_SYNC_DATA_TYPES_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("M2M2_SENSOR_PPG_SYNC_DATA_TYPES_NO_SYNC",     0x00),
    ("M2M2_SENSOR_PPG_SYNC_DATA_TYPES_HW_SYNC1",    0x01),
    ("M2M2_SENSOR_PPG_SYNC_DATA_TYPES_HW_SYNC2",    0x02),
    ("M2M2_SENSOR_PPG_SYNC_DATA_TYPES_HW_SYNC3",    0x03),
    ("M2M2_SENSOR_PPG_SYNC_DATA_TYPES_SW_SYNC",     0x04),
    ]
}

M2M2_SENSOR_PPG_LCFG_ID_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("M2M2_SENSOR_PPG_LCFG_ID_ADPD107",   107),
    ("M2M2_SENSOR_PPG_LCFG_ID_ADPD185",   185),
    ("M2M2_SENSOR_PPG_LCFG_ID_ADPD108",   108),
    ("M2M2_SENSOR_PPG_LCFG_ID_ADPD188",   188),
    ("M2M2_SENSOR_PPG_LCFG_ID_ADPD4000",   40),
    ]
}

ppg_app_lib_state_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"states",
    "type":c_uint8,
    "length":10}
  ]
}

ppg_app_ctrValue_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"ctrValue",
    "type":c_uint16}
  ]
}

ppg_app_signal_metrics_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"metrics",
    "type":c_uint16,
    "length":3}
  ]
}

ppg_app_lcfg_op_t = {
  "struct_fields": [
    {"name":"field",
    "type":c_uint8},
    {"name":"value",
    "type":c_uint32},
  ]
}

ppg_app_lcfg_op_hdr_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"num_ops",
    "type":c_uint8},
    {"name":"ops",
    "length":0,
    "type":ppg_app_lcfg_op_t},
  ]
}

ppg_app_hr_debug_stream_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_data_stream_hdr_t},
    {"name":"timestamp",
    "type":c_uint32},
    {"name":"adpdlibstate",
    "type":c_uint16},
    {"name":"hr",
    "type":c_uint16},
    {"name":"confidence",
    "type":c_uint16},
    {"name":"hr_type",
    "type":c_uint16},
    {"name":"rr_interval",
    "type":c_uint16},
    {"name":"debugInfo",
    "length":10,
    "type":c_uint16},
  ]
}

ppg_app_state_info_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"state",
    "type":c_uint8},
    {"name":"info",
    "length":20,
    "type":c_uint8},
  ]
}

ppg_app_cmd_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"commandVal",
    "type":c_uint16},
  ]
}

ppg_app_set_lcfg_req_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"lcfgid",
    "type":M2M2_SENSOR_PPG_LCFG_ID_ENUM_t},
  ]
}

ppg_app_set_lcfg_resp_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
  ]
}

ppg_app_agc_info_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_data_stream_hdr_t},
    {"name":"timestamp",
    "type":c_uint32},
    {"name":"mts",
    "length":6,
    "type":c_uint16},
    {"name":"setting",
    "length":10,
    "type":c_uint16},
  ]
}

hrv_data_set_t = {
  "struct_fields": [
    {"name":"timestamp",
    "type":c_uint16},
    {"name":"rr_interval",
    "type":c_int16},
    {"name":"is_gap",
    "type":c_uint16},
  ]
}

ppg_app_hrv_info_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_data_stream_hdr_t},
    {"name":"timestamp",
    "type":c_uint32},
    {"name":"first_rr_interval",
    "type":c_int16},
    {"name":"first_is_gap",
    "type":c_uint16},
    {"name":"hrv_data",
    "length":3,
    "type":hrv_data_set_t},
  ]
}
