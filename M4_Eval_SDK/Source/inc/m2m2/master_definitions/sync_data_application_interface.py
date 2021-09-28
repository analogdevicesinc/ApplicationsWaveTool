#!/usr/bin/env python3

from ctypes import *

import m2m2_core

import common_application_interface

M2M2_SENSOR_SYNC_DATA_TYPES_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("M2M2_SENSOR_ADPD_ADXL_SYNC_DATA",     0x00),
    ]
}

adpd_adxl_sync_data_format_t = {
  "struct_fields": [
    {"name":"ppgTS",
    "type":c_uint32},
    {"name":"adxlTS",
    "type":c_uint32},
    {"name":"incPpgTS",
    "length":3,
    "type":c_uint16},
    {"name":"incAdxlTS",
    "length":3,
    "type":c_uint16},
    {"name":"ppgData",
    "length":4,
    "type":c_uint32},
    {"name":"xData",
    "length":4,
    "type":c_int16},
    {"name":"yData",
    "length":4,
    "type":c_int16},
    {"name":"zData",
    "length":4,
    "type":c_int16},
  ]
}

adpd_adxl_sync_data_stream_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_data_stream_hdr_t},
    {"name":"syncData",
    "type":adpd_adxl_sync_data_format_t},
  ]
}
