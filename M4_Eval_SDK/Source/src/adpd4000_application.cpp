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
#include "adpd4000_application.hpp"
#define Debug 0
adpd4000_application::~adpd4000_application(void) {

}

adpd4000_application::adpd4000_application(watch *sdk) :
	m2m2_application(sdk),
	adpd4000_stream1(M2M2_ADDR_SENSOR_ADPD_STREAM1, this),
	adpd4000_stream2(M2M2_ADDR_SENSOR_ADPD_STREAM2, this),
	adpd4000_stream3(M2M2_ADDR_SENSOR_ADPD_STREAM3, this), 
	adpd4000_stream4(M2M2_ADDR_SENSOR_ADPD_STREAM4, this), 
	adpd4000_stream5(M2M2_ADDR_SENSOR_ADPD_STREAM5, this), 
	adpd4000_stream6(M2M2_ADDR_SENSOR_ADPD_STREAM6, this), 
	adpd4000_stream7(M2M2_ADDR_SENSOR_ADPD_STREAM7, this), 
	adpd4000_stream8(M2M2_ADDR_SENSOR_ADPD_STREAM8, this), 
	adpd4000_stream9(M2M2_ADDR_SENSOR_ADPD_STREAM9, this), 
	adpd4000_stream10(M2M2_ADDR_SENSOR_ADPD_STREAM10, this),
	adpd4000_stream11(M2M2_ADDR_SENSOR_ADPD_STREAM11, this),
	adpd4000_stream12(M2M2_ADDR_SENSOR_ADPD_STREAM12, this),
	adpd4000_optional_stream(M2M2_ADDR_SENSOR_ADPD_OPTIONAL_BYTES_STREAM, this){}


/*!
\brief Fetches a human-readable string describing the application.
*/
std::string adpd4000_application::get_name(void) {
	return "ADPD4000 optical sensor application";
}

/*!
\brief Fetches the address of the application.
*/ 
M2M2_ADDR_ENUM_t adpd4000_application::get_address(void) {
	return M2M2_ADDR_SENSOR_ADPDCL;
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
std::vector<std::pair<uint16_t, uint16_t>> adpd4000_application::register_read(
	std::vector<uint16_t> addresses //!< A vector of register addresses to be read.
) {
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	if (addresses.size() == 0) {
		// return a single pair of zeroes
		return std::vector<std::pair<uint16_t, uint16_t>> { {0, 0}};
	}
	// Assemble this packet the old school way to fill out the variable-length list of register operations
	uint16_t pkt_size = offsetof(m2m2_hdr_t, data)
		+ offsetof(m2m2_sensor_common_reg_op_16_hdr_t, ops)
		+ addresses.size() * sizeof(m2m2_sensor_common_reg_op_16_t);
	std::vector<uint8_t> pkt(pkt_size);
	m2m2_hdr_t *p_hdr = (m2m2_hdr_t*)&pkt[0];
	m2m2_sensor_common_reg_op_16_hdr_t *p_ops_hdr = (m2m2_sensor_common_reg_op_16_hdr_t*)&p_hdr->data[0];
	m2m2_sensor_common_reg_op_16_t *p_ops = (m2m2_sensor_common_reg_op_16_t*)&p_ops_hdr->ops[0];
	p_hdr->length = pkt_size;
	p_ops_hdr->command = M2M2_SENSOR_COMMON_CMD_READ_REG_16_REQ;
	p_ops_hdr->num_ops = addresses.size();
	for (unsigned int i = 0; i < addresses.size(); i++) {
		p_ops[i].address = addresses[i];
		p_ops[i].value = 0x0000;
	}
	auto resp = this->sync_send(pkt);

	std::vector<std::pair<uint16_t, uint16_t>> ret_vals;
	p_hdr = (m2m2_hdr_t*)&resp[0];
	p_ops_hdr = (m2m2_sensor_common_reg_op_16_hdr_t*)&p_hdr->data[0];
	if (p_ops_hdr->status != M2M2_APP_COMMON_STATUS_OK) {
		// return a single pair of zeroes
		return std::vector<std::pair<uint16_t, uint16_t>> { {0, 0}};
	}
	p_ops = (m2m2_sensor_common_reg_op_16_t*)&p_ops_hdr->ops[0];
	for (unsigned int i = 0; i < p_ops_hdr->num_ops; i++) {
		std::pair<uint16_t, uint16_t> v;
		v.first = p_ops[i].address;
		v.second = p_ops[i].value;
		ret_vals.push_back(v);
	}
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
#if Debug
	std::cout << "SDK:: Time difference in register_read= " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;
#endif
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
std::vector<std::pair<uint16_t, uint16_t>> adpd4000_application::register_write(
	std::vector<std::pair<uint16_t, uint16_t>> addr_vals //!< A vector of register (address,value) pairs to be written.
) {
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	if (addr_vals.size() == 0) {
		// return a single pair of zeroes
		return std::vector<std::pair<uint16_t, uint16_t>> { {0, 0}};
	}
	// Assemble this packet the old school way to fill out the variable-length list of register operations
	uint16_t pkt_size = offsetof(m2m2_hdr_t, data)
		+ offsetof(m2m2_sensor_common_reg_op_16_hdr_t, ops)
		+ addr_vals.size() * sizeof(m2m2_sensor_common_reg_op_16_t);
	std::vector<uint8_t> pkt(pkt_size);
	m2m2_hdr_t *p_hdr = (m2m2_hdr_t*)&pkt[0];
	m2m2_sensor_common_reg_op_16_hdr_t *p_ops_hdr = (m2m2_sensor_common_reg_op_16_hdr_t*)&p_hdr->data[0];
	m2m2_sensor_common_reg_op_16_t *p_ops = (m2m2_sensor_common_reg_op_16_t*)&p_ops_hdr->ops[0];
	p_hdr->length = pkt_size;
	p_ops_hdr->command = M2M2_SENSOR_COMMON_CMD_WRITE_REG_16_REQ;
	p_ops_hdr->num_ops = addr_vals.size();
	for (unsigned int i = 0; i < addr_vals.size(); i++) {
		p_ops[i].address = addr_vals[i].first;
		p_ops[i].value = addr_vals[i].second;
	}
	auto resp = this->sync_send(pkt);

	std::vector<std::pair<uint16_t, uint16_t>> ret_vals;
	p_hdr = (m2m2_hdr_t*)&resp[0];
	p_ops_hdr = (m2m2_sensor_common_reg_op_16_hdr_t*)&p_hdr->data[0];
	if (p_ops_hdr->status != M2M2_APP_COMMON_STATUS_OK) {
		// return a single pair of zeroes
		return std::vector<std::pair<uint16_t, uint16_t>> { {0, 0}};
	}
	p_ops = (m2m2_sensor_common_reg_op_16_t*)&p_ops_hdr->ops[0];
	for (unsigned int i = 0; i < p_ops_hdr->num_ops; i++) {
		std::pair<uint16_t, uint16_t> v;
		v.first = p_ops[i].address;
		v.second = p_ops[i].value;
		ret_vals.push_back(v);
	}
#if Debug
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "SDK:: Time difference in register_write= " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;
#endif
	return ret_vals;
}

/*!
\brief Load a pre-defined configuration from the device.
Default load configuration is ADPD_DEVICE_105
*/
ret::sdk_status adpd4000_application::load_cfg(SENSOR_ADPD4000_DEVICE_ID device_id) //!< The ID number of the config to be loaded
{
	m2m2_pkt<m2m2_sensor_adpd_resp_t> pkt(this->get_address());
	pkt.payload.command = M2M2_SENSOR_ADPD_COMMAND_LOAD_CFG_REQ;
	pkt.payload.deviceid = static_cast<M2M2_SENSOR_ADPD_DEVICE_ID_ENUM_t>(device_id);
	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);
	if (pkt.payload.status == M2M2_APP_COMMON_STATUS_OK) {
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


/*!
\Set the Adpd slot active. 
 0 = inactive, 1 = active
*/
ret::sdk_status adpd4000_application::set_slot_active(uint8_t slot_number, uint8_t slot_active)
{
	m2m2_pkt<m2m2_sensor_adpdCl_slot_active_resp_t> pkt(this->get_address());
	pkt.payload.command = M2M2_SENSOR_ADPD_COMMAND_SET_SLOT_ACTIVE_REQ;
	pkt.payload.slot_num = slot_number;
	pkt.payload.slot_active = slot_active;
	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);
	if (pkt.payload.status == M2M2_APP_COMMON_STATUS_OK) {
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

/*!
\Get the Adpd slots active state.
 0 = inactive, 1 = active
*/
ret::sdk_status adpd4000_application::get_slot_active(uint8_t slotnum,uint8_t& slot_number, uint8_t& slot_active)
{
	m2m2_pkt<m2m2_sensor_adpdCl_slot_active_resp_t> pkt(this->get_address());
	pkt.payload.command = M2M2_SENSOR_ADPD_COMMAND_GET_SLOT_ACTIVE_REQ;
	pkt.payload.slot_num = slotnum;
	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);
	slot_number = pkt.payload.slot_num;
	slot_active = pkt.payload.slot_active;
	if (pkt.payload.status == M2M2_APP_COMMON_STATUS_OK) {
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

/*!
\brief Calibrate the ADPD4000 sample clock
*/
ret::sdk_status adpd4000_application::calibrate_clock(uint8_t clockcalid) {
	m2m2_pkt<m2m2_sensor_clockcal_resp_t> pkt(this->get_address());
	pkt.payload.command = M2M2_SENSOR_ADPD_COMMAND_CLOCK_CAL_REQ;
	pkt.payload.clockcalid = clockcalid;

	uint16_t timeout = this->GetTimeout();
	this->setTimeout(5000);
	auto resp = this->sync_send(pkt.pack());
	this->setTimeout(timeout);
	pkt.unpack(resp);
	if (pkt.payload.status == M2M2_APP_COMMON_STATUS_OK) {
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

/*!
\brief Set the Adpd device slot with slot num, slot enable, slot format, and channel number.
*/
ret::sdk_status adpd4000_application::set_slot(uint8_t  slot_num,
	uint8_t  slot_enable, uint16_t  slot_format, uint8_t  channel_num) {
	m2m2_pkt<m2m2_sensor_adpdCl_slot_resp_t> pkt(this->get_address());
	pkt.payload.command = M2M2_SENSOR_ADPD_COMMAND_SET_SLOT_REQ;
	pkt.payload.slot_num = slot_num;
	pkt.payload.slot_enable = slot_enable;
	pkt.payload.slot_format = slot_format;
	pkt.payload.channel_num = channel_num;
	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);
	if (pkt.payload.status == M2M2_APP_COMMON_STATUS_OK) {
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

/*!
\brief Get the Adpd device slot information for the respective slot number.
returns the slot num, slot enable, slot format, and channel number.

*/
ret::sdk_status adpd4000_application::get_slot(uint8_t slotnum, get_slot_info_t* get_slot)
{
	m2m2_pkt<m2m2_sensor_adpdCl_slot_resp_t> pkt(this->get_address());
	pkt.payload.command = M2M2_SENSOR_ADPD_COMMAND_GET_SLOT_REQ;
	pkt.payload.slot_num = slotnum;
	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);
	get_slot->slot_num = pkt.payload.slot_num;
	get_slot->slot_enable = pkt.payload.slot_enable;
	get_slot->slot_format = pkt.payload.slot_format;
	get_slot->channel_num = pkt.payload.channel_num;
	if (pkt.payload.status == M2M2_APP_COMMON_STATUS_OK) {
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


/*!
\brief get DCFG

This function takes a vector of addresses to read, and returns a vector of
(address, value) pairs which contain the addresses and values read.
*/
std::vector<std::pair<uint16_t, uint16_t>> adpd4000_application::get_dcfg()
{

	uint8_t firstbyte, secondbyte, thirdbyte, fourthbyte;
	uint16_t  address, value;
	std::vector<std::pair<uint16_t, uint16_t>> dcfg_vector_pair;

	m2m2_pkt<_m2m2_app_common_cmd_t> pkt(this->get_address());
	m2m2_pkt<m2m2_sensor_dcfg_data_t> respkt(this->get_address());

	pkt.payload.command = M2M2_SENSOR_COMMON_CMD_GET_DCFG_REQ;
	pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;
	auto resp = this->sync_send(pkt.pack());
	respkt.unpack(resp);

	for (unsigned int i = 0; i < respkt.payload.size; i++)
	{
		firstbyte = respkt.payload.dcfgdata[4 * i];
		secondbyte = respkt.payload.dcfgdata[4 * i + 1];
		thirdbyte = respkt.payload.dcfgdata[4 * i + 2];
		fourthbyte = respkt.payload.dcfgdata[4 * i + 3];

		address = (thirdbyte | (fourthbyte << 8));
		value = (firstbyte | (secondbyte << 8));

		dcfg_vector_pair.push_back(std::make_pair(address, value));
	}
	return dcfg_vector_pair;
}
