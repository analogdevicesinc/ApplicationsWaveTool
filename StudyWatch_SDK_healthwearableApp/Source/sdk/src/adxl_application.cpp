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
#include "adxl_application.hpp"

adxl_application::~adxl_application(void) {

}

adxl_application::adxl_application(watch *sdk):
      m2m2_application(sdk),
      adxl_stream(M2M2_ADDR_SENSOR_ADXL_STREAM, this){}

/*!
   \brief Fetches a human-readable string describing the application.
*/
std::string adxl_application::get_name(void) {
  return "ADXL accelerometer application";
}

/*!
   \brief Fetches the address of the application.
*/
M2M2_ADDR_ENUM_t adxl_application::get_address(void) {
  return M2M2_ADDR_SENSOR_ADXL;
}


/*!
   \brief Read from a set of registers.

   This function takes a vector of addresses to read, and returns a vector of
   (address, value) pairs which contain the addresses and values read.
   \note If an invalid list of addresses is given (i.e. an empty list), or an
   error occured on the device, a vector of size 1 with a (address, value) pair
   of 0 is returned.
   \note The SDK does not perform any validation on the correctness of the addresses
   given.
   \see adxl_application::register_write
*/
std::vector<std::pair<uint16_t, uint16_t>> adxl_application::register_read(
                                            std::vector<uint16_t> addresses //!< A vector of register addresses to be read.
                                            ) {
  if (addresses.size() == 0) {
    // return a single pair of zeroes
    return std::vector<std::pair<uint16_t, uint16_t>> {{0,0}};
  }
  // Assemble this packet the old school way to fill out the variable-length list of register operations
  uint16_t pkt_size = offsetof(m2m2_hdr_t, data)
                      + offsetof(m2m2_sensor_common_reg_op_16_hdr_t, ops)
                      + addresses.size() * sizeof(m2m2_sensor_common_reg_op_16_t);
  std::vector<uint8_t> pkt(pkt_size);
  m2m2_hdr_t *p_hdr = (m2m2_hdr_t*) &pkt[0];
  m2m2_sensor_common_reg_op_16_hdr_t *p_ops_hdr = (m2m2_sensor_common_reg_op_16_hdr_t*) &p_hdr->data[0];
  m2m2_sensor_common_reg_op_16_t *p_ops = (m2m2_sensor_common_reg_op_16_t*) &p_ops_hdr->ops[0];
  p_hdr->length = pkt_size;
  p_ops_hdr->command = M2M2_SENSOR_COMMON_CMD_READ_REG_16_REQ;
  p_ops_hdr->num_ops = addresses.size();
  for (unsigned int i = 0; i < addresses.size(); i++) {
    p_ops[i].address = addresses[i];
    p_ops[i].value = 0x0000;
  }
  auto resp = this->sync_send(pkt);

  std::vector<std::pair<uint16_t, uint16_t>> ret_vals;
  p_hdr = (m2m2_hdr_t*) &resp[0];
  p_ops_hdr = (m2m2_sensor_common_reg_op_16_hdr_t*) &p_hdr->data[0];
  if (p_ops_hdr->status != M2M2_APP_COMMON_STATUS_OK) {
    // return a single pair of zeroes
    return std::vector<std::pair<uint16_t, uint16_t>> {{0,0}};
  }
  p_ops = (m2m2_sensor_common_reg_op_16_t*) &p_ops_hdr->ops[0];
  for (unsigned int i = 0; i < p_ops_hdr->num_ops; i++) {
    std::pair<uint16_t, uint16_t> v;
    v.first = p_ops[i].address;
    v.second = p_ops[i].value;
    ret_vals.push_back(v);
  }
  return ret_vals;
}

/*!
   \brief Write to a set of registers.
   This function takes a vector of (address, value) pairs to write, and returns
   a vector of (address, value) pairs which contain the addresses and values written.
   \note If an invalid list of addresses is given (i.e. an empty list), or an
   error occured on the device, a vector of size 1 with a (address, value) pair
   of 0 is returned.
   \note The SDK does not perform any validation on the correctness of the addresses
   or values given.
   \see adxl_application::register_read
*/
std::vector<std::pair<uint16_t, uint16_t>> adxl_application::register_write(
                                            std::vector<std::pair<uint16_t, uint16_t>> addr_vals //!< A vector of register (address,value) pairs to be written.
                                            ) {
  if (addr_vals.size() == 0) {
    // return a single pair of zeroes
    return std::vector<std::pair<uint16_t, uint16_t>> {{0,0}};
  }
  // Assemble this packet the old school way to fill out the variable-length list of register operations
  uint16_t pkt_size = offsetof(m2m2_hdr_t, data)
                      + offsetof(m2m2_sensor_common_reg_op_16_hdr_t, ops)
                      + addr_vals.size() * sizeof(m2m2_sensor_common_reg_op_16_t);
  std::vector<uint8_t> pkt(pkt_size);
  m2m2_hdr_t *p_hdr = (m2m2_hdr_t*) &pkt[0];
  m2m2_sensor_common_reg_op_16_hdr_t *p_ops_hdr = (m2m2_sensor_common_reg_op_16_hdr_t*) &p_hdr->data[0];
  m2m2_sensor_common_reg_op_16_t *p_ops = (m2m2_sensor_common_reg_op_16_t*) &p_ops_hdr->ops[0];
  p_hdr->length = pkt_size;
  p_ops_hdr->command = M2M2_SENSOR_COMMON_CMD_WRITE_REG_16_REQ;
  p_ops_hdr->num_ops = addr_vals.size();
  for (unsigned int i = 0; i < addr_vals.size(); i++) {
    p_ops[i].address = addr_vals[i].first;
    p_ops[i].value = addr_vals[i].second;
  }
  auto resp = this->sync_send(pkt);

  std::vector<std::pair<uint16_t, uint16_t>> ret_vals;
  p_hdr = (m2m2_hdr_t*) &resp[0];
  p_ops_hdr = (m2m2_sensor_common_reg_op_16_hdr_t*) &p_hdr->data[0];
  if (p_ops_hdr->status != M2M2_APP_COMMON_STATUS_OK) {
    // return a single pair of zeroes
    return std::vector<std::pair<uint16_t, uint16_t>> {{0,0}};
  }
  p_ops = (m2m2_sensor_common_reg_op_16_t*) &p_ops_hdr->ops[0];
  for (unsigned int i = 0; i < p_ops_hdr->num_ops; i++) {
    std::pair<uint16_t, uint16_t> v;
    v.first = p_ops[i].address;
    v.second = p_ops[i].value;
    ret_vals.push_back(v);
  }
  return ret_vals;
}

/*!
   \brief get DCFG

   This function takes a vector of addresses to read, and returns a vector of
   (address, value) pairs which contain the addresses and values read.
*/
std::vector<std::pair<uint16_t, uint16_t>> adxl_application::get_dcfg() {

  uint8_t firstbyte, secondbyte;
  uint16_t  address, value;
  std::vector<std::pair<uint16_t, uint16_t>> dcfg_vector_pair;

  m2m2_pkt<_m2m2_app_common_cmd_t> pkt(this->get_address());
  m2m2_pkt<m2m2_sensor_dcfg_data_t> respkt(this->get_address());

  pkt.payload.command = M2M2_SENSOR_COMMON_CMD_GET_DCFG_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;

  auto resp = this->sync_send(pkt.pack());

  respkt.unpack(resp);

  for (unsigned int i = 0; i < respkt.payload.size; i++) {
    firstbyte = respkt.payload.dcfgdata[2*i];
    secondbyte = respkt.payload.dcfgdata[2*i+1];

    address = secondbyte;
    value = firstbyte;

    dcfg_vector_pair.push_back(std::make_pair(address, value));
  }
  return dcfg_vector_pair;
}

ret::sdk_status adxl_application::load_lcfg(SENSOR_ADXL_LCFG_ID lcfg_id //!< The ID number of the LCFG to be loaded
) {
	m2m2_pkt<m2m2_sensor_adxl_resp_t> pkt(this->get_address());
	pkt.payload.command = M2M2_SENSOR_ADXL_COMMAND_LOAD_CFG_REQ;
	pkt.payload.deviceid = static_cast<M2M2_SENSOR_ADXL_DEVICE_ID_ENUM_t>(lcfg_id);
	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);
	if (pkt.payload.status == M2M2_APP_COMMON_STATUS_OK) {
		return ret::SDK_OK;
	}
	else {
		return ret::SDK_ERR_DEVICE_ERR;
	}
}

ret::sdk_status  adxl_application::self_test(void)
{
	m2m2_pkt<m2m2_sensor_adxl_resp_t> pkt(this->get_address());
	pkt.payload.command = M2M2_SENSOR_ADXL_COMMAND_SELF_TEST_REQ;
	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);
	if (pkt.payload.status == M2M2_APP_COMMON_STATUS_OK) {
		return ret::SDK_OK;
	}
	else {
		return ret::SDK_ERR_DEVICE_ERR;
	}
}



ret::sdk_status  adxl_application::dcb_delete_config(void)
{
	m2m2_pkt<m2m2_dcb_adxl_data_t> pkt(this->get_address());
	pkt.payload.command = M2M2_DCB_COMMAND_ERASE_CONFIG_REQ;
	pkt.payload.status = M2M2_DCB_STATUS_ERR_NOT_CHKD;

	auto resp = this->sync_send(pkt.pack());

	pkt.unpack(resp);

	if (pkt.payload.status == M2M2_DCB_STATUS_OK) {
#if Debug
		std::cout << "ret::SDK_OK" << std::endl;
#endif
		return ret::SDK_OK;
	}
	else {
#if Debug
		std::cout << "ret::SDK_ERR_DEVICE_ERR" << std::endl;
#endif
		return ret::SDK_ERR_DEVICE_ERR;
	}
}



ret::sdk_status adxl_application::write_dcb_config(std::vector<std::pair<uint16_t, uint16_t>> addr_vals)
{
	if (addr_vals.size() == 0 || addr_vals.size() > MAXADXLDCBSIZE) {
		// return a single pair of zeroes
		return ret::SDK_ERR;
	}
	uint8_t addrs;
	uint16_t value;
	uint32_t  dcb_data_value;
	m2m2_pkt<m2m2_dcb_adxl_data_t> pkt(this->get_address());
	pkt.payload.command = M2M2_DCB_COMMAND_WRITE_CONFIG_REQ;
	pkt.payload.size = addr_vals.size();
	for (unsigned int i = 0; i < addr_vals.size(); i++) {
		addrs = addr_vals[i].first;
		value = addr_vals[i].second;
		dcb_data_value = static_cast<uint32_t>((addrs) | (value << 16));
		pkt.payload.dcbdata[i] = dcb_data_value;
	}

	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);

	if (pkt.payload.status == M2M2_DCB_STATUS_OK) {
		return ret::SDK_OK;
	}
	else {
		return ret::SDK_ERR;
	}

}


std::vector<std::pair<uint16_t, uint16_t>> adxl_application::read_dcb_config(void)
{
	uint32_t  dcb_data_value;
	m2m2_pkt<m2m2_dcb_adxl_data_t> pkt(this->get_address());
	pkt.payload.command = M2M2_DCB_COMMAND_READ_CONFIG_REQ;
	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);

	std::vector<std::pair<uint16_t, uint16_t>> ret_vals;
	for (unsigned int i = 0; i < pkt.payload.size; i++) {
		std::pair<uint16_t, uint16_t> v;

		dcb_data_value = pkt.payload.dcbdata[i];
		v.first = dcb_data_value & 0xffff;
		v.second = (dcb_data_value >> 16) & 0xffff;
		ret_vals.push_back(v);
	}
	return ret_vals;
}
