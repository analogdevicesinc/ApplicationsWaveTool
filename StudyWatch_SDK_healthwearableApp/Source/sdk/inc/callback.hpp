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

#include <vector>
#include "stdint.h"
#include <sstream>
#include "m2m2/cpp/system_interface.hpp"
#include "m2m2/cpp/sensor_adpd_application_interface.hpp"
#include"sys_alert_enum.hpp"
// #############################################################################
// ## WATCH CALLBACKS                                                         ##
// #############################################################################
/*!
   \brief Defines a callback object to be used when registering a callback within the SDK.
   This callback also notifies the System Alerts such as Memory Limit Reached, Low Battery Warning.

   This object should be subclassed with an implementation of the watch_phy_callback::call method inorder to send the M2M2 pkt to device
   and watch_phy_callback::sys_alert_call method inorder to receive the system alerts from device to SDK then to notify the user application.
*/
class watch_phy_callback {
public:
  virtual ~watch_phy_callback(void);
  virtual void call(std::vector<uint8_t>) = 0;
  virtual void sys_alert_call(uint8_t) = 0;
};


// #############################################################################
// ## ADXL STREAM CALLBACKS                                                   ##
// #############################################################################
/*!
   \brief A callback struct to be used with the ADXL data stream.
*/
struct adxl_stream_cb_data_t {
  double  timestamp;  //!< System timestamp in milliseconds
  int16_t x;          //!< X-axis accelerometer value
  int16_t y;          //!< Y-axis accelerometer value
  int16_t z;          //!< Z-acis accelerometer value
};

/*!
   \brief A callback class to be used with the ADXL data stream.
*/
class adxl_stream_callback {
public:
  virtual ~adxl_stream_callback(void);
  virtual void call(std::vector<adxl_stream_cb_data_t>, uint16_t sequence_num);
};

// #############################################################################
// ## EDA STREAM CALLBACKS                                                   ##
// #############################################################################
/*!
   \brief A callback struct to be used with the EDA data stream.
*/
struct eda_stream_cb_data_t {
  double timestamp;  //!< System timestamp in milliseconds
  double admittance_real;
  double admittance_img;
  double impedance_real;
  double impedance_img;
  double admittance_magnitude;
  double admittance_phase;
  double impedance_magnitude;
  double impedance_phase;
};

class eda_stream_callback {
public:
  virtual ~eda_stream_callback(void);

  virtual void call(std::vector<eda_stream_cb_data_t>, uint16_t sequence_num);
};

// #############################################################################
// ## ECG STREAM CALLBACKS                                                    ##
// #############################################################################
/*!
   \brief An enum used to specify if a particular data sample is from SPORT mode ECG
   or MONITOR mode ECG.
*/
enum ecg_datatype_enum_t : uint8_t {
  ecg_monitor = 0,
  ecg_sport = 1,
};

/*!
   \brief A callback struct to be used with the temperature data stream.
*/
struct ecg_stream_cb_data_t {
  double timestamp;  //!< System timestamp in milliseconds
  ecg_datatype_enum_t datatype;   //!< ECG device type
  bool leadsoff;   //!< ECG lead detection
  uint8_t HR;         //!< ECG algorithm heart rate
  uint16_t ecg_data;   //!< ECG data value
};

/*!
   \brief A callback class to be used with the ECG data stream.
*/
class ecg_stream_callback {
public:
  virtual ~ecg_stream_callback(void);

  virtual void call(std::vector<ecg_stream_cb_data_t>, uint16_t sequence_num);
};
// #############################################################################
// ## TEMPERATURE STREAM CALLBACKS                                            ##
// #############################################################################
/*!
   \brief A callback struct to be used with the temperature data stream.
*/
struct temperature_stream_cb_data_t {
  double timestamp;    //!< System timestamp in milliseconds
  uint16_t temp_skin;    //!< Skin temperature in degrees Celcius
  uint16_t temp_ambient; //!< Ambient temperature in degrees Celcius
};

/*!
   \brief A callback class to be used with the temperature data stream.
*/
class temperature_stream_callback {
public:
  virtual ~temperature_stream_callback(void);

  virtual void call(std::vector<temperature_stream_cb_data_t>, uint16_t sequence_num);
};
// #############################################################################
// ## ADPD STREAM CALLBACKS                                                   ##
// #############################################################################
/*!
   \brief A callback struct to be used with the ADPD data stream.
*/
//
//enum ADPD_RAW_DATA_TYPES_ENUM_t :uint8_t {
//	RAW_DATA_ADPD_A_4CH_16b = M2M2_SENSOR_ADPD_RAW_DATA_TYPES_ENUM_t::M2M2_SENSOR_RAW_DATA_ADPD_A_4CH_16b,
//	RAW_DATA_ADPD_A_4CH_32b = M2M2_SENSOR_ADPD_RAW_DATA_TYPES_ENUM_t::M2M2_SENSOR_RAW_DATA_ADPD_A_4CH_32b,
//	RAW_DATA_ADPD_A_SUM_16b = M2M2_SENSOR_ADPD_RAW_DATA_TYPES_ENUM_t::M2M2_SENSOR_RAW_DATA_ADPD_A_SUM_16b,
//	RAW_DATA_ADPD_A_SUM_32b = M2M2_SENSOR_ADPD_RAW_DATA_TYPES_ENUM_t::M2M2_SENSOR_RAW_DATA_ADPD_A_SUM_32b,
//	RAW_DATA_ADPD_B_4CH_16b = M2M2_SENSOR_ADPD_RAW_DATA_TYPES_ENUM_t::M2M2_SENSOR_RAW_DATA_ADPD_B_4CH_16b,
//	RAW_DATA_ADPD_B_4CH_32b = M2M2_SENSOR_ADPD_RAW_DATA_TYPES_ENUM_t::M2M2_SENSOR_RAW_DATA_ADPD_B_4CH_32b,
//	RAW_DATA_ADPD_B_SUM_16b = M2M2_SENSOR_ADPD_RAW_DATA_TYPES_ENUM_t::M2M2_SENSOR_RAW_DATA_ADPD_B_SUM_16b,
//	RAW_DATA_ADPD_B_SUM_32b = M2M2_SENSOR_ADPD_RAW_DATA_TYPES_ENUM_t::M2M2_SENSOR_RAW_DATA_ADPD_B_SUM_32b,
//};
//
//
//struct adpd_stream_cb_data_t_sum32 {
//   ADPD_RAW_DATA_TYPES_ENUM_t datatype;
//   double    sum_timestamp;  //!< System timestamp in milliseconds
//   uint32_t  sum32_adpd_data;  //!< ADPD data value
//};
//struct adpd_stream_cb_data_t_sum16 {
//	ADPD_RAW_DATA_TYPES_ENUM_t datatype;
//	double    sum_timestamp;  //!< System timestamp in milliseconds
//	uint16_t  sum16_adpd_data;  //!< ADPD data value
//};
//
//struct adpd_stream_cb_data_t_4CH16 {
//	ADPD_RAW_DATA_TYPES_ENUM_t datatype;
//	double ch_timestamp;
//	std::vector<uint16_t> ch_data;
//};
//struct adpd_stream_cb_data_t_4CH32 {
//	ADPD_RAW_DATA_TYPES_ENUM_t datatype;
//	double ch_timestamp;
//	std::vector<uint32_t> ch_data;
//};
//
///*!
//   \brief A callback class to be used with the ADPD data stream.
//*/
//class adpd_stream_callback {
//public:
//  virtual ~adpd_stream_callback(void);
//  virtual void call(std::vector<adpd_stream_cb_data_t_sum32>, uint16_t sequence_num) = 0;
//  virtual void call(std::vector<adpd_stream_cb_data_t_sum16>, uint16_t sequence_num) = 0;
//  virtual void call(std::vector<adpd_stream_cb_data_t_4CH16>, uint16_t sequence_num) = 0;
//  virtual void call(std::vector<adpd_stream_cb_data_t_4CH32>, uint16_t sequence_num) = 0;
//};



// #############################################################################
// ## PPG STREAM CALLBACKS                                                    ##
// #############################################################################
/*!
   \brief A callback struct to be used with the ADPD data stream.
*/
struct ppg_stream_cb_data_t {
  double timestamp;    //!< System timestamp in milliseconds
  uint16_t adpdlibstate; //!< Internal ADPDlib state
  float hr;           //!< PPG-derived heart rate
  float confidence;   //!< Heart rate confidence level
  uint16_t hr_type;      //!< Internal HR type
  uint16_t rr_interval;  //!< R-R peak interval
};

/*!
   \brief A callback class to be used with the PPG data stream.
*/
class ppg_stream_callback {
public:
  virtual ~ppg_stream_callback(void);

  virtual void call(std::vector<ppg_stream_cb_data_t>, uint16_t sequence_num);
};

// #############################################################################
// ## SYNCPPG STREAM CALLBACKS                                                ##
// #############################################################################
/*!
 \brief A callback struct to be used with the Sync PPG data stream.
 */
struct syncppg_stream_cb_data_t {
  double timestamp;
  uint32_t ppg;
  int16_t x;
  int16_t y;
  int16_t z;
};

/*!
 \brief A callback class to be used with the Sync PPG data stream.
 */
class syncppg_stream_callback {
public:
  virtual ~syncppg_stream_callback(void);

  virtual void call(std::vector<syncppg_stream_cb_data_t>, uint16_t sequence_num);
};
// #############################################################################
// ## ADPDCL STREAM CALLBACKS                                                   ##
// #############################################################################
/*!
\brief A callback struct to be used with the ADPDCL data stream.
*/

enum ADDR_SENSOR_STREAM_ENUM_t :uint16_t {
	ADPD_SENSOR_STREAM1 = 1,
	ADPD_SENSOR_STREAM2,
	ADPD_SENSOR_STREAM3,
	ADPD_SENSOR_STREAM4,
	ADPD_SENSOR_STREAM5,
	ADPD_SENSOR_STREAM6,
	ADPD_SENSOR_STREAM7,
	ADPD_SENSOR_STREAM8,
	ADPD_SENSOR_STREAM9,
	ADPD_SENSOR_STREAM10,
	ADPD_SENSOR_STREAM11,
	ADPD_SENSOR_STREAM12,
	ADPD_SENSOR_INTERRUPT,
};


struct adpd4000_stream_cb_data_t{
	ADDR_SENSOR_STREAM_ENUM_t adpd_stream;
	uint8_t  dark;
	uint8_t  signal;
	uint8_t  impulse;
	uint8_t  lit;
	uint8_t  channel_num;
	double  timestamp;
	std::vector<uint32_t>  adpddata_d;
	std::vector<uint32_t>  adpddata_s;
	std::vector<uint32_t>  adpddata_l;
};

struct adpd4000_optional_stream_cb_data_t {
	ADDR_SENSOR_STREAM_ENUM_t adpd_stream;
	double  timestamp;
	uint16_t  data_int;
	uint16_t  level0_int;
	uint16_t  level1_int;
	uint16_t  tia_ch1_int;
	uint16_t  tia_ch2_int;
};

/*!
\brief A callback class to be used with the ADPDCL data stream.
*/
class adpd4000_stream_callback {
public:
	virtual ~adpd4000_stream_callback(void);
	virtual void call(std::vector<adpd4000_stream_cb_data_t>, uint16_t sequence_num);
	virtual void call(std::vector<adpd4000_optional_stream_cb_data_t>, uint16_t sequence_num);

};


// #############################################################################
// ## BATTERY STREAM CALLBACKS                                                ##
// #############################################################################
/*!
 \brief A callback struct to be used with the battery stream.
 */

enum PM_SYS_BAT_STATE_ENUM_t : uint8_t {
  SYS_BAT_STATE_NOT_AVAIL = M2M2_PM_SYS_BAT_STATE_ENUM_t::M2M2_PM_SYS_BAT_STATE_NOT_AVAIL,
  SYS_BAT_STATE_NOT_CHARGING = M2M2_PM_SYS_BAT_STATE_ENUM_t::M2M2_PM_SYS_BAT_STATE_NOT_CHARGING,
  SYS_BAT_STATE_CHARGING = M2M2_PM_SYS_BAT_STATE_ENUM_t::M2M2_PM_SYS_BAT_STATE_CHARGING,
  SYS_BAT_STATE_COMPLETE = M2M2_PM_SYS_BAT_STATE_ENUM_t::M2M2_PM_SYS_BAT_STATE_COMPLETE,
  SYS_BAT_STATE_DETECTION = M2M2_PM_SYS_BAT_STATE_ENUM_t::M2M2_PM_SYS_BAT_STATE_DETECTION,
  SYS_BAT_STATE_CHARGE_ERR = M2M2_PM_SYS_BAT_STATE_ENUM_t::M2M2_PM_SYS_BAT_STATE_CHARGE_ERR,
};

struct battery_stream_cb_data_t {
  double timestamp;
  PM_SYS_BAT_STATE_ENUM_t bat_chrg_stat;
  uint8_t bat_lvl;
  uint16_t bat_mv;
  uint16_t bat_temp;
};

/*!
 \brief A callback class to be used with the battery stream.
 */
class battery_stream_callback {
public:
  virtual ~battery_stream_callback(void);

  virtual void call(std::vector<battery_stream_cb_data_t>);
};

struct ping_stream_cb_data_t {
	uint16_t pktSize;
};

/*!
 \brief A callback class to be used with the ping stream.
 */
class ping_stream_callback {
public:
	virtual ~ping_stream_callback(void);

	virtual void call(std::vector<ping_stream_cb_data_t>, uint16_t sequence_num);
};
// #############################################################################
// ## FS STREAM CALLBACKS                                                    ##
// #############################################################################
/*!
 \brief A callback struct to be used with the FS data stream.
 */
struct fs_stream_cb_data_t {
    uint8_t  status;
    uint16_t  length_Stream;
    std::vector<uint8_t> byte_Stream;
    uint16_t  crc16;
};
/*!
 \brief A callback class to be used with the FS data stream.
 */
class fs_stream_callback {
public:
    virtual ~fs_stream_callback(void);
    virtual void call(std::vector<fs_stream_cb_data_t>);
};

/*!
 \brief A callback struct to be used with the FS data stream.
 */
struct fs_ls_data_t {
    uint8_t  status;
    std::string filename;
    uint8_t  filetype;
    uint32_t  filesize;
};
/*!
 \brief A callback class to be used with the FS ls.
 */
class fs_ls_callback {
public:
    virtual ~fs_ls_callback(void);
    virtual void call(std::vector<fs_ls_data_t>);
};

// #############################################################################
// ## PEDOMETER STREAM CALLBACKS                                              ##
// #############################################################################
/*!
 \brief A callback struct to be used with the EDA data stream.
 */
struct pedometer_stream_cb_data_t {
  int32_t NumSteps;
  uint16_t AlgoStatus;
  double TimeStamp;
  int8_t Reserved;
};

class pedometer_stream_callback {
public:
  virtual ~pedometer_stream_callback(void);

  virtual void call(std::vector<pedometer_stream_cb_data_t>, uint16_t sequence_num);
};

// #############################################################################
// ## BCM STREAM CALLBACKS                                                    ##
// #############################################################################
/*!
 \brief A callback struct to be used with the BCM data stream.
 */

struct bcm_stream_cb_data_t {
  double timestamp;
  int32_t impedance_real;
  int32_t impedance_img;
  uint8_t is_finger_on_leads;
  uint8_t signal_stability;
  double impedance_magnitude;
  double impedance_phase;
  double admittance_magnitude;
  double admittance_phase;
};

class bcm_stream_callback {
public:
  virtual ~bcm_stream_callback(void);

  virtual void call(std::vector<bcm_stream_cb_data_t>, uint16_t sequence_num);
};

// #############################################################################
// ## HRV STREAM CALLBACKS                                                    ##
// #############################################################################
/*!
 \brief A callback struct to be used with the HRV data stream.
 */
//
//struct hrv_stream_cb_data_t {
//  double timestamp;
//  int16_t rr_interval;
//  uint16_t is_gap;
//};
//
//class hrv_stream_callback {
//public:
//  virtual ~hrv_stream_callback(void);
//
//  virtual void call(std::vector<hrv_stream_cb_data_t>, uint16_t sequence_num);
//};

// #############################################################################
// ## AGC STREAM CALLBACKS                                                    ##
// #############################################################################
/*!
 \brief A callback struct to be used with the AGC data stream.
 */

struct agc_stream_cb_data_t {
  double timestamp;
  uint16_t  mts0;
  uint16_t  mts1;
  uint16_t  mts2;
  uint16_t  mts3;
  uint16_t  mts4;
  uint16_t  mts5;
  uint16_t  setting0;
  uint16_t  setting1;
  uint16_t  setting2;
  uint16_t  setting3;
  uint16_t  setting4;
  uint16_t  setting5;
  uint16_t  setting6;
  uint16_t  setting7;
  uint16_t  setting8;
  uint16_t  setting9;
};

class agc_stream_callback {
public:
  virtual ~agc_stream_callback(void);

  virtual void call(std::vector<agc_stream_cb_data_t>, uint16_t sequence_num);
};

// #############################################################################
// ## SQI STREAM CALLBACKS                                                    ##
// #############################################################################
/*!
 \brief A callback struct to be used with the AGC data stream.
 */


struct sqi_stream_cb_data_t {
	float     sqi;
	uint16_t  nSQISlot;
	uint16_t  nAlgoStatus;
	double  nTimeStamp;
	int8_t    nReserved;
};

class sqi_stream_callback {
public:
	virtual ~sqi_stream_callback(void);

	virtual void call(std::vector<sqi_stream_cb_data_t>, uint16_t sequence_num);
};