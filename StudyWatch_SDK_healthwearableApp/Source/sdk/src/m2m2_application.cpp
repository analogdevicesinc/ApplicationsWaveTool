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
#include "m2m2_application.hpp"
#include "watch.hpp"
#include "byte_swap.hpp"
static ret::device_identifiers device_identifier;

void set_device_id(ret::device_identifiers identifier) {
  device_identifier = identifier;
}

ret::device_identifiers get_device_id() {
  return device_identifier;
}

m2m2_application::m2m2_application(watch *parent):
                sdk(parent){}

std::vector<uint8_t> m2m2_application::get_packet(void) {
  return this->pkt_q.pop();
}

/*!
   \brief Send a packet and return immediately.
*/
void m2m2_application::async_send(std::vector<uint8_t> bytes) {
  m2m2_hdr_t * p_hdr = (m2m2_hdr_t*)&bytes[0];
  p_hdr->dest = this->get_address();
  this->sdk->write_bytes(bytes);
}

/*!
   \brief Send a packet and block until a response is received.
*/
std::vector<uint8_t> m2m2_application::sync_send(std::vector<uint8_t> bytes) {
  m2m2_hdr_t * p_hdr = (m2m2_hdr_t*)&bytes[0];
  p_hdr->dest = this->get_address();
  this->sdk->write_bytes(bytes);
  auto resp = this->get_packet();
  if (resp == Empty_packet)
	  this->sdk->system_alert_call(ADI_SDK_PACKET_TIMED_OUT);

  return resp;
}


void m2m2_application::dispatch(std::vector<uint8_t> bytes) {
  this->pkt_q.push(bytes);
}

m2m2_application::~m2m2_application(void) {}

/*!
 \brief Fetches the version info of the application.
 */
void m2m2_application::get_version(common_app_version_t* app_version) {
  m2m2_pkt<m2m2_app_common_version_t> pkt(this->get_address());
  pkt.payload.command = M2M2_APP_COMMON_CMD_GET_VERSION_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;
  
  auto resp = this->sync_send(pkt.pack());
  
  m2m2_pkt<m2m2_app_common_version_t> respkt;
  respkt.unpack(resp);
  
  app_version->major = respkt.payload.major;
  app_version->minor = respkt.payload.minor;
  app_version->patch = respkt.payload.patch;
  app_version->verstr = std::string(reinterpret_cast<char *>(respkt.payload.verstr),
                                    strlen((const char *)respkt.payload.verstr));
  app_version->vendstr = std::string(reinterpret_cast<char *>(respkt.payload.str),
                                     strlen((const char *) respkt.payload.str));
}

/*!
   \brief converts byte array to hex string
*/
std::string m2m2_application::bytes_to_string(const bytes &in) {
  bytes::const_iterator from = in.cbegin();
  bytes::const_iterator to = in.cend();
  std::ostringstream oss;
  for (; from != to; ++from)
    oss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase
        << static_cast<int>(*from);
  return oss.str();
}

/*!
\brief Set timeot for packet to be recived.
*/
void m2m2_application::setTimeout(uint16_t timeout)
{
	this->pkt_q.set_wait_time(timeout);
}

/*!
\brief Get timeot for packet to be recived.
*/
uint16_t m2m2_application::GetTimeout(void)
{
	return this->pkt_q.get_wait_time();
}

