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

#include <iostream>

/*!
   \brief Constructs a string of hex values representing the data of an object.
*/
template <typename T>
void print_bytes(const T& input, std::ostream& os = std::cout)
{
  const unsigned char* p = reinterpret_cast<const unsigned char*>(&input);
  for (unsigned int i=0; i<sizeof(T); ++i)
    os << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(*(p++)) << " ";
}

/*!
   \brief Returns the type of the member pointed to by an object member pointer.
*/
template <class C, typename T>
T get_member_ptr_type(T C::*v);


/*!
   \brief Packs a struct into a vector of bytes.
*/
template <typename T>
inline std::vector<uint8_t> serialize_item(T &item) {
  std::vector<uint8_t> ret;
  uint8_t *src = reinterpret_cast<uint8_t *>(&item);
  ret.insert(ret.end(), src, src + (sizeof(T)));
  return ret;
}

/*!
   \brief Packs a vector of structs into a vector of bytes.
*/
template <typename T>
inline std::vector<uint8_t> serialize_vec(std::vector<T> &vec) {
  std::vector<uint8_t> ret;
  for (unsigned int i = 0; i < vec.size(); i++) {
    auto ser = serialize_item(vec[i]);
    ret.insert(ret.end(), ser.begin(), ser.end());
  }
  return ret;
}

/*!
   \brief Unpacks a vector of bytes into a vector of structs.
*/
template <typename T>
inline std::vector<T> deserialize_vec(std::vector<uint8_t> &data) {
  std::vector<T> out_vec;
  T item;
  for (unsigned int i = 0; i < data.size(); i+= sizeof(T)) {
    std::copy(&data[i], &data[i + sizeof(T)], reinterpret_cast<uint8_t*>(&item));
    out_vec.push_back(item);
  }
  return out_vec;
}
