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
#include "m2m2/cpp/display_interface.hpp"

/*!
 \brief A class representing the SyncPPG application.

 */

class display_application : public m2m2_application {
	friend watch;
public:
	enum  DISPLAY_SET_COMMAND_ENUM_t {
		WHITE_DISPLAY = M2M2_DISPLAY_SET_COMMAND_ENUM_t::M2M2_DISPLAY_SET_WHITE,
		BLACK_DISPLAY = M2M2_DISPLAY_SET_COMMAND_ENUM_t::M2M2_DISPLAY_SET_BLACK,
		RED_DISPLAY = M2M2_DISPLAY_SET_COMMAND_ENUM_t::M2M2_DISPLAY_SET_RED,
		GREEN_DISPLAY = M2M2_DISPLAY_SET_COMMAND_ENUM_t::M2M2_DISPLAY_SET_GREEN,
		BLUE_DISPLAY = M2M2_DISPLAY_SET_COMMAND_ENUM_t::M2M2_DISPLAY_SET_BLUE,
	};
	display_application(watch *sdk = NULL);

	~display_application(void);

	m2m2_data_stream<pedometer_stream_callback> display_stream;


	std::string get_name(void);

	M2M2_ADDR_ENUM_t get_address(void);

	ret::sdk_status set_display_colour(DISPLAY_SET_COMMAND_ENUM_t display_color);
	ret::sdk_status backlight_control(bool enable);
	ret::sdk_status key_press_test(bool enable);
};
