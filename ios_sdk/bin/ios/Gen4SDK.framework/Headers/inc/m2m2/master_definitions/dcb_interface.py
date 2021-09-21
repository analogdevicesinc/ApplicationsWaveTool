#!/usr/bin/env python3

from ctypes import *

import m2m2_core

import common_application_interface

MAXADPD4000DCBSIZE = (58) #//Max size of adpd4000 DCB size in double word length; 58 uint32_t elements in dcfg
MAXADXLDCBSIZE = (25)
MAXPPGDCBSIZE = (53)
MAXECGDCBSIZE = (2)
MAXEDADCBSIZE = (2)

M2M2_DCB_COMMAND_ENUM_t =
{
    "type":c_uint8,
    "enum_values":
    [
        ("__M2M2_DCB_COMMAND_LOWEST",                      0x96),
        ("M2M2_DCB_COMMAND_READ_CONFIG_REQ",               0x97),
        ("M2M2_DCB_COMMAND_READ_CONFIG_RESP",              0x98),
        ("M2M2_DCB_COMMAND_WRITE_CONFIG_REQ",              0x99),
        ("M2M2_DCB_COMMAND_WRITE_CONFIG_RESP",             0x9A),
        ("M2M2_DCB_COMMAND_ERASE_CONFIG_REQ",              0x9B),
        ("M2M2_DCB_COMMAND_ERASE_CONFIG_RESP",             0x9C),
    ]
}

M2M2_DCB_STATUS_ENUM_t =
{
    "type":c_uint8,
    "enum_values":
    [
        ("__M2M2_DCB_STATUS_LOWEST",                       0x96),
        ("M2M2_DCB_STATUS_OK",                             0x97),
        ("M2M2_DCB_STATUS_ERR_ARGS",                       0x98),
        ("M2M2_DCB_STATUS_ERR_NOT_CHKD",                   0xFF),
    ]
}

m2m2_dcb_cmd_t =
{
    "struct_fields":
    [
        {"name":"command",
        "type":c_uint8},
        {"name":"status",
        "type":c_uint8},
    ]
}

m2m2_dcb_adpd4000_dcfg_data_t =
{
    "struct_fields":
    [
        {"name":"command",
         "type":c_uint8},
        {"name":"status",
         "type":c_uint8},
        {"name":"size",
         "type":c_uint16},
        {"name":"dcfgdata",
         "length":MAXADPD4000DCBSIZE,
         "type":c_uint32},

    ]
}

m2m2_dcb_adxl_dcfg_data_t =
{
    "struct_fields":
    [
        {"name":"command",
         "type":c_uint8},
        {"name":"status",
         "type":c_uint8},
        {"name":"size",
         "type":c_uint16},
        {"name":"dcfgdata",
         "length":MAXADXLDCBSIZE,
         "type":c_uint32},

    ]
}

m2m2_dcb_ppg_lcfg_data_t =
{
    "struct_fields":
    [
        {"name":"command",
         "type":c_uint8},
        {"name":"status",
         "type":c_uint8},
        {"name":"size",
         "type":c_uint16},
        {"name":"dcfgdata",
         "length":MAXPPGDCBSIZE,
         "type":c_int32},

    ]
}

m2m2_dcb_ecg_lcfg_data_t =
{
    "struct_fields":
    [
        {"name":"command",
         "type":c_uint8},
        {"name":"status",
         "type":c_uint8},
        {"name":"size",
         "type":c_uint16},
        {"name":"dcfgdata",
         "length":MAXECGDCBSIZE,
         "type":c_uint32},

    ]
}

m2m2_dcb_eda_lcfg_data_t =
{
    "struct_fields":
    [
        {"name":"command",
         "type":c_uint8},
        {"name":"status",
         "type":c_uint8},
        {"name":"size",
         "type":c_uint16},
        {"name":"dcfgdata",
         "length":MAXEDADCBSIZE,
         "type":c_uint32},

    ]
}