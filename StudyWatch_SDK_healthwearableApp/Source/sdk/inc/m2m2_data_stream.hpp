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

#include <math.h>

#include "blocking_queue.hpp"
#include "callback.hpp"
#include "m2m2_application.hpp"
#include "sdk_err.hpp"
#include "callback.hpp"

#include "m2m2/cpp/m2m2_core.hpp"
#include "m2m2/cpp/common_application_interface.hpp"
#include "m2m2/cpp/common_sensor_interface.hpp"

// Includes for the specialized stream callback templates
#include "m2m2/cpp/sensor_adxl_application_interface.hpp"
#include "m2m2/cpp/eda_application_interface.hpp"
#include "m2m2/cpp/ecg_application_interface.hpp"
#include "m2m2/cpp/temperature_application_interface.hpp"
#include "m2m2/cpp/ppg_application_interface.hpp"
#include "m2m2/cpp/sensor_adpd_application_interface.hpp"
#include "m2m2/cpp/sync_data_application_interface.hpp"
#include "m2m2/cpp/system_interface.hpp"
#include "m2m2/cpp/file_system_interface.hpp"
#include "m2m2/cpp/sqi_application_interface.hpp"
#include "m2m2/cpp/pedometer_application_interface.hpp"
//#include "m2m2/cpp/ad7689_application_interface.hpp"
#include "m2m2/cpp/bcm_application_interface.hpp"
//#include "m2m2/cpp/sensor_ad5940_application_interface.hpp"

#define FIRMWARE_TS_PER_MS (32.0)
#define SECONDS_IN_MILLIS (1000)
#define SDK_DATA_STREAM_TIME_LIMIT_SECS (10)
#define SDK_TIMEOUT 0
#define Debug 0
static int BCM_STABILITY_OHMS_DIFF = 6;
static int BCM_STABILITY_SAMPLES_COUNT = 5;
static double prev_impedance_real = 0;
static int g_samplecount = 0;
static std::vector<double> mImpedanceList;

template <typename T>
class m2m2_data_stream {
private:
  T *callback = NULL; /*!< Use a pointer instead of a reference so
                                  that we don't need to code a custom subclass
                                  for every stream (references have to be valid
                                  100% of the time). */
  int assemble_int(int b0, int b1)
  {
	  return (int)((b1 << 8) | b0);
  }

  short assemble_uint(unsigned int b0, unsigned int  b1)
  {
	  return (short)((b1 << 8) | b0);
  }

  int assemble_int(int b0)
  {
	  return (int)(b0);
  }

  int assemble_int(int b0, int b1, int b2, int b3)
  {
	  return (int)((b3 << 24) | (b2 << 16) | (b1 << 8) | b0);
  }

  long assemble_unsignedint(int b0, int b1, int b2, int b3)
  {
	  return (long)((b3 << 24) | (b2 << 16) | (b1 << 8) | b0);
  }

  int assemble_signedint(int b0, int b1, int b2)
  {
	  return (int)((b2 << 16) | (b1 << 8) | b0);
  }

protected:
  M2M2_ADDR_ENUM_t address;
  m2m2_application *p_owner = NULL;
public:
  blocking_queue<std::vector<uint8_t>> pkt_q;
  // Copy assignment operator.
  // Provide a copy assignment operator so that SWIG's generated code will work
  // THIS SHOULD NEVER BE INVOKED
  m2m2_data_stream& operator = (const m2m2_data_stream& other) {
    // "use" the parameter to make the compiler happy
    (void)other;
    std::cout << "ADI SDK:: FATAL ERROR:: THE COPY CONSTRUCTOR OF CLASS 'm2m2_data_stream' HAS BEEN INVOKED! THIS SHOULD NEVER HAPPEN!" << std::endl;
    std::cerr << "ADI SDK:: FATAL ERROR:: THE COPY CONSTRUCTOR OF CLASS 'm2m2_data_stream' HAS BEEN INVOKED! THIS SHOULD NEVER HAPPEN!" << std::endl;
    std::exit(-1);
    return *this;
  }

 

  m2m2_data_stream(const m2m2_data_stream &obj){}

  m2m2_data_stream( M2M2_ADDR_ENUM_t address,
                                      m2m2_application *p_owner):
                                    address(address),
                                    p_owner(p_owner){}


  /*!
     \brief Returns the m2m2 address of the stream
  */
  M2M2_ADDR_ENUM_t get_address(void) {
    return this->address;
  }
    
  /*!
   \brief set callback
   */
  void set_callback(T &cb) {
    this->callback = &cb;
  }
  /*!
  \brief clear the Impedance list buffer for BCM stream
  */
  void clearImpedanceListBuffer(void) {
	  mImpedanceList.clear();
  }

  /*!
     \brief Get the status of the stream.
     This function fetches the status of the stream. The status contains information
     like whether the stream has been started, stopped, etc.
     \note
     The stream may still be running even if the SDK has requested that it stop.
     The application on the device that manages the stream will only stop the stream
     once all subscribers have unsubscribed.
     \see m2m2_data_stream::subscribe
     \see m2m2_data_stream::unsubscribe
     \see m2m2_data_stream::start
  */
  void get_status(uint8_t &status, //!< Current status of the stream
                                    uint8_t &num_subscribers, //!< Number of applications (internal to the device or external) that have subscribed to the stream. If a stream has no subscribers, it will not produce data.
                                    uint8_t &num_start_requests //!< Number of start requests a stream has received; a stream will not stop producing data until it has received the same number of stop requests as start requests.
                                  ) {
    m2m2_pkt<m2m2_app_common_status_t> pkt(this->p_owner->get_address());
    pkt.payload.command = M2M2_APP_COMMON_CMD_SENSOR_STATUS_QUERY_REQ;
    pkt.payload.stream = this->get_address();
    auto resp = this->p_owner->sync_send(pkt.pack());
    pkt.unpack(resp);
    status = pkt.payload.status;
    num_subscribers = pkt.payload.num_subscribers;
    num_start_requests = pkt.payload.num_start_reqs;
  }


  /*!
     \brief Subscribe to the stream
     This function subscribes the SDK from the stream. Once subscribed, the device
     will begin sending data from this stream to the SDK.
     \note
     Subscribing to a stream only signals that you are interested in its content;
     it does not necessarily cause the stream to begin producing data. Some streams
     produce data "on their own" (i.e. system battery level), while others represent sensors
     that must be started before they begin to produce data.
     \see m2m2_data_stream::start
     \see m2m2_data_stream::stop
     \see m2m2_data_stream::unsubscribe
  */
  ret::sdk_status subscribe(T &cb) {
    this->callback = &cb;
    m2m2_pkt<m2m2_app_common_sub_op_t> pkt(this->p_owner->get_address());
    pkt.payload.command = M2M2_APP_COMMON_CMD_STREAM_SUBSCRIBE_REQ;
    pkt.payload.stream = this->get_address();
    auto resp = this->p_owner->sync_send(pkt.pack());
    pkt.unpack(resp);
    if (pkt.payload.status != M2M2_APP_COMMON_STATUS_OK
        && pkt.payload.status != M2M2_APP_COMMON_STATUS_SUBSCRIBER_ADDED) {
      return ret::SDK_ERR_DEVICE_ERR;
    }
	//std::cout << "SDK:: Subscribe SDK_OK" << std::endl;
    return ret::SDK_OK;
  }


  /*!
     \brief Unsubscribe from the stream
     This function unsubscribes the SDK from the stream. Once unsubscribed, the device
     will no longer send data from this stream to the SDK.
     \note
     The stream (and sensor it represents) may keep running even after being
     unsubscribed, as applications on the device may also be subscribed to the stream.
     \see m2m2_data_stream::subscribe
     \see m2m2_data_stream::start
     \see m2m2_data_stream::stop
  */
  ret::sdk_status unsubscribe(void) {
    m2m2_pkt<m2m2_app_common_sub_op_t> pkt(this->p_owner->get_address());
    pkt.payload.command = M2M2_APP_COMMON_CMD_STREAM_UNSUBSCRIBE_REQ;
	pkt.payload.stream = this->get_address();
    auto resp = this->p_owner->sync_send(pkt.pack());
    pkt.unpack(resp);
    if (pkt.payload.status != M2M2_APP_COMMON_STATUS_OK
        && pkt.payload.status != M2M2_APP_COMMON_STATUS_SUBSCRIBER_REMOVED) {
      return ret::SDK_ERR_DEVICE_ERR;
    }
    this->callback = NULL;
	//std::cout << "SDK:: Unsubscribe SDK_OK" << std::endl;
    return ret::SDK_OK;
  }


  /*!
     \brief Start the stream
     This function requests that the stream be started.
     \see m2m2_data_stream::stop
  */
  m2m2_application::APP_COMMON_STATUS_ENUM_t start(void) {
	  m2m2_pkt<m2m2_app_common_sub_op_t> pkt(this->p_owner->get_address());
#if Debug
	  std::cout << "SD::c++ start " << std::endl;
#endif
	  M2M2_ADDR_ENUM_t addrs = static_cast <M2M2_ADDR_ENUM_t>(this->get_address());
	  if (addrs == M2M2_ADDR_ENUM_t::M2M2_ADDR_SYS_FS)
	  {
		  pkt.payload.command = M2M2_FILE_SYS_CMD_START_LOGGING_REQ;

	  }
	  else
	  {
		  pkt.payload.command = M2M2_APP_COMMON_CMD_STREAM_START_REQ;
		  //pkt.payload.stream = addrs;

	  }
	  auto resp = this->p_owner->sync_send(pkt.pack());
	  pkt.unpack(resp);
	  return static_cast<m2m2_application::APP_COMMON_STATUS_ENUM_t>(pkt.payload.status);

	  if (pkt.payload.status != M2M2_APP_COMMON_STATUS_STREAM_STARTED && pkt.payload.status != M2M2_APP_COMMON_STATUS_STREAM_IN_PROGRESS)
	  {
		  std::cout << "SDK:: c++ start ERR" << std::endl;
		  //return M2M2_APP_COMMON_STATUS_ERROR;
	  }
	  std::cout << "SD::c++ start success" << std::endl;
	  //return M2M2_APP_COMMON_STATUS_OK;
  }


  /*!
     \brief Stop the stream
     This function requests that the stream be stopped.
     \note
     Because the stream may have received multiple requests to start, a stop
     request may not necessarily stop the flow of data. Only when the stream has
     received the same number of stop requests as start requests will it actually
     be stopped.
     \see m2m2_data_stream::start
     \see m2m2_data_stream::unsubscribe
  */
  m2m2_application::APP_COMMON_STATUS_ENUM_t stop(void) {
#if Debug
	  std::cout << "SD::c++ stop" << std::endl;
#endif
    m2m2_pkt<m2m2_app_common_sub_op_t> pkt(this->p_owner->get_address());
	M2M2_ADDR_ENUM_t addrs = static_cast <M2M2_ADDR_ENUM_t>(this->get_address());
	if (addrs == M2M2_ADDR_ENUM_t::M2M2_ADDR_SYS_FS)
	{
		pkt.payload.command = M2M2_FILE_SYS_CMD_STOP_LOGGING_REQ;


	}
	else
	{
		pkt.payload.command = M2M2_APP_COMMON_CMD_STREAM_STOP_REQ;
		//pkt.payload.stream = addrs;


	}
    auto resp = this->p_owner->sync_send(pkt.pack());
    pkt.unpack(resp);
    
    return static_cast<m2m2_application::APP_COMMON_STATUS_ENUM_t>(pkt.payload.status);
   
//    if (pkt.payload.status != M2M2_APP_COMMON_STATUS_STREAM_STOPPED
//        && pkt.payload.status != M2M2_APP_COMMON_STATUS_STREAM_COUNT_DECREMENT) {
//      return ret::SDK_ERR_DEVICE_ERR;
//    }
//    return ret::SDK_OK;
  }


  /*!
     \brief Get the stream's decimation factor.
     This function fetches the decimation factor of a stream. The decimation factor
     defines how many samples are sent over the stream. A decimation factor of 1
     means that every sample is sent, a value of 2 means every 2nd sample is send,
     3 means every 3rd sample, and so on.
     \note
     If the device returns an error code, this function returns the (otherwise
     invalid) value of 0.
     \see m2m2_data_stream::set_decimation_factor
  */
  uint8_t get_decimation_factor(void) {
    m2m2_pkt<m2m2_sensor_common_decimate_stream_t> pkt(this->p_owner->get_address());
    pkt.payload.command = M2M2_SENSOR_COMMON_CMD_GET_STREAM_DEC_FACTOR_REQ;
    pkt.payload.stream = this->get_address();
    auto resp = this->p_owner->sync_send(pkt.pack());
    pkt.unpack(resp);
    if (pkt.payload.status != M2M2_APP_COMMON_STATUS_OK) {
      return 0;
    }
    return pkt.payload.dec_factor;
  }


  /*!
     \brief Set the stream's decimation factor.
     This function sets the decimation factor of a stream. The decimation factor
     defines how many samples are sent over the stream. A decimation factor of 1
     means that every sample is sent, a value of 2 means every 2nd sample is send,
     3 means every 3rd sample, and so on.
     /note
     The decimation factor should only be increased if the stream's data
     rate is too high, and packets are being dropped.
     \see m2m2_data_stream::get_decimation_factor
  */
  ret::sdk_status set_decimation_factor(uint8_t decimation_factor) {
    if (decimation_factor == 0) {
      return ret::SDK_ERR_INVALID_VALUE;
    }
    m2m2_pkt<m2m2_sensor_common_decimate_stream_t> pkt(this->p_owner->get_address());
    pkt.payload.command = M2M2_SENSOR_COMMON_CMD_SET_STREAM_DEC_FACTOR_REQ;
    pkt.payload.stream = this->get_address();
    pkt.payload.dec_factor = decimation_factor;
    auto resp = this->p_owner->sync_send(pkt.pack());
    pkt.unpack(resp);
    if (pkt.payload.status != M2M2_APP_COMMON_STATUS_OK) {
      return ret::SDK_ERR_DEVICE_ERR;
    }
    return ret::SDK_OK;
  }


  /*!
     \brief Default declaration of the dispatch function.
     \note This should never be called; it should always be re-defined as a
     specialized template for each stream callback class.
  */
  void dispatch(std::vector<uint8_t> bytes);
};


// #############################################################################
// ## ADXL STREAM CALLBACKS                                                   ##
// #############################################################################
/*!
   \brief Dispatch a packet to an adxl_stream_callback.
*/
template<>
inline void m2m2_data_stream<adxl_stream_callback>::dispatch(std::vector<uint8_t> bytes) {
  if (!this->callback) {
    std::cout << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
              << (int) this->get_address() << std::endl;
    std::cerr << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
              << (int) this->get_address() << std::endl;
    return;
  }
  m2m2_pkt<m2m2_sensor_adxl_data_no_compress_stream_t> pkt;
  std::vector<adxl_stream_cb_data_t> data_samples;
  pkt.unpack(bytes);
  uint32_t current_timestamp = pkt.payload.timestamp;

  data_samples.push_back({current_timestamp/FIRMWARE_TS_PER_MS,
                          pkt.payload.first_xdata,
                          pkt.payload.first_ydata,
                          pkt.payload.first_zdata});
  // Use some nasty sizeof()'s to get the number of elements in the m2m2_sensor_adxl_data_no_compress_stream_t.adxldata[] array
  for (int i = 0;
       i < sizeof(m2m2_sensor_adxl_data_no_compress_stream_t().adxldata) /
           sizeof(m2m2_sensor_adxl_data_no_compress_stream_t().adxldata[0]);
       i++) {
    current_timestamp += pkt.payload.adxldata[i].incTS;
    data_samples.push_back({current_timestamp/FIRMWARE_TS_PER_MS,
                            pkt.payload.adxldata[i].xdata,
                            pkt.payload.adxldata[i].ydata,
                            pkt.payload.adxldata[i].zdata});
  }

     #if SDK_TIMEOUT == 1

          if (pkt.payload.sequence_num < 12000) {
            this->callback->call(data_samples, pkt.payload.sequence_num);
          }
          else if (pkt.payload.sequence_num == 12000)
          {
            std::cout << "ADI SDK:: Error! Timered out callback for stream: " << std::hex << (int)this->get_address() << std::endl;
          }
     #else
        this->callback->call(data_samples, pkt.payload.sequence_num);
     #endif

}


// #############################################################################
// ## EDA STREAM CALLBACKS                                                    ##
// #############################################################################
/*!
   \brief Dispatch a packet to an eda_stream_callback.
*/
template<>
inline void m2m2_data_stream<eda_stream_callback>::dispatch(std::vector<uint8_t> bytes) {
  if (!this->callback) {
    std::cout << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
              << (int) this->get_address() << std::endl;
    std::cerr << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
              << (int) this->get_address() << std::endl;
    return;
  }
  m2m2_pkt<eda_app_stream_t> pkt;
  std::vector<eda_stream_cb_data_t> data_samples;
  pkt.unpack(bytes);

  ret::device_identifiers device_identifier = get_device_id();

  for (int i = 0; i < sizeof(eda_app_stream_t().eda_data) / sizeof(eda_app_stream_t().eda_data[0]); i++) {

    double admittance_real = 0;
    double admittance_img = 0;
    double impedance_real = 0;
    double impedance_img = 0;
    double admittance_magnitude = 0;
    double admittance_phase = 0;
    double impedance_magnitude = 0;
    double impedance_phase = 0;

    if (pkt.payload.eda_data[i].realdata == 0) {
      pkt.payload.eda_data[i].realdata = 1;
    }
    if (pkt.payload.eda_data[i].imgdata == 0) {
      pkt.payload.eda_data[i].imgdata = 1;
    }

    if (device_identifier == ret::device_identifiers::SENSORBOARD3) {
      admittance_real = pkt.payload.eda_data[i].realdata * pow(10, 3);
      admittance_img = pkt.payload.eda_data[i].imgdata * pow(10, 3);
    } else {
      admittance_real = pkt.payload.eda_data[i].realdata / pow(10, 9);
      admittance_img = pkt.payload.eda_data[i].imgdata / pow(10, 9);
    }
    admittance_magnitude = sqrt(admittance_real * admittance_real + admittance_img * admittance_img);
    admittance_phase = atan2(admittance_img, admittance_real);
    impedance_real = admittance_real / (admittance_real * admittance_real + admittance_img * admittance_img);
    impedance_img = -(admittance_img / (admittance_real * admittance_real + admittance_img * admittance_img));
    impedance_magnitude = 1 / (admittance_magnitude);
    impedance_phase = atan2(impedance_img, impedance_real);

    data_samples.push_back({pkt.payload.eda_data[i].timestamp / FIRMWARE_TS_PER_MS,
                            admittance_real,
                            admittance_img,
                            impedance_real,
                            impedance_img,
                            admittance_magnitude,
                            admittance_phase,
                            impedance_magnitude,
                            impedance_phase
                           });
  }


#if SDK_TIMEOUT == 1
  if (pkt.payload.sequence_num < 4800) {
    this->callback->call(data_samples, pkt.payload.sequence_num);
  }
  else if (pkt.payload.sequence_num == 4800)
  {
    std::cout << "ADI SDK:: Error! Timered out callback for stream: " << std::hex << (int)this->get_address() << std::endl;
  }
#else
  this->callback->call(data_samples, pkt.payload.sequence_num);
#endif
}


// #############################################################################
// ## ECG STREAM CALLBACKS                                                    ##
// #############################################################################
/*!
  \brief Dispatch a packet to an ecg_stream_callback.
*/
template<>
inline void m2m2_data_stream<ecg_stream_callback>::dispatch(std::vector<uint8_t> bytes) {
  if (!this->callback) {
    std::cout << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
              << (int) this->get_address();
    std::cerr << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
              << (int) this->get_address();
    return;
  }
  m2m2_pkt<ecg_app_stream_t> pkt;
  std::vector<ecg_stream_cb_data_t> data_samples;
  ecg_datatype_enum_t ecg_type;
  bool leadsoff;

  pkt.unpack(bytes);
  uint32_t current_timestamp = pkt.payload.timestamp;

  // Map from m2m2 internal types to standard ones
  if (pkt.payload.datatype == M2M2_SENSOR_ECG_MONITOR) {
    ecg_type = ecg_datatype_enum_t::ecg_monitor;
  } else {
    ecg_type = ecg_datatype_enum_t::ecg_sport;
  }

  if (pkt.payload.ecg_info & 0x01) {
    leadsoff = false;
  } else {
    leadsoff = true;
  }

  data_samples.push_back({current_timestamp / FIRMWARE_TS_PER_MS,
                          ecg_type,
                          leadsoff,
                          pkt.payload.HR,
                          pkt.payload.firstecgdata});
  for (int i = 0;
       i < sizeof(ecg_app_stream_t().ecg_data) / sizeof(ecg_app_stream_t().ecg_data[0]);
       i++) {
    current_timestamp += pkt.payload.ecg_data[i].timestamp;
    data_samples.push_back({current_timestamp / FIRMWARE_TS_PER_MS,
                            ecg_type,
                            leadsoff,
                            pkt.payload.HR,
                            pkt.payload.ecg_data[i].ecgdata});
  }

#if SDK_TIMEOUT == 1
  if (pkt.payload.sequence_num < 10910) {
    this->callback->call(data_samples, pkt.payload.sequence_num);
  }
  else if (pkt.payload.sequence_num == 10910)
  {
    std::cout << "ADI SDK:: Error! Timered out callback for stream: " << std::hex << (int)this->get_address() << std::endl;
  }
#else
  this->callback->call(data_samples, pkt.payload.sequence_num);
#endif
}


// #############################################################################
// ## TEMPERATURE STREAM CALLBACKS                                            ##
// #############################################################################
/*!
  \brief Dispatch a packet to a temperature_stream_callback.
*/
template<>
inline void m2m2_data_stream<temperature_stream_callback>::dispatch(std::vector<uint8_t> bytes) {
  if (!this->callback) {
    std::cout << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
              << (int) this->get_address();
    std::cerr << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
              << (int) this->get_address();
    return;
  }
  m2m2_pkt<temperature_app_stream_t> pkt;
  std::vector<temperature_stream_cb_data_t> data_samples;

  pkt.unpack(bytes);
  uint32_t current_timestamp = pkt.payload.nTS;

  // Conversion from raw ADC LSBs to Celcius.
  // This is incredibly firmware/HW dependent.
  float R1 = 39.0;
  float R0 = 47.0;
  float adc_VDD = 2.5;
  float adc_full_scale = 65636;
  float B = 4025;
  float T0 = 25 + 273.16;

  float vAdc = (adc_VDD / adc_full_scale) * pkt.payload.nTemperature1;
  float alpha = (R1 / R0);
  alpha = alpha * (vAdc / (adc_VDD - vAdc));
  float temp_skin = B * T0 / (B + log(alpha) * T0) - 273.15;

  vAdc = (adc_VDD / adc_full_scale) * pkt.payload.nTemperature2;
  alpha = (R1 / R0);
  alpha = alpha * (vAdc / (adc_VDD - vAdc));
  float temp_ambient = B * T0 / (B + log(alpha) * T0) - 273.15;

  data_samples.push_back({current_timestamp / FIRMWARE_TS_PER_MS,
                          pkt.payload.nTemperature1,
                          pkt.payload.nTemperature2});


#if SDK_TIMEOUT == 1

  if (pkt.payload.sequence_num < 1200) {
   this->callback->call(data_samples, pkt.payload.sequence_num);
  }
  else if (pkt.payload.sequence_num == 1200)
  {
   std::cout << "ADI SDK:: Error! Timered out callback for stream: " << std::hex << (int)this->get_address() << std::endl;
  }
#else
  this->callback->call(data_samples, pkt.payload.sequence_num);
#endif
}

// #############################################################################
// ## PPG STREAM CALLBACKS                                                   ##
// #############################################################################
/*!
   \brief Dispatch a packet to an ppg_stream_callback.
*/

template<>
inline void m2m2_data_stream<ppg_stream_callback>::dispatch(std::vector<uint8_t> bytes) {
  if (!this->callback) {
    std::cout << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
              << (int) this->get_address() << std::endl;
    std::cerr << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
              << (int) this->get_address() << std::endl;
    return;
  }
  m2m2_pkt<ppg_app_hr_debug_stream_t> pkt;
  pkt.unpack(bytes);
  std::vector<ppg_stream_cb_data_t> data_samples;
  uint32_t current_timestamp = pkt.payload.timestamp;

  data_samples.push_back({current_timestamp / FIRMWARE_TS_PER_MS,
                          pkt.payload.adpdlibstate,
                             // 16Q4 fixed point to float
                          pkt.payload.hr / 16.0f,
                             // 16Q10 fixed point to float, and turn into a %
                          100.0f * (pkt.payload.confidence / 1024.0f),
                          pkt.payload.hr_type,
                          pkt.payload.rr_interval});

#if SDK_TIMEOUT == 1

  if (pkt.payload.sequence_num < 1200) {
      this->callback->call(data_samples, pkt.payload.sequence_num);
  }
  else if (pkt.payload.sequence_num == 1200)
  {
      std::cout << "ADI SDK:: Error! Timered out callback for stream: " << std::hex << (int)this->get_address() << std::endl;
  }

#else
  this->callback->call(data_samples, pkt.payload.sequence_num);
#endif

}
// #############################################################################
// ## ADPD STREAM CALLBACKS                                                   ##
// #############################################################################
/*!
   \brief Dispatch a packet to an ppg_stream_callback.
//*/
//
//template<>
//inline void m2m2_data_stream<adpd_stream_callback>::dispatch(std::vector<uint8_t> bytes) {
//	if (!this->callback) {
//		std::cout << "ADI SDK:: Error! Undefined callback for stream: " << std::hex << (int)this->get_address() << std::endl;
//		std::cerr << "ADI SDK:: Error! Undefined callback for stream: " << std::hex << (int)this->get_address() << std::endl;
//		return;
//	}
//	// Peek at the data type of the packet to make sure that we can handle it
//	adpd_data_header_t *ptr = (adpd_data_header_t*)&bytes[offsetof(m2m2_hdr_t, data)];
//#if Debug
//	std::cout << "stream callback5:" << ptr->data_type << std::endl;
//#endif
//	if (ptr->data_type == M2M2_SENSOR_RAW_DATA_ADPD_A_SUM_32b
//		|| ptr->data_type == M2M2_SENSOR_RAW_DATA_ADPD_B_SUM_32b) {
//#if Debug
//		std::cout << "stream callback4:" << ptr->data_type << std::endl;
//#endif
//
//		m2m2_pkt<m2m2_sensor_adpd_data_sum32_no_compress_stream_t> pkt;
//		pkt.unpack(bytes);
//
//		std::vector<adpd_stream_cb_data_t_sum32> data_samples;
//		uint32_t current_timestamp = pkt.payload.timestamp;
//
//		data_samples.push_back({ static_cast<ADPD_RAW_DATA_TYPES_ENUM_t>(ptr->data_type), current_timestamp / FIRMWARE_TS_PER_MS,
//			pkt.payload.first_adpddata});
//		for (unsigned int i = 0;
//			i < sizeof(m2m2_sensor_adpd_data_sum32_no_compress_stream_t().adpddata) / sizeof(m2m2_sensor_adpd_data_sum32_no_compress_stream_t().adpddata[0]);
//			i++) {
//			current_timestamp += pkt.payload.adpddata[i].incTS;
//			data_samples.push_back({ static_cast<ADPD_RAW_DATA_TYPES_ENUM_t>(ptr->data_type), current_timestamp / FIRMWARE_TS_PER_MS,
//				pkt.payload.adpddata[i].sum32data});
//		}
//	#if SDK_TIMEOUT == 1
//
//		if (pkt.payload.sequence_num < 8372) {
//			this->callback->call(data_samples, pkt.payload.sequence_num);
//		}
//		else if (pkt.payload.sequence_num == 8372)
//		{
//			std::cout << "ADI SDK:: Error! Timered out callback for stream: " << std::hex << (int)this->get_address() << std::endl;
//		}
//	#else
//		this->callback->call(data_samples, pkt.payload.sequence_num);
//	#endif
//	}
//
//	 if (ptr->data_type == M2M2_SENSOR_RAW_DATA_ADPD_A_SUM_16b
//		|| ptr->data_type == M2M2_SENSOR_RAW_DATA_ADPD_B_SUM_16b) 
//	{
//#if Debug
//		 std::cout << "stream callback3:" << ptr->data_type << std::endl;
//#endif
//
//		m2m2_pkt<m2m2_sensor_adpd_data_sum16_no_compress_stream_t> pkt;
//		pkt.unpack(bytes);
//
//		std::vector<adpd_stream_cb_data_t_sum16> data_samples;
//		uint32_t current_timestamp = pkt.payload.timestamp;
//
//		data_samples.push_back({ static_cast<ADPD_RAW_DATA_TYPES_ENUM_t>(ptr->data_type), current_timestamp / FIRMWARE_TS_PER_MS,
//			pkt.payload.first_adpddata });
//		for (unsigned int i = 0;
//			i < sizeof(m2m2_sensor_adpd_data_sum16_no_compress_stream_t().adpddata) / sizeof(m2m2_sensor_adpd_data_sum16_no_compress_stream_t().adpddata[0]);
//			i++) {
//			current_timestamp += pkt.payload.adpddata[i].incTS;
//			data_samples.push_back({ static_cast<ADPD_RAW_DATA_TYPES_ENUM_t>(ptr->data_type), current_timestamp / FIRMWARE_TS_PER_MS,
//				pkt.payload.adpddata[i].sum16data});
//		}
//#if SDK_TIMEOUT == 1
//
//		if (pkt.payload.sequence_num < 8372) {
//			this->callback->call(data_samples, pkt.payload.sequence_num);
//		}
//		else if (pkt.payload.sequence_num == 8372)
//		{
//			std::cout << "ADI SDK:: Error! Timered out callback for stream: " << std::hex << (int)this->get_address() << std::endl;
//		}
//#else
//		this->callback->call(data_samples, pkt.payload.sequence_num);
//#endif
//	}
//	if (ptr->data_type == M2M2_SENSOR_RAW_DATA_ADPD_A_4CH_16b
//		|| ptr->data_type == M2M2_SENSOR_RAW_DATA_ADPD_B_4CH_16b)
//	{
//#if Debug
//		std::cout << "stream callback2:" << ptr->data_type << std::endl;
//#endif
//
//		m2m2_pkt<m2m2_sensor_adpd_data_4ch16_no_compress_stream_t> pkt;
//		pkt.unpack(bytes);
//
//		std::vector<adpd_stream_cb_data_t_4CH16> data_samples;
//		std::vector<uint16_t> first_adpddatas;
//
//		double ch_timestamp;
//		std::vector<uint16_t> ch_data;
//		double first_timestamp = pkt.payload.timestamp;
//
//		//first set of data
//			ch_data.push_back(pkt.payload.first_adpddata.ch1);
//			ch_data.push_back(pkt.payload.first_adpddata.ch2);
//			ch_data.push_back(pkt.payload.first_adpddata.ch3);
//			ch_data.push_back(pkt.payload.first_adpddata.ch4);
//	
//
//		
//		////std::vector<uint8_t> byte_Streams;
//
//		//for (int i = 0; i < sizeof(pkt.payload.byte_stream); i++) {
//		//	byte_Streams.push_back(pkt.payload.byte_stream[i]);
//		//}
//
//
//			data_samples.push_back({ static_cast<ADPD_RAW_DATA_TYPES_ENUM_t>(ptr->data_type), first_timestamp / FIRMWARE_TS_PER_MS,
//				ch_data });
//		{
//			for (unsigned int i = 0;
//				i < sizeof(m2m2_sensor_adpd_data_4ch16_no_compress_stream_t().adpddata) / sizeof(m2m2_sensor_adpd_data_4ch16_no_compress_stream_t().adpddata[0]);
//				i++) {
//				ch_data.clear();
//				first_timestamp += pkt.payload.adpddata[i].incTS;
//				ch_data.push_back(pkt.payload.adpddata[i].ch16data.ch1);
//				ch_data.push_back(pkt.payload.adpddata[i].ch16data.ch2);
//				ch_data.push_back(pkt.payload.adpddata[i].ch16data.ch3);
//				ch_data.push_back(pkt.payload.adpddata[i].ch16data.ch4);
//				data_samples.push_back({ static_cast<ADPD_RAW_DATA_TYPES_ENUM_t>(ptr->data_type), first_timestamp / FIRMWARE_TS_PER_MS,
//					ch_data });
//			}
//#if SDK_TIMEOUT == 1
//
//			if (pkt.payload.sequence_num < 8372) {
//				this->callback->call(data_samples, pkt.payload.sequence_num);
//			}
//			else if (pkt.payload.sequence_num == 8372)
//			{
//				std::cout << "ADI SDK:: Error! Timered out callback for stream: " << std::hex << (int)this->get_address() << std::endl;
//			}
//#else
//			this->callback->call(data_samples, pkt.payload.sequence_num);
//#endif
//		}
//
//	}
//	if (ptr->data_type == M2M2_SENSOR_RAW_DATA_ADPD_A_4CH_32b
//		|| ptr->data_type == M2M2_SENSOR_RAW_DATA_ADPD_B_4CH_32b)
//	{
//
//#if Debug
//		std::cout << "stream callback" << ptr->data_type << std::endl;
//#endif
//		m2m2_pkt<m2m2_sensor_adpd_data_4ch32_no_compress_stream_t> pkt;
//		pkt.unpack(bytes);
//
//		std::vector<adpd_stream_cb_data_t_4CH32> data_samples;
//		double ch_timestamp;
//		std::vector<uint32_t> ch_data;
//		double first_timestamp = pkt.payload.timestamp;
//
//		//first set of data
//		ch_data.push_back(pkt.payload.first_adpddata.ch1);
//		ch_data.push_back(pkt.payload.first_adpddata.ch2);
//		ch_data.push_back(pkt.payload.first_adpddata.ch3);
//		ch_data.push_back(pkt.payload.first_adpddata.ch4);
//
//
//
//		////std::vector<uint8_t> byte_Streams;
//
//		//for (int i = 0; i < sizeof(pkt.payload.byte_stream); i++) {
//		//	byte_Streams.push_back(pkt.payload.byte_stream[i]);
//		//}
//
//
//		data_samples.push_back({ static_cast<ADPD_RAW_DATA_TYPES_ENUM_t>(ptr->data_type), first_timestamp / FIRMWARE_TS_PER_MS,
//			ch_data });
//		{
//			for (unsigned int i = 0;
//				i < sizeof(m2m2_sensor_adpd_data_4ch32_no_compress_stream_t().adpddata) / sizeof(m2m2_sensor_adpd_data_4ch32_no_compress_stream_t().adpddata[0]);
//				i++) {
//				ch_data.clear();
//				first_timestamp += pkt.payload.adpddata[i].incTS;
//				ch_data.push_back(pkt.payload.adpddata[i].ch32data.ch1);
//				ch_data.push_back(pkt.payload.adpddata[i].ch32data.ch2);
//				ch_data.push_back(pkt.payload.adpddata[i].ch32data.ch3);
//				ch_data.push_back(pkt.payload.adpddata[i].ch32data.ch4);
//				data_samples.push_back({ static_cast<ADPD_RAW_DATA_TYPES_ENUM_t>(ptr->data_type),first_timestamp / FIRMWARE_TS_PER_MS,
//					ch_data });
//			}
//#if SDK_TIMEOUT == 1
//
//			if (pkt.payload.sequence_num < 8372) {
//				this->callback->call(data_samples, pkt.payload.sequence_num);
//			}
//			else if (pkt.payload.sequence_num == 8372)
//			{
//				std::cout << "ADI SDK:: Error! Timered out callback for stream: " << std::hex << (int)this->get_address() << std::endl;
//			}
//#else
//			this->callback->call(data_samples, pkt.payload.sequence_num);
//#endif
//		}
//
//	}
//}

// #############################################################################
// ## Sync PPG STREAM CALLBACKS                                               ##
// #############################################################################
/*!
 \brief Dispatch a packet to an sync_ppg_stream_callback.
 */

template<>
inline void m2m2_data_stream<syncppg_stream_callback>::dispatch(std::vector<uint8_t> bytes) {
  if (!this->callback) {
    std::cout << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
              << (int) this->get_address() << std::endl;
    std::cerr << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
              << (int) this->get_address() << std::endl;
    return;
  }
  m2m2_pkt<adpd_adxl_sync_data_stream_t> pkt;
  pkt.unpack(bytes);
  std::vector<syncppg_stream_cb_data_t> data_samples;
  uint32_t current_timestamp = pkt.payload.syncData.ppgTS;

  for (unsigned int i = 0;
       i < sizeof(adpd_adxl_sync_data_format_t().ppgData) /
           sizeof(adpd_adxl_sync_data_format_t().ppgData[0]);
       i++) {
    data_samples.push_back({current_timestamp / FIRMWARE_TS_PER_MS,
                            pkt.payload.syncData.ppgData[i],
                            pkt.payload.syncData.xData[i],
                            pkt.payload.syncData.yData[i],
                            pkt.payload.syncData.zData[i]});
    current_timestamp += pkt.payload.syncData.incPpgTS[i];
  }

#if SDK_TIMEOUT == 1

  if (pkt.payload.sequence_num < 15000) {
    this->callback->call(data_samples, pkt.payload.sequence_num);
  }
  else if (pkt.payload.sequence_num == 15000)
  {
      std::cout << "ADI SDK:: Error! Timered out callback for stream: " << std::hex << (int)this->get_address() << std::endl;
  }
#else
  this->callback->call(data_samples, pkt.payload.sequence_num);
#endif

}
// #############################################################################
// ## ADPD4000 STREAM CALLBACKS                                                   ##
// #############################################################################
/*!
\brief Dispatch a packet to an adpdcl_stream_callback.
*/

template<>
inline void m2m2_data_stream<adpd4000_stream_callback>::dispatch(std::vector<uint8_t> bytes) {
	if (!this->callback) {
		std::cout << "ADI SDK:: Error! Undefined callback for stream: " << std::hex << (int)this->get_address() << std::endl;
		std::cerr << "ADI SDK:: Error! Undefined callback for stream: " << std::hex << (int)this->get_address() << std::endl;
		return;
	}
	m2m2_hdr_t *hdr = reinterpret_cast<m2m2_hdr_t *> (&bytes[0]);

	uint16_t source = BYTE_SWAP_16(hdr->src);

	if (source == M2M2_ADDR_SENSOR_ADPD_OPTIONAL_BYTES_STREAM)
	{
		
		uint16_t seq_no;
		m2m2_pkt<m2m2_sensor_fifo_status_bytes_t> packet;
		packet.unpack(bytes);
		seq_no = packet.payload.sequence_num;
		std::vector<adpd4000_optional_stream_cb_data_t> data_samples;

		data_samples.push_back({ ADPD_SENSOR_INTERRUPT, packet.payload.timestamp / FIRMWARE_TS_PER_MS, packet.payload.data_int, packet.payload.level0_int, packet.payload.level1_int, packet.payload.tia_ch1_int, packet.payload.tia_ch2_int });

		this->callback->call(data_samples, seq_no);

	}
	else
	{
		m2m2_pkt<m2m2_sensor_adpd4000_data_stream_t> packet;
		packet.unpack(bytes);
		int i = 0;
		int j = 0;
		uint16_t Dataformat;
		uint8_t Impulse;
		uint8_t Dark;
		uint8_t Signal;
		uint8_t Lit;
		uint16_t seq_no;
		uint8_t Channel_num;
		int stream_num;
		uint32_t Current_timestamp;
		uint32_t timestamp;
		uint8_t sample_num;
		uint8_t source_stream = 0;
		std::vector<uint32_t> adpddata_D;
		std::vector<uint32_t> adpddata_S;
		std::vector<uint32_t> adpddata_L;

		std::vector<adpd4000_stream_cb_data_t> data_samples;
		seq_no = packet.payload.sequence_num;
		//seq_no = (UINT16)assemble_uint(packet.payload.sequence_num);
		m2m2_hdr_t *packet_source = reinterpret_cast<m2m2_hdr_t *> (&bytes[0]);
		source_stream = (int)BYTE_SWAP_16(packet_source->src) - (int)M2M2_ADDR_ENUM_t::M2M2_ADDR_SENSOR_ADPD_STREAM1 + 1;

		Dataformat = (short)packet.payload.data_format;
		Dark = (short)((Dataformat & 0xF0) >> 4);
		Signal = (short)((Dataformat & 0x0F));
		Impulse = (short)((Dataformat >> 8) & 0x000F);
		Lit = (short)((Dataformat >> 12) & 0x000F);

		Channel_num = packet.payload.channel_num;
		Current_timestamp = packet.payload.timestamp;
		sample_num = packet.payload.sample_num;

		for (int count = 0; count < sample_num; count++)
		{
			switch (Dark)
			{
			case 0:
				break;
			case 1:
				adpddata_D.push_back(assemble_int(packet.payload.adpddata[i++]));
				break;
			case 2:
				adpddata_D.push_back(assemble_int(packet.payload.adpddata[i], packet.payload.adpddata[i + 1]));
				i += 2;
				break;
			case 3:
				adpddata_D.push_back(assemble_signedint(packet.payload.adpddata[i], packet.payload.adpddata[i + 1], packet.payload.adpddata[i + 2]));
				i += 3;
				break;
			case 4:
				adpddata_D.push_back(assemble_int(packet.payload.adpddata[i], packet.payload.adpddata[i + 1], packet.payload.adpddata[i + 2], packet.payload.adpddata[i + 3]));
				i += 4;
				break;
			}
			switch (Signal)
			{
			case 0:
				break;
			case 1:
				adpddata_S.push_back(assemble_int(packet.payload.adpddata[i++]));
				break;
			case 2:
				adpddata_S.push_back(assemble_int(packet.payload.adpddata[i], packet.payload.adpddata[i + 1]));
				i += 2;
				break;
			case 3:
				adpddata_S.push_back(assemble_signedint(packet.payload.adpddata[i], packet.payload.adpddata[i + 1], packet.payload.adpddata[i + 2]));
				i += 3;
				break;
			case 4:
				adpddata_S.push_back(assemble_int(packet.payload.adpddata[i], packet.payload.adpddata[i + 1], packet.payload.adpddata[i + 2], packet.payload.adpddata[i + 3]));
				i += 4;
				break;
			}
			switch (Lit)
			{
			case 0:
				break;
			case 1:
				adpddata_L.push_back(assemble_int(packet.payload.adpddata[i++]));
				break;
			case 2:
				adpddata_L.push_back(assemble_int(packet.payload.adpddata[i], packet.payload.adpddata[i + 1]));
				i += 2;
				break;
			case 3:
				adpddata_L.push_back(assemble_signedint(packet.payload.adpddata[i], packet.payload.adpddata[i + 1], packet.payload.adpddata[i + 2]));
				i += 3;
				break;
			case 4:
				adpddata_L.push_back(assemble_int(packet.payload.adpddata[i], packet.payload.adpddata[i + 1], packet.payload.adpddata[i + 2], packet.payload.adpddata[i + 3]));
				i += 4;
				break;
			}

		}
		data_samples.push_back({ static_cast<ADDR_SENSOR_STREAM_ENUM_t>(source_stream),Dark,Signal,Impulse,Lit,Channel_num,Current_timestamp / FIRMWARE_TS_PER_MS,adpddata_D,adpddata_S,adpddata_L});

		this->callback->call(data_samples, packet.payload.sequence_num);
	}

#if Debug
	std::cout << "stream callback5:" << ptr->data_type << std::endl;
#endif

} 


// #############################################################################
// ## BATTERY STREAM CALLBACKS                                                ##
// #############################################################################
/*!
 \brief Dispatch a packet to an battery_stream_callback.
 */
template<>
inline void m2m2_data_stream<battery_stream_callback>::dispatch(std::vector<uint8_t> bytes) {
  if (!this->callback) {
    std::cout << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
    << (int) this->get_address();
    std::cerr << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
    << (int) this->get_address();
    return;
  }
  m2m2_pkt<m2m2_pm_sys_bat_info_resp_t> pkt;
  std::vector<battery_stream_cb_data_t> data_samples;
  
  pkt.unpack(bytes);
  uint32_t current_timestamp = pkt.payload.timestamp;
  
  data_samples.push_back({current_timestamp / FIRMWARE_TS_PER_MS,
    static_cast<PM_SYS_BAT_STATE_ENUM_t>(pkt.payload.bat_chrg_stat),
    pkt.payload.bat_lvl,
    pkt.payload.bat_mv,
    0});
  
  this->callback->call(data_samples);
  
}

// #############################################################################
// ## PING STREAM CALLBACKS				                                       ##
// #############################################################################
/*!
 \brief Dispatch a packet to an ping_stream_callback.
 */
template<>
inline void m2m2_data_stream<ping_stream_callback>::dispatch(std::vector<uint8_t> bytes) {
	if (!this->callback) {
		std::cout << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
			<< (int)this->get_address();
		std::cerr << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
			<< (int)this->get_address();
		return;
	}
	m2m2_hdr_t *hdr = reinterpret_cast<m2m2_hdr_t *> (&bytes[0]);

	uint16_t length = BYTE_SWAP_16(hdr->length);


	m2m2_pkt<m2m2_app_common_ping_t> pkt;
	std::vector<ping_stream_cb_data_t> data_samples;

	pkt.unpack(bytes);

	data_samples.push_back({ length });

	this->callback->call(data_samples, pkt.payload.sequence_num);

}



// #############################################################################
// ## FS STREAM CALLBACKS                                                     ##
// #############################################################################
/*!
 \brief Dispatch a packet to an sync_ppg_stream_callback.
 */

template<>
inline void m2m2_data_stream<fs_ls_callback>::dispatch(std::vector<uint8_t> bytes) {
  if (!this->callback) {
    std::cout << "ADI SDK:: Error! Undefined callback for stream: " << std::hex << (int)this->get_address() << std::endl;
    std::cerr << "ADI SDK:: Error! Undefined callback for stream: " << std::hex << (int)this->get_address() << std::endl;
    return;
  }
  m2m2_pkt<m2m2_file_sys_ls_resp_t> pkt;
  pkt.unpack(bytes);
  std::vector<fs_ls_data_t> data_sample;
  std::string filename = std::string(reinterpret_cast<char *>(pkt.payload.full_file_name),
                                     strlen((const char *)pkt.payload.full_file_name));
  data_sample.push_back({pkt.payload.status,
    filename,
    pkt.payload.filetype,
    pkt.payload.filesize});
  
  this->callback->call(data_sample);
  if (pkt.payload.status == M2M2_FILE_SYS_END_OF_DIR) {
    this->callback = NULL;
  }
}

// #############################################################################
// ## PEDOMETER STREAM CALLBACKS                                              ##
// #############################################################################
/*!
 \brief Dispatch a packet to an pedometer_stream_callback.
 */

template<>
inline void m2m2_data_stream<pedometer_stream_callback>::dispatch(std::vector<uint8_t> bytes) {
  if (!this->callback) {
    std::cout << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
              << (int) this->get_address() << std::endl;
    std::cerr << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
              << (int) this->get_address() << std::endl;
    return;
  }
  m2m2_pkt<pedometer_app_stream_t> pkt;
  pkt.unpack(bytes);
  std::vector<pedometer_stream_cb_data_t> data_samples;
  uint32_t current_timestamp = pkt.payload.nTimeStamp;

  data_samples.push_back({pkt.payload.nNumSteps,
                          pkt.payload.nAlgoStatus,
                          current_timestamp / FIRMWARE_TS_PER_MS,
                          pkt.payload.nReserved});

#if SDK_TIMEOUT == 1
  if (pkt.payload.sequence_num < 1200) {
    this->callback->call(data_samples, pkt.payload.sequence_num);
  }
  else if (pkt.payload.sequence_num == 1200)
  {
    std::cout << "ADI SDK:: Error! Timered out callback for stream: " << std::hex << (int)this->get_address() << std::endl;
  }
#else
  this->callback->call(data_samples, pkt.payload.sequence_num);
#endif
}

// #############################################################################
// ## FS STREAM CALLBACKS                                                     ##
// #############################################################################
/*!
 \brief Dispatch a packet to an fs_stream_callback.
 */

template<>
inline void m2m2_data_stream<fs_stream_callback>::dispatch(std::vector<uint8_t> bytes) {
  if (!this->callback) {
    std::cout << "ADI SDK:: Error! Undefined callback for stream: " << std::hex << (int)this->get_address() << std::endl;
    std::cerr << "ADI SDK:: Error! Undefined callback for stream: " << std::hex << (int)this->get_address() << std::endl;
    return;
  }
  m2m2_pkt<m2m2_file_sys_download_log_stream_t> pkt;
  pkt.unpack(bytes);
  std::vector<fs_stream_cb_data_t> data_samples;
  
  std::vector<uint8_t> byte_Streams;
  
  for (int i = 0; i < sizeof(pkt.payload.byte_stream); i++) {
    byte_Streams.push_back(pkt.payload.byte_stream[i]);
  }
  
  data_samples.push_back({pkt.payload.status,
    pkt.payload.len_stream,
    byte_Streams,
    pkt.payload.crc16});
  
  this->callback->call(data_samples);
  
  if (pkt.payload.status == M2M2_FILE_SYS_END_OF_FILE) {
    this->callback = NULL;
  }
  
}
// #############################################################################
// ## BCM STREAM CALLBACKS                                                    ##
// #############################################################################


/*!
   \brief Dispatch a packet to an bcm_stream_callback.
*/
template<>
inline void m2m2_data_stream<bcm_stream_callback>::dispatch(std::vector<uint8_t> bytes) {
  if (!this->callback) {
    std::cout << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
              << (int) this->get_address() << std::endl;
    std::cerr << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
              << (int) this->get_address() << std::endl;
    return;
  }

  m2m2_pkt<bcm_app_stream_t> pkt;
  std::vector<bcm_stream_cb_data_t> data_samples;
  pkt.unpack(bytes);

  for (int i = 0; i < sizeof(bcm_app_stream_t().bcm_data) / sizeof(bcm_app_stream_t().bcm_data[0]); i++) {
    double admittance_real = 0;
    double admittance_img = 0;
    double admittance_magnitude = 0;
    double admittance_phase = 0;
    double impedance_real = 0;
    double impedance_img = 0;
    double previous_timestamp = 0;
    double impedance_magnitude = 0;
    double impedance_phase = 0;
    uint8_t is_finger_on_leads = 0;
    uint8_t signal_stability = 0;

    if (pkt.payload.bcm_data[i].real == 0) {
      pkt.payload.bcm_data[i].real = 1;
    }

    if (pkt.payload.bcm_data[i].img == 0) {
      pkt.payload.bcm_data[i].img = 1;
    }

    impedance_real = ((double) pkt.payload.bcm_data[i].real) / 1024;
    impedance_img = ((double) pkt.payload.bcm_data[i].img) / 1024;

    if (((impedance_real > 100) && (impedance_real < 5000)) && ((impedance_img > -500) && (impedance_img < 100))) {
      is_finger_on_leads = 1; // set the electrode touched status = 1
    } else {
      is_finger_on_leads = 0;
    }
    int ohmsdiff = 0;

	if (is_finger_on_leads == 1)
	{
		mImpedanceList.push_back(impedance_real);
		if (mImpedanceList.size() == 20)  //define 20 in macro
		{
			ohmsdiff = abs(mImpedanceList[0] - mImpedanceList[mImpedanceList.size() - 1]);
			mImpedanceList.clear();
			if (ohmsdiff < BCM_STABILITY_OHMS_DIFF) //Checking ohms difference
			{
				if (++g_samplecount >= BCM_STABILITY_SAMPLES_COUNT) //counting samples
				{
					signal_stability = 1; // set signal stability
				}
			}
			else
			{
				g_samplecount = 0;
				signal_stability = 0;
			}
		}
	}
	else
	{
		mImpedanceList.clear();
		g_samplecount = 0;
		signal_stability = 0;
	}

    admittance_real = impedance_real / (impedance_real * impedance_real + impedance_img * impedance_img);
    admittance_img = (-impedance_img) / (impedance_real * impedance_real + impedance_img * impedance_img);
    admittance_magnitude = sqrt(admittance_real * admittance_real + admittance_img * admittance_img);
    admittance_phase = atan2(admittance_img, admittance_real);
    admittance_phase = admittance_phase * (180 / 3.14); // in degrees

    impedance_magnitude = sqrt(impedance_real * impedance_real + impedance_img * impedance_img);
    impedance_phase = atan2(impedance_img, impedance_real);
    impedance_phase = impedance_phase * (180 / 3.14); // in degrees

    data_samples.push_back({pkt.payload.bcm_data[i].timestamp / FIRMWARE_TS_PER_MS,
                            (int32_t) impedance_real,
                            (int32_t) impedance_img,
                            is_finger_on_leads,
                            signal_stability,
                            impedance_magnitude,
                            impedance_phase,
                            admittance_magnitude,
                            admittance_phase,
                           });
  }

#if SDK_TIMEOUT == 1
  if (pkt.payload.sequence_num < 4800) {
            this->callback->call(data_samples, pkt.payload.sequence_num);
          }
          else if (pkt.payload.sequence_num == 4800)
          {
            std::cout << "ADI SDK:: Error! Timered out callback for stream: " << std::hex << (int)this->get_address() << std::endl;
          }
#else
  this->callback->call(data_samples, pkt.payload.sequence_num);
#endif
}

// #############################################################################
// ## HRV STREAM CALLBACKS                                                    ##
// #############################################################################
/*!
   \brief Dispatch a packet to an bcm_stream_callback.
*/
//template<>
//inline void m2m2_data_stream<hrv_stream_callback>::dispatch(std::vector<uint8_t> bytes) {
//  if (!this->callback) {
//    std::cout << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
//              << (int) this->get_address() << std::endl;
//    std::cerr << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
//              << (int) this->get_address() << std::endl;
//    return;
//  }
//  m2m2_pkt<ppg_app_hrv_info_t> pkt;
//  std::vector<hrv_stream_cb_data_t> data_samples;
//  pkt.unpack(bytes);
//
//  data_samples.push_back({pkt.payload.timestamp / FIRMWARE_TS_PER_MS,
//                          pkt.payload.first_rr_interval,
//                          pkt.payload.first_is_gap,
//                         });
//
//  for (int i = 0;
//       i < sizeof(ppg_app_hrv_info_t().hrv_data) / sizeof(ppg_app_hrv_info_t().hrv_data[0]); i++) {
//    data_samples.push_back({pkt.payload.hrv_data[i].timestamp / FIRMWARE_TS_PER_MS,
//                            pkt.payload.hrv_data[i].rr_interval,
//                            pkt.payload.hrv_data[i].is_gap,
//                           });
//  }
//
//#if SDK_TIMEOUT == 1
//  if (pkt.payload.sequence_num < 4800) {
//            this->callback->call(data_samples, pkt.payload.sequence_num);
//          }
//          else if (pkt.payload.sequence_num == 4800)
//          {
//            std::cout << "ADI SDK:: Error! Timered out callback for stream: " << std::hex << (int)this->get_address() << std::endl;
//          }
//#else
//  this->callback->call(data_samples, pkt.payload.sequence_num);
//#endif
//}

// #############################################################################
// ## AGC STREAM CALLBACKS                                                    ##
// #############################################################################
/*!
   \brief Dispatch a packet to an bcm_stream_callback.
*/
template<>
inline void m2m2_data_stream<agc_stream_callback>::dispatch(std::vector<uint8_t> bytes) {
  if (!this->callback) {
    std::cout << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
              << (int) this->get_address() << std::endl;
    std::cerr << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
              << (int) this->get_address() << std::endl;
    return;
  }
  m2m2_pkt<ppg_app_agc_info_t> pkt;
  std::vector<agc_stream_cb_data_t> data_samples;
  pkt.unpack(bytes);

  data_samples.push_back({pkt.payload.timestamp / FIRMWARE_TS_PER_MS,
                          pkt.payload.mts[0],
                          pkt.payload.mts[1],
                          pkt.payload.mts[2],
                          pkt.payload.mts[3],
                          pkt.payload.mts[4],
                          pkt.payload.mts[5],
                          pkt.payload.setting[0],
                          pkt.payload.setting[1],
                          pkt.payload.setting[2],
                          pkt.payload.setting[3],
                          pkt.payload.setting[4],
                          pkt.payload.setting[5],
                          pkt.payload.setting[6],
                          pkt.payload.setting[7],
                          pkt.payload.setting[8],
                          pkt.payload.setting[9],
                         });

#if SDK_TIMEOUT == 1
  if (pkt.payload.sequence_num < 4800) {
            this->callback->call(data_samples, pkt.payload.sequence_num);
          }
          else if (pkt.payload.sequence_num == 4800)
          {
            std::cout << "ADI SDK:: Error! Timered out callback for stream: " << std::hex << (int)this->get_address() << std::endl;
          }
#else
  this->callback->call(data_samples, pkt.payload.sequence_num);
#endif
}



// #############################################################################
// ##		SQI STREAM CALLBACKS                                               ##
// #############################################################################
/*!
 \brief Dispatch a packet to an sqi_stream_callback.
 */

template<>
inline void m2m2_data_stream<sqi_stream_callback>::dispatch(std::vector<uint8_t> bytes) {
	if (!this->callback) {
		std::cout << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
			<< (int)this->get_address() << std::endl;
		std::cerr << "ADI SDK:: Error! Undefined callback for stream: " << std::hex
			<< (int)this->get_address() << std::endl;
		return;
	}
	m2m2_pkt<sqi_app_stream_t> pkt;
	pkt.unpack(bytes);
	std::vector<sqi_stream_cb_data_t> data_samples;
	uint32_t current_timestamp = pkt.payload.nTimeStamp;

	
		data_samples.push_back({ pkt.payload.sqi, pkt.payload.nSQISlot, pkt.payload.nAlgoStatus ,current_timestamp / FIRMWARE_TS_PER_MS,
								pkt.payload.nReserved });
		current_timestamp += pkt.payload.nTimeStamp;
	

#if SDK_TIMEOUT == 1

	if (pkt.payload.sequence_num < 15000) {
		this->callback->call(data_samples, pkt.payload.sequence_num);
	}
	else if (pkt.payload.sequence_num == 15000)
	{
		std::cout << "ADI SDK:: Error! Timered out callback for stream: " << std::hex << (int)this->get_address() << std::endl;
	}
#else
	this->callback->call(data_samples, pkt.payload.sequence_num);
#endif

}
