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
#include "m2m2_data_stream.hpp"
#include "sdk_err.hpp"
#include "m2m2/cpp/sensor_adpd_application_interface.hpp"
#include"sys_alert_enum.hpp"


/*!
   \brief A class representing the ADPD application.

   The ADPD application manages the ADPD1XX optical device on the watch.
*/

typedef struct get_slot_info_t {
	uint8_t  slot_num;
	uint8_t  slot_enable;
	uint16_t slot_format;
	uint8_t  channel_num;
};

class adpd4000_application:public m2m2_application {
  friend watch;

public:
	enum SENSOR_ADPD4000_DEVICE_ID {
		
		ADPDCL_DEVICE_4000 = M2M2_SENSOR_ADPD_DEVICE_ID_ENUM_t::M2M2_SENSOR_ADPDCL_DEVICE_4000,
		ADPDCL_DEVICE_4100 = M2M2_SENSOR_ADPD_DEVICE_ID_ENUM_t::M2M2_SENSOR_ADPDCL_DEVICE_4100
	};
	adpd4000_application(watch *sdk=NULL);
  ~adpd4000_application(void);

  m2m2_data_stream<adpd4000_stream_callback> adpd4000_stream1; //!< ADPDCL data stream
  m2m2_data_stream<adpd4000_stream_callback> adpd4000_stream2;
  m2m2_data_stream<adpd4000_stream_callback> adpd4000_stream3;
  m2m2_data_stream<adpd4000_stream_callback> adpd4000_stream4;
  m2m2_data_stream<adpd4000_stream_callback> adpd4000_stream5;
  m2m2_data_stream<adpd4000_stream_callback> adpd4000_stream6;
  m2m2_data_stream<adpd4000_stream_callback> adpd4000_stream7;
  m2m2_data_stream<adpd4000_stream_callback> adpd4000_stream8;
  m2m2_data_stream<adpd4000_stream_callback> adpd4000_stream9;
  m2m2_data_stream<adpd4000_stream_callback> adpd4000_stream10;
  m2m2_data_stream<adpd4000_stream_callback> adpd4000_stream11;
  m2m2_data_stream<adpd4000_stream_callback> adpd4000_stream12;
	m2m2_data_stream<adpd4000_stream_callback> adpd4000_optional_stream;

  std::string get_name(void);
  M2M2_ADDR_ENUM_t get_address(void);
  std::vector<std::pair<uint16_t, uint16_t>> register_read(std::vector<uint16_t> addresses);
  std::vector<std::pair<uint16_t, uint16_t>> register_write(std::vector<std::pair<uint16_t, uint16_t>> addr_vals);
  ret::sdk_status load_cfg(SENSOR_ADPD4000_DEVICE_ID device_id = ADPDCL_DEVICE_4000);
  std::vector<std::pair<uint16_t, uint16_t>> get_dcfg(void);
  // void get_version(void);
  ret::sdk_status set_slot(uint8_t  slot_num,
	   uint8_t  slot_enable, uint16_t  slot_format, uint8_t  channel_num);
  ret::sdk_status get_slot(uint8_t slotnum, get_slot_info_t *get_slot);
  ret::sdk_status set_slot_active(uint8_t slot_number, uint8_t slot_active);
  ret::sdk_status get_slot_active(uint8_t slotnum, uint8_t& slot_number, uint8_t& slot_active);
  ret::sdk_status calibrate_clock(uint8_t clockcalid);
  // void write_lcfg(void);
  // void read_lcfg(void);
};
