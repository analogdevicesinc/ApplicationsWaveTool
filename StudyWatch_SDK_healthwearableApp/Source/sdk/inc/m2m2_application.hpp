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

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include "blocking_queue.hpp"
#include "byte_swap.hpp"
#include "util.hpp"

#include "m2m2/cpp/m2m2_core.hpp"
#include "m2m2/cpp/common_application_interface.hpp"
#include "m2m2/cpp/file_system_interface.hpp"
#include "m2m2/cpp/dcb_interface.hpp"

#include <stddef.h>
#include "sdk_err.hpp"


void set_device_id(ret::device_identifiers identifier);

ret::device_identifiers get_device_id();
class watch;

template <typename...> class m2m2_pkt;

/*!
   \brief Convenience template to wrap an m2m2 packet.

   This implementation is for simple packets whose payloads contain a single fixed-size struct.
*/
template<typename T>
class m2m2_pkt<T> {
private:
public:
  m2m2_hdr_t  header;
  T           payload;
  m2m2_pkt(M2M2_ADDR_ENUM_t destination) {
    std::memset(&this->payload, 0x00, sizeof(this->payload));
    header.length = offsetof(m2m2_hdr_t, data) + sizeof(this->payload);
    header.checksum = 0x0000;
    header.dest = destination;
  }

  m2m2_pkt(void) {
    std::memset(&this->payload, 0x00, sizeof(this->payload));
    header.length = offsetof(m2m2_hdr_t, data) + sizeof(this->payload);
    header.checksum = 0x0000;
    header.dest = M2M2_ADDR_UNDEFINED;
  }

  /*!
     \brief Pack the internal header and payload structures of this packet into a byte vector.
  */
  std::vector<uint8_t> pack() {
    std::vector<uint8_t> out_pkt;

    for (unsigned int i = 0; i < offsetof(m2m2_hdr_t, data); i++) {
      out_pkt.push_back(((uint8_t *)(&this->header))[i]);
    }

    for (unsigned int i = 0; i < sizeof(this->payload); i++) {
      out_pkt.push_back(((uint8_t *)(&this->payload))[i]);
    }
    return out_pkt;
  }

  /*!
	\brief Pack the internal header and payload structures of this packet into a byte vector.
 */
  std::vector<uint8_t> pack_ping() {
	  std::vector<uint8_t> out_pkt;

	  for (unsigned int i = 0; i < offsetof(m2m2_hdr_t, data); i++) {
		  out_pkt.push_back(((uint8_t *)(&this->header))[i]);
	  }

	  for (unsigned int i = 0; i < sizeof(this->payload); i++) {
		  out_pkt.push_back(((uint8_t *)(&this->payload))[i]);
	  }
	  for (unsigned int i = 0; i <= (this->header.length - sizeof(this->header) - sizeof(this->payload)); i++) {
		  out_pkt.push_back(0);
	  }

	  return out_pkt;
  }

  /*!
     \brief Unpack a byte vector into the internal header and payload structures of this packet.
  */
  void unpack(std::vector<uint8_t> &bytes) {

	  //return if the packet is empty
	  if (bytes == std::vector<uint8_t>{0})
		  return;
    // Copy the header into our internal structure objects
    this->header.src = static_cast<M2M2_ADDR_ENUM_t> (BYTE_SWAP_16(this->header.src));
    this->header.dest = static_cast<M2M2_ADDR_ENUM_t> (BYTE_SWAP_16(this->header.dest));
    this->header.length = static_cast<M2M2_ADDR_ENUM_t> (BYTE_SWAP_16(this->header.length));
    this->header.checksum = static_cast<M2M2_ADDR_ENUM_t> (BYTE_SWAP_16(this->header.checksum));
    std::memcpy(&this->payload, &bytes[offsetof(m2m2_hdr_t, data)], sizeof(T));
  }

  /*!
     \brief Print out a string of the hex values of this packet.
  */
  std::string dump(void) {
    std::stringstream stream;

    print_bytes(this->header.src, stream);
    print_bytes(this->header.dest, stream);
    print_bytes(this->header.length, stream);
    print_bytes(this->header.checksum, stream);
    print_bytes(this->payload, stream);
    stream << std::endl;
    return stream.str();
  }

  /*!
     \brief Get the length of this packet.
  */
  size_t get_length(void) {
    return this->header.length;
  }
};

typedef struct common_app_version_t {
  uint16_t major;
  uint16_t minor;
  uint16_t patch;
  std::string verstr;
  std::string vendstr;
};

typedef struct sys_date_time_t {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint32_t TZ_sec;
};
class m2m2_application {
  friend watch;
private:
  blocking_queue<std::vector<uint8_t>> pkt_q; /*!< This acts on std::vector<uint8_t> instead of m2m2_pkt because SWIG requires explicit template declarations for every single possible template instance */
protected:
  watch                          *sdk;
  void dispatch(std::vector<uint8_t> bytes);
  std::vector<uint8_t> get_packet(void);
  void async_send(std::vector<uint8_t> bytes);

	typedef std::vector<unsigned char> bytes;

	std::string bytes_to_string(const bytes &in);


public:
  enum APP_COMMON_STATUS_ENUM_t {
    APP_COMMON_STATUS_OK = M2M2_APP_COMMON_STATUS_ENUM_t::M2M2_APP_COMMON_STATUS_OK,
    APP_COMMON_STATUS_ERROR = M2M2_APP_COMMON_STATUS_ENUM_t::M2M2_APP_COMMON_STATUS_ERROR,
    APP_COMMON_STATUS_STREAM_STARTED = M2M2_APP_COMMON_STATUS_ENUM_t::M2M2_APP_COMMON_STATUS_STREAM_STARTED,
    APP_COMMON_STATUS_STREAM_STOPPED = M2M2_APP_COMMON_STATUS_ENUM_t::M2M2_APP_COMMON_STATUS_STREAM_STOPPED,
    APP_COMMON_STATUS_STREAM_IN_PROGRESS = M2M2_APP_COMMON_STATUS_ENUM_t::  M2M2_APP_COMMON_STATUS_STREAM_IN_PROGRESS,
    APP_COMMON_STATUS_STREAM_DEACTIVATED = M2M2_APP_COMMON_STATUS_ENUM_t::M2M2_APP_COMMON_STATUS_STREAM_DEACTIVATED,
    APP_COMMON_STATUS_STREAM_COUNT_DECREMENT = M2M2_APP_COMMON_STATUS_ENUM_t::M2M2_APP_COMMON_STATUS_STREAM_COUNT_DECREMENT,
    APP_COMMON_STATUS_STREAM_NOT_STARTED = M2M2_APP_COMMON_STATUS_ENUM_t::M2M2_APP_COMMON_STATUS_STREAM_NOT_STARTED,
    APP_COMMON_STATUS_STREAM_NOT_STOPPED = M2M2_APP_COMMON_STATUS_ENUM_t::M2M2_APP_COMMON_STATUS_STREAM_NOT_STOPPED,
    APP_COMMON_STATUS_SUBSCRIBER_ADDED = M2M2_APP_COMMON_STATUS_ENUM_t::M2M2_APP_COMMON_STATUS_SUBSCRIBER_ADDED,
    APP_COMMON_STATUS_SUBSCRIBER_REMOVED = M2M2_APP_COMMON_STATUS_ENUM_t::M2M2_APP_COMMON_STATUS_SUBSCRIBER_REMOVED,
    APP_COMMON_STATUS_SUBSCRIBER_COUNT_DECREMENT = M2M2_APP_COMMON_STATUS_ENUM_t::M2M2_APP_COMMON_STATUS_SUBSCRIBER_COUNT_DECREMENT,
    APP_COMMON_STATUS_HIGHEST = M2M2_APP_COMMON_STATUS_ENUM_t::__M2M2_APP_COMMON_STATUS_HIGHEST,
		FILE_SYS_STATUS_ERROR = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_STATUS_ERROR,
		FILE_SYS_ERR_INVALID = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_ERR_INVALID,
		FILE_SYS_ERR_CONFIG_FILE_POSITION = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_ERR_CONFIG_FILE_POSITION,
		FILE_SYS_ERR_MAX_FILE_COUNT = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_ERR_MAX_FILE_COUNT,
		FILE_SYS_STATUS_LOGGING_STOPPED = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_STATUS_LOGGING_STOPPED,
		FILE_SYS_STATUS_OK = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_STATUS_OK,
		FILE_SYS_STATUS_LOGGING_ERROR = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_STATUS_LOGGING_ERROR,


  };
  // A dummy copy constructor to allow the SWIG generated code to compile.
  m2m2_application& operator = (const m2m2_application& other) {
    // "use" the parameter to make the compiler happy
    (void)other;
    std::cout << "ADI SDK:: FATAL ERROR:: THE COPY CONSTRUCTOR OF CLASS 'm2m2_application' HAS BEEN INVOKED! THIS SHOULD NEVER HAPPEN!" << std::endl;
    std::cerr << "ADI SDK:: FATAL ERROR:: THE COPY CONSTRUCTOR OF CLASS 'm2m2_application' HAS BEEN INVOKED! THIS SHOULD NEVER HAPPEN!" << std::endl;
    std::exit(-1);
    return *this;
  }
  virtual ~m2m2_application(void);
  m2m2_application(watch *parent);

  void get_version(common_app_version_t* app_version);
  std::vector<uint8_t> sync_send(std::vector<uint8_t> bytes);

  m2m2_application (const m2m2_application &obj);
  virtual M2M2_ADDR_ENUM_t get_address(void) = 0;
  virtual std::string get_name(void) = 0;
  const std::vector<uint8_t> Empty_packet = {0};
  void setTimeout(uint16_t timeout);
  uint16_t GetTimeout(void);
};
