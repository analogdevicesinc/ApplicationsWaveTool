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
#include "pm_application.hpp"
#include "watch.hpp"
#define SECONDS_IN_MILLIS 1000.00
#define Debug 0
pm_application::~pm_application(void) {}

pm_application::pm_application(watch * sdk):
    m2m2_application(sdk),
    battery_stream(M2M2_ADDR_SYS_BATT_STREAM, this) {}

/*!
   \brief Fetches a human-readable string describing the application.
*/
std::string pm_application::get_name() {
  return "PM application";
}

/*!
   \brief Fetches the address of the application.
*/
M2M2_ADDR_ENUM_t pm_application::get_address(void) {
  return M2M2_ADDR_SYS_PM;
}


/*!
   \brief get system information such as device id, mac address
*/
void pm_application::get_system_info(pm_sys_info_t* sys_info) {
  m2m2_pkt<m2m2_pm_sys_cmd_t> pkt(this->get_address());
  pkt.payload.command = M2M2_PM_SYS_COMMAND_GET_INFO_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;
  
  auto resp = this->sync_send(pkt.pack());
  
  m2m2_pkt<m2m2_pm_sys_info_t> respkt;
  respkt.unpack(resp);
  bytes bytesaddr;
  for (int i = 0; i < sizeof(respkt.payload.mac_addr); i++) {
    bytesaddr.push_back(respkt.payload.mac_addr[i]);
  }
  
  sys_info->version = respkt.payload.version;
  sys_info->mac_addr = this->bytes_to_string(bytesaddr);
  sys_info->device_id = respkt.payload.device_id;
  sys_info->model_number = respkt.payload.model_number;
  sys_info->hw_id = respkt.payload.hw_id;
  sys_info->bom_id = respkt.payload.bom_id;
  sys_info->batch_id = respkt.payload.batch_id;
  sys_info->date = respkt.payload.date;
}

/*!
   \brief get battery information
*/
void pm_application::get_battery_info(pm_sys_bat_info_t* sys_bat_info) {
  m2m2_pkt<m2m2_pm_sys_cmd_t> pkt(this->get_address());
  pkt.payload.command = M2M2_PM_SYS_COMMAND_GET_BAT_INFO_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;

  auto resp = this->sync_send(pkt.pack());

  m2m2_pkt<m2m2_pm_sys_bat_info_resp_t> respkt;
  respkt.unpack(resp);

  sys_bat_info->timestamp = (double)respkt.payload.timestamp * SECONDS_IN_MILLIS;
  sys_bat_info->bat_chrg_stat = static_cast<PM_SYS_BAT_STATE_ENUM_t>(respkt.payload.bat_chrg_stat);
  sys_bat_info->bat_lvl = respkt.payload.bat_lvl;
  sys_bat_info->bat_mv = respkt.payload.bat_mv;
  sys_bat_info->bat_temp = respkt.payload.bat_temp;
    
}

/*!
   \brief set low level and critical level battery values
*/
ret::sdk_status pm_application::set_battery_threshold(uint8_t bat_low_lvl,
                                                      uint8_t bat_critical_lvl) {
  m2m2_pkt<m2m2_pm_sys_bat_thr_req_t> pkt(this->get_address());
  pkt.payload.command = M2M2_PM_SYS_COMMAND_SET_BAT_THR_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;
  pkt.payload.bat_level_low = bat_low_lvl;
  pkt.payload.bat_level_critical = bat_critical_lvl;
  
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  if (pkt.payload.status != M2M2_PM_SYS_STATUS_OK) {
    return ret::SDK_ERR;
  }
  return ret::SDK_OK;
}

/*!
   \brief Enable/Disable and Get/Set usb power
*/
ret::sdk_status pm_application::set_usb_power(bool enable,
                                              bool action) {
  m2m2_pkt<m2m2_pm_sys_usb_pwr_cmd_t> pkt(this->get_address());
  pkt.payload.command = M2M2_PM_SYS_COMMAND_SET_PWR_STATE_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;
  if (enable == true) {
    pkt.payload.enable = M2M2_PM_SYS_USB_PWR_ENABLE;
  } else {
    pkt.payload.enable = M2M2_PM_SYS_USB_PWR_DISABLE;
  }
  if (action == true) {
    pkt.payload.action = M2M2_PM_SYS_USB_PWR_SET;
  } else {
    pkt.payload.action = M2M2_PM_SYS_USB_PWR_GET;
  }
  
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  if (pkt.payload.status != M2M2_PM_SYS_STATUS_OK) {
    return ret::SDK_ERR;
  }
  return ret::SDK_OK;
}

/*!
   \brief To set power state HIBERNATE or FLEXI or SHUTDOWN
*/
ret::sdk_status pm_application::set_power_state(uint8_t state) {
  m2m2_pkt<m2m2_pm_sys_pwr_state_t> pkt(this->get_address());
  pkt.payload.command = M2M2_PM_SYS_COMMAND_SET_PWR_STATE_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;
  if (state >= 0 && state <= 3) {
    pkt.payload.state = static_cast<M2M2_PM_SYS_PWR_STATE_ENUM_t>(state);
  } else {
    std::cout << "ADI SDK:: Error! Power State Input Value:" << state << "Invalid";
  }
  
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  if (pkt.payload.status != M2M2_PM_SYS_STATUS_OK) {
    return ret::SDK_ERR;
  }
  return ret::SDK_OK;
  
}

/*!
   \brief ping returns whether the application is alive or not
*/
ret::sdk_status pm_application::ping() {
  m2m2_pkt<m2m2_app_common_ping_t> pkt(this->get_address());
  m2m2_pkt<m2m2_app_common_ping_t> respkt(this->get_address());
  
  pkt.payload.command = M2M2_APP_COMMON_CMD_PING_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;
  pkt.payload.sequence_num = 1;
  auto resp = this->sync_send(pkt.pack());
  
  respkt.unpack(resp);
  
  if (respkt.payload.sequence_num != pkt.payload.sequence_num + 1) {
#if Debug
	  std::cout << "ret::SDK_ERR_DEVICE_ERR" << std::endl;
#endif
    return ret::SDK_ERR;
  }
#if Debug
  std::cout << "ret::SDK_OK" << std::endl;
#endif
  return ret::SDK_OK;
}

/*!
   \brief enable or disable battery charger
*/
ret::sdk_status pm_application::enable_or_disable_battery_charger(bool enable) {
  m2m2_pkt<m2m2_pm_sys_cmd_t> pkt(this->get_address());
  if (enable) {
    pkt.payload.command = M2M2_PM_SYS_COMMAND_ENABLE_BAT_CHARGE_REQ;
  } else {
    pkt.payload.command = M2M2_PM_SYS_COMMAND_DISABLE_BAT_CHARGE_REQ;
  }
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;
  
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  if (pkt.payload.status != M2M2_APP_COMMON_STATUS_OK) {
#if Debug
	  std::cout << "ret::SDK_ERR_DEVICE_ERR" << std::endl;
#endif
    return ret::SDK_ERR;
  }
#if Debug
  std::cout << "ret::SDK_OK" << std::endl;
#endif
  return ret::SDK_OK;
}

/*!
   \brief enable or disable thermistor state change
*/
ret::sdk_status pm_application::enable_or_disable_thermistor_state_change(bool enable) {
  m2m2_pkt<m2m2_pm_sys_bat_thermistor_cmd_t> pkt(this->get_address());
  pkt.payload.command = M2M2_PM_SYS_COMMAND_THERMISTOR_STATE_CHANGE_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;
  if (enable) {
    pkt.payload.enable = M2M2_PM_SYS_THERMISTOR_ENABLE;
  } else {
    pkt.payload.enable = M2M2_PM_SYS_THERMISTOR_DISABLE;
  }
  
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  if (pkt.payload.status != M2M2_APP_COMMON_STATUS_OK) {
#if Debug
	  std::cout << "ret::SDK_ERR_DEVICE_ERR" << std::endl;
#endif
    return ret::SDK_ERR;
  }
#if Debug
  std::cout << "ret::SDK_OK" << std::endl;
#endif
  return ret::SDK_OK;
}

/*!
   \brief get mcu version
*/
void pm_application::get_mcu_version(uint8_t& mcu_type) {
  m2m2_pkt<m2m2_pm_sys_cmd_t> pkt(this->get_address());
  pkt.payload.command = M2M2_PM_SYS_COMMAND_GET_MCU_VERSION_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;
  
  auto resp = this->sync_send(pkt.pack());
  
  m2m2_pkt<m2m2_pm_sys_mcu_version_t> respkt;
  respkt.unpack(resp);
  
  mcu_type = (PM_SYS_MCU_TYPE_ENUM_t) respkt.payload.mcu;
#if Debug
  std::cout << "mcu type"<<mcu_type<< std::endl;
#endif
  return;
}
/*!
\brief Bluetooth request 
*/
void pm_application::bluetooth_request()
{
	m2m2_pkt<m2m2_pm_sys_cmd_t> pkt(this->get_address());
	pkt.payload.command = M2M2_PM_SYS_COMMAND_BLUETOOTH_REQ;
	//pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;

	auto resp = this->sync_send(pkt.pack());

	pkt.unpack(resp);
	if (pkt.payload.status == M2M2_APP_COMMON_STATUS_OK) {
#if Debug
		std::cout << "ret::SDK_OK" << std::endl;
#endif
		}
	else 
		{
#if Debug
		std::cout << ret::SDK_ERR << std::endl;
#endif
		}
	}
/*!
   \brief set local time information to PM application
*/
ret::sdk_status pm_application::set_date_time(sys_date_time_t *time_info) {
  
  m2m2_pkt<m2m2_pm_sys_date_time_req_t> pkt(this->get_address());
  pkt.payload.command = M2M2_PM_SYS_COMMAND_SET_DATE_TIME_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;
  
  sys_date_time_t *sys_date_time = &time_info[0];
  
  pkt.payload.year = sys_date_time->year;
  pkt.payload.month = sys_date_time->month;
  pkt.payload.day = sys_date_time->day;
  pkt.payload.hour = sys_date_time->hour;
  pkt.payload.minute = sys_date_time->minute;
  pkt.payload.second = sys_date_time->second;
  pkt.payload.TZ_sec = sys_date_time->TZ_sec;
  
  auto resp = this->sync_send(pkt.pack());
  m2m2_pkt<m2m2_pm_sys_cmd_t> rpkt(this->get_address());
  rpkt.unpack(resp);
  
  if (rpkt.payload.status != M2M2_APP_COMMON_STATUS_OK) {
#if Debug
	  std::cout << "ret::SDK_ERR" << std::endl;
#endif
    return ret::SDK_ERR;
  }
#if Debug
  std::cout << "ret::SDK_OK" << std::endl;
#endif
  return ret::SDK_OK;
}

/*!
   \brief returns date time information from PM application
*/
void pm_application::get_date_time(sys_date_time_t *time_info) {
  m2m2_pkt<m2m2_pm_sys_cmd_t> pkt(this->get_address());
  pkt.payload.command = M2M2_PM_SYS_COMMAND_GET_DATE_TIME_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;
  
  auto resp = this->sync_send(pkt.pack());
  
  m2m2_pkt<m2m2_pm_sys_date_time_req_t> respkt;
  respkt.unpack(resp);
  
  time_info->year = respkt.payload.year;
  time_info->month = respkt.payload.month;
  time_info->day = respkt.payload.day;
  time_info->hour = respkt.payload.hour;
  time_info->minute = respkt.payload.minute;
  time_info->second = respkt.payload.second;
  time_info->TZ_sec = respkt.payload.TZ_sec;
  
}
/*!
\brief It is use to Control the voltage in M3 motherboard.
*/
ret::sdk_status pm_application::powerboost_m3(uint8_t boostenable, uint8_t OVPValue) {
  m2m2_pkt<m2m2_pm_sys_boost_cmd_t> pkt(this->get_address());
  pkt.payload.command = M2M2_PM_SYS_COMMAND_BOOST_REQ;
  pkt.payload.enable = (M2M2_PM_SYS_BOOST_ENUM_t) boostenable;
  pkt.payload.ovp = (M2M2_PM_SYS_OVP_ENUM_t) OVPValue;
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  if (pkt.payload.status == M2M2_APP_COMMON_STATUS_OK) {
    return ret::SDK_OK;
  } else {
    return ret::SDK_ERR_DEVICE_ERR;
  }
}

/*!
\brief store the RDAC value into EEPROM AD5258.
*/
uint8_t pm_application::powerboost_m4(uint8_t boostenable)
{
	m2m2_pkt<m2m2_pm_sys_boost_cmd_t> pkt(this->get_address());
	pkt.payload.command = M2M2_PM_SYS_COMMAND_STORE_TO_EEPROM_REQ;
	pkt.payload.enable = (M2M2_PM_SYS_BOOST_ENUM_t)boostenable;
    pkt.payload.ovp = M2M2_PM_SYS_OVP_ENUM_t::M2M2_PM_SYS_OVP_18_5V;
	pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;
	auto  resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);
if (pkt.payload.status == M2M2_APP_COMMON_STATUS_OK)  {
#if Debug
	std::cout << "ret::SDK_OK" << std::endl;
#endif
	return  ret::SDK_OK;
}
else {
#if Debug
	std::cout << "ret::SDK_DEVICE_ERR" << std::endl;
#endif
	return  ret::SDK_ERR_DEVICE_ERR;
}
}
/*!
\brief vary the voltage level of AD5258 chip 
*/
uint8_t pm_application::boost_m4(uint8_t boostenable, uint8_t  Boostvoltage)
{
	m2m2_pkt<m2m2_pm_sys_powerboost_cmd_t> pkt(this->get_address());
	pkt.payload.command = M2M2_PM_SYS_COMMAND_ADP5258_WRITE_RDAC_REQ;
	pkt.payload.enable = (M2M2_PM_SYS_BOOST_ENUM_t)boostenable;
	pkt.payload.ovp = Boostvoltage;
	pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;
	auto  resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);
	if (pkt.payload.status == M2M2_APP_COMMON_STATUS_OK)  {
#if Debug
		std::cout << "ret::SDK_OK" << std::endl;
#endif
		return  ret::SDK_OK;
	}
	else{
#if Debug
		std::cout << "ret::SDK_DEVICE_ERR" << std::endl;
#endif
		return  ret::SDK_ERR_DEVICE_ERR;
	}
}

uint8_t pm_application::read_EEPROM(eeprom_info_t *eeprom)
{
	uint8_t byte_stream[16] = { };
	m2m2_pkt<m2m2_pm_sys_eeprom_cmd_t> pkt(this->get_address());
	pkt.payload.command = M2M2_PM_SYS_COMMAND_READ_EEPROM_REQ;
	pkt.payload.size = 16;
	auto  resp = this->sync_send(pkt.pack());
	m2m2_pkt<m2m2_pm_sys_eeprom_cmd_t> respkt;
	pkt.unpack(resp);

	if (respkt.payload.status == M2M2_APP_COMMON_STATUS_OK)
	{
		//printf("EEPROM DETAILS ARE:");

		eeprom->manufacture_id = (pkt.payload.byte_stream[0] << 16) | (pkt.payload.byte_stream[1] << 8) | pkt.payload.byte_stream[2];
		eeprom->hw_id = (pkt.payload.byte_stream[3] << 8) | pkt.payload.byte_stream[4];
		eeprom->bom_id = (pkt.payload.byte_stream[5] << 8) | pkt.payload.byte_stream[6];
		eeprom->batch_id = pkt.payload.byte_stream[7];
		eeprom->date = (pkt.payload.byte_stream[8] << 24) | (pkt.payload.byte_stream[9] << 16) | (pkt.payload.byte_stream[10] << 8) | pkt.payload.byte_stream[11];
		eeprom->additional_detail = (pkt.payload.byte_stream[12] << 24) | (pkt.payload.byte_stream[13] << 16) | (pkt.payload.byte_stream[14] << 8) | pkt.payload.byte_stream[15];

		return true;
	}
	else if(respkt.payload.status == M2M2_APP_COMMON_STATUS_ERROR)
	{
		std::cout << "Ther is no EEPROM!\n" << std::endl;
	}
	return false;
}

/*	void pm_application::write_EEPROM(int bytestream)
	{
		
		m2m2_pkt<m2m2_pm_sys_eeprom_cmd_t> pkt(this->get_address());
		pkt.payload.command = M2M2_PM_SYS_COMMAND_WRITE_EEPROM_REQ;
		pkt.payload.size = 16;

		pkt.payload.byte_stream[1] = bytestream;
		//for (int i = 0; i < 16; i++)
		//{
		//	std::cout << "EEPROM Write Success = %0x"  <<std::endl;
		//	pkt.payload.byte_stream[i] = bytestream[i];
		//}
		auto  resp = this->sync_send(pkt.pack());
		pkt.unpack(resp);
		if (pkt.payload.status == M2M2_APP_COMMON_STATUS_OK) 
		{
			std::cout << "EEPROM Write Success"<<std::endl;
		}
		else if (pkt.payload.status == M2M2_APP_COMMON_STATUS_ERROR)
		{
			std::cout << "There is no EEPROM" << std::endl;
		}
	}
	*/




/*!
   \brief return the status of low touch logging.
*/
uint8_t pm_application::get_low_touch_status(void) {
  m2m2_pkt<m2m2_pm_sys_cmd_t> pkt(this->get_address());
  pkt.payload.command = M2M2_PS_SYS_COMMAND_GET_LOW_TOUCH_LOGGING_STATUS_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  return pkt.payload.status;
}

/*!
   \brief enable/disable low touch logging. (set True to Enable and False to Disable)
*/
uint8_t pm_application::enable_config_log(bool enable) {
  sdk->is_user_cmd = true;
  m2m2_pkt<m2m2_pm_sys_cmd_t> pkt(this->get_address());
  if (enable)
    pkt.payload.command = M2M2_PM_SYS_COMMAND_ENABLE_USER_CONFIG_LOG_REQ;
  else
    pkt.payload.command = M2M2_PM_SYS_COMMAND_DISABLE_USER_CONFIG_LOG_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  sdk->is_user_cmd = false;
  return pkt.payload.status;
}
