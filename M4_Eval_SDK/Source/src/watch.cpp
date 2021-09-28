/******************************************************************************
* Copyright (c) 2019 Analog Devices, Inc.  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* - Redistributions of source code must retain the above copyright notice, this
*   list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*   and/or other materials provided with the distribution.
* - Modified versions of the software must be conspicuously marked as such.
* - This software is licensed solely and exclusively for use with
*   processors/products manufactured by or for Analog Devices, Inc.
* - This software may not be combined or merged with other code in any manner
*   that would cause the software to become subject to terms and conditions
*   which differ from those listed here.
* - Neither the name of Analog Devices, Inc. nor the names of its contributors
*   may be used to endorse or promote products derived from this software
*   without specific prior written permission.
* - The use of this software may or may not infringe the patent rights of one
*   or more patent holders.  This license does not release you from the
*   requirement that you obtain separate licenses from these patent holders to
*   use this software.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
* NONINFRINGEMENT, TITLE, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL ANALOG DEVICES, INC. OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, PUNITIVE OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, DAMAGES ARISING OUT OF
* CLAIMS OF INTELLECTUAL PROPERTY RIGHTS INFRINGEMENT; PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/
#include <iostream>
#include <string>
#include "stdint.h"

#ifdef SWIG_PY
#include "Python.h"
#endif // SWIG_PY

#include "watch.hpp"
#include "git_version.hpp"
#include "byte_swap.hpp"

#define MAXIMUM_FILE 64
#define BATTERY_LEVEL 5
#define MEMORY_REACHED 5
#define Debug 0
#define MIN_PKT_SIZE 9
#define HARDWARE_ID_185 0x8002
#define HARDWARE_ID_107 0x8001
#define HARDWARE_ID_SB3 0x8003
#define HARDWARE_ID_OPL_103 0x9080


#define HARDWARE_ID_OPL_108 0x90
#define HARDWARE_ID_OPL_4000 0x91
#define HARDWARE_ID_OPL_4100 0x92

#define BOM_ID_107_CHESTSTRAP 0x0101
#define BOM_ID_SB3 0x0000
#define CHIP_ID1_188_WATCH 0x0916
#define CHIP_ID2_188_WATCH 0x0A16

/*!
   \brief Constructor for the top-level Watch class
*/
watch::watch(watch_phy_callback &cb):
      pkt_phy_writer(cb),
    led_app(this),
    adxl_app(this),
			eda_app(this),
      ecg_app(this),
    ad7689_app(this),
    ad5940_app(this),
    temperature_app(this),
      adpd_app(this),
			ppg_app(this),
			syncppg_app(this),
			adpd4000_app(this),
      pm_app(this),
    ps_app(this),
    ped_app(this),
    bcm_app(this),
    fs_app(this) {}


const std::string watch::SDK_version = "4.0.0";
const std::string watch::git_hash = g_GIT_HASH;
const std::string watch::git_describe = g_GIT_DESCRIBE;

/*!
   \brief Perform a basic self-test
*/
void watch::self_test(void) {
  std::cout << "=======================================================" << std::endl;
  std::cout << "SELF TEST" << std::endl;
  std::cout << "=======================================================" << std::endl;
  std::cout << "Welcome to ADI's Watch SDK" << std::endl;
  std::cout << "Version: " << this->SDK_version << std::endl;
  //std::cout << "Git hash: " << this->git_hash << std::endl;
  //std::cout << "Git describe: " << this->git_describe << std::endl;
}

/*!
   \brief set the platform. @param platForm (python/windows/android/ios)
   Note:Platform must be set before executing any command
*/
void watch::initialize_sdk(platforms platForm) {
  platform = platForm;
  pm_sys_info_t sys_info;
  this->pm_app.get_system_info(&sys_info);
  if (sys_info.hw_id == HARDWARE_ID_185) {
    static const uint16_t chip_id_arr[] = {8};
    std::vector<uint16_t> chip_id_reg_address(chip_id_arr, chip_id_arr + sizeof(chip_id_arr) / sizeof(chip_id_arr[0]));
    std::vector<std::pair<uint16_t, uint16_t>> ret_vals = this->adpd_app.register_read(chip_id_reg_address);
    uint16_t chip_id = ret_vals[0].second;
    if (chip_id == CHIP_ID1_188_WATCH || chip_id == CHIP_ID2_188_WATCH) {
      set_device_id(ret::device_identifiers::ADPD188WATCH);
    } else {
      set_device_id(ret::device_identifiers::ADPD185WATCH);
    }
  } else if (sys_info.hw_id == HARDWARE_ID_107 && sys_info.bom_id == BOM_ID_107_CHESTSTRAP) {
    set_device_id(ret::device_identifiers::ADPD107CHESTSTRAP);
  } else if (sys_info.hw_id == HARDWARE_ID_SB3) {
    set_device_id(ret::device_identifiers::SENSORBOARD3);
  }
  else if (sys_info.hw_id == HARDWARE_ID_107) {
	  set_device_id(ret::device_identifiers::ADPD107WATCH);
  }
  else if (sys_info.hw_id == HARDWARE_ID_OPL_103) {
	  set_device_id(ret::device_identifiers::OPTICAL_BOARD_103);
  }
  else {

	  uint16_t opl_dev_id = (sys_info.hw_id & 0xFF00) >> 8;

	  if (opl_dev_id == HARDWARE_ID_OPL_108)
	  {
		  set_device_id(ret::device_identifiers::OPTICAL_BOARD_108);

	  }
	  else if (opl_dev_id == HARDWARE_ID_OPL_4000)
	  {
		  set_device_id(ret::device_identifiers::OPTICAL_BOARD_4000);

	  }
	  else if (opl_dev_id == HARDWARE_ID_OPL_4100)
	  {
		  set_device_id(ret::device_identifiers::OPTICAL_BOARD_4100);

	  }
	  else
	  {
		  // Set default dev id if none of the above hardware found
		  uint8_t temp_dev_id = (opl_dev_id & 0xF0) << 4;

		  if (temp_dev_id == 0x08) // Hardware id starts from 8 means, set default to 107 watch
		  {
			  set_device_id(ret::device_identifiers::ADPD107WATCH);

		  }
		  else // else set default to optical board 108
		  {
			  set_device_id(ret::device_identifiers::OPTICAL_BOARD_108);

		  }

	  }

  }
}

ret::device_identifiers watch::getDeviceID(void)
{
	return get_device_id();
}


/*!
   \brief set the platform. @param platForm (python/windows/android/ios)
   Note:Platform must be set before executing any command
*/
void watch::set_platform(platforms platForm) {
  platform = platForm;
}

/*!
   \brief Returns a string describing the version of the SDK.
*/
std::string watch::get_version(void) {
  return this->git_hash;
}

/*!
\brief Call physical callback sys_alert_method with the system error message
*/
void watch::system_alert_call(uint8_t alert_message)
{
	this->pkt_phy_writer.sys_alert_call(ADI_SDK_PACKET_TIMED_OUT);
}

/*!
   \brief Called by Watch application objects to send a packet over a physical interface
*/
void watch::write_bytes(std::vector<uint8_t> &bytes) {
#ifdef SWIG_PY
// We need to make sure that this thread is registered with Python's Global Interpreter Lock before calling out to Python code
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();
#endif // SWIG_PY

  m2m2_hdr_t * p_hdr = (m2m2_hdr_t*)&bytes[0];
  m2m2_pkt<_m2m2_app_common_cmd_t> pkt;
  pkt.unpack(bytes);

  if (pkt.payload.command == M2M2_SENSOR_COMMON_CMD_STREAM_DATA && is_fs_refhr_cmd) {
    p_hdr->src = this->get_stream_address();
  } else {
    p_hdr->src = this->get_address();
  }
  // Swap the endianness of the header fields
  // src
  std::swap(bytes[0], bytes[1]);
  // dest
  std::swap(bytes[2], bytes[3]);
  // length
  std::swap(bytes[4], bytes[5]);
  // TODO: calculate checksum
  // Call the PHY callback
  this->pkt_phy_writer.call(bytes);


#ifdef SWIG_PY
// Clean up Python's GIL lock
  PyGILState_Release(gstate);
#endif // SWIG_PY
}

/*!
   \brief Returns the M2M2 address assigned to the SDK.
*/
M2M2_ADDR_ENUM_t watch::get_address(void) {
  switch (platform) {
    case python:
      return M2M2_ADDR_APP_CLI;
    case windows:
      return M2M2_ADDR_APP_WT;
    case android:
      return M2M2_ADDR_APP_DROID;
    case ios:
      return M2M2_ADDR_APP_IOS;
    default:
      return M2M2_ADDR_UNDEFINED;
  }
}

/*!
   \brief Returns the M2M2 address assigned to the SDK.
*/
M2M2_ADDR_ENUM_t watch::get_stream_address(void) {
  switch (platform) {
    case python:
      return M2M2_ADDR_APP_CLI_STREAM;
    case windows:
      return M2M2_ADDR_APP_WT_STREAM;
    case android:
      return M2M2_ADDR_APP_DROID_STREAM;
    case ios:
      return M2M2_ADDR_APP_IOS_STREAM;
    default:
      return M2M2_ADDR_UNDEFINED;
  }
}

/*!
   \brief Dispatches a packet of bytes to the appropriate application objects within the SDK.

   Call this function whenever you receive bytes over the physical interface from the device.
*/
void watch::dispatch(std::vector<uint8_t> bytes) {
  m2m2_hdr_t *hdr = reinterpret_cast<m2m2_hdr_t *> (&bytes[0]);
#if Debug
  std::cout << "C++ return: Received a Packet " << BYTE_SWAP_16(hdr->src) <<std::endl;
#endif
  switch(BYTE_SWAP_16(hdr->src)) {
    case M2M2_ADDR_SYS_LED_0:
      this->led_app.dispatch(bytes);
      break;
    case M2M2_ADDR_SENSOR_ADXL:
      this->adxl_app.dispatch(bytes);
      break;
    case M2M2_ADDR_SENSOR_ADXL_STREAM:
      this->adxl_app.adxl_stream.dispatch(bytes);
      break;
    case M2M2_ADDR_SYS_AGC_STREAM:
      this->ppg_app.agc_stream.dispatch(bytes);
      break;
    case M2M2_ADDR_MED_ECG:
      this->ecg_app.dispatch(bytes);
      break;
    case M2M2_ADDR_MED_EDA:
      this->eda_app.dispatch(bytes);
      break;
    case M2M2_ADDR_MED_EDA_STREAM:
      this->eda_app.eda_stream.dispatch(bytes);
      break;
    case M2M2_ADDR_MED_ECG_STREAM:
      this->ecg_app.ecg_stream.dispatch(bytes);
      break;
    case M2M2_ADDR_SENSOR_AD7689:
      this->ad7689_app.dispatch(bytes);
      break;
    case M2M2_ADDR_SENSOR_AD5940:
      this->ad5940_app.dispatch(bytes);
      break;
    case M2M2_ADDR_MED_TEMPERATURE:
      this->temperature_app.dispatch(bytes);
      break;
    case M2M2_ADDR_MED_TEMPERATURE_STREAM:
      this->temperature_app.temperature_stream.dispatch(bytes);
      break;
    case M2M2_ADDR_SENSOR_ADPD_STREAM:
#if Debug
		std::cout << "C++ return: Received a Packet : M2M2_ADDR_SENSOR_ADPD_STREAM!" << std::endl;
#endif
      this->adpd_app.adpd_stream.dispatch(bytes);
      break;
    case M2M2_ADDR_SENSOR_ADPD:
      this->adpd_app.dispatch(bytes);
      break;
		case M2M2_ADDR_MED_PPG:
			this->ppg_app.dispatch(bytes);
			break;
		case M2M2_ADDR_MED_PPG_STREAM:
			this->ppg_app.ppg_stream.dispatch(bytes);
			break;
		case M2M2_ADDR_MED_SYNC_ADPD_ADXL:
			this->syncppg_app.dispatch(bytes);
			break;
		case M2M2_ADDR_MED_SYNC_ADPD_ADXL_STREAM:
			this->syncppg_app.syncppg_stream.dispatch(bytes);
			break;
		case M2M2_ADDR_SENSOR_ADPDCL:
			this->adpd4000_app.dispatch(bytes);
			break;
		case M2M2_ADDR_SENSOR_ADPD_STREAM1:
			this->adpd4000_app.adpd4000_stream1.dispatch(bytes);
			break;
		case M2M2_ADDR_SENSOR_ADPD_STREAM2:
			this->adpd4000_app.adpd4000_stream2.dispatch(bytes);
			break;
		case M2M2_ADDR_SENSOR_ADPD_STREAM3:
			this->adpd4000_app.adpd4000_stream3.dispatch(bytes);
			break;
		case M2M2_ADDR_SENSOR_ADPD_STREAM4:
			this->adpd4000_app.adpd4000_stream4.dispatch(bytes);
			break;
		case M2M2_ADDR_SENSOR_ADPD_STREAM5:
			this->adpd4000_app.adpd4000_stream5.dispatch(bytes);
			break;
		case M2M2_ADDR_SENSOR_ADPD_STREAM6:
			this->adpd4000_app.adpd4000_stream6.dispatch(bytes);
			break;
		case M2M2_ADDR_SENSOR_ADPD_STREAM7:
			this->adpd4000_app.adpd4000_stream7.dispatch(bytes);
			break;
		case M2M2_ADDR_SENSOR_ADPD_STREAM8:
			this->adpd4000_app.adpd4000_stream8.dispatch(bytes);
			break;
		case M2M2_ADDR_SENSOR_ADPD_STREAM9:
			this->adpd4000_app.adpd4000_stream9.dispatch(bytes);
			break;
		case M2M2_ADDR_SENSOR_ADPD_STREAM10:
			this->adpd4000_app.adpd4000_stream10.dispatch(bytes);
			break;
		case M2M2_ADDR_SENSOR_ADPD_STREAM11:
			this->adpd4000_app.adpd4000_stream11.dispatch(bytes);
			break;
		case M2M2_ADDR_SENSOR_ADPD_STREAM12:
			this->adpd4000_app.adpd4000_stream12.dispatch(bytes);
			break;
		case M2M2_ADDR_SENSOR_ADPD_OPTIONAL_BYTES_STREAM:
			this->adpd4000_app.adpd4000_optional_stream.dispatch(bytes);
			break;
    case M2M2_ADDR_SYS_FS:
      this->dispatch_fs(bytes);
      break;
    case M2M2_ADDR_MED_PED:
      this->ped_app.dispatch(bytes);
      break;
    case M2M2_ADDR_MED_PED_STREAM:
      this->ped_app.pedometer_stream.dispatch(bytes);
      break;
		case M2M2_ADDR_SYS_PM:
      this->dispatch_pm(bytes);
      break;
		case M2M2_ADDR_SYS_BATT_STREAM:
			this->pm_app.battery_stream.dispatch(bytes);
      break;
    case M2M2_ADDR_MED_BCM_STREAM:
      this->bcm_app.bcm_stream.dispatch(bytes);
      break;
    case M2M2_ADDR_SYS_HRV_STREAM:
      this->ppg_app.hrv_stream.dispatch(bytes);
      break;
    case M2M2_ADDR_MED_BCM:
      this->bcm_app.dispatch(bytes);
      break;
    case M2M2_ADDR_SYS_PS:
      this->ps_app.dispatch(bytes);
			break;
		default:
			std::cout << "C++::WARNING: Received a Packet that couldn't be dispatched!" << std::endl;
			std::cout << "    Packet is:" << std::endl;
			for (unsigned int i = 0; i < bytes.size(); i++) {
				std::cout << " " << std::hex << std::setfill('0') << std::setw(2) << (int) bytes[i];
			}
			std::cout << std::endl;
  }
}

/*!
   \brief Decode if the byte stream and route to proper dispatcher.
*/
void watch::dispatch_fs(std::vector<uint8_t> bytes) {
  if (bytes.size() >= MIN_PKT_SIZE) {
    m2m2_pkt<_m2m2_app_common_cmd_t> pkt;
    pkt.unpack(bytes);
    if (pkt.payload.command == M2M2_FILE_SYS_CMD_LS_RESP) {
      this->fs_app.fs_ls_response.dispatch(bytes);
    } else if (pkt.payload.command == M2M2_FILE_SYS_CMD_GET_RESP) {
      this->fs_app.fs_stream.dispatch(bytes);
    } else {
      this->dispatch_fs_system_alert(bytes);
    }
  } else {
    this->fs_app.dispatch(bytes);
  }
}

/*!
   \brief Decode if the byte stream is system alert or route to fs dispatch.
*/
void watch::dispatch_fs_system_alert(std::vector<uint8_t> bytes) {
  m2m2_pkt<_m2m2_app_common_cmd_t> pkt;
  pkt.unpack(bytes);
  if (pkt.payload.command == M2M2_FILE_SYS_CMD_FORCE_STOP_LOG_RESP) {
    if (pkt.payload.status == M2M2_FILE_SYS_ERR_MEMORY_FULL) {
      this->pkt_phy_writer.sys_alert_call(FILE_SYS_LOG_STOPPED_MEMORY_FULL);
    } else if (pkt.payload.status == M2M2_FILE_SYS_ERR_LOG_FORCE_STOPPED) {
      this->pkt_phy_writer.sys_alert_call(FILE_SYS_LOG_STOPPED_LOW_BATTERY);
    }
  } else {
    this->fs_app.dispatch(bytes);
  }
}

/*!
   \brief Decode if the byte stream is any low touch alert such as low touch memory full, low touch max file count or route to proper dispatch.
*/
void watch::dispatch_pm(std::vector<uint8_t> bytes) {
  if (this->is_user_cmd) {
    this->pm_app.dispatch(bytes);
  } else {
    if (bytes.size() >= MIN_PKT_SIZE) {
      m2m2_pkt<_m2m2_app_common_cmd_t> pkt;
      pkt.unpack(bytes);
      if (pkt.payload.command == M2M2_PM_SYS_COMMAND_ENABLE_USER_CONFIG_LOG_RESP) {
        if (pkt.payload.status == M2M2_PM_SYS_STATUS_USER_CONFIG_LOG_ENABLED) {
          this->pkt_phy_writer.sys_alert_call(USER_CONFIG_AUTO_LOGGING_STARTED);
        } else if (pkt.payload.status == M2M2_PM_SYS_STATUS_LOW_TOUCH_MEMORY_FULL_ERR) {
          this->pkt_phy_writer.sys_alert_call(USER_CONFIG_AUTO_LOGGING_NOT_STARTED_MEMORY_FULL_ERR);
        } else if (pkt.payload.status == M2M2_PM_SYS_STATUS_LOW_TOUCH_MAX_FILE_ERR) {
          this->pkt_phy_writer.sys_alert_call(USER_CONFIG_AUTO_LOGGING_NOT_STARTED__MAX_FILE_ERR);
        } else if (pkt.payload.status == M2M2_PM_SYS_STATUS_USER_CONFIG_LOG_DISABLED) {
          this->pkt_phy_writer.sys_alert_call(USER_CONFIG_AUTO_LOGGING_WRIST_OFF_DETECTED);
        }
      } else if (pkt.payload.command == M2M2_PM_SYS_COMMAND_DISABLE_USER_CONFIG_LOG_RESP) {
        if (pkt.payload.status == M2M2_PM_SYS_STATUS_LOG_STOPPED_THROUGH_BUTTON_A) {
          this->pkt_phy_writer.sys_alert_call(FILE_SYS_LOG_STOPPED_THROUGH_BUTTON_A);
        } else if (pkt.payload.status == M2M2_PM_SYS_STATUS_USER_CONFIG_LOG_DISABLED) {
          this->pkt_phy_writer.sys_alert_call(USER_CONFIG_AUTO_LOGGING_STOPPED);
        }
      } else if (pkt.payload.command == M2M2_FILE_SYS_CMD_FORCE_STOP_LOG_RESP) {
        if (pkt.payload.status == M2M2_PM_SYS_STATUS_USER_CONFIG_LOG_DISABLED) {
          this->pkt_phy_writer.sys_alert_call(USER_CONFIG_AUTO_LOGGING_STOPPED_LOW_BATTERY);
        }
      } else {
        this->pm_app.dispatch(bytes);
      }
    } else {
      this->pm_app.dispatch(bytes);
    }
  }
}


/**
 * Start the PPG, sync-adpd-adxl stream with a default configuration.
 * @param ppg_stream_cb ppg stream callback to be registered
 * @param sync_stream_cb sync ppg stream callback to be registered
 * @param enableAGC set true to enable AGC stream
 * @return returns the status
 */
ret::err_status watch::start_syncppg(ppg_stream_callback &ppg_stream_cb, syncppg_stream_callback &sync_stream_cb) {
	ret::sdk_status status;
	m2m2_application::APP_COMMON_STATUS_ENUM_t stream_start_status;
	ret::device_identifiers device_id = get_device_id();
	switch (device_id) {
	case ret::device_identifiers::ADPD107WATCH:
	case ret::device_identifiers::SENSORBOARD3:
		status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_107);
		if (status == ret::SDK_OK) {
			status = this->ppg_app.load_lcfg(ppg_application::PPG_LCFG_ID_ADPD107);
		}
		else {
			return ret::LOAD_CFG_FAILED;
		}
		if (status == ret::SDK_OK) {
			status = this->adpd_app.calibrate_clock();
		}
		else {
			return ret::PPG_LOAD_LCFG_FAILED;
		}
		break;
	case ret::device_identifiers::ADPD185WATCH:
		status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_185);
		if (status == ret::SDK_OK) {
			status = this->ppg_app.load_lcfg(ppg_application::PPG_LCFG_ID_ADPD185);
		}
		else {
			return ret::LOAD_CFG_FAILED;
		}
		if (status == ret::SDK_OK) {
			status = this->adpd_app.calibrate_clock();
		}
		else {
			return ret::PPG_LOAD_LCFG_FAILED;
		}
		break;
	case ret::device_identifiers::ADPD188WATCH:
		status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_188);
		if (status == ret::SDK_OK) {
			status = this->ppg_app.load_lcfg(ppg_application::PPG_LCFG_ID_ADPD188);
		}
		else {
			return ret::LOAD_CFG_FAILED;
		}
		if (status == ret::SDK_OK) {
			status = this->adpd_app.calibrate_clock();
		}
		else {
			return ret::PPG_LOAD_LCFG_FAILED;
		}
		break;
	case ret::device_identifiers::OPTICAL_BOARD_103:
	case ret::device_identifiers::OPTICAL_BOARD_108:
		status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_108);
		if (status == ret::SDK_OK) {
			status = this->ppg_app.load_lcfg(ppg_application::PPG_LCFG_ID_ADPD108);
		}
		else {
			return ret::LOAD_CFG_FAILED;
		}
		if (status == ret::SDK_OK) {
			status = this->adpd_app.calibrate_clock();
		}
		else {
			return ret::PPG_LOAD_LCFG_FAILED;
		}
		break;
	case ret::device_identifiers::OPTICAL_BOARD_4000:
		status = this->adpd4000_app.load_cfg(adpd4000_application::ADPDCL_DEVICE_4000);
		if (status == ret::SDK_OK) {
			status = this->ppg_app.load_lcfg(ppg_application::PPG_LCFG_ID_ADPD4000);
		}
		else {
			return ret::LOAD_CFG_FAILED;
		}
		if (status == ret::SDK_OK) {
			status = this->adpd4000_app.calibrate_clock(6);
		}
		else {
			return ret::PPG_LOAD_LCFG_FAILED;
		}
		break;
	case ret::device_identifiers::OPTICAL_BOARD_4100:
		status = this->adpd4000_app.load_cfg(adpd4000_application::ADPDCL_DEVICE_4100);
		if (status == ret::SDK_OK) {
			status = this->ppg_app.load_lcfg(ppg_application::PPG_LCFG_ID_ADPD4000);
		}
		else {
			return ret::LOAD_CFG_FAILED;
		}
		if (status == ret::SDK_OK) {
			status = this->adpd4000_app.calibrate_clock(6);
		}
		else {
			return ret::PPG_LOAD_LCFG_FAILED;
		}
		break;
	case ret::device_identifiers::ADPD107CHESTSTRAP:
		std::cerr << "ADI SDK:: Error! PPG not supported for ADPD107CHESTSTRAP: " << std::endl;
		return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
	default:
		std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
		return ret::UNKNOWN_DEVICE;
	}
	
	if (status == ret::SDK_OK) {
		status = this->ppg_app.ppg_stream.subscribe(ppg_stream_cb);
	}
	else {
		//if (enableAGC) {
		return ret::CLOCK_CALIBRATE_FAILED;
		//}
	}
	if (status == ret::SDK_OK) {
		status = this->syncppg_app.syncppg_stream.subscribe(sync_stream_cb);
	}
	else {
		return ret::SUBSCRIBE_FAILED;
	}
	if (device_id == ret::device_identifiers::OPTICAL_BOARD_4100 || device_id == ret::device_identifiers::OPTICAL_BOARD_4000)
	{
		if (status == ret::SDK_OK) {
			stream_start_status = this->ppg_app.ppg_stream.start();
		}
		else {
			return ret::SUBSCRIBE_FAILED;
		}
		if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
			return ret::STREAM_START_FAILED;
		}
		else {
			return ret::STREAM_STARTED;
		}
	}
	else
	{
		if (status == ret::SDK_OK) {
			stream_start_status = this->adpd_app.adpd_stream.start();
		}
		else {
			return ret::SUBSCRIBE_FAILED;
		}
		if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
			return ret::STREAM_START_FAILED;
		}
		else {
			stream_start_status = this->adxl_app.adxl_stream.start();
		}
		if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
			return ret::STREAM_START_FAILED;
		}
		else {
			stream_start_status = this->ppg_app.ppg_stream.start();
		}
		if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
			return ret::STREAM_START_FAILED;
		}
		else {
			return ret::STREAM_STARTED;
		}
	}
	
}

/**
 * Stop the PPG, sync-adpd-adxl streams
 * @return returns the status
 */
ret::err_status watch::stop_syncppg(void) {
	m2m2_application::APP_COMMON_STATUS_ENUM_t stream_stop_status;
	ret::sdk_status unsub_status;
	switch (get_device_id()) {
	case ret::device_identifiers::ADPD107WATCH:
	case ret::device_identifiers::SENSORBOARD3:
	case ret::device_identifiers::ADPD185WATCH:
	case ret::device_identifiers::ADPD188WATCH:
	case ret::device_identifiers::OPTICAL_BOARD_103:
	case ret::device_identifiers::OPTICAL_BOARD_108:

		stream_stop_status = this->ppg_app.ppg_stream.stop();
		if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
			return ret::STREAM_STOP_FAILED;
		}
		else {
			stream_stop_status = this->adpd_app.adpd_stream.stop();
		}
		if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
			return ret::STREAM_STOP_FAILED;
		}
		else {
			stream_stop_status = this->adxl_app.adxl_stream.stop();
		}
		if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
			return ret::STREAM_STOP_FAILED;
		}
		else {
			unsub_status = this->ppg_app.ppg_stream.unsubscribe();
		}
		if (unsub_status == ret::SDK_OK) {
			unsub_status = this->syncppg_app.syncppg_stream.unsubscribe();
		}
		else {
			return ret::UNSUBSCRIBE_FAILED;
		}
		/* if (disableAGC) {
		   if (unsub_status == ret::SDK_OK) {
			 unsub_status = this->ppg_app.agc_stream.unsubscribe();
		   } else {
			 return ret::UNSUBSCRIBE_FAILED;
		   }
		 }*/
		if (unsub_status == ret::SDK_OK) {
			return ret::STREAM_STOPPED;
		}
		else {
			return ret::UNSUBSCRIBE_FAILED;
		}
		break;
	case ret::device_identifiers::OPTICAL_BOARD_4000:
	case ret::device_identifiers::OPTICAL_BOARD_4100:

		stream_stop_status = this->ppg_app.ppg_stream.stop();
		if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
			return ret::STREAM_STOP_FAILED;
		}
		else {
			unsub_status = this->ppg_app.ppg_stream.unsubscribe();
		}
		if (unsub_status == ret::SDK_OK) {
			unsub_status = this->syncppg_app.syncppg_stream.unsubscribe();
		}
		else {
			return ret::UNSUBSCRIBE_FAILED;
		}
		/* if (disableAGC) {
		   if (unsub_status == ret::SDK_OK) {
			 unsub_status = this->ppg_app.agc_stream.unsubscribe();
		   } else {
			 return ret::UNSUBSCRIBE_FAILED;
		   }
		 }*/
		if (unsub_status == ret::SDK_OK) {
			return ret::STREAM_STOPPED;
		}
		else {
			return ret::UNSUBSCRIBE_FAILED;
		}
		break;
	case ret::device_identifiers::ADPD107CHESTSTRAP:
		std::cerr << "ADI SDK:: Error! PPG not supported for ADPD107CHESTSTRAP: " << std::endl;
		return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
		break;
	default:
		std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
		return ret::UNKNOWN_DEVICE;
		break;
	}
}



/*!
   \brief Start the ADPD stream with a default configuration.
*/
ret::err_status watch::start_adpd(adpd_stream_callback &stream_cb) {
	ret::sdk_status status;
	m2m2_application::APP_COMMON_STATUS_ENUM_t stream_start_status;
	switch (get_device_id()) {
	case ret::device_identifiers::ADPD107WATCH:
	case ret::device_identifiers::SENSORBOARD3:
		status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_107);
		break;
	case ret::device_identifiers::ADPD185WATCH:
		status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_185);
		break;
	case ret::device_identifiers::ADPD188WATCH:
		status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_188);
		break;
	case ret::device_identifiers::OPTICAL_BOARD_103:
	case ret::device_identifiers::OPTICAL_BOARD_108:
		status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_108);
		break;
	case ret::device_identifiers::OPTICAL_BOARD_4000:
		status = this->adpd_app.load_cfg(adpd_application::ADPDCL_DEVICE_4000);
		break;
	case ret::device_identifiers::OPTICAL_BOARD_4100:
		status = this->adpd_app.load_cfg(adpd_application::ADPDCL_DEVICE_4100);
		break;
	case ret::device_identifiers::ADPD107CHESTSTRAP:
		std::cerr << "ADI SDK:: Error! ADPD not supported for ADPD107CHESTSTRAP: " << std::endl;
		return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
	default:
		std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
		return ret::UNKNOWN_DEVICE;
	}
	if (status == ret::SDK_OK) {
		status = this->adpd_app.calibrate_clock();
	}
	else {
		return ret::LOAD_CFG_FAILED;
	}
	if (status == ret::SDK_OK) {
		status = this->adpd_app.set_slot(this->adpd_app.slotA, this->adpd_app.slotB);
		
	}
	else {
		return ret::CLOCK_CALIBRATE_FAILED;
	}
	if (status == ret::SDK_OK) {
		status = this->adpd_app.adpd_stream.subscribe(stream_cb);
	}
	else {
		return ret::SET_SLOT_FAILED;
	}
	if (status == ret::SDK_OK) {
		stream_start_status = this->adpd_app.adpd_stream.start();
	}
	else {
		return ret::SUBSCRIBE_FAILED;
	}
	if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
		stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
		return ret::STREAM_START_FAILED;
	}
	else {
		return ret::STREAM_STARTED;
	}
}

/*!
   \brief Stop the ADPD stream.
*/
ret::err_status watch::stop_adpd(void) {
	m2m2_application::APP_COMMON_STATUS_ENUM_t stream_stop_status;
	ret::sdk_status unsub_status;
	switch (get_device_id()) {
	case ret::device_identifiers::ADPD107WATCH:
	case ret::device_identifiers::SENSORBOARD3:
	case ret::device_identifiers::ADPD185WATCH:
	case ret::device_identifiers::ADPD188WATCH:
	case ret::device_identifiers::OPTICAL_BOARD_103:
	case ret::device_identifiers::OPTICAL_BOARD_108:
	case ret::device_identifiers::OPTICAL_BOARD_4000:
	case ret::device_identifiers::OPTICAL_BOARD_4100:
		stream_stop_status = this->adpd_app.adpd_stream.stop();
		if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
			return ret::STREAM_STOP_FAILED;
		}
		else {
			unsub_status = this->adpd_app.adpd_stream.unsubscribe();
		}
		if (unsub_status == ret::SDK_OK) {
			return ret::STREAM_STOPPED;
		}
		else {
			return ret::UNSUBSCRIBE_FAILED;
		}
	case ret::device_identifiers::ADPD107CHESTSTRAP:
		std::cerr << "ADI SDK:: Error! ADPD not supported for ADPD107CHESTSTRAP: " << std::endl;
		return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
	default:
		std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
		return ret::UNKNOWN_DEVICE;
	}
}

/*!
   \brief Start the temperature stream with a default configuration.
*/
ret::err_status watch::start_temperature(temperature_stream_callback &stream_cb) {

	ret::sdk_status status;
	m2m2_application::APP_COMMON_STATUS_ENUM_t stream_start_status;
	switch (get_device_id()) {
	case ret::device_identifiers::SENSORBOARD3:
	case ret::device_identifiers::OPTICAL_BOARD_103:
	case ret::device_identifiers::OPTICAL_BOARD_108:
	case ret::device_identifiers::OPTICAL_BOARD_4000:
	case ret::device_identifiers::OPTICAL_BOARD_4100:
		status = this->temperature_app.temperature_stream.subscribe(stream_cb);
		
		if (status == ret::SDK_OK) {
			stream_start_status = this->temperature_app.temperature_stream.start();
		}
		else {
			return ret::SUBSCRIBE_FAILED;
		}
		if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
			return ret::STREAM_START_FAILED;
		}
		else {
			return ret::STREAM_STARTED;
		}
	case ret::device_identifiers::ADPD107WATCH:
		status = this->temperature_app.temperature_stream.subscribe(stream_cb);
		if (status == ret::SDK_OK) {
			stream_start_status = this->temperature_app.temperature_stream.start();
		}
		else {
			return ret::SUBSCRIBE_FAILED;
		}
		if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
			return ret::STREAM_START_FAILED;
		}
		else {
			status = this->ad7689_app.start();
		}
		if (status == ret::SDK_OK) {
			return ret::STREAM_STARTED;
		}
		else {
			return ret::AD7689_START_FAILED;
		}
	case ret::device_identifiers::ADPD185WATCH:
	case ret::device_identifiers::ADPD188WATCH:
	case ret::device_identifiers::ADPD107CHESTSTRAP:
		std::cerr << "ADI SDK:: Error! EDA not supported for this Device Id: " << std::endl;
		return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
	default:
		std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
		return ret::UNKNOWN_DEVICE;
	}

}

/*!
   \brief Stop the temperature stream.
*/
ret::err_status watch::stop_temperature(void) {

	m2m2_application::APP_COMMON_STATUS_ENUM_t stream_stop_status;
	ret::sdk_status status;
	switch (get_device_id()) {
	case ret::device_identifiers::SENSORBOARD3:
	case ret::device_identifiers::OPTICAL_BOARD_103:
	case ret::device_identifiers::OPTICAL_BOARD_108:
	case ret::device_identifiers::OPTICAL_BOARD_4000:
	case ret::device_identifiers::OPTICAL_BOARD_4100:
		stream_stop_status = this->temperature_app.temperature_stream.stop();

		if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
			return ret::STREAM_STOP_FAILED;
		}
		else {
			status = this->temperature_app.temperature_stream.unsubscribe();
		}
		if (status == ret::SDK_OK) {
			return ret::STREAM_STOPPED;
		}
		else {
			return ret::UNSUBSCRIBE_FAILED;
		}
	case ret::device_identifiers::ADPD107WATCH:
		stream_stop_status = this->temperature_app.temperature_stream.stop();
		if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
			return ret::STREAM_STOP_FAILED;
		}
		else {
			status = this->ad7689_app.stop();
		}
		if (status == ret::SDK_OK) {
			status = this->temperature_app.temperature_stream.unsubscribe();
		}
		else {
			return ret::AD7689_STOP_FAILED;
		}
		if (status == ret::SDK_OK) {
			return ret::STREAM_STOPPED;
		}
		else {
			return ret::UNSUBSCRIBE_FAILED;
		}
	case ret::device_identifiers::ADPD185WATCH:
	case ret::device_identifiers::ADPD188WATCH:
	case ret::device_identifiers::ADPD107CHESTSTRAP:
		std::cerr << "ADI SDK:: Error! EDA not supported for this Device Id: " << std::endl;
		return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
	default:
		std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
		return ret::UNKNOWN_DEVICE;
	}

}

/*!
   \brief Start the ADXL stream with a default configuration.
*/
ret::err_status watch::start_adxl(adxl_stream_callback &stream_cb) {
  ret::sdk_status status;
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_status;

  status = this->adxl_app.adxl_stream.subscribe(stream_cb);
	if (status == ret::SDK_OK)
	{
		stream_status = this->adxl_app.adxl_stream.start();
	}
	else
	{
		return ret::SUBSCRIBE_FAILED;
	}

	if (stream_status != m2m2_application::APP_COMMON_STATUS_STREAM_STARTED && stream_status != m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
		return ret::STREAM_START_FAILED;
	}
	else {
		return ret::STREAM_STARTED;
	}

}

/*!
   \brief Stop the ADXL stream.
*/
ret::err_status watch::stop_adxl(void) {
	ret::sdk_status status;
	m2m2_application::APP_COMMON_STATUS_ENUM_t stream_status;

	stream_status = this->adxl_app.adxl_stream.stop();

	if (stream_status != m2m2_application::APP_COMMON_STATUS_STREAM_STOPPED && stream_status != m2m2_application::APP_COMMON_STATUS_STREAM_COUNT_DECREMENT) {
		return ret::STREAM_STOP_FAILED;
	}
	else {
		status = this->adxl_app.adxl_stream.unsubscribe();
	}
	if (status == ret::SDK_OK)
	{
		return ret::STREAM_STOPPED;
	}
	else
	{
		return ret::UNSUBSCRIBE_FAILED;
	}
  
}

/*!
\brief Start the ADPD4000 stream with default configuration.
*/
ret::err_status watch::start_adpd4000(adpd4000_stream_callback &stream_cb) {
	ret::sdk_status status;
	m2m2_application::APP_COMMON_STATUS_ENUM_t stream_status;

	this->adpd4000_app.adpd4000_stream1.subscribe(stream_cb);
	this->adpd4000_app.adpd4000_stream2.subscribe(stream_cb);
	this->adpd4000_app.adpd4000_stream3.subscribe(stream_cb);
	this->adpd4000_app.adpd4000_stream4.subscribe(stream_cb);
	this->adpd4000_app.adpd4000_stream5.subscribe(stream_cb);
	this->adpd4000_app.adpd4000_stream6.subscribe(stream_cb);
	this->adpd4000_app.adpd4000_stream7.subscribe(stream_cb);
	this->adpd4000_app.adpd4000_stream8.subscribe(stream_cb);
	this->adpd4000_app.adpd4000_stream9.subscribe(stream_cb);
	this->adpd4000_app.adpd4000_stream10.subscribe(stream_cb);
	this->adpd4000_app.adpd4000_stream11.subscribe(stream_cb);
	this->adpd4000_app.adpd4000_stream12.subscribe(stream_cb);
	stream_status =  this->adpd4000_app.adpd4000_stream1.start();

	if (stream_status != m2m2_application::APP_COMMON_STATUS_STREAM_STARTED && stream_status != m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
		return ret::STREAM_START_FAILED;
}
	else {
		return ret::STREAM_STARTED;
	}


	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
#if Debug
	std::cout << "SDK:: Time difference in start_adpd= " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;
#endif
}

/*!
\brief Stop the ADPD4000 stream.
*/
ret::err_status watch::stop_adpd4000(void) {
	ret::sdk_status status;
	m2m2_application::APP_COMMON_STATUS_ENUM_t stream_status;

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	stream_status = this->adpd4000_app.adpd4000_stream1.stop();
	this->adpd4000_app.adpd4000_stream1.unsubscribe();
	this->adpd4000_app.adpd4000_stream2.unsubscribe();
	this->adpd4000_app.adpd4000_stream3.unsubscribe();
	this->adpd4000_app.adpd4000_stream4.unsubscribe();
	this->adpd4000_app.adpd4000_stream5.unsubscribe();
	this->adpd4000_app.adpd4000_stream6.unsubscribe();
	this->adpd4000_app.adpd4000_stream7.unsubscribe();
	this->adpd4000_app.adpd4000_stream8.unsubscribe();
	this->adpd4000_app.adpd4000_stream9.unsubscribe();
	this->adpd4000_app.adpd4000_stream10.unsubscribe();
	this->adpd4000_app.adpd4000_stream11.unsubscribe();
	this->adpd4000_app.adpd4000_stream12.unsubscribe();
	
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	if (stream_status != m2m2_application::APP_COMMON_STATUS_STREAM_STOPPED && stream_status != m2m2_application::APP_COMMON_STATUS_STREAM_COUNT_DECREMENT) {
		return ret::STREAM_STOP_FAILED;
	}
	else {
		return ret::STREAM_STOPPED;
	}


#if Debug
	std::cout << "SDK:: Time difference in stop_adpd= " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;
#endif
}




/*!
   \brief Start the ECG stream with a default configuration.
*/
ret::err_status watch::start_ecg(ecg_stream_callback &stream_cb) {
	ret::sdk_status status;
	m2m2_application::APP_COMMON_STATUS_ENUM_t stream_start_status;
	std::vector<std::pair<uint8_t, uint16_t>> addr_vals;
	std::vector<std::pair<uint8_t, uint16_t>> ret_vals;
	status = this->ecg_app.ecg_stream.subscribe(stream_cb);
	if (status == ret::SDK_OK) {
		addr_vals.push_back(std::make_pair(static_cast<uint8_t>(0), static_cast<uint16_t>(100)));
		addr_vals.push_back(std::make_pair(static_cast<uint8_t>(2), static_cast<uint16_t>(1)));
		addr_vals.push_back(std::make_pair(static_cast<uint8_t>(5), static_cast<uint16_t>(0)));
		ret_vals = this->ecg_app.lcfg_write(addr_vals);
		if (ret_vals.size() == 3 && ret_vals[0].second == 100 && ret_vals[1].second == 1) {
			addr_vals.clear();
			ret_vals.clear();
			switch (get_device_id()) {
			case ret::device_identifiers::SENSORBOARD3:
			case ret::device_identifiers::OPTICAL_BOARD_103:
			case ret::device_identifiers::OPTICAL_BOARD_108:
			case ret::device_identifiers::OPTICAL_BOARD_4000:
			case ret::device_identifiers::OPTICAL_BOARD_4100:
				status = this->ad5940_app.start();
				if (status == ret::SDK_OK) {
					stream_start_status = this->ecg_app.ecg_stream.start();
				}
				else {
					return ret::AD5940_START_FAILED;
				}
				if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
					stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
					return ret::STREAM_START_FAILED;
				}
				else {
					return ret::STREAM_STARTED;
				}
			case ret::device_identifiers::ADPD107WATCH:
			case ret::device_identifiers::ADPD107CHESTSTRAP:
				stream_start_status = this->ecg_app.ecg_stream.start();
				if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
					stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
					return ret::STREAM_START_FAILED;
				}
				else {
					status = this->ad7689_app.start();
				}
				if (status == ret::SDK_OK) {
					return ret::STREAM_STARTED;
				}
				else {
					return ret::AD7689_START_FAILED;
				}
			case ret::device_identifiers::ADPD185WATCH:
				status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_ECG_185);
				if (status == ret::SDK_OK) {
					status = this->adpd_app.calibrate_clock();
				}
				else {
					return ret::LOAD_CFG_FAILED;
				}
				if (status == ret::SDK_OK) {
					addr_vals.push_back(std::make_pair(static_cast<uint8_t>(1), static_cast<uint16_t>(2)));
					ret_vals = this->ecg_app.lcfg_write(addr_vals);
				}
				else {
					return ret::CLOCK_CALIBRATE_FAILED;
				}
				if (ret_vals.size() == 1 && ret_vals[0].second == 2) {
					stream_start_status = this->ecg_app.ecg_stream.start();
				}
				else {
					return ret::LCFG_WRITE_FAILED;
				}
				addr_vals.clear();
				ret_vals.clear();
				if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
					stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
					return ret::STREAM_START_FAILED;
				}
				else {
					stream_start_status = this->adpd_app.adpd_stream.start();
				}
				if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
					stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
					return ret::STREAM_START_FAILED;
				}
				else {
					return ret::STREAM_STARTED;
				}
			case ret::device_identifiers::ADPD188WATCH:
				status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_ECG_188);
				if (status == ret::SDK_OK) {
					status = this->adpd_app.calibrate_clock();
				}
				else {
					return ret::LOAD_CFG_FAILED;
				}
				if (status == ret::SDK_OK) {
					addr_vals.push_back(std::make_pair(static_cast<uint8_t>(1), static_cast<uint16_t>(2)));
					ret_vals = this->ecg_app.lcfg_write(addr_vals);
				}
				else {
					return ret::CLOCK_CALIBRATE_FAILED;
				}
				if (ret_vals.size() == 1 && ret_vals[0].second == 2) {
					stream_start_status = this->ecg_app.ecg_stream.start();
				}
				else {
					return ret::LCFG_WRITE_FAILED;
				}
				addr_vals.clear();
				ret_vals.clear();
				if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
					stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
					return ret::STREAM_START_FAILED;
				}
				else {
					stream_start_status = this->adpd_app.adpd_stream.start();
				}
				if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
					stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
					return ret::STREAM_START_FAILED;
				}
				else {
					return ret::STREAM_STARTED;
				}
			default:
				std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
				return ret::UNKNOWN_DEVICE;
			}
		}
		else {
			return ret::LCFG_WRITE_FAILED;
		}
	}
	else {
		return ret::SUBSCRIBE_FAILED;
	}
}

/*!
   \brief Stop the ECG stream.
*/
ret::err_status watch::stop_ecg(void) {
	m2m2_application::APP_COMMON_STATUS_ENUM_t stream_stop_status;
	ret::sdk_status status;
	switch (get_device_id()) {
	case ret::device_identifiers::SENSORBOARD3:
	case ret::device_identifiers::OPTICAL_BOARD_103:
	case ret::device_identifiers::OPTICAL_BOARD_108:
	case ret::device_identifiers::OPTICAL_BOARD_4000:
	case ret::device_identifiers::OPTICAL_BOARD_4100:
		status = this->ad5940_app.stop();
		if (status == ret::SDK_OK) {
			stream_stop_status = this->ecg_app.ecg_stream.stop();
		}
		else {
			return ret::AD5940_STOP_FAILED;
		}
		if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
			return ret::STREAM_STOP_FAILED;
		}
		else {
			status = this->ecg_app.ecg_stream.unsubscribe();
		}
		if (status == ret::SDK_OK) {
			return ret::STREAM_STOPPED;
		}
		else {
			return ret::UNSUBSCRIBE_FAILED;
		}
	case ret::device_identifiers::ADPD107WATCH:
	case ret::device_identifiers::ADPD107CHESTSTRAP:
		stream_stop_status = this->ecg_app.ecg_stream.stop();
		if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
			return ret::STREAM_STOP_FAILED;
		}
		else {
			status = this->ad7689_app.stop();
		}
		if (status == ret::SDK_OK) {
			status = this->ecg_app.ecg_stream.unsubscribe();
		}
		else {
			return ret::AD7689_STOP_FAILED;
		}
		if (status == ret::SDK_OK) {
			return ret::STREAM_STOPPED;
		}
		else {
			return ret::UNSUBSCRIBE_FAILED;
		}
	case ret::device_identifiers::ADPD185WATCH:
	case ret::device_identifiers::ADPD188WATCH:
		stream_stop_status = this->ecg_app.ecg_stream.stop();
		if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
			return ret::STREAM_STOP_FAILED;
		}
		else {
			stream_stop_status = this->adpd_app.adpd_stream.stop();
		}
		if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
			return ret::STREAM_STOP_FAILED;
		}
		else {
			status = this->ecg_app.ecg_stream.unsubscribe();
		}
		if (status == ret::SDK_OK) {
			return ret::STREAM_STOPPED;
		}
		else {
			return ret::UNSUBSCRIBE_FAILED;
		}
	default:
		std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
		return ret::UNKNOWN_DEVICE;
	}
}

/*!
   \brief Start the EDA stream with a default configuration.
*/
ret::err_status watch::start_eda(eda_stream_callback &stream_cb) {
	ret::sdk_status status;
	m2m2_application::APP_COMMON_STATUS_ENUM_t stream_start_status;
	switch (get_device_id()) {
	case ret::device_identifiers::SENSORBOARD3:
	case ret::device_identifiers::OPTICAL_BOARD_103:
	case ret::device_identifiers::OPTICAL_BOARD_108:
	case ret::device_identifiers::OPTICAL_BOARD_4000:
	case ret::device_identifiers::OPTICAL_BOARD_4100:
		status = this->eda_app.eda_stream.subscribe(stream_cb);
		if (status == ret::SDK_OK) {
			status = this->ad5940_app.start();
		}
		else {
			return ret::SUBSCRIBE_FAILED;
		}
		if (status == ret::SDK_OK) {
			stream_start_status = this->eda_app.eda_stream.start();
		}
		else {
			return ret::AD5940_START_FAILED;
		}
		if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
			return ret::STREAM_START_FAILED;
		}
		else {
			return ret::STREAM_STARTED;
		}
	case ret::device_identifiers::ADPD107WATCH:
		status = this->eda_app.eda_stream.subscribe(stream_cb);
		if (status == ret::SDK_OK) {
			stream_start_status = this->eda_app.eda_stream.start();
		}
		else {
			return ret::SUBSCRIBE_FAILED;
		}
		if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
			return ret::STREAM_START_FAILED;
		}
		else {
			status = this->ad7689_app.start();
		}
		if (status == ret::SDK_OK) {
			return ret::STREAM_STARTED;
		}
		else {
			return ret::AD5940_START_FAILED;
		}
	case ret::device_identifiers::ADPD185WATCH:
	case ret::device_identifiers::ADPD188WATCH:
	case ret::device_identifiers::ADPD107CHESTSTRAP:
		std::cerr << "ADI SDK:: Error! EDA not supported for this Device Id: " << std::endl;
		return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
	default:
		std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
		return ret::UNKNOWN_DEVICE;
	}
}

/*!
   \brief Stop the EDA stream.
*/
ret::err_status watch::stop_eda(void) {
	m2m2_application::APP_COMMON_STATUS_ENUM_t stream_stop_status;
	ret::sdk_status status;
	switch (get_device_id()) {
	case ret::device_identifiers::SENSORBOARD3:
	case ret::device_identifiers::OPTICAL_BOARD_103:
	case ret::device_identifiers::OPTICAL_BOARD_108:
	case ret::device_identifiers::OPTICAL_BOARD_4000:
	case ret::device_identifiers::OPTICAL_BOARD_4100:
		status = this->ad5940_app.stop();
		if (status == ret::SDK_OK) {
			stream_stop_status = this->eda_app.eda_stream.stop();
		}
		else {
			return ret::AD5940_STOP_FAILED;
		}
		if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
			return ret::STREAM_STOP_FAILED;
		}
		else {
			status = this->eda_app.eda_stream.unsubscribe();
		}
		if (status == ret::SDK_OK) {
			return ret::STREAM_STOPPED;
		}
		else {
			return ret::UNSUBSCRIBE_FAILED;
		}
	case ret::device_identifiers::ADPD107WATCH:
		stream_stop_status = this->eda_app.eda_stream.stop();
		if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
			return ret::STREAM_STOP_FAILED;
		}
		else {
			status = this->ad7689_app.stop();
		}
		if (status == ret::SDK_OK) {
			status = this->eda_app.eda_stream.unsubscribe();
		}
		else {
			return ret::AD7689_STOP_FAILED;
		}
		if (status == ret::SDK_OK) {
			return ret::STREAM_STOPPED;
		}
		else {
			return ret::UNSUBSCRIBE_FAILED;
		}
	case ret::device_identifiers::ADPD185WATCH:
	case ret::device_identifiers::ADPD188WATCH:
	case ret::device_identifiers::ADPD107CHESTSTRAP:
		std::cerr << "ADI SDK:: Error! EDA not supported for this Device Id: " << std::endl;
		return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
	default:
		std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
		return ret::UNKNOWN_DEVICE;
	}
}



/*!
   \brief Start the Pedometer stream with a default configuration.
*/
ret::err_status watch::start_pedometer(pedometer_stream_callback &stream_cb) {
	ret::sdk_status status;
	m2m2_application::APP_COMMON_STATUS_ENUM_t stream_start_status;
	status = this->ped_app.pedometer_stream.subscribe(stream_cb);
	if (status == ret::SDK_OK) {
		stream_start_status = this->adxl_app.adxl_stream.start();
	}
	else {
		return ret::SUBSCRIBE_FAILED;
	}
	if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
		stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
		return ret::STREAM_START_FAILED;
	}
	else {
		stream_start_status = this->ped_app.pedometer_stream.start();
	}
	if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
		stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
		return ret::STREAM_START_FAILED;
	}
	else {
		return ret::STREAM_STARTED;
	}
}

/*!
   \brief Stop the Pedometer stream.
*/
ret::err_status watch::stop_pedometer(void) {
	m2m2_application::APP_COMMON_STATUS_ENUM_t stream_stop_status;
	ret::sdk_status status;
	stream_stop_status = this->ped_app.pedometer_stream.stop();
	if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
		stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
		return ret::STREAM_STOP_FAILED;
	}
	else {
		status = this->ped_app.pedometer_stream.unsubscribe();
	}
	if (status == ret::SDK_OK) {
		stream_stop_status = this->adxl_app.adxl_stream.stop();
	}
	else {
		return ret::UNSUBSCRIBE_FAILED;
	}
	if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
		stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
		return ret::STREAM_STOP_FAILED;
	}
	else {
		return ret::STREAM_STOPPED;
	}
}


ret::err_status watch::start_battery(battery_stream_callback &stream_cb) {
  ret::sdk_status status = this->pm_app.battery_stream.subscribe(stream_cb);
  if (status == ret::SDK_OK) {
    return ret::SUBSCRIBED;
  } else {
    return ret::SUBSCRIBE_FAILED;
  }
}

ret::err_status watch::stop_battery(void) {
  ret::sdk_status status = this->pm_app.battery_stream.unsubscribe();
  if (status == ret::SDK_OK) {
    return ret::UNSUBSCRIBED;
  } else {
    return ret::UNSUBSCRIBE_FAILED;
  }
}

/*!
   \brief Start the BCM stream with a default configuration.
*/
ret::err_status watch::start_bcm(bcm_stream_callback &stream_cb) {
	ret::sdk_status status;
	m2m2_application::APP_COMMON_STATUS_ENUM_t stream_start_status;
	this->bcm_app.bcm_stream.clearImpedanceListBuffer();

	if (get_device_id() == ret::device_identifiers::SENSORBOARD3) {
		status = this->ad5940_app.set_wg_freq(50000);
		if (status == ret::SDK_OK) {
			status = this->bcm_app.bcm_stream.subscribe(stream_cb);
		}
		else {
			return ret::SET_FREQ_FAILED;
		}
		if (status == ret::SDK_OK) {
			status = this->ad5940_app.start();
		}
		else {
			return ret::SUBSCRIBE_FAILED;
		}
		if (status == ret::SDK_OK) {
			stream_start_status = this->bcm_app.bcm_stream.start();
		}
		else {
			return ret::AD5940_START_FAILED;
		}
		if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
			return ret::STREAM_START_FAILED;
		}
		else {
			return ret::STREAM_STARTED;
		}
	}
	else {
		std::cerr << "ADI SDK:: Error! BCM not supported for Device Id: " << std::endl;
		return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
	}
}

/*!
   \brief Stop the BCM stream.
*/
ret::err_status watch::stop_bcm(void) {
	m2m2_application::APP_COMMON_STATUS_ENUM_t stream_stop_status;
	ret::sdk_status status;
	if (get_device_id() == ret::device_identifiers::SENSORBOARD3) {
		stream_stop_status = this->bcm_app.bcm_stream.stop();
		this->bcm_app.bcm_stream.clearImpedanceListBuffer();
		if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
			return ret::STREAM_STOP_FAILED;
		}
		else {
			status = this->bcm_app.bcm_stream.unsubscribe();
		}
		if (status == ret::SDK_OK) {
			status = this->ad5940_app.stop();
		}
		else {
			return ret::UNSUBSCRIBE_FAILED;
		}
		if (status == ret::SDK_OK) {
			return ret::STREAM_STOPPED;
		}
		else {
			return ret::AD5940_STOP_FAILED;
		}
	}
	else {
		std::cerr << "ADI SDK:: Error! BCM not supported for Device Id: " << std::endl;
		return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
	}
}

/*!
   \brief Start the HRV stream with a default configuration.
*/
ret::err_status watch::start_hrv(hrv_stream_callback &stream_cb) {
	ret::sdk_status status;
	m2m2_application::APP_COMMON_STATUS_ENUM_t stream_start_status;
	switch (get_device_id()) {
	case ret::device_identifiers::ADPD107WATCH:
	case ret::device_identifiers::SENSORBOARD3:
		status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_107);
		if (status == ret::SDK_OK) {
			status = this->ppg_app.load_lcfg(ppg_application::PPG_LCFG_ID_ADPD107);
		}
		else {
			return ret::LOAD_CFG_FAILED;
		}
		break;
	case ret::device_identifiers::ADPD185WATCH:
		status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_185);
		if (status == ret::SDK_OK) {
			status = this->ppg_app.load_lcfg(ppg_application::PPG_LCFG_ID_ADPD185);
		}
		else {
			return ret::LOAD_CFG_FAILED;
		}
		break;
	case ret::device_identifiers::ADPD188WATCH:
		status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_188);
		if (status == ret::SDK_OK) {
			status = this->ppg_app.load_lcfg(ppg_application::PPG_LCFG_ID_ADPD188);
		}
		else {
			return ret::LOAD_CFG_FAILED;
		}
		break;
	case ret::device_identifiers::ADPD107CHESTSTRAP:
		std::cerr << "ADI SDK:: Error! HRV not supported for ADPD107CHESTSTRAP: " << std::endl;
		return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
	default:
		std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
		return ret::UNKNOWN_DEVICE;
	}
	if (status == ret::SDK_OK) {
		status = this->adpd_app.calibrate_clock();
	}
	else {
		return ret::PPG_LOAD_LCFG_FAILED;
	}
	if (status == ret::SDK_OK) {
		status = this->ppg_app.hrv_stream.subscribe(stream_cb);
	}
	else {
		return ret::CLOCK_CALIBRATE_FAILED;
	}
	if (status == ret::SDK_OK) {
		stream_start_status = this->adpd_app.adpd_stream.start();
	}
	else {
		return ret::SUBSCRIBE_FAILED;
	}
	if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
		stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
		return ret::STREAM_START_FAILED;
	}
	else {
		stream_start_status = this->adxl_app.adxl_stream.start();
	}
	if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
		stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
		return ret::STREAM_START_FAILED;
	}
	else {
		stream_start_status = this->ppg_app.ppg_stream.start();
	}
	if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
		stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
		return ret::STREAM_START_FAILED;
	}
	else {
		return ret::STREAM_STARTED;
	}
}

/*!
   \brief Stop the HRV stream.
*/
ret::err_status watch::stop_hrv(void) {
	m2m2_application::APP_COMMON_STATUS_ENUM_t stream_stop_status;
	ret::sdk_status status;
	switch (get_device_id()) {
	case ret::device_identifiers::ADPD107WATCH:
	case ret::device_identifiers::SENSORBOARD3:
	case ret::device_identifiers::ADPD185WATCH:
	case ret::device_identifiers::ADPD188WATCH:
		stream_stop_status = this->adpd_app.adpd_stream.stop();
		if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
			return ret::STREAM_STOP_FAILED;
		}
		else {
			stream_stop_status = this->adxl_app.adxl_stream.stop();
		}
		if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
			return ret::STREAM_STOP_FAILED;
		}
		else {
			stream_stop_status = this->ppg_app.ppg_stream.stop();
		}
		if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
			stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
			return ret::STREAM_STOP_FAILED;
		}
		else {
			status = this->ppg_app.hrv_stream.unsubscribe();
		}
		if (status == ret::SDK_OK) {
			return ret::STREAM_STOPPED;
		}
		else {
			return ret::UNSUBSCRIBE_FAILED;
		}
	case ret::device_identifiers::ADPD107CHESTSTRAP:
		std::cerr << "ADI SDK:: Error! PPG not supported for ADPD107CHESTSTRAP: " << std::endl;
		return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
	default:
		std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
		return ret::UNKNOWN_DEVICE;
	}
}



/*!
 \brief Start the Logging of ADXL stream.
 */
ret::err_status watch::start_log_adxl(void) {
  ret::sdk_status status;
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_start_status;
  status = this->fs_app.fs_subscribe(fs_application::ADXL_Stream);
  if (status == ret::SDK_OK) {
    stream_start_status = this->fs_app.fs_stream.start();
  } else {
    return ret::FS_SUBSCRIBE_FAILED;
  }
  if (stream_start_status == m2m2_application::APP_COMMON_STATUS_OK ||
      stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
    stream_start_status = this->adxl_app.adxl_stream.start();
  } else {
    if (stream_start_status == m2m2_application::FILE_SYS_STATUS_ERROR) {
      return ret::FS_STATUS_ERROR;
    } else if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
      return ret::APP_COMMON_ERROR;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_INVALID) {
      return ret::FS_ERR_INVALID;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_CONFIG_FILE_POSITION) {
      return ret::FS_ERR_CONFIG_FILE_POSITION;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_MAX_FILE_COUNT) {
      return ret::FS_ERR_MAX_FILE_COUNT;
    }
  }
  if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
      stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
    return ret::LOGGING_NOT_STARTED;
  } else {
    return ret::LOGGING_STARTED;
  }
}

/*!
 \brief Stop the Logging of ADXL stream.
 */
ret::err_status watch::stop_log_adxl(void) {
  ret::sdk_status status;
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_stop_status;
  stream_stop_status = this->adxl_app.adxl_stream.stop();
  if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
      stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
    return ret::STREAM_STOP_FAILED;
  } else {
    status = this->fs_app.fs_unsubscribe(fs_application::ADXL_Stream);
  }
  if (status == ret::SDK_OK) {
    stream_stop_status = this->fs_app.fs_stream.stop();
  } else {
    return ret::FS_UNSUBSCRIBE_FAILED;
  }
  if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_OK) {
    return ret::LOGGING_STOPPED;
  } else {
    if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
      return ret::FILE_NOT_CLOSED;
    } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
      return ret::STREAM_IN_PROGRESS;
    } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
      return ret::FILE_NOT_OPENED;
    }
  }
  return ret::LOGGING_STOPPED;

}

/*!
 \brief Start the Logging of EDA stream.
 */
ret::err_status watch::start_log_eda(void) {
  ret::sdk_status status;
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_start_status;
  status = this->fs_app.fs_subscribe(fs_application::EDA_Stream);
  if (status == ret::SDK_OK) {
    stream_start_status = this->fs_app.fs_stream.start();
  } else {
    return ret::FS_SUBSCRIBE_FAILED;
  }
  switch (get_device_id()) {
    case ret::device_identifiers::SENSORBOARD3:
      if (stream_start_status == m2m2_application::APP_COMMON_STATUS_OK ||
          stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
        status = this->ad5940_app.start();
      } else {
        if (stream_start_status == m2m2_application::FILE_SYS_STATUS_ERROR) {
          return ret::FS_STATUS_ERROR;
        } else if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
          return ret::APP_COMMON_ERROR;
        } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_INVALID) {
          return ret::FS_ERR_INVALID;
        } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_CONFIG_FILE_POSITION) {
          return ret::FS_ERR_CONFIG_FILE_POSITION;
        } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_MAX_FILE_COUNT) {
          return ret::FS_ERR_MAX_FILE_COUNT;
        }
      }
      if (status == ret::SDK_OK) {
        stream_start_status = this->eda_app.eda_stream.start();
      } else {
        return ret::AD5940_START_FAILED;
      }
      if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
          stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
        return ret::LOGGING_NOT_STARTED;
      } else {
        return ret::LOGGING_STARTED;
      }
    case ret::device_identifiers::ADPD107WATCH:
      if (stream_start_status == m2m2_application::APP_COMMON_STATUS_OK ||
          stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
        stream_start_status = this->eda_app.eda_stream.start();
      } else {
        if (stream_start_status == m2m2_application::FILE_SYS_STATUS_ERROR) {
          return ret::FS_STATUS_ERROR;
        } else if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
          return ret::APP_COMMON_ERROR;
        } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_INVALID) {
          return ret::FS_ERR_INVALID;
        } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_CONFIG_FILE_POSITION) {
          return ret::FS_ERR_CONFIG_FILE_POSITION;
        } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_MAX_FILE_COUNT) {
          return ret::FS_ERR_MAX_FILE_COUNT;
        }
      }
      if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
          stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
        return ret::STREAM_START_FAILED;
      } else {
        status = this->ad7689_app.start();
      }
      if (status == ret::SDK_OK) {
        return ret::LOGGING_STARTED;
      } else {
        return ret::AD7689_START_FAILED;
      }
    case ret::device_identifiers::ADPD185WATCH:
    case ret::device_identifiers::ADPD188WATCH:
    case ret::device_identifiers::ADPD107CHESTSTRAP:
      std::cerr << "ADI SDK:: Error! EDA not supported for this Device Id: " << std::endl;
      return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
    default:
      std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
      return ret::UNKNOWN_DEVICE;
  }
}

/*!
 \brief Stop the Logging of EDA stream.
 */
ret::err_status watch::stop_log_eda(void) {
  ret::sdk_status status;
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_stop_status;
  switch (get_device_id()) {
    case ret::device_identifiers::SENSORBOARD3:
      status = this->ad5940_app.stop();
      if (status == ret::SDK_OK) {
        stream_stop_status = this->eda_app.eda_stream.stop();
      } else {
        return ret::AD5940_STOP_FAILED;
      }
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
          stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
        return ret::STREAM_STOP_FAILED;
      } else {
        status = this->fs_app.fs_unsubscribe(fs_application::EDA_Stream);
      }
      if (status == ret::SDK_OK) {
        stream_stop_status = this->fs_app.fs_stream.stop();
      } else {
        return ret::FS_UNSUBSCRIBE_FAILED;
      }
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_OK) {
        return ret::LOGGING_STOPPED;
      } else {
        if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
          return ret::FILE_NOT_CLOSED;
        } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
          return ret::STREAM_IN_PROGRESS;
        } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
          return ret::FILE_NOT_OPENED;
        }
      }
    case ret::device_identifiers::ADPD107WATCH:
      stream_stop_status = this->eda_app.eda_stream.stop();
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
          stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
        return ret::STREAM_STOP_FAILED;
      } else {
        status = this->ad7689_app.stop();
      }
      if (status == ret::SDK_OK) {
        status = this->fs_app.fs_unsubscribe(fs_application::EDA_Stream);
      } else {
        return ret::AD7689_STOP_FAILED;
      }
      if (status == ret::SDK_OK) {
        stream_stop_status = this->fs_app.fs_stream.stop();
      } else {
        return ret::FS_UNSUBSCRIBE_FAILED;
      }
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_OK) {
        return ret::LOGGING_STOPPED;
      } else {
        if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
          return ret::FILE_NOT_CLOSED;
        } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
          return ret::STREAM_IN_PROGRESS;
        } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
          return ret::FILE_NOT_OPENED;
        }
      }
    case ret::device_identifiers::ADPD185WATCH:
    case ret::device_identifiers::ADPD188WATCH:
    case ret::device_identifiers::ADPD107CHESTSTRAP:
      std::cerr << "ADI SDK:: Error! EDA not supported for this Device Id: " << std::endl;
      return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
    default:
      std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
      return ret::UNKNOWN_DEVICE;
  }
}

/*!
 \brief Start the Logging of ECG stream.
 */
ret::err_status watch::start_log_ecg(void) {
  ret::sdk_status status;
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_start_status;
  std::vector<std::pair<uint8_t, uint16_t>> addr_vals;
  std::vector<std::pair<uint8_t, uint16_t>> ret_vals;
  status = this->fs_app.fs_subscribe(fs_application::ECG_Stream);
  if (status == ret::SDK_OK) {
    stream_start_status = this->fs_app.fs_stream.start();
  } else {
    return ret::FS_SUBSCRIBE_FAILED;
  }
  if (stream_start_status == m2m2_application::APP_COMMON_STATUS_OK ||
      stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
    addr_vals.push_back(std::make_pair(static_cast<uint8_t>(0), static_cast<uint16_t>(100)));
    addr_vals.push_back(std::make_pair(static_cast<uint8_t>(2), static_cast<uint16_t>(1)));
    ret_vals = this->ecg_app.lcfg_write(addr_vals);
  } else {
    if (stream_start_status == m2m2_application::FILE_SYS_STATUS_ERROR) {
      return ret::FS_STATUS_ERROR;
    } else if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
      return ret::APP_COMMON_ERROR;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_INVALID) {
      return ret::FS_ERR_INVALID;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_CONFIG_FILE_POSITION) {
      return ret::FS_ERR_CONFIG_FILE_POSITION;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_MAX_FILE_COUNT) {
      return ret::FS_ERR_MAX_FILE_COUNT;
    }
  }
  if (ret_vals.size() == 2 && ret_vals[0].second == 100 && ret_vals[1].second == 1) {
    addr_vals.clear();
    ret_vals.clear();
    switch (get_device_id()) {
      case ret::device_identifiers::SENSORBOARD3:
        status = this->ad5940_app.start();
        if (status == ret::SDK_OK) {
          stream_start_status = this->ecg_app.ecg_stream.start();
        } else {
          return ret::AD5940_START_FAILED;
        }
        if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
            stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
          return ret::STREAM_START_FAILED;
        } else {
          return ret::LOGGING_STARTED;
        }
      case ret::device_identifiers::ADPD107WATCH:
      case ret::device_identifiers::ADPD107CHESTSTRAP:
        stream_start_status = this->ecg_app.ecg_stream.start();
        if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
            stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
          return ret::STREAM_START_FAILED;
        } else {
          status = this->ad7689_app.start();
        }
        if (status == ret::SDK_OK) {
          return ret::LOGGING_STARTED;
        } else {
          return ret::AD7689_START_FAILED;
        }
      case ret::device_identifiers::ADPD185WATCH:
        status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_ECG_185);
        if (status == ret::SDK_OK) {
          status = this->adpd_app.calibrate_clock();
        } else {
          return ret::LOAD_CFG_FAILED;
        }
        if (status == ret::SDK_OK) {
          addr_vals.push_back(std::make_pair(static_cast<uint8_t>(1), static_cast<uint16_t>(2)));
          ret_vals = this->ecg_app.lcfg_write(addr_vals);
        } else {
          return ret::CLOCK_CALIBRATE_FAILED;
        }
        if (ret_vals.size() == 1 && ret_vals[0].second == 2) {
          stream_start_status = this->ecg_app.ecg_stream.start();
        } else {
          return ret::LCFG_WRITE_FAILED;
        }
        addr_vals.clear();
        ret_vals.clear();
        if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
            stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
          return ret::STREAM_START_FAILED;
        } else {
          stream_start_status = this->adpd_app.adpd_stream.start();
        }
        if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
            stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
          return ret::STREAM_START_FAILED;
        } else {
          return ret::LOGGING_STARTED;
        }
      case ret::device_identifiers::ADPD188WATCH:
        status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_ECG_188);
        if (status == ret::SDK_OK) {
          status = this->adpd_app.calibrate_clock();
        } else {
          return ret::LOAD_CFG_FAILED;
        }
        if (status == ret::SDK_OK) {
          addr_vals.push_back(std::make_pair(static_cast<uint8_t>(1), static_cast<uint16_t>(2)));
          ret_vals = this->ecg_app.lcfg_write(addr_vals);
        } else {
          return ret::CLOCK_CALIBRATE_FAILED;
        }
        if (ret_vals.size() == 1 && ret_vals[0].second == 2) {
          stream_start_status = this->ecg_app.ecg_stream.start();
        } else {
          return ret::LCFG_WRITE_FAILED;
        }
        addr_vals.clear();
        ret_vals.clear();
        if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
            stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
          return ret::STREAM_START_FAILED;
        } else {
          stream_start_status = this->adpd_app.adpd_stream.start();
        }
        if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
            stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
          return ret::STREAM_START_FAILED;
        } else {
          return ret::LOGGING_STARTED;
        }
      default:
        std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
        return ret::UNKNOWN_DEVICE;
    }
  } else {
    return ret::LCFG_WRITE_FAILED;
  }
}

/*!
 \brief Stop the Logging of ECG stream.
 */
ret::err_status watch::stop_log_ecg(void) {
  ret::sdk_status status;
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_stop_status;
  switch (get_device_id()) {
    case ret::device_identifiers::SENSORBOARD3:
      status = this->ad5940_app.stop();
      if (status == ret::SDK_OK) {
        stream_stop_status = this->ecg_app.ecg_stream.stop();
      } else {
        return ret::AD5940_STOP_FAILED;
      }
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
          stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
        return ret::STREAM_STOP_FAILED;
      } else {
        status = this->fs_app.fs_unsubscribe(fs_application::ECG_Stream);
      }
      if (status == ret::SDK_OK) {
        stream_stop_status = this->fs_app.fs_stream.stop();
      } else {
        return ret::FS_UNSUBSCRIBE_FAILED;
      }
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_OK) {
        return ret::LOGGING_STOPPED;
      } else {
        if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
          return ret::FILE_NOT_CLOSED;
        } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
          return ret::STREAM_IN_PROGRESS;
        } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
          return ret::FILE_NOT_OPENED;
        }
      }
    case ret::device_identifiers::ADPD107WATCH:
    case ret::device_identifiers::ADPD107CHESTSTRAP:
      stream_stop_status = this->ecg_app.ecg_stream.stop();
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
          stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
        return ret::STREAM_STOP_FAILED;
      } else {
        status = this->ad7689_app.stop();
      }
      if (status == ret::SDK_OK) {
        status = this->fs_app.fs_unsubscribe(fs_application::ECG_Stream);
      } else {
        return ret::AD7689_STOP_FAILED;
      }
      if (status == ret::SDK_OK) {
        stream_stop_status = this->fs_app.fs_stream.stop();
      } else {
        return ret::FS_UNSUBSCRIBE_FAILED;
      }
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_OK) {
        return ret::LOGGING_STOPPED;
      } else {
        if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
          return ret::FILE_NOT_CLOSED;
        } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
          return ret::STREAM_IN_PROGRESS;
        } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
          return ret::FILE_NOT_OPENED;
        }
      }
    case ret::device_identifiers::ADPD185WATCH:
    case ret::device_identifiers::ADPD188WATCH:
      stream_stop_status = this->ecg_app.ecg_stream.stop();
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
          stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
        return ret::STREAM_STOP_FAILED;
      } else {
        stream_stop_status = this->adpd_app.adpd_stream.stop();
      }
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
          stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
        return ret::STREAM_STOP_FAILED;
      } else {
        status = this->fs_app.fs_unsubscribe(fs_application::ECG_Stream);
      }
      if (status == ret::SDK_OK) {
        stream_stop_status = this->fs_app.fs_stream.stop();
      } else {
        return ret::FS_UNSUBSCRIBE_FAILED;
      }
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_OK) {
        return ret::LOGGING_STOPPED;
      } else {
        if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
          return ret::FILE_NOT_CLOSED;
        } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
          return ret::STREAM_IN_PROGRESS;
        } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
          return ret::FILE_NOT_OPENED;
        }
      }
      if (status == ret::SDK_OK) {
        return ret::LOGGING_STOPPED;
      } else {
        return ret::FS_UNSUBSCRIBE_FAILED;
      }
    default:
      std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
      return ret::UNKNOWN_DEVICE;
  }
}

/*!
 \brief Start the Logging of temperature stream.
 */
ret::err_status watch::start_log_temperature(void) {
  ret::sdk_status status;
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_start_status;
  ret::device_identifiers device_id = get_device_id();
  if ((device_id == ret::device_identifiers::ADPD107WATCH) || (device_id == ret::device_identifiers::SENSORBOARD3)) {
	  status = this->fs_app.fs_subscribe(fs_application::TEMPERATURE_Stream);
    if (status == ret::SDK_OK) {
      stream_start_status = this->fs_app.fs_stream.start();
    } else {
      return ret::FS_SUBSCRIBE_FAILED;
    }
    if (stream_start_status == m2m2_application::APP_COMMON_STATUS_OK ||
        stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
      stream_start_status = this->temperature_app.temperature_stream.start();
    } else {
      if (stream_start_status == m2m2_application::FILE_SYS_STATUS_ERROR) {
        return ret::FS_STATUS_ERROR;
      } else if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
        return ret::APP_COMMON_ERROR;
      } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_INVALID) {
        return ret::FS_ERR_INVALID;
      } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_CONFIG_FILE_POSITION) {
        return ret::FS_ERR_CONFIG_FILE_POSITION;
      } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_MAX_FILE_COUNT) {
        return ret::FS_ERR_MAX_FILE_COUNT;
      }
    }
    if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
        stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
      return ret::STREAM_START_FAILED;
    } else if(device_id == ret::device_identifiers::ADPD107WATCH) {
      status = this->ad7689_app.start();
    }
	else
	{
		return ret::LOGGING_STARTED;
	}
    if (status == ret::SDK_OK) {
      return ret::LOGGING_STARTED;
    } else {
      return ret::AD7689_START_FAILED;
    }
  } else {
    std::cerr << "ADI SDK:: Error! Temperature not supported for this Device Id: " << std::endl;
    return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
  }
}

/*!
 \brief Stop the logging of temperature stream.
 */
ret::err_status watch::stop_log_temperature(void) {
  ret::sdk_status status;
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_stop_status;
  ret::device_identifiers device_id = get_device_id();
  if ((device_id == ret::device_identifiers::ADPD107WATCH) || (device_id == ret::device_identifiers::SENSORBOARD3)) {
	  stream_stop_status = this->temperature_app.temperature_stream.stop();
    if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
        stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
      return ret::STREAM_STOP_FAILED;
    } else if (device_id == ret::device_identifiers::ADPD107WATCH){
      status = this->ad7689_app.stop();
    } else
	{
		//For sensor board 3, AD7689 is not required to stop temperature
		status = ret::SDK_OK;
	}
    if (status == ret::SDK_OK) {
      status = this->fs_app.fs_unsubscribe(fs_application::TEMPERATURE_Stream);
    }
    if (status == ret::SDK_OK) {
      stream_stop_status = this->fs_app.fs_stream.stop();
    } else {
      return ret::FS_UNSUBSCRIBE_FAILED;
    }
    if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_OK) {
      return ret::LOGGING_STOPPED;
    } else {
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
        return ret::FILE_NOT_CLOSED;
      } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
        return ret::STREAM_IN_PROGRESS;
      } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
        return ret::FILE_NOT_OPENED;
      }
    }
  } else {
    std::cerr << "ADI SDK:: Error! Temperature not supported for this Device Id: " << std::endl;
    return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
  }
  return ret::LOGGING_STOPPED;

}

/**
 * Start the Logging of PPG, sync-adpd-adxl and AGC stream.
 * @param enableAGC set true to start AGC log
 * @return returns the status
 */
ret::err_status watch::start_log_syncppg(void) {
  ret::sdk_status status;
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_start_status;
  switch (get_device_id()) {
    case ret::device_identifiers::ADPD107WATCH:
    case ret::device_identifiers::SENSORBOARD3:
      status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_107);
      if (status == ret::SDK_OK) {
        status = this->ppg_app.load_lcfg(ppg_application::PPG_LCFG_ID_ADPD107);
      } else {
        return ret::LOAD_CFG_FAILED;
      }
      break;
    case ret::device_identifiers::ADPD185WATCH:
      status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_185);
      if (status == ret::SDK_OK) {
        status = this->ppg_app.load_lcfg(ppg_application::PPG_LCFG_ID_ADPD185);
      } else {
        return ret::LOAD_CFG_FAILED;
      }
      break;
    case ret::device_identifiers::ADPD188WATCH:
      status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_188);
      if (status == ret::SDK_OK) {
        status = this->ppg_app.load_lcfg(ppg_application::PPG_LCFG_ID_ADPD188);
      } else {
        return ret::LOAD_CFG_FAILED;
      }
      break;
    case ret::device_identifiers::ADPD107CHESTSTRAP:
      std::cerr << "ADI SDK:: Error! PPG not supported for ADPD107CHESTSTRAP: " << std::endl;
      return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
    default:
      std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
      return ret::UNKNOWN_DEVICE;
  }
  if (status == ret::SDK_OK) {
    status = this->adpd_app.calibrate_clock();
  } else {
    return ret::PPG_LOAD_LCFG_FAILED;
  }
//  if (status == ret::SDK_OK) {
//    if (enableAGC) {
//      std::vector<uint8_t> lcfg_addresses{3};
//      std::vector<std::pair<uint8_t, uint32_t>> ret_vals = this->ppg_app.lcfg_read(lcfg_addresses);
//      uint8_t address;
//      uint32_t prev_value;
//      for (int i = 0; i < ret_vals.size(); i++) {
//        address = ret_vals[i].first;
//        prev_value = ret_vals[i].second;
//      }
//      uint32_t new_value = set_n_th_bit(prev_value, 7, 0);
////      printf("prev_value: %d, new_value: %d", prev_value, new_value);
//      for (int i = 0; i < ret_vals.size(); i++) {
//        ret_vals[i].first = 3;
//        ret_vals[i].second = new_value;
//      }
//      this->ppg_app.lcfg_write(ret_vals);
//    }
//  } else {
//    return ret::CLOCK_CALIBRATE_FAILED;
//  }
  if (status == ret::SDK_OK) {
    status = this->fs_app.fs_subscribe(fs_application::PPG_Stream);
  } else {
    return ret::CLOCK_CALIBRATE_FAILED;
  }
  if (status == ret::SDK_OK) {
    status = this->fs_app.fs_subscribe(fs_application::SYNCPPG_Stream);
  } else {
    return ret::FS_SUBSCRIBE_FAILED;
  }
  /*if (enableAGC) {
    if (status == ret::SDK_OK) {
      status = this->fs_app.fs_subscribe(fs_application::AGC_Stream);
    } else {
      return ret::FS_SUBSCRIBE_FAILED;
    }
  }*/
  if (status == ret::SDK_OK) {
    stream_start_status = this->fs_app.fs_stream.start();
  } else {
    return ret::FS_SUBSCRIBE_FAILED;
  }
  if (stream_start_status == m2m2_application::APP_COMMON_STATUS_OK ||
      stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
    stream_start_status = this->adpd_app.adpd_stream.start();
  } else {
    if (stream_start_status == m2m2_application::FILE_SYS_STATUS_ERROR) {
      return ret::FS_STATUS_ERROR;
    } else if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
      return ret::APP_COMMON_ERROR;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_INVALID) {
      return ret::FS_ERR_INVALID;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_CONFIG_FILE_POSITION) {
      return ret::FS_ERR_CONFIG_FILE_POSITION;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_MAX_FILE_COUNT) {
      return ret::FS_ERR_MAX_FILE_COUNT;
    }
  }
  if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
      stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
    return ret::STREAM_START_FAILED;
  } else {
    stream_start_status = this->adxl_app.adxl_stream.start();
  }
  if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
      stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
    return ret::STREAM_START_FAILED;
  } else {
    stream_start_status = this->ppg_app.ppg_stream.start();
  }
  if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
      stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
    return ret::STREAM_START_FAILED;
  } else {
    return ret::LOGGING_STARTED;
  }
}

/**
 * Stop the Logging of PPG, sync-adpd-adxl and AGC stream.
 * @param disableAGC set true to stop AGC log
 * @return returns the status
 */
ret::err_status watch::stop_log_syncppg(void) {
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_stop_status;
  ret::sdk_status unsub_status;
  switch (get_device_id()) {
    case ret::device_identifiers::ADPD107WATCH:
    case ret::device_identifiers::SENSORBOARD3:
    case ret::device_identifiers::ADPD185WATCH:
    case ret::device_identifiers::ADPD188WATCH:
      stream_stop_status = this->ppg_app.ppg_stream.stop();
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
          stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
        return ret::STREAM_STOP_FAILED;
      } else {
        stream_stop_status = this->adpd_app.adpd_stream.stop();
      }
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
          stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
        return ret::STREAM_STOP_FAILED;
      } else {
        stream_stop_status = this->adxl_app.adxl_stream.stop();
      }
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
          stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
        return ret::STREAM_STOP_FAILED;
      } else {
        break;
      }
    case ret::device_identifiers::ADPD107CHESTSTRAP:
      std::cerr << "ADI SDK:: Error! PPG not supported for ADPD107CHESTSTRAP: " << std::endl;
      return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
    default:
      std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
      return ret::UNKNOWN_DEVICE;
  }
  unsub_status = this->fs_app.fs_unsubscribe(fs_application::PPG_Stream);
  if (unsub_status == ret::SDK_OK) {
    unsub_status = this->fs_app.fs_unsubscribe(fs_application::SYNCPPG_Stream);
  } else {
    return ret::FS_UNSUBSCRIBE_FAILED;
  }
 /* if (disableAGC) {
    if (unsub_status == ret::SDK_OK) {
      unsub_status = this->fs_app.fs_unsubscribe(fs_application::AGC_Stream);
    } else {
      return ret::FS_UNSUBSCRIBE_FAILED;
    }
  }*/
  if (unsub_status == ret::SDK_OK) {
    stream_stop_status = this->fs_app.fs_stream.stop();
  } else {
    return ret::FS_UNSUBSCRIBE_FAILED;
  }
  if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_OK) {
    return ret::LOGGING_STOPPED;
  } else {
    if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
      return ret::FILE_NOT_CLOSED;
    } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
      return ret::STREAM_IN_PROGRESS;
    } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
      return ret::FILE_NOT_OPENED;
    }
  }
  return ret::LOGGING_STOPPED;

}

/*!
 \brief Start the logging of ADPD stream.
 */
ret::err_status watch::start_log_adpd(void) {
  ret::sdk_status status;
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_start_status;
  switch (get_device_id()) {
    case ret::device_identifiers::ADPD107WATCH:
    case ret::device_identifiers::SENSORBOARD3:
      status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_107);
      break;
    case ret::device_identifiers::ADPD185WATCH:
      status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_185);
      break;
    case ret::device_identifiers::ADPD188WATCH:
      status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_188);
      break;
    case ret::device_identifiers::ADPD107CHESTSTRAP:
      std::cerr << "ADI SDK:: Error! ADPD not supported for ADPD107CHESTSTRAP: " << std::endl;
      return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
    default:
      std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
      return ret::UNKNOWN_DEVICE;
  }
  if (status == ret::SDK_OK) {
    status = this->adpd_app.calibrate_clock();
  } else {
    return ret::LOAD_CFG_FAILED;
  }
  if (status == ret::SDK_OK) {
    status = this->adpd_app.set_slot(adpd_application::SLOTMODE_DISABLED,
                                     adpd_application::SLOTMODE_SUM32b);
  } else {
    return ret::CLOCK_CALIBRATE_FAILED;
  }
  if (status == ret::SDK_OK) {
    status = this->fs_app.fs_subscribe(fs_application::ADPD_Stream);
  } else {
    return ret::SET_SLOT_FAILED;
  }
  if (status == ret::SDK_OK) {
    stream_start_status = this->fs_app.fs_stream.start();
  } else {
    return ret::FS_SUBSCRIBE_FAILED;
  }
  if (stream_start_status == m2m2_application::APP_COMMON_STATUS_OK ||
      stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
    stream_start_status = this->adpd_app.adpd_stream.start();
  } else {
    if (stream_start_status == m2m2_application::FILE_SYS_STATUS_ERROR) {
      return ret::FS_STATUS_ERROR;
    } else if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
      return ret::APP_COMMON_ERROR;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_INVALID) {
      return ret::FS_ERR_INVALID;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_CONFIG_FILE_POSITION) {
      return ret::FS_ERR_CONFIG_FILE_POSITION;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_MAX_FILE_COUNT) {
      return ret::FS_ERR_MAX_FILE_COUNT;
    }
  }
  if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
      stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
    return ret::STREAM_START_FAILED;
  } else {
    return ret::LOGGING_STARTED;
  }
}

/*!
 \brief Stop the logging of ADPD stream.
 */
ret::err_status watch::stop_log_adpd(void) {
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_stop_status;
  ret::sdk_status unsub_status;
  switch (get_device_id()) {
    case ret::device_identifiers::ADPD107WATCH:
    case ret::device_identifiers::SENSORBOARD3:
    case ret::device_identifiers::ADPD185WATCH:
    case ret::device_identifiers::ADPD188WATCH:
      stream_stop_status = this->adpd_app.adpd_stream.stop();
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
          stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
        return ret::STREAM_STOP_FAILED;
      } else {
        unsub_status = this->fs_app.fs_unsubscribe(fs_application::ADPD_Stream);
      }
      if (unsub_status == ret::SDK_OK) {
        stream_stop_status = this->fs_app.fs_stream.stop();
      } else {
        return ret::FS_UNSUBSCRIBE_FAILED;
      }
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_OK) {
        return ret::LOGGING_STOPPED;
      } else {
        if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
          return ret::FILE_NOT_CLOSED;
        } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
          return ret::STREAM_IN_PROGRESS;
        } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
          return ret::FILE_NOT_OPENED;
        }
      }
    case ret::device_identifiers::ADPD107CHESTSTRAP:
      std::cerr << "ADI SDK:: Error! ADPD not supported for ADPD107CHESTSTRAP: " << std::endl;
      return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
    default:
      std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
      return ret::UNKNOWN_DEVICE;
  }
}

/*!
 \brief Start the logging of pedometer stream.
 */
ret::err_status watch::start_log_pedometer(void) {
  ret::sdk_status status;
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_start_status;
  status = this->fs_app.fs_subscribe(fs_application::PEDOMETER_Stream);
  if (status == ret::SDK_OK) {
    stream_start_status = this->fs_app.fs_stream.start();
  }
  if (stream_start_status == m2m2_application::APP_COMMON_STATUS_OK ||
      stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
    stream_start_status = this->adxl_app.adxl_stream.start();
  } else {
    if (stream_start_status == m2m2_application::FILE_SYS_STATUS_ERROR) {
      return ret::FS_STATUS_ERROR;
    } else if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
      return ret::APP_COMMON_ERROR;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_INVALID) {
      return ret::FS_ERR_INVALID;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_CONFIG_FILE_POSITION) {
      return ret::FS_ERR_CONFIG_FILE_POSITION;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_MAX_FILE_COUNT) {
      return ret::FS_ERR_MAX_FILE_COUNT;
    }
  }
  if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
      stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
    return ret::STREAM_START_FAILED;
  } else {
    stream_start_status = this->ped_app.pedometer_stream.start();
  }
  if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
      stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
    return ret::STREAM_START_FAILED;
  } else {
    return ret::LOGGING_STARTED;
  }

}

/*!
 \brief Stop the logging of pedometer stream.
 */
ret::err_status watch::stop_log_pedometer(void) {
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_stop_status;
  ret::sdk_status unsub_status;
  stream_stop_status = this->ped_app.pedometer_stream.stop();
  if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
      stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
    return ret::STREAM_STOP_FAILED;
  } else {
    stream_stop_status = this->adxl_app.adxl_stream.stop();
  }
  if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
      stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
    return ret::STREAM_STOP_FAILED;
  } else {
    unsub_status = this->fs_app.fs_unsubscribe(fs_application::PEDOMETER_Stream);
  }
  if (unsub_status == ret::SDK_OK) {
    stream_stop_status = this->fs_app.fs_stream.stop();
  } else {
    return ret::FS_UNSUBSCRIBE_FAILED;
  }
  if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_OK) {
    return ret::LOGGING_STOPPED;
  } else {
    if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
      return ret::FILE_NOT_CLOSED;
    } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
      return ret::STREAM_IN_PROGRESS;
    } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
      return ret::FILE_NOT_OPENED;
    }
  }
  return ret::LOGGING_STOPPED;

}

/*!
 \brief Start the logging of battery stream.
 */
ret::err_status watch::start_log_battery(void) {
  ret::sdk_status status;
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_start_status;
  status = this->fs_app.fs_subscribe(fs_application::BATTERY_Stream);
  if (status == ret::SDK_OK) {
    stream_start_status = this->fs_app.fs_stream.start();
  } else {
    return ret::FS_SUBSCRIBE_FAILED;
  }
  if (stream_start_status == m2m2_application::APP_COMMON_STATUS_OK ||
      stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
    return ret::LOGGING_STARTED;
  } else {
    if (stream_start_status == m2m2_application::FILE_SYS_STATUS_ERROR) {
      return ret::FS_STATUS_ERROR;
    } else if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
      return ret::APP_COMMON_ERROR;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_INVALID) {
      return ret::FS_ERR_INVALID;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_CONFIG_FILE_POSITION) {
      return ret::FS_ERR_CONFIG_FILE_POSITION;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_MAX_FILE_COUNT) {
      return ret::FS_ERR_MAX_FILE_COUNT;
    }
  }
  return ret::LOGGING_STARTED;

}

/*!
 \brief Stop the logging of battery stream.
 */
ret::err_status watch::stop_log_battery(void) {
  ret::sdk_status status;
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_stop_status;
  status = this->fs_app.fs_unsubscribe(fs_application::BATTERY_Stream);
  if (status == ret::SDK_OK) {
    stream_stop_status = this->fs_app.fs_stream.stop();
  }
  if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_OK) {
    return ret::LOGGING_STOPPED;
  } else {
    if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
      return ret::FILE_NOT_CLOSED;
    } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
      return ret::STREAM_IN_PROGRESS;
    } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
      return ret::FILE_NOT_OPENED;
    }
  }
  return ret::LOGGING_STOPPED;

}

/*!
 \brief Start the logging of BCM stream.
 */
ret::err_status watch::start_log_bcm(void) {
  ret::sdk_status status;
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_start_status;
  if (get_device_id() == ret::device_identifiers::SENSORBOARD3) {
    status = this->ad5940_app.set_wg_freq(50000);
    if (status == ret::SDK_OK) {
      status = this->fs_app.fs_subscribe(fs_application::BCM_Stream);
    } else {
      return ret::SET_FREQ_FAILED;
    }
    if (status == ret::SDK_OK) {
      stream_start_status = this->fs_app.fs_stream.start();
    } else {
      return ret::FS_SUBSCRIBE_FAILED;
    }
    if (stream_start_status == m2m2_application::APP_COMMON_STATUS_OK ||
        stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
      status = this->ad5940_app.start();
    } else {
      if (stream_start_status == m2m2_application::FILE_SYS_STATUS_ERROR) {
        return ret::FS_STATUS_ERROR;
      } else if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
        return ret::APP_COMMON_ERROR;
      } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_INVALID) {
        return ret::FS_ERR_INVALID;
      } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_CONFIG_FILE_POSITION) {
        return ret::FS_ERR_CONFIG_FILE_POSITION;
      } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_MAX_FILE_COUNT) {
        return ret::FS_ERR_MAX_FILE_COUNT;
      }
    }
    if (status == ret::SDK_OK) {
      stream_start_status = this->bcm_app.bcm_stream.start();
    } else {
      return ret::AD5940_START_FAILED;
    }
    if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
        stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
      return ret::STREAM_START_FAILED;
    } else {
      return ret::LOGGING_STARTED;
    }
  } else {
    std::cerr << "ADI SDK:: Error! BCM not supported for Device Id: " << std::endl;
    return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
  }
}

/*!
 \brief Stop the logging of BCM stream.
 */
ret::err_status watch::stop_log_bcm(void) {
  ret::sdk_status status;
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_stop_status;
  if (get_device_id() == ret::device_identifiers::SENSORBOARD3) {
    stream_stop_status = this->bcm_app.bcm_stream.stop();
    if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
        stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
      return ret::STREAM_STOP_FAILED;
    } else {
      status = this->ad5940_app.stop();
    }
    if (status == ret::SDK_OK) {
      status = this->fs_app.fs_unsubscribe(fs_application::BCM_Stream);
    } else {
      return ret::AD5940_STOP_FAILED;
    }
    if (status == ret::SDK_OK) {
      stream_stop_status = this->fs_app.fs_stream.stop();
    } else {
      return ret::FS_UNSUBSCRIBE_FAILED;
    }
    if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_OK) {
      return ret::LOGGING_STOPPED;
    } else {
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
        return ret::FILE_NOT_CLOSED;
      } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
        return ret::STREAM_IN_PROGRESS;
      } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
        return ret::FILE_NOT_OPENED;
      }
    }
  } else {
    std::cerr << "ADI SDK:: Error! BCM not supported for Device Id: " << std::endl;
    return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
  }
}


/*!
 \brief Start the logging of HRV stream.
 */
ret::err_status watch::start_log_hrv(void) {
  ret::sdk_status status;
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_start_status;
  switch (get_device_id()) {
    case ret::device_identifiers::ADPD107WATCH:
    case ret::device_identifiers::SENSORBOARD3:
      status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_107);
      if (status == ret::SDK_OK) {
        status = this->ppg_app.load_lcfg(ppg_application::PPG_LCFG_ID_ADPD107);
      } else {
        return ret::LOAD_CFG_FAILED;
      }
      break;
    case ret::device_identifiers::ADPD185WATCH:
      status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_185);
      if (status == ret::SDK_OK) {
        status = this->ppg_app.load_lcfg(ppg_application::PPG_LCFG_ID_ADPD185);
      } else {
        return ret::LOAD_CFG_FAILED;
      }
      break;
    case ret::device_identifiers::ADPD188WATCH:
      status = this->adpd_app.load_cfg(adpd_application::ADPD_DEVICE_188);
      if (status == ret::SDK_OK) {
        status = this->ppg_app.load_lcfg(ppg_application::PPG_LCFG_ID_ADPD188);
      } else {
        return ret::LOAD_CFG_FAILED;
      }
      break;
    case ret::device_identifiers::ADPD107CHESTSTRAP:
      std::cerr << "ADI SDK:: Error! HRV not supported for ADPD107CHESTSTRAP: " << std::endl;
      return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
    default:
      std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
      return ret::UNKNOWN_DEVICE;
  }
  if (status == ret::SDK_OK) {
    status = this->adpd_app.calibrate_clock();
  } else {
    return ret::PPG_LOAD_LCFG_FAILED;
  }
  if (status == ret::SDK_OK) {
    status = this->fs_app.fs_subscribe(fs_application::HRV_Stream);
  } else {
    return ret::CLOCK_CALIBRATE_FAILED;
  }
  if (status == ret::SDK_OK) {
    stream_start_status = this->fs_app.fs_stream.start();
  } else {
    return ret::CLOCK_CALIBRATE_FAILED;
  }
  if (stream_start_status == m2m2_application::APP_COMMON_STATUS_OK ||
      stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
    stream_start_status = this->adpd_app.adpd_stream.start();
  } else {
    if (stream_start_status == m2m2_application::FILE_SYS_STATUS_ERROR) {
      return ret::FS_STATUS_ERROR;
    } else if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
      return ret::APP_COMMON_ERROR;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_INVALID) {
      return ret::FS_ERR_INVALID;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_CONFIG_FILE_POSITION) {
      return ret::FS_ERR_CONFIG_FILE_POSITION;
    } else if (stream_start_status == m2m2_application::FILE_SYS_ERR_MAX_FILE_COUNT) {
      return ret::FS_ERR_MAX_FILE_COUNT;
    }
  }
  if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
      stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
    return ret::STREAM_START_FAILED;
  } else {
    stream_start_status = this->adxl_app.adxl_stream.start();
  }
  if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
      stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
    return ret::STREAM_START_FAILED;
  } else {
    stream_start_status = this->ppg_app.ppg_stream.start();
  }
  if (stream_start_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
      stream_start_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
    return ret::STREAM_START_FAILED;
  } else {
    return ret::LOGGING_STARTED;
  }
}

/*!
 \brief Stop the logging of HRV stream.
 */
ret::err_status watch::stop_log_hrv(void) {
  ret::sdk_status status;
  m2m2_application::APP_COMMON_STATUS_ENUM_t stream_stop_status;
  switch (get_device_id()) {
    case ret::device_identifiers::ADPD107WATCH:
    case ret::device_identifiers::SENSORBOARD3:
    case ret::device_identifiers::ADPD185WATCH:
    case ret::device_identifiers::ADPD188WATCH:
      stream_stop_status = this->adpd_app.adpd_stream.stop();
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
          stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
        return ret::STREAM_STOP_FAILED;
      } else {
        stream_stop_status = this->adxl_app.adxl_stream.stop();
      }
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
          stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
        return ret::STREAM_STOP_FAILED;
      } else {
        stream_stop_status = this->ppg_app.ppg_stream.stop();
      }
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR ||
          stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STOPPED) {
        return ret::STREAM_STOP_FAILED;
      } else {
        status = this->fs_app.fs_unsubscribe(fs_application::HRV_Stream);
      }
      if (status == ret::SDK_OK) {
        stream_stop_status = this->fs_app.fs_stream.stop();
      } else {
        return ret::FS_UNSUBSCRIBE_FAILED;
      }
      if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_OK) {
        return ret::LOGGING_STOPPED;
      } else {
        if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_ERROR) {
          return ret::FILE_NOT_CLOSED;
        } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_IN_PROGRESS) {
          return ret::STREAM_IN_PROGRESS;
        } else if (stream_stop_status == m2m2_application::APP_COMMON_STATUS_STREAM_NOT_STARTED) {
          return ret::FILE_NOT_OPENED;
        }
      }
    case ret::device_identifiers::ADPD107CHESTSTRAP:
      std::cerr << "ADI SDK:: Error! PPG not supported for ADPD107CHESTSTRAP: " << std::endl;
      return ret::OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE;
    default:
      std::cerr << "ADI SDK:: Error! Unknown Device Id: " << std::endl;
      return ret::UNKNOWN_DEVICE;
  }
}

/*!
   \brief writing the buffer into the config file.
   Following commands and status are checked in push_config_file.
	 1.Check whether the file count reach to 64  (for reference see fs_get_file_count())
	 2.check whether file is open or not (for reference see fs_config_log(true))
	 3.check whether buffer written to config file or not (for reference see fs_config_write())
	 4.check whether file is closed or not  (for reference see fs_config_log(false))
*/
uint8_t watch::push_config_file(std::vector<uint8_t> buffer) {

  uint16_t fileCount = 0; // The fileCount of the file system
  uint8_t status = 0;

  this->fs_app.fs_get_file_count(fileCount);
  if (fileCount != MAXIMUM_FILE) {
    status = this->fs_app.fs_config_log(true);
  } else {
    return M2M2_LOW_TOUCH_STATUS_ENUM_t::FILE_SYS_ERR_MAX_FILE_COUNT;
  }

  if (status == M2M2_FILE_SYS_STATUS_OK) {
    status = this->fs_app.fs_config_write(buffer);
  } else {
    return M2M2_LOW_TOUCH_STATUS_ENUM_t::M2M2_FILE_SYS_STATUS_ERROR;
  }

  if (status == M2M2_APP_COMMON_STATUS_OK) {
    status = this->fs_app.fs_config_log(false);
    if (status == M2M2_FILE_SYS_STATUS_OK) {
      return M2M2_LOW_TOUCH_STATUS_ENUM_t::M2M2_FILE_SYS_STATUS_OK;
    } else {
      return status;
    }
  } else {
    return M2M2_LOW_TOUCH_STATUS_ENUM_t::M2M2_FILE_SYS_STATUS_ERROR;
  }
}

// function to set the n'th bit -- uint8_t bit should be either 1 or 0
uint32_t watch::set_n_th_bit(uint32_t val, uint32_t k, uint8_t bit) {
  // n'th bit of 'val' is being set as "bit" by this operation
  return ((bit << k) | val);
}