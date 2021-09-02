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

#include <condition_variable>
#include <mutex>
#include <queue>
#include <memory>
#include <chrono>
#include <ostream>
#include <iostream>
#include "m2m2_application.hpp"

#ifdef SWIG_PY
#include <Python.h>
#endif // SWIG_PY

template <typename T>
class blocking_queue {
 public:
   blocking_queue():mutex_(){}
   blocking_queue(const blocking_queue &obj):mutex_() {}
  // Copy assignment operator.
  // Provide a copy assignment operator so that SWIG's generated code will work
  // THIS SHOULDN'T EVER BE INVOKED
  blocking_queue& operator = (const blocking_queue& other) {
    // "use" the parameter to make the compiler happy
    (void)other;
    std::cout << "ADI SDK:: FATAL ERROR:: THE COPY CONSTRUCTOR OF CLASS 'blocking_queue' HAS BEEN INVOKED! THIS SHOULD NEVER HAPPEN!" << std::endl;
    std::cerr << "ADI SDK:: FATAL ERROR:: THE COPY CONSTRUCTOR OF CLASS 'blocking_queue' HAS BEEN INVOKED! THIS SHOULD NEVER HAPPEN!" << std::endl;
    std::exit(-1);
    return *this;
  }

  void set_wait_time(uint16_t p_wait_time) {
    wait_time = p_wait_time;
  }

  uint16_t get_wait_time() {
    return wait_time;
  }

  void push(const T &item) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      queue_.push(item);
    }
    cv_.notify_one();
    // cv_.notify_all();
  }

  T pop() {
#ifdef SWIG_PY
    // This is needed so that Python threads can continue to run properly if we end up blocking from within C++
    Py_BEGIN_ALLOW_THREADS;
#endif // SWIG_PY

    std::chrono::milliseconds timeout(wait_time);

    while (queue_.empty()) {
      std::unique_lock<std::mutex> lock(mutex_);
      // This thread isn't properly woken up when someone else notifies the condition variable!
      if (cv_.wait_for(lock, timeout) == std::cv_status::timeout) {
        std::cout << "ADI SDK:: Waiting for a packet timed out!" << std::endl;
		T item = { 0 };
		queue_.push(item);
      }
    }

#ifdef SWIG_PY
    // Cleanup
    Py_END_ALLOW_THREADS;
#endif // SWIG_PY

    auto result = queue_.front();
    queue_.pop();
    return result;
  }

 private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cv_;
  uint16_t wait_time = 9000;
  // std::unique_ptr<std::mutex> p_mutex_ = std::make_unique<std::mutex>();
  // std::unique_ptr<std::condition_variable> p_cv_ = std::make_unique<std::condition_variable>();
};
