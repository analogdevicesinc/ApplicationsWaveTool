#!/usr/bin/env python3

from ctypes import *

import m2m2_core

import common_application_interface

import common_sensor_interface

M2M2_AD7689_APP_CMD_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("_M2M2_AD7689_APP_CMD_LOWEST",             0x40),
    ("M2M2_AD7689_APP_CMD_WRITE_LCFG_REQ",      0x42),
    ("M2M2_AD7689_APP_CMD_WRITE_LCFG_RESP",     0x43),
    ("M2M2_AD7689_APP_CMD_READ_LCFG_REQ",       0x44),
    ("M2M2_AD7689_APP_CMD_READ_LCFG_RESP",      0x45),
    ("M2M2_AD7689_APP_CMD_SET_LCFG_REQ",        0x46),
    ("M2M2_AD7689_APP_CMD_SET_LCFG_RESP",       0x47),
    ]
}

M2M2_SENSOR_AD7689_NSAMPLES_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("M2M2_SENSOR_AD7689_NSAMPLES", 0x6),
    ]
}

M2M2_SENSOR_AD7689_RAW_DATA_TYPES_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("M2M2_SENSOR_AD7689_DATA", 0x0),
    ]
}

AD7689_app_lib_state_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"states",
    "length":10,
    "type":c_uint8},
  ]
}

AD7689_app_lcfg_op_t = {
  "struct_fields": [
    {"name":"field",
    "type":c_uint8},
    {"name":"value",
    "type":c_uint32},
  ]
}

AD7689_app_lcfg_op_hdr_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"num_ops",
    "type":c_uint8},
    {"name":"ops",
    "length":0,
    "type":AD7689_app_lcfg_op_t},
  ]
}

AD7689_app_set_lcfg_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"lcfgid",
    "type":c_uint16},
  ]
}
