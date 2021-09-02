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

#include "display_application.hpp"

display_application::~display_application(void) {

}

display_application::display_application(watch *sdk) :
	m2m2_application(sdk),
	display_stream(M2M2_ADDR_APP_DISPLAY_STREAM, this) {}

/*!
 \brief Fetches a human-readable string describing the application.
 */
std::string display_application::get_name(void) {
	return "Display";
}

/*!
 \brief Fetches the address of the application.
 */
M2M2_ADDR_ENUM_t display_application::get_address(void) {
	return M2M2_ADDR_DISPLAY;
}

ret::sdk_status display_application::set_display_colour(DISPLAY_SET_COMMAND_ENUM_t display_color)
{
	m2m2_pkt<m2m2_display_set_command_t> pkt(this->get_address());
	pkt.payload.command = M2M2_DISPLAY_APP_CMD_SET_DISPLAY_REQ;
	pkt.payload.colour = static_cast<M2M2_DISPLAY_SET_COMMAND_ENUM_t>(display_color);
	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);
	if (pkt.payload.status == M2M2_APP_COMMON_STATUS_OK) {
		return ret::SDK_OK;
	}
	else {
		return ret::SDK_ERR;
	}
}

ret::sdk_status display_application::backlight_control(bool enable)
{
	m2m2_pkt<m2m2_backlight_cntrl_command_t> pkt(this->get_address());
	pkt.payload.command = M2M2_DISPLAY_APP_CMD_BACKLIGHT_CNTRL_REQ;
	pkt.payload.control = static_cast<M2M2_BACKLIGHT_CNTRL_COMMAND_ENUM_t>(enable);
	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);
	if (pkt.payload.status == M2M2_APP_COMMON_STATUS_OK) {
		return ret::SDK_OK;
	}
	else {
		return ret::SDK_ERR;
	}
}

ret::sdk_status display_application::key_press_test(bool enable)
{
	m2m2_pkt<m2m2_key_test_command_t> pkt(this->get_address());
	pkt.payload.command = M2M2_DISPLAY_APP_CMD_KEY_TEST_REQ;
	pkt.payload.enable = static_cast<uint8_t>(enable);
	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);
	if (pkt.payload.status == M2M2_APP_COMMON_STATUS_OK) {
		return ret::SDK_OK;
	}
	else {
		return ret::SDK_ERR;
	}
}