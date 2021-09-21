#!/usr/bin/env python3

from ctypes import *

import m2m2_core

import common_sensor_interface

import common_application_interface

M2M2_PED_APP_CMD_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("_M2M2_PED_APP_CMD_LOWEST",                        0x5A),
    ("M2M2_PED_APP_CMD_GET_ALGO_VENDOR_VERSION_REQ",    0x5B),
    ("M2M2_PED_APP_CMD_GET_ALGO_VENDOR_VERSION_RESP",   0x5C),
    ]
}

ped_app_lcfg_op_t = {
  "struct_fields": [
    {"name":"field",
    "type":c_uint8},
    {"name":"value",
    "type":c_uint16},
  ]
}

ped_app_lcfg_op_hdr_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"num_ops",
    "type":c_uint8},
    {"name":"ops",
    "length":0,
    "type":ped_app_lcfg_op_t},
  ]
}

pedometer_app_stream_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_data_stream_hdr_t},
    {"name":"nNumSteps",
    "type":c_int32},
    {"name":"nAlgoStatus",
    "type":c_uint16},
    {"name":"nTimeStamp",
    "type":c_uint32},
    {"name":"nReserved",
    "type":c_int8},
  ]
}
