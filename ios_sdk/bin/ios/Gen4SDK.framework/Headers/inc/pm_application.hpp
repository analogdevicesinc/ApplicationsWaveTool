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

#include "m2m2_application.hpp"
//#include "m2m2/cpp/pm_application_interface.hpp"
#include "m2m2/cpp/system_interface.hpp"

#include "m2m2_data_stream.hpp"
#include "sdk_err.hpp"
#include "callback.hpp"


struct pm_sys_bat_info_t {
  double timestamp;
  PM_SYS_BAT_STATE_ENUM_t bat_chrg_stat;
  uint8_t bat_lvl;
  uint16_t bat_mv;
  uint16_t bat_temp;
};
struct pm_sys_info_t {
  uint16_t version;
  std::string mac_addr;
  uint32_t device_id;
  uint32_t model_number;
  uint16_t hw_id;
  uint16_t bom_id;
  uint8_t batch_id;
  uint32_t date;
};

struct eeprom_info_t {
	uint32_t manufacture_id;
	uint16_t hw_id;
	uint16_t bom_id;
	uint8_t batch_id;
	uint32_t date;
	uint32_t additional_detail;

};

class pm_application : public m2m2_application {
  friend watch;
private:
  typedef std::vector<unsigned char> bytes;
  std::string BytesToStr(const bytes &in);
public:
  enum PM_SYS_MCU_TYPE_ENUM_t : uint8_t {
    PM_SYS_MCU_INVALID = M2M2_PM_SYS_MCU_TYPE_ENUM_t::M2M2_PM_SYS_MCU_INVALID,
    PM_SYS_MCU_M3 = M2M2_PM_SYS_MCU_TYPE_ENUM_t::M2M2_PM_SYS_MCU_M3,
    PM_SYS_MCU_M4 = M2M2_PM_SYS_MCU_TYPE_ENUM_t::M2M2_PM_SYS_MCU_M4,
  };
  enum PM_SYS_DG2502_SELECT_ENUM_t :uint8_t {
	  PM_SYS_DG2502_8233_SW = M2M2_PM_SYS_DG2502_SELECT_ENUM_t::M2M2_PM_SYS_DG2502_8233_SW,
	  PM_SYS_DG2502_5940_SW = M2M2_PM_SYS_DG2502_SELECT_ENUM_t::M2M2_PM_SYS_DG2502_5940_SW,
	  PM_SYS_DG2502_4K_SW = M2M2_PM_SYS_DG2502_SELECT_ENUM_t::M2M2_PM_SYS_DG2502_4K_SW,
  };

  enum PM_SYS_CHIP_ID_ENUM_t :uint8_t {
	  M2M2_PM_SYS_ADXL362 = M2M2_PM_SYS_CHIP_ID_ENUM_t::M2M2_PM_SYS_ADXL362,
	  M2M2_PM_SYS_ADPD4K = M2M2_PM_SYS_CHIP_ID_ENUM_t::M2M2_PM_SYS_ADPD4K,
	  M2M2_PM_SYS_ADP5360 = M2M2_PM_SYS_CHIP_ID_ENUM_t::M2M2_PM_SYS_ADP5360,
	  M2M2_PM_SYS_AD5940 = M2M2_PM_SYS_CHIP_ID_ENUM_t::M2M2_PM_SYS_AD5940,
	  M2M2_PM_SYS_NAND_FLASH = M2M2_PM_SYS_CHIP_ID_ENUM_t::M2M2_PM_SYS_NAND_FLASH,
	  M2M2_PM_SYS_AD7156 = M2M2_PM_SYS_CHIP_ID_ENUM_t::M2M2_PM_SYS_AD7156,
	  //M2M2_PM_SYS_LCD_DISPLAY = 3,
	  //M2M2_PM_SYS_AD8233 = 3,
  };
  pm_application(watch *sdk = NULL);
  ~pm_application(void);
  m2m2_data_stream<battery_stream_callback> battery_stream;
  m2m2_data_stream<ping_stream_callback> ping_stream;

  std::string get_name(void);
  M2M2_ADDR_ENUM_t get_address(void);
  void get_system_info(pm_sys_info_t* sys_info);
  void get_mcu_version(uint8_t& mcu_type);
  ret::sdk_status set_date_time(sys_date_time_t *time_info);
  void get_date_time(sys_date_time_t *time_info);
  void get_battery_info(pm_sys_bat_info_t* sys_bat_info);
  ret::sdk_status set_battery_threshold(uint8_t bat_low_lvl,
                                        uint8_t bat_critical_lvl);
  ret::sdk_status set_usb_power(bool enable,
                                bool action);
  ret::sdk_status set_power_state(uint8_t state);
  void ping(uint16_t nTimes, uint16_t pktsize, ping_stream_callback &cb);
  ret::sdk_status enable_or_disable_battery_charger(bool enable);
  ret::sdk_status enable_or_disable_thermistor_state_change(bool enable);
  uint8_t get_low_touch_status(void);

  uint8_t enable_config_log(bool enable);

  ret::sdk_status powerboost_m3(uint8_t boostenable, uint8_t OVPValue);
  uint8_t powerboost_m4(uint8_t boostenable);
  uint8_t boost_m4(uint8_t boostenable, uint8_t  Boostvoltage);
  void bluetooth_request();
  ret::sdk_status read_EEPROM(eeprom_info_t *eeprom);
  ret::sdk_status flash_reset(void);
  ret::sdk_status system_reset(void);
  ret::sdk_status system_hardware_reset(void);
  ret::sdk_status dg502_software_control(PM_SYS_DG2502_SELECT_ENUM_t sw_name, bool enable);
  ret::sdk_status ldo_control(uint8_t ldo_num, bool ldo_enable);
  uint16_t get_chipID(PM_SYS_CHIP_ID_ENUM_t chipname);
  ret::sdk_status enter_bootloader(void);
  ret::sdk_status cap_sense_test(bool enable);
  ret::sdk_status setMaxPktCombineCnt(uint8_t count);
  uint16_t getMaxPktCombineCnt(void);
  ret::sdk_status  dcb_delete_config_wrist_detect(void);
  std::vector<uint8_t> query_dcb_blk_status();
  
  bool getHibernateModeStatus(void);
  ret::sdk_status setHibernateModeStatus(bool value);

  std::vector<std::pair<uint16_t, uint16_t>> register_read(std::vector<uint16_t> addresses);
  std::vector<std::pair<uint16_t, uint16_t>> register_write(std::vector<std::pair<uint16_t, uint16_t>> addr_vals);
  
 // void pm_application::write_EEPROM(int bytestream);
};
