from ctypes import *

from common_application_interface_def import *

from common_sensor_interface_def import *

from m2m2_core_def import *

class M2M2_EDA_APP_CMD_ENUM_t(c_ubyte):
    _M2M2_EDA_APP_CMD_LOWEST = 0x40
    M2M2_EDA_APP_CMD_DYNAMIC_SCALE_REQ  = 0x42
    M2M2_EDA_APP_CMD_DYNAMIC_SCALE_RESP = 0x43
    M2M2_EDA_APP_CMD_SET_DATA_RATE_REQ = 0x44
    M2M2_EDA_APP_CMD_SET_DATA_RATE_RESP = 0x45
    M2M2_EDA_APP_CMD_SET_DFT_NUM_REQ = 0x46
    M2M2_EDA_APP_CMD_SET_DFT_NUM_RESP = 0x47

class M2M2_SENSOR_EDA_NSAMPLES_ENUM_t(c_ubyte):
    M2M2_SENSOR_EDA_NSAMPLES = 0x6

class M2M2_SENSOR_EDA_RAW_DATA_TYPES_ENUM_t(c_ubyte):
    M2M2_SENSOR_EDA_DATA = 0x0

class eda_app_lib_state_t(Structure):
    _pack_ = 1
    _fields_ = [
              ("command", c_ubyte),
              ("status", c_ubyte),
              ("states", c_ubyte * 10),
              ]

class eda_app_lcfg_op_t(Structure):
    _pack_ = 1
    _fields_ = [
              ("field", c_ubyte),
              ("value", c_ulong),
              ]

def eda_app_lcfg_op_hdr_t(array_size):
  class eda_app_lcfg_op_hdr_t_internal(Structure):
    _pack_ = 1
    _fields_ = [
              ("command", c_ubyte),
              ("status", c_ubyte),
              ("num_ops", c_ubyte),
              ("ops", eda_app_lcfg_op_t * array_size),
              ]
  return eda_app_lcfg_op_hdr_t_internal()

class eda_data_set_t(Structure):
    _pack_ = 1
    _fields_ = [
              ("timestamp", c_ulong),
              ("realdata", c_short),
              ("imgdata", c_short),
              ]

class eda_app_stream_t(Structure):
    _pack_ = 1
    _fields_ = [
              ("command", c_ubyte),
              ("status", c_ubyte),
              ("sequence_num", c_ushort),
              ("datatype", c_ubyte),
              ("eda_data", eda_data_set_t * 6),
              ]

class eda_app_dynamic_scale_t(Structure):
    _pack_ = 1
    _fields_ = [
              ("command", c_ubyte),
              ("status", c_ubyte),
              ("dscale", c_ubyte),
              ("minscale", c_ushort),
              ("maxscale",c_ushort),
              ("lprtiasel",c_ushort),
              ]

class eda_app_set_data_rate_t(Structure):
    _pack_ = 1
    _fields_ = [
              ("command", c_ubyte),
              ("status", c_ubyte),
              ("datarate", c_ushort),
              ]

class eda_app_set_dft_num_t(Structure):
    _pack_ = 1
    _fields_ = [
              ("command", c_ubyte),
              ("status", c_ubyte),
              ("dftnum", c_ubyte),
              ]

class eda_app_dcb_lcfg_t(Structure):
    _pack_ = 1
    _fields_ = [
              ("command", c_ubyte),
              ("status", c_ubyte),
              ]
