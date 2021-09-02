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

#include <iostream>
#include <thread>
#include <bitset>
#include "watch.hpp"

// Icky globals so we can do a quick and dirty loopback test
std::vector<uint8_t> g_pkt;
std::condition_variable g_c_var;
std::mutex g_mutex;
bool g_d_ready = false;

// Define our own physical interface callback. This one saves the packet being sent instead of actually sending it.
class phy_cb: public watch_phy_callback {
public:
  void call(std::vector<uint8_t> pkt) {
      // Store this packet so that it can be looped back as the response
      g_pkt = pkt;
      std::cout << "This is PHY, got something to write:" << std::endl;
      // Dump bytes in hex
      for (unsigned int i = 0; i < pkt.size(); i++) {
        std::cout << " " << std::hex << std::setfill('0') << std::setw(2) << (int) pkt[i];
      }
      std::cout << std::endl;
      g_d_ready = true;
      g_c_var.notify_one();
  };

  void sys_alert_call(uint8_t sys_alert) {
    std::cout << "This is System Alert:" << sys_alert << std::endl;
  };
};

// More icky globals for loopback
phy_cb g_phy_callback = phy_cb();
watch g_mw = watch(g_phy_callback);

// This is our physical interface receive thread; it just loopbacks the last sent packet
void loopback_thread_fn(void) {
  while(!g_d_ready) {
    std::unique_lock<std::mutex> lock(g_mutex);
    g_c_var.wait(lock);
  }
  // Make sure that the message looks like it's coming from the correct application
  g_pkt[0] = 0xc5;
  g_pkt[1] = 0x02;
  std::cout << "spoofing an LED response:" << std::endl;
  // Dump the packet for debug purposes
  for (unsigned int i = 0; i < g_pkt.size(); i++) {
    std::cout << " " << std::hex << std::setfill('0') << std::setw(2) << (int) g_pkt[i];
  }
  std::cout << std::endl;
  g_mw.dispatch(g_pkt);
  std::cout << "loopback thread:: exiting!" << std::endl;
}


int main (void) {
  uint8_t r_pattern;
  uint8_t g_pattern;
  uint8_t a;
  uint8_t b;
  uint8_t c;
  std::thread loopback_thread = std::thread(loopback_thread_fn);
  if (loopback_thread.joinable()) {
    std::cout << "FATAL::C++: loopback thread is not running!" << std::endl;
  } else {
    std::cout << "C++:: loopback thread is running!" << std::endl;
  }
  std::cout << "LED red pattern: "
    << (int)r_pattern
    << "("
    << std::bitset<8>(r_pattern)
    << ")"
    << ", LED blue pattern: "
    << (int)g_pattern
    << "("
    << std::bitset<8>(g_pattern)
    << ")"
    << std::endl;

  loopback_thread.join();

  return 0;
}
