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
#pragma once

#include <iostream>
#include <functional>
#include "callback.hpp"
#include "m2m2_application.hpp"
#include "ecg_application.hpp"
#include "adxl_application.hpp"
#include "temperature_application.hpp"
#include "eda_application.hpp"
#include "ppg_application.hpp"
#include "syncppg_application.hpp"
#include "pm_application.hpp"
#include "fs_application.hpp"
#include "pedometer_application.hpp"
#include "bcm_application.hpp"
#include "m2m2/cpp/m2m2_core.hpp"
#include "adpd4000_application.hpp"
#include "display_application.hpp"
#include "sqi_application.hpp"
#include "lowTouch_application.hpp"
#include "ad7156_application.hpp"

/*!
   \brief The top-level watch class that encapsulates all of the SDK applications and operations.
*/
class watch {
private:
  static const std::string git_hash;
  static const std::string git_describe;
  watch_phy_callback &pkt_phy_writer;
  void splitAndDispatch(std::vector<uint8_t> bytes);

public:

enum M2M2_LOW_TOUCH_STATUS_ENUM_t : uint8_t {
	M2M2_FILE_SYS_STATUS_OK = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_STATUS_OK,
	M2M2_FILE_SYS_STATUS_ERROR = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_STATUS_ERROR,
    PM_SYS_STATUS_OK = M2M2_PM_SYS_STATUS_ENUM_t::M2M2_PM_SYS_STATUS_OK,
    PM_SYS_STATUS_ERROR = M2M2_PM_SYS_STATUS_ENUM_t::M2M2_PM_SYS_STATUS_ERR_ARGS,
    FILE_SYS_CONFIG_FILE_FOUND = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_CONFIG_FILE_FOUND,
    FILE_SYS_CONFIG_FILE_NOT_FOUND = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_CONFIG_FILE_NOT_FOUND,
    FILE_SYS_STREAM_NOT_STARTED =M2M2_APP_COMMON_STATUS_ENUM_t::M2M2_APP_COMMON_STATUS_STREAM_NOT_STARTED,
    FILE_SYS_STREAM_IN_PROGRESS =M2M2_APP_COMMON_STATUS_ENUM_t::M2M2_APP_COMMON_STATUS_STREAM_IN_PROGRESS,
    FILE_SYS_ERR_CRITICAL_BATTERY_LEVEL = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_STATUS_ERROR,
    FILE_SYS_ERR_MEMORY_FULL=M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_ERR_MEMORY_FULL,
    FILE_SYS_ERR_MAX_FILE_COUNT = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_ERR_MAX_FILE_COUNT,
    PM_SYS_STATUS_ENABLE_USER_CONFIG_LOG_FAILED = M2M2_PM_SYS_STATUS_ENUM_t::M2M2_PM_SYS_STATUS_ENABLE_USER_CONFIG_LOG_FAILED,
    PM_SYS_STATUS_USER_CONFIG_LOG_ENABLED = M2M2_PM_SYS_STATUS_ENUM_t::M2M2_PM_SYS_STATUS_USER_CONFIG_LOG_ENABLED,
} M2M2_LOW_TOUCH_STATUS_ENUM_t;



  static const std::string SDK_version;
	bool is_user_cmd = false;

	bool is_fs_refhr_cmd = false;
  enum platforms : uint8_t {
    python_usb = 0, windows = 1, android = 2, ios = 3, python_ble = 4
  } platform;
  ret::device_identifiers getDeviceID(void);


  watch(watch_phy_callback &cb);
  void self_test(void);
  void set_platform(platforms platForm);
  void initialize_sdk(platforms platForm);
  void dispatch(std::vector<uint8_t> bytes);

  void dispatch_fs(std::vector<uint8_t> bytes);

  void dispatch_fs_system_alert(std::vector<uint8_t> bytes);

  void dispatch_pm(std::vector<uint8_t> bytes);
  void write_bytes(std::vector<uint8_t> &bytes);
  std::string get_version(void);
  std::string getMacAddress(void);
  M2M2_ADDR_ENUM_t get_address(void);

  M2M2_ADDR_ENUM_t get_stream_address(void);

 
  adxl_application adxl_app;
  eda_application eda_app;
  ecg_application ecg_app;
  temperature_application temperature_app;
  ppg_application ppg_app;
  syncppg_application syncppg_app;
  adpd4000_application adpd4000_app;
  pm_application pm_app;
  fs_application fs_app;
  pedometer_application ped_app;
  bcm_application bcm_app;
  display_application display_app;
  sqi_application sqi_app;
  lowTouch_application lowTouch_app;
  ad7156_application ad7156_app;


  ret::err_status start_syncppg(ppg_stream_callback &ppg_stream_cb, syncppg_stream_callback &sync_stream_cb);

  ret::err_status stop_syncppg(void);

  //ret::err_status start_adpd(adpd_stream_callback &stream_cb);

  //ret::err_status stop_adpd(void);

  ret::err_status start_adxl(adxl_stream_callback &stream_cb);

  ret::err_status stop_adxl(void);


  ret::err_status start_adpd4000(adpd4000_stream_callback &stream_cb);
  ret::err_status stop_adpd4000(void);



  ret::err_status start_adpd4000_g(adpd4000_stream_callback &stream_cb);
  ret::err_status stop_adpd4000_g(void);


  ret::err_status start_adpd4000_r(adpd4000_stream_callback &stream_cb);
  ret::err_status stop_adpd4000_r(void);


  ret::err_status start_adpd4000_b(adpd4000_stream_callback &stream_cb);
  ret::err_status stop_adpd4000_b(void);


  ret::err_status start_adpd4000_ir(adpd4000_stream_callback &stream_cb);
  ret::err_status stop_adpd4000_ir(void);

  ret::err_status start_ecg(ecg_stream_callback &stream_cb);
  ret::err_status stop_ecg(void);

  ret::err_status start_temperature(temperature_stream_callback &stream_cb);

  ret::err_status stop_temperature(void);
  ret::err_status start_eda(eda_stream_callback &stream_cb);
  ret::err_status stop_eda(void);

  ret::err_status start_bcm(bcm_stream_callback &stream_cb);

  ret::err_status stop_bcm(void);

  //ret::err_status start_hrv(hrv_stream_callback &stream_cb);

  //ret::err_status stop_hrv(void);

  ret::err_status start_pedometer(pedometer_stream_callback &stream_cb);

  ret::err_status stop_pedometer(void);

  ret::err_status start_battery(battery_stream_callback &stream_cb);

  ret::err_status stop_battery(void);

  ret::err_status start_log_adxl(void);
  ret::err_status stop_log_adxl(void);

  //ret::err_status start_log_adpd(void);
  //ret::err_status stop_log_adpd(void);

  ret::err_status start_log_syncppg(void);

  ret::err_status stop_log_syncppg(void);

  ret::err_status start_log_ecg(void);

  ret::err_status stop_log_ecg(void);

  ret::err_status start_log_temperature(void);

  ret::err_status stop_log_temperature(void);

  ret::err_status start_log_eda(void);

  ret::err_status stop_log_eda(void);

  ret::err_status start_log_pedometer(void);

  ret::err_status stop_log_pedometer(void);

  ret::err_status start_log_bcm(void);

  ret::err_status stop_log_bcm(void);

  //ret::err_status start_log_hrv(void);

  //ret::err_status stop_log_hrv(void);

  ret::err_status start_log_battery(void);

  ret::err_status stop_log_battery(void);

  ret::err_status start_log_adpd4000_g(void);
  ret::err_status stop_log_adpd4000_g(void);

  ret::err_status start_log_adpd4000_r(void);
  ret::err_status stop_log_adpd4000_r(void);

  ret::err_status start_log_adpd4000_ir(void);
  ret::err_status stop_log_adpd4000_ir(void);

  ret::err_status start_log_adpd4000_b(void);
  ret::err_status stop_log_adpd4000_b(void);


  uint8_t push_config_file(std::vector<uint8_t> buffer);

  uint32_t set_n_th_bit(uint32_t val, uint32_t n, uint8_t bit);
  void system_alert_call(uint8_t alert_message);
};
