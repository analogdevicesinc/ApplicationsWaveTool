#!/usr/bin/env python3

from ctypes import *

import m2m2_core

import common_application_interface

M2M2_PM_SYS_COMMAND_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("__M2M2_PM_SYS_COMMAND_LOWEST",                        0x40),
    ("M2M2_PM_SYS_COMMAND_SET_DATE_TIME_REQ",               0x42),
    ("M2M2_PM_SYS_COMMAND_SET_DATE_TIME_RESP",              0x43),
    ("M2M2_PM_SYS_COMMAND_GET_BAT_INFO_REQ",                0x44),
    ("M2M2_PM_SYS_COMMAND_GET_BAT_INFO_RESP",               0x45),
    ("M2M2_PM_SYS_COMMAND_SET_BAT_THR_REQ",                 0x46),
    ("M2M2_PM_SYS_COMMAND_SET_BAT_THR_RESP",                0x47),
    ("M2M2_PM_SYS_COMMAND_SET_PWR_STATE_REQ",               0x48),
    ("M2M2_PM_SYS_COMMAND_SET_PWR_STATE_RESP",              0x49),
    ("M2M2_PM_SYS_COMMAND_GET_INFO_REQ",                    0x4A),
    ("M2M2_PM_SYS_COMMAND_GET_INFO_RESP",                   0x4B),
    ("M2M2_PM_SYS_COMMAND_ENABLE_BAT_CHARGE_REQ",           0x4C),
    ("M2M2_PM_SYS_COMMAND_ENABLE_BAT_CHARGE_RESP",          0x4D),
    ("M2M2_PM_SYS_COMMAND_DISABLE_BAT_CHARGE_REQ",          0x4E),
    ("M2M2_PM_SYS_COMMAND_DISABLE_BAT_CHARGE_RESP",         0x4F),
    ("M2M2_PM_SYS_COMMAND_USB_PWR_REQ",                     0x50),
    ("M2M2_PM_SYS_COMMAND_USB_PWR_RESP",                    0x51),
    ("M2M2_PM_SYS_COMMAND_GET_DATE_TIME_REQ",               0x52),
    ("M2M2_PM_SYS_COMMAND_GET_DATE_TIME_RESP",              0x53),
    ("M2M2_PM_SYS_COMMAND_GET_BOARD_INFO_REQ",              0x54),
    ("M2M2_PM_SYS_COMMAND_GET_BOARD_INFO_RESP",             0x55),
    ("M2M2_PM_SYS_COMMAND_THERMISTOR_STATE_CHANGE_REQ",     0x56),
    ("M2M2_PM_SYS_COMMAND_THERMISTOR_STATE_CHANGE_RESP",    0x57),
    ("M2M2_PM_SYS_COMMAND_GET_MCU_VERSION_REQ",             0x58),
    ("M2M2_PM_SYS_COMMAND_GET_MCU_VERSION_RESP",            0x59),
    ("M2M2_PM_SYS_COMMAND_BOOST_REQ",                       0x5A),
    ("M2M2_PM_SYS_COMMAND_BOOST_RESP",                      0x5B),
    ("M2M2_PM_SYS_COMMAND_ADP5258_WRITE_RDAC_REQ",          0x5C),
    ("M2M2_PM_SYS_COMMAND_ADP5258_WRITE_RDAC_RESP",         0x5D),
    ("M2M2_PM_SYS_COMMAND_BLUETOOTH_REQ",                   0x5E),
    ("M2M2_PM_SYS_COMMAND_BLUETOOTH_RESP",                  0x5F),
    ("M2M2_PM_SYS_COMMAND_ENABLE_USER_CONFIG_LOG_REQ",      0x60),
    ("M2M2_PM_SYS_COMMAND_ENABLE_USER_CONFIG_LOG_RESP",     0x61),
    ("M2M2_PM_SYS_COMMAND_DISABLE_USER_CONFIG_LOG_REQ",     0x62),
    ("M2M2_PM_SYS_COMMAND_DISABLE_USER_CONFIG_LOG_RESP",    0x63),    
    ("M2M2_PM_SYS_COMMAND_STORE_TO_EEPROM_REQ",             0x64),
    ("M2M2_PM_SYS_COMMAND_STORE_TO_EEPROM_RESP",            0x65),
    ("M2M2_PS_SYS_COMMAND_GET_LOW_TOUCH_LOGGING_STATUS_REQ", 0x66),
    ("M2M2_PS_SYS_COMMAND_GET_LOW_TOUCH_LOGGING_STATUS_RESP", 0x67),
    ("M2M2_PM_SYS_COMMAND_WRITE_EEPROM_REQ",                  0x68),
    ("M2M2_PM_SYS_COMMAND_WRITE_EEPROM_RESP",                 0x69),
    ("M2M2_PM_SYS_COMMAND_READ_EEPROM_REQ",                   0x6A),
    ("M2M2_PM_SYS_COMMAND_READ_EEPROM_RESP",                  0x6B),
    ("M2M2_PM_SYS_COMMAND_AD5110_WRITE_RDAC_REQ",             0x6C),
    ("M2M2_PM_SYS_COMMAND_AD5110_WRITE_RDAC_RESP",            0x6D),
    ("M2M2_PM_SYS_COMMAND_AD5110_READ_RDAC_REQ",              0x6E),
    ("M2M2_PM_SYS_COMMAND_AD5110_READ_RDAC_RESP",             0x6F),
    ("M2M2_PM_SYS_COMMAND_ADP5258_READ_RDAC_REQ",             0x70),
    ("M2M2_PM_SYS_COMMAND_ADP5258_READ_RDAC_RESP",            0x71),
    ("M2M2_PM_SYS_COMMAND_ADP5350_OVP_SETTING_REQ",           0x72),
    ("M2M2_PM_SYS_COMMAND_ADP5350_OVP_SETTING_RESP",          0x73),
    ("M2M2_PM_SYS_COMMAND_AD5110_Store_to_EEPROM_REQ",        0x74),
    ("M2M2_PM_SYS_COMMAND_AD5110_Store_to_EEPROM_RESP",       0x75),
    ("M2M2_PM_SYS_COMMAND_SYSTEM_RESET_REQ",                  0x76),
    ("M2M2_PM_SYS_COMMAND_SYSTEM_RESET_RESP",                 0x77),
    ("M2M2_PM_SYS_COMMAND_DUT_POWER_ON_REQ",                  0x78),
    ("M2M2_PM_SYS_COMMAND_DUT_POWER_ON_RESP",                 0x79),
    ("M2M2_PM_SYS_COMMAND_DUT_POWER_OFF_REQ",                 0x7A),
    ("M2M2_PM_SYS_COMMAND_DUT_POWER_OFF_RESP",                0x7B),
    ]
}

M2M2_PM_SYS_STATUS_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("__M2M2_PM_SYS_STATUS_LOWEST",0x40),
    ("M2M2_PM_SYS_STATUS_OK",0x41),
    ("M2M2_PM_SYS_STATUS_ERR_ARGS",0x42),
    ("M2M2_PM_SYS_STATUS_LOW_TOUCH_LOGGING_ALREADY_STARTED", 0x43),
    ("M2M2_PM_SYS_STATUS_CONFIG_FILE_NOT_FOUND", 0x44),
    ("M2M2_PM_SYS_STATUS_CONFIG_FILE_READ_ERR", 0x45),
    ("M2M2_PM_SYS_STATUS_ENABLE_USER_CONFIG_LOG_FAILED", 0x46),
    ("M2M2_PM_SYS_STATUS_USER_CONFIG_LOG_ENABLED", 0x47),
    ("M2M2_PM_SYS_STATUS_DISABLE_USER_CONFIG_LOG_FAILED", 0x48),
    ("M2M2_PM_SYS_STATUS_USER_CONFIG_LOG_DISABLED", 0x49), 
    ("M2M2_PM_SYS_STATUS_LOG_STOPPED_THROUGH_BUTTON_A", 0x4A),
    ("M2M2_PM_SYS_STATUS_LOW_TOUCH_LOGGING_IN_PROGRESS", 0x4B),
    ("M2M2_PM_SYS_STATUS_LOW_TOUCH_LOGGING_NOT_STARTED", 0x4C),   
    ("M2M2_PM_SYS_STATUS_LOW_TOUCH_MAX_FILE_ERR", 0x4D),
    ("M2M2_PM_SYS_STATUS_LOW_TOUCH_MEMORY_FULL_ERR", 0x4E),
    ("M2M2_PM_SYS_STATUS_ERR_NOT_CHKD",0xFF),
    ]
}

M2M2_PM_SYS_BAT_STATE_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("M2M2_PM_SYS_BAT_STATE_NOT_AVAIL",0x0),
    ("M2M2_PM_SYS_BAT_STATE_NOT_CHARGING",0x1),
    ("M2M2_PM_SYS_BAT_STATE_CHARGING",0x2),
    ("M2M2_PM_SYS_BAT_STATE_COMPLETE",0x3),
    ("M2M2_PM_SYS_BAT_STATE_DETECTION",0x4),
    ("M2M2_PM_SYS_BAT_STATE_CHARGE_ERR",0x5),
    ]
}

M2M2_PM_SYS_USB_PWR_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("M2M2_PM_SYS_USB_PWR_ENABLE",0x0),
    ("M2M2_PM_SYS_USB_PWR_DISABLE",0x1),
    ]
}

M2M2_PM_SYS_THERMISTOR_STATE_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("M2M2_PM_SYS_THERMISTOR_ENABLE",0x0),
    ("M2M2_PM_SYS_THERMISTOR_DISABLE",0x1),
    ]
}

M2M2_PM_SYS_USB_PWR_ACTION_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("M2M2_PM_SYS_USB_PWR_SET",0x0),
    ("M2M2_PM_SYS_USB_PWR_GET",0x1),
    ]
}

M2M2_PM_SYS_PWR_STATE_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("M2M2_PM_SYS_PWR_STATE_ACTIVE",0x0),
    ("M2M2_PM_SYS_PWR_STATE_FLEXI",0x1),
    ("M2M2_PM_SYS_PWR_STATE_HIBERNATE",0x2),
    ("M2M2_PM_SYS_PWR_STATE_SHUTDOWN",0x3),
    ]
}

M2M2_PM_SYS_MCU_TYPE_ENUM_t = {
  "type":c_uint8,
  "enum_values": [
    ("M2M2_PM_SYS_MCU_INVALID",0x0),
    ("M2M2_PM_SYS_MCU_M3",0x1),
    ("M2M2_PM_SYS_MCU_M4",0x2),
    ]
}

M2M2_PM_SYS_OVP_ENUM_t = {
    "type":c_uint8,
    "enum_values":[
        ("M2M2_PM_SYS_OVP_18_5V",0x0),
        ("M2M2_PM_SYS_OVP_15V",0x1),
        ("M2M2_PM_SYS_OVP_10V",0x2),
        ("M2M2_PM_SYS_OVP_5_6V",0x3),
        ]
}

M2M2_PM_SYS_BOOST_ENUM_t = {
    "type":c_uint8,
    "enum_values":[
        ("M2M2_PM_SYS_BOOST_ENABLE",0x0),
        ("M2M2_PM_SYS_BOOST_DISABLE",0x1),
  ]
}

ADI_PM_BOARD_TYPE_t = {
  "type":c_uint8,
  "enum_values": [
    ("ADI_PM_BOARD_TYPE_UNKNOWN",0x0),
    ("ADI_PM_BOARD_TYPE_ADPD107_WATCH",0x1),
    ("ADI_PM_BOARD_TYPE_ADPD107_CHEST_STRAP",0x2),
    ("ADI_PM_BOARD_TYPE_ADPD185_WATCH",0x3),
    ("ADI_PM_BOARD_TYPE_ADPD188_WATCH",0x4),
    ]
}

m2m2_pm_sys_cmd_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
  ]
}

m2m2_pm_sys_pwr_state_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"state",
    "type":M2M2_PM_SYS_PWR_STATE_ENUM_t},
  ]
}

m2m2_pm_sys_info_t = {
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
    "type": ADI_PM_BOARD_TYPE_t},
  ]
}

m2m2_pm_sys_date_time_req_t = {
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

m2m2_pm_sys_bat_info_resp_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"timestamp",
    "type":c_uint32},
    {"name":"bat_chrg_stat",
    "type":M2M2_PM_SYS_BAT_STATE_ENUM_t},
    {"name":"bat_lvl",
    "type":c_uint8},
    {"name":"bat_mv",
    "type":c_uint16},
    {"name":"bat_temp",
    "type":c_uint16},
  ]
}

m2m2_pm_sys_bat_thr_req_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"bat_level_low",
    "type":c_uint8},
    {"name":"bat_level_critical",
    "type":c_uint8},
  ]
}

m2m2_bat_crit_info_resp_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"bat_mv",
    "type":c_uint16},
  ]
}

m2m2_pm_sys_usb_pwr_cmd_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"enable",
    "type":M2M2_PM_SYS_USB_PWR_ENUM_t},
    {"name":"action",
    "type":M2M2_PM_SYS_USB_PWR_ACTION_ENUM_t},
  ]
}

m2m2_pm_sys_bat_thermistor_cmd_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"enable",
    "type":M2M2_PM_SYS_THERMISTOR_STATE_ENUM_t},
  ]
}

m2m2_pm_sys_mcu_version_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"mcu",
    "type":M2M2_PM_SYS_MCU_TYPE_ENUM_t},
  ]
}

m2m2_pm_sys_boost_cmd_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"enable",
    "type":M2M2_PM_SYS_BOOST_ENUM_t},
    {"name":"ovp",
     "type":M2M2_PM_SYS_OVP_ENUM_t},
  ]
}

m2m2_pm_sys_powerboost_cmd_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"enable",
    "type":M2M2_PM_SYS_BOOST_ENUM_t},
    {"name":"ovp",
     "type":c_uint8},
  ]
}

m2m2_pm_sys_eeprom_cmd_t = {
  "struct_fields": [
    {"name":None,
    "type":common_application_interface._m2m2_app_common_cmd_t},
    {"name":"size",
    "type":c_uint8},
    {"name":"byte_stream",
     "length":16,
     "type":c_uint8},
  ]
}
