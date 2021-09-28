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
#include "callback.hpp"

watch_phy_callback::~watch_phy_callback(void) {}
void watch_phy_callback::call(std::vector<uint8_t>) {}
void watch_phy_callback::sys_alert_call(uint8_t) {}

adxl_stream_callback::~adxl_stream_callback(void) {}
void adxl_stream_callback::call(std::vector<adxl_stream_cb_data_t>, uint16_t sequence_num) {}

eda_stream_callback::~eda_stream_callback(void) {}

void eda_stream_callback::call(std::vector<eda_stream_cb_data_t>, uint16_t sequence_num) {}

ecg_stream_callback::~ecg_stream_callback(void) {}

void ecg_stream_callback::call(std::vector<ecg_stream_cb_data_t>, uint16_t sequence_num) {}
temperature_stream_callback::~temperature_stream_callback(void) {}

void temperature_stream_callback::call(std::vector<temperature_stream_cb_data_t>,
                                       uint16_t sequence_num) {}
adpd_stream_callback::~adpd_stream_callback(void) {}
void adpd_stream_callback::call(std::vector<adpd_stream_cb_data_t_sum32>, uint16_t sequence_num) {}
void adpd_stream_callback::call(std::vector<adpd_stream_cb_data_t_sum16>, uint16_t sequence_num) {}
void adpd_stream_callback::call(std::vector<adpd_stream_cb_data_t_4CH16>, uint16_t sequence_num) {}
void adpd_stream_callback::call(std::vector<adpd_stream_cb_data_t_4CH32>, uint16_t sequence_num) {}

ppg_stream_callback::~ppg_stream_callback(void) {}

void ppg_stream_callback::call(std::vector<ppg_stream_cb_data_t>, uint16_t sequence_num) {}

syncppg_stream_callback::~syncppg_stream_callback(void) {}

void syncppg_stream_callback::call(std::vector<syncppg_stream_cb_data_t>, uint16_t sequence_num) {}

adpd4000_stream_callback::~adpd4000_stream_callback(void) {}
void adpd4000_stream_callback::call(std::vector<adpd4000_stream_cb_data_t>, uint16_t sequence_num) {}
void adpd4000_stream_callback::call(std::vector<adpd4000_optional_stream_cb_data_t>, uint16_t sequence_num) {}

battery_stream_callback::~battery_stream_callback(void){}
void battery_stream_callback::call(std::vector<battery_stream_cb_data_t>){}

fs_ls_callback::~fs_ls_callback(void){}
void fs_ls_callback::call(std::vector<fs_ls_data_t>){}

pedometer_stream_callback::~pedometer_stream_callback(void) {}

void pedometer_stream_callback::call(std::vector<pedometer_stream_cb_data_t>, uint16_t sequence_num) {}

fs_stream_callback::~fs_stream_callback(void) {}
void fs_stream_callback::call(std::vector<fs_stream_cb_data_t>){}

bcm_stream_callback::~bcm_stream_callback(void) {}

void bcm_stream_callback::call(std::vector<bcm_stream_cb_data_t>, uint16_t sequence_num) {}

hrv_stream_callback::~hrv_stream_callback(void) {}

void hrv_stream_callback::call(std::vector<hrv_stream_cb_data_t>, uint16_t sequence_num) {}

agc_stream_callback::~agc_stream_callback(void) {}

void agc_stream_callback::call(std::vector<agc_stream_cb_data_t>, uint16_t sequence_num) {}