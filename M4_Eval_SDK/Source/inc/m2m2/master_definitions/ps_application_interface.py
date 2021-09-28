#!/usr/bin/env python3

from ctypes import *

import m2m2_core

import common_application_interface

M2M2_PS_SYS_PWR_STATE_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("M2M2_PS_SYS_PWR_STATE_ACTIVE",0x0),
    ("M2M2_PS_SYS_PWR_STATE_FLEXI",0x1),
    ("M2M2_PS_SYS_PWR_STATE_HIBERNATE",0x2),
    ("M2M2_PS_SYS_PWR_STATE_SHUTDOWN",0x3),
    ]
}

M2M2_PS_SYS_COMMAND_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("__M2M2_PS_SYS_COMMAND_LOWEST",0x40),
    ("M2M2_PS_SYS_COMMAND_SET_DATE_TIME_REQ",0x42),
    ("M2M2_PS_SYS_COMMAND_SET_DATE_TIME_RESP",0x43),
    ("M2M2_PS_SYS_COMMAND_SET_PWR_STATE_REQ",0x44),
    ("M2M2_PS_SYS_COMMAND_SET_PWR_STATE_RESP",0x45),
    ("M2M2_PS_SYS_COMMAND_GET_PS_APPS_INFO_REQ",0x48),
    ("M2M2_PS_SYS_COMMAND_GET_PS_APPS_INFO_RESP",0x49),    
    ("M2M2_PS_SYS_COMMAND_ACTIVATE_TOUCH_SENSOR_REQ",0x4A),
    ("M2M2_PS_SYS_COMMAND_ACTIVATE_TOUCH_SENSOR_RESP",0x4B),    
    ("M2M2_PS_SYS_COMMAND_DEACTIVATE_TOUCH_SENSOR_REQ",0x4C),
    ("M2M2_PS_SYS_COMMAND_DEACTIVATE_TOUCH_SENSOR_RESP",0x4D),
    ("M2M2_PS_SYS_COMMAND_GET_BOARD_INFO_REQ", 0x4E),
    ("M2M2_PS_SYS_COMMAND_GET_BOARD_INFO_RESP", 0x4F),
    ("M2M2_PS_SYS_COMMAND_SYSTEM_RESET_REQ",0x50),
    ("M2M2_PS_SYS_COMMAND_SYSTEM_RESET_RESP",0x51),
    ]
}

ADI_PS_BOARD_TYPE_t = {
  "type":c_uint8,
  "enum_values": [
    ("ADI_PS_BOARD_TYPE_UNKNOWN",0x0),
    ("ADI_PS_BOARD_TYPE_ADPD107_WATCH",0x1),
    ("ADI_PS_BOARD_TYPE_ADPD107_CHEST_STRAP",0x2),
    ("ADI_PS_BOARD_TYPE_ADPD185_WATCH",0x3),
    ("ADI_PS_BOARD_TYPE_ADPD188_WATCH",0x4),
    ]
}

M2M2_PS_SYS_STATUS_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("__M2M2_PS_SYS_STATUS_LOWEST",0x40),
    ("M2M2_PS_SYS_STATUS_OK",0x41),
    ("M2M2_PS_SYS_STATUS_ERR_ARGS",0x42),
    ("M2M2_PS_SYS_STATUS_ERR_NOT_CHKD",0xFF),
    ]
}

m2m2_ps_sys_cmd_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
  ]
}

m2m2_ps_sys_pwr_state_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"state",
    "type":M2M2_PS_SYS_PWR_STATE_ENUM_t},
  ]
}

m2m2_ps_sys_date_time_req_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"year",
    "type":c_uint16},
    {"name":"month",
    "type":c_uint8},
    {"name":"day",
    "type":c_uint8},
    {"name":"hour",
    "type":c_uint8},
    {"name":"minute",
    "type":c_uint8},
    {"name":"second",
    "type":c_uint8},
    {"name":"TZ_sec",
    "type":c_uint32},
  ]
}

m2m2_ps_sys_board_info_req_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"version",
    "type":c_uint16},
    {"name":"mac_addr",
    "length":6,
    "type":c_uint8},
    {"name":"device_id",
    "type":c_uint32},
    {"name":"model_number",
    "type":c_uint32},
    {"name":"hw_id",
    "type":c_uint16},
    {"name":"bom_id",
    "type":c_uint16},
    {"name":"batch_id",
    "type":c_uint8},
    {"name":"date",
    "type":c_uint32},
    {"name": "board_type",
    "type": ADI_PS_BOARD_TYPE_t},
  ]
}

m2m2_ps_sys_sensor_app_status = {
	"struct_fields":[
	{"name":"sensor_app_id",
    "type":m2m2_core.M2M2_ADDR_ENUM_t},
    {"name":"num_subscribers",
    "type":c_uint8},
    {"name":"num_start_reqs",
    "type":c_uint8},
	]
}

m2m2_ps_sys_sensor_apps_info_req_t = {
  "struct_fields":[
    {"name":"command",
    "type":c_uint8},
    {"name":"status",
    "type":c_uint8},
    {"name":"num_sensor_apps",
    "type":c_uint16},
    {"name":"app_info",
    "length":15,
    "type":m2m2_ps_sys_sensor_app_status},
  ]
}



