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


/*!
   \brief A class representing the ADPD application.

   The ADPD application manages the ADPD1XX optical device on the watch.
*/

class adpd_application:public m2m2_application {
  friend watch;
public:
  enum ADPD_SLOT_MODE_ENUM_t {
    SLOTMODE_DISABLED =  M2M2_SENSOR_ADPD_SLOTMODE_ENUM_t::M2M2_SENSOR_ADPD_SLOTMODE_DISABLED,
    SLOTMODE_4CH_16b = M2M2_SENSOR_ADPD_SLOTMODE_ENUM_t::M2M2_SENSOR_ADPD_SLOTMODE_4CH_16b,
    SLOTMODE_4CH_32b = M2M2_SENSOR_ADPD_SLOTMODE_ENUM_t::M2M2_SENSOR_ADPD_SLOTMODE_4CH_32b,
    SLOTMODE_SUM16b = M2M2_SENSOR_ADPD_SLOTMODE_ENUM_t::M2M2_SENSOR_ADPD_SLOTMODE_SUM_16b,
    SLOTMODE_SUM32b = M2M2_SENSOR_ADPD_SLOTMODE_ENUM_t::M2M2_SENSOR_ADPD_SLOTMODE_SUM_32b,
  };
  enum SENSOR_ADPD_DEVICE_ID {
    ADPD_DEVICE_107 =  M2M2_SENSOR_ADPD_DEVICE_ID_ENUM_t::M2M2_SENSOR_ADPD_DEVICE_107,
    ADPD_DEVICE_185 = M2M2_SENSOR_ADPD_DEVICE_ID_ENUM_t::M2M2_SENSOR_ADPD_DEVICE_185,
    ADPD_DEVICE_ECG_PPG_185 = M2M2_SENSOR_ADPD_DEVICE_ID_ENUM_t::M2M2_SENSOR_ADPD_DEVICE_ECG_PPG_185,
    ADPD_DEVICE_ECG_185 = M2M2_SENSOR_ADPD_DEVICE_ID_ENUM_t::M2M2_SENSOR_ADPD_DEVICE_ECG_185,
    ADPD_DEVICE_105 = M2M2_SENSOR_ADPD_DEVICE_ID_ENUM_t::M2M2_SENSOR_ADPD_DEVICE_105,
	ADPD_DEVICE_188 = M2M2_SENSOR_ADPD_DEVICE_ID_ENUM_t::M2M2_SENSOR_ADPD_DEVICE_188,
	ADPD_DEVICE_108 = M2M2_SENSOR_ADPD_DEVICE_ID_ENUM_t::M2M2_SENSOR_ADPD_DEVICE_108,
	ADPD_DEVICE_188F = M2M2_SENSOR_ADPD_DEVICE_ID_ENUM_t::M2M2_SENSOR_ADPD_DEVICE_188F,
	ADPD_DEVICE_ECG_PPG_188 = M2M2_SENSOR_ADPD_DEVICE_ID_ENUM_t::M2M2_SENSOR_ADPD_DEVICE_ECG_PPG_188,
	ADPD_DEVICE_ECG_188 = M2M2_SENSOR_ADPD_DEVICE_ID_ENUM_t::M2M2_SENSOR_ADPD_DEVICE_ECG_188,
	ADPD_DEVICE_144 = M2M2_SENSOR_ADPD_DEVICE_ID_ENUM_t::M2M2_SENSOR_ADPD_DEVICE_144,
	ADPDCL_DEVICE_4000 = M2M2_SENSOR_ADPD_DEVICE_ID_ENUM_t::M2M2_SENSOR_ADPDCL_DEVICE_4000,
	ADPDCL_DEVICE_4100 = M2M2_SENSOR_ADPD_DEVICE_ID_ENUM_t::M2M2_SENSOR_ADPDCL_DEVICE_4100
  };
  adpd_application(watch *sdk=NULL);
  ~adpd_application(void);
  ADPD_SLOT_MODE_ENUM_t slotA = SLOTMODE_4CH_16b, slotB = SLOTMODE_SUM32b;

  m2m2_data_stream<adpd_stream_callback> adpd_stream; //!< ADXL data stream
  std::string get_name(void);
  M2M2_ADDR_ENUM_t get_address(void);
  std::vector<std::pair<uint16_t, uint16_t>> register_read(std::vector<uint16_t> addresses);
  std::vector<std::pair<uint16_t, uint16_t>> register_write(std::vector<std::pair<uint16_t, uint16_t>> addr_vals);
  ret::sdk_status load_cfg(SENSOR_ADPD_DEVICE_ID device_id = ADPD_DEVICE_108);
  ret::sdk_status calibrate_clock(void);
   std::vector<std::pair<uint16_t, uint16_t>> get_dcfg(void);
  // void get_version(void);
  ret::sdk_status set_slot(ADPD_SLOT_MODE_ENUM_t slot_a, ADPD_SLOT_MODE_ENUM_t slot_b);
  ret::sdk_status get_ctrvalue(uint32_t &value1, //!< 1st 32-bit output - CTR Value
                                                 uint32_t &value2, //!<Output2 - garbage value
                                                 uint32_t &value3//!<Output3 - garbage value
  );
  void get_testcommandA(uint16_t input, uint32_t& value1, uint32_t& value2, uint32_t& value3);
  void get_testcommandB(uint16_t input, uint32_t& value1, uint32_t& value2, uint32_t& value3);
  void get_testcommandC(uint16_t input, uint32_t& value1, uint32_t& value2, uint32_t& value3);
  void darkoffset(uint32_t& value1, uint32_t& value2, uint32_t& value3, uint32_t& value4);
  void floatconfig(uint32_t& value1, uint32_t& value2, uint32_t& value3);

  // void write_lcfg(void);
  // void read_lcfg(void);
};
