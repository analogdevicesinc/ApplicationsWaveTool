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
#include "watch.hpp"
#include "fs_application.hpp"
#include "m2m2/cpp/file_system_interface.hpp"
#include "m2m2/cpp/common_application_interface.hpp"


fs_application::~fs_application(void) {
    
}

fs_application::fs_application(watch *sdk):
   m2m2_application(sdk),
   fs_stream(M2M2_ADDR_SYS_FS,this),
   fs_ls_response(M2M2_ADDR_SYS_FS,this){}

/*!
 \brief Fetches a human-readable string describing the application.
 */
std::string fs_application::get_name(void) {
  return "File System application";
}

/*!
 \brief Fetches the address of the application.
 */
M2M2_ADDR_ENUM_t fs_application::get_address(void) {
  return M2M2_ADDR_SYS_FS;
}

/*!
 \brief Clear the Logs in the file system.
 */
ret::sdk_status fs_application::fs_format(void) {
  m2m2_pkt<m2m2_file_sys_cmd_t> pkt(this->get_address());
  pkt.payload.command = M2M2_FILE_SYS_CMD_FORMAT_REQ;
  
  auto resp = this->sync_send(pkt.pack());
  
  pkt.unpack(resp);
  
  if (pkt.payload.status != M2M2_FILE_SYS_STATUS_OK || pkt.payload.status != M2M2_APP_COMMON_STATUS_OK) {
    return ret::SDK_ERR;
  }
  return ret::SDK_OK;
}


/*!
 \brief Add participant ID for the Log file
 */
ret::sdk_status fs_application::fs_keyvaluepair(std::string value_id) {
  
  std::vector<uint8_t> myVector(value_id.begin(),
                                value_id.end());
  
  m2m2_pkt<m2m2_file_sys_set_key_value_pair_req_t> pkt(this->get_address());
  pkt.payload.command = M2M2_FILE_SYS_CMD_SET_KEY_VALUE_PAIR_REQ;
  for (unsigned int i = 0; i < sizeof(pkt.payload.valueID) ; i++) {
    pkt.payload.valueID[i] = (int)myVector[i];
  }
  auto resp = this->sync_send(pkt.pack());
  
  pkt.unpack(resp);
  
  if (pkt.payload.status != M2M2_FILE_SYS_STATUS_OK) {
    return ret::SDK_ERR;
  }
  return ret::SDK_OK;
  
}

/*!
 \brief Get the status of Logging whether it is ON or OFF
 */
uint8_t fs_application::fs_status(void) {
  
  m2m2_pkt<m2m2_file_sys_cmd_t> pkt(this->get_address());
  pkt.payload.command = M2M2_FILE_SYS_CMD_GET_FS_STATUS_REQ;
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  return pkt.payload.status;
  
}

/*!
 \brief Get the Volume information of the File system
 */
void fs_application::fs_volume_info(uint32_t& total_Memory,  //!< The total memory of the file system
                                    uint32_t& used_Memory,  //!< The Used memory of the file system
                                    uint16_t& free_Memory) { //!< The Free memory of the file system
  m2m2_pkt<m2m2_file_sys_vol_info_resp_t> pkt(this->get_address());
  pkt.payload.command = M2M2_FILE_SYS_CMD_VOL_INFO_REQ;
  auto resp = this->sync_send(pkt.pack());
  
  pkt.unpack(resp);
  total_Memory = pkt.payload.totalmemory;
  used_Memory = pkt.payload.usedmemory;
  free_Memory = pkt.payload.availmemory;
  
  return;
}

/*!
 \brief Get the list of files available in the file system
 */
void fs_application::fs_ls(fs_ls_callback &cb) {
  
  this->fs_ls_response.set_callback(cb);
  m2m2_pkt<m2m2_file_sys_ls_req_t> pkt(this->get_address());
  pkt.payload.command = M2M2_FILE_SYS_CMD_LS_REQ;
  this->async_send(pkt.pack());
  
}

/*!
 \brief Get the stream of the selective LOG file
 */
void fs_application::fs_get(std::string filename,
                            fs_stream_callback &cb) {
  
  this->fs_stream.set_callback(cb);
  std::vector<uint8_t> myVector(filename.begin(),
                                filename.end());
  uint16_t pktsize =
      myVector.size() + (offsetof(m2m2_hdr_t, data)) + offsetof(m2m2_file_sys_get_req_t, file_name);
  std::vector<uint8_t> pkt(pktsize);
  m2m2_hdr_t *p_hdr = (m2m2_hdr_t*) &pkt[0];
  m2m2_file_sys_get_req_t *p_fileName_hdr = (m2m2_file_sys_get_req_t *) &p_hdr->data[0];
  file_name *p_fileName = (file_name *) &p_fileName_hdr->file_name[0];
  
  p_hdr->length = pktsize;
  p_fileName_hdr->command = M2M2_FILE_SYS_CMD_GET_REQ;
  p_fileName_hdr->status = APP_COMMON_STATUS_OK;
  for (unsigned int i = 0; i < myVector.size(); i++) {
    p_fileName[i].namefile = (int) myVector[i];
  }
  this->async_send(pkt);
  
}

/*!
 \brief Get the particular chunk of data from file by specifying the chunk number.
 */
void fs_application::fs_get_stream_chunk(uint8_t roll_over, uint16_t chunk_num, std::string filename, fs_stream_callback &cb) {

  this->fs_stream.set_callback(cb);

  std::vector<uint8_t> myVector(filename.begin(),
                                filename.end());
  uint16_t pktsize =
      myVector.size() + (offsetof(m2m2_hdr_t, data)) + offsetof(m2m2_file_sys_get_req_t, file_name);

  std::vector<uint8_t> pkt(pktsize);
  m2m2_hdr_t *p_hdr = (m2m2_hdr_t *) &pkt[0];
  m2m2_file_sys_pkt_retransmit_req_t *pkt_retransmit_req = (m2m2_file_sys_pkt_retransmit_req_t *) &p_hdr->data[0];
  file_name *p_fileName = (file_name *) &pkt_retransmit_req->file_name[0];

  p_hdr->length = pktsize;
  pkt_retransmit_req->command = M2M2_FILE_SYS_CMD_CHUNK_RETRANSMIT_REQ;
  pkt_retransmit_req->status = APP_COMMON_STATUS_OK;
  pkt_retransmit_req->Roll_over = roll_over;
  pkt_retransmit_req->chunk_number = chunk_num;
  for (unsigned int i = 0; i < myVector.size(); i++) {
    p_fileName[i].namefile = (int) myVector[i];
  }
  this->async_send(pkt);

}

/*!
 \brief Subscribe to the stream to File System
 This function subscribes the stream to File system. Once subscribed, the device
 will begin sending the stream to the file system.
 \note
 Subscribing to a stream only signals that you are interested in its content;
 it does not necessarily cause the stream to begin producing data. Some streams
 produce data "on their own" (i.e. system battery level), while others represent sensors
 that must be started before they begin to produce data.
 \see m2m2_data_stream::start
 \see m2m2_data_stream::stop
 \see m2m2_data_stream::unsubscribe
 */
ret::sdk_status fs_application::fs_subscribe(ADDR_ENUM_STREAM stream) {
  m2m2_pkt<m2m2_app_common_sub_op_t> pkt(this->get_address());
  pkt.payload.command = M2M2_FILE_SYS_CMD_LOG_STREAM_REQ;
  pkt.payload.stream = getAddrStream(stream);
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  if (pkt.payload.status == M2M2_APP_COMMON_STATUS_ERROR) {
    return ret::SDK_ERR_DEVICE_ERR;
  }
  return ret::SDK_OK;
}


/*!
 \brief Unsubscribe the stream from file system
 This function unsubscribes the stream from the file system. Once unsubscribed, the device
 will no longer send data from the stream to file system.
 \note
 The stream (and sensor it represents) may keep running even after being
 unsubscribed, as applications on the device may also be subscribed to the stream.
 \see m2m2_data_stream::subscribe
 \see m2m2_data_stream::start
 \see m2m2_data_stream::stop
 */
ret::sdk_status fs_application::fs_unsubscribe(ADDR_ENUM_STREAM stream) {
  m2m2_pkt<m2m2_app_common_sub_op_t> pkt(this->get_address());
  pkt.payload.command = M2M2_FILE_SYS_CMD_STOP_STREAM_REQ;
  pkt.payload.stream = getAddrStream(stream);
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  if (pkt.payload.status == M2M2_APP_COMMON_STATUS_ERROR) {
    return ret::SDK_ERR_DEVICE_ERR;
  }
  return ret::SDK_OK;
}



/*!
 \brief Get the debug information of the respective file stream
 */
void fs_application::fs_debug_info(ADDR_ENUM_STREAM stream,
                                   uint32_t& packets_received,
                                   uint32_t& packets_missed) {
  m2m2_pkt<m2m2_file_sys_debug_info_req_t> pkt(this->get_address());
  pkt.payload.command = M2M2_FILE_SYS_CMD_GET_FS_DEBUG_INFO_REQ;
  pkt.payload.stream = getAddrStream(stream);
  auto resp = this->sync_send(pkt.pack());
  
  m2m2_pkt<m2m2_file_sys_debug_info_resp_t> resp_pkt;
  resp_pkt.unpack(resp);
  packets_missed = resp_pkt.payload.packets_missed;
  packets_received = resp_pkt.payload.packets_received;
}


/*!
 \brief return the file system subscription status of the respective stream
 */
uint8_t fs_application::fs_stream_subscription_status(ADDR_ENUM_STREAM stream) {
  m2m2_pkt<m2m2_app_common_sub_op_t> pkt(this->get_address());
  pkt.payload.command = M2M2_FILE_SYS_CMD_GET_FS_STREAM_SUB_STATUS_REQ;
  pkt.payload.stream = getAddrStream(stream);
  
  auto resp = this->sync_send(pkt.pack());
  
  m2m2_pkt<m2m2_file_sys_get_subs_status_resp_t> resp_pkt;
  resp_pkt.unpack(resp);
  
  return resp_pkt.payload.subs_state;
  
}


ret::sdk_status fs_application::fs_logStart(void) {
	m2m2_pkt<m2m2_app_common_sub_op_t> pkt(this->get_address());
	pkt.payload.command = M2M2_FILE_SYS_CMD_START_LOGGING_REQ;
	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);
	
		if (pkt.payload.status != M2M2_FILE_SYS_STATUS_OK
			&& pkt.payload.status != M2M2_FILE_SYS_STATUS_LOGGING_IN_PROGRESS) {
			return ret::SDK_ERR;
		}
	return ret::SDK_OK;
}

ret::sdk_status fs_application::fs_logStop(void) {
	m2m2_pkt<m2m2_app_common_sub_op_t> pkt(this->get_address());
	pkt.payload.command = M2M2_FILE_SYS_CMD_STOP_LOGGING_REQ;
	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);
	if (pkt.payload.status != M2M2_FILE_SYS_STATUS_OK) {
		return ret::SDK_ERR;
	}
	return ret::SDK_OK;
}



/*!
 \brief Get the number of files in file system
 */
 void fs_application::fs_get_file_count(uint16_t& maxFileCount) {
  m2m2_pkt<m2m2_file_sys_get_file_count_pkt_t> pkt(this->get_address());
  pkt.payload.command = M2M2_FILE_SYS_CMD_GET_NUMBER_OF_FILE_REQ;
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  maxFileCount = pkt.payload.file_count;

}

/*!
 \brief To check the Config file exists in the filesystem if Config file exists return the SDK status
 */
ret::sdk_status fs_application::fs_find_config_file(void) {
  m2m2_pkt<m2m2_file_sys_cmd_t> pkt(this->get_address());
  pkt.payload.command = M2M2_FILE_SYS_CMD_FIND_CONFIG_FILE_REQ;
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  if (pkt.payload.status != M2M2_FILE_SYS_CONFIG_FILE_FOUND) {
    return ret::SDK_ERR;
  }
  return ret::SDK_OK;

}

/*!
 \brief To delete the Config file in the filesystem
 */
uint8_t fs_application::delete_config_file(void) {
  m2m2_pkt<m2m2_file_sys_cmd_t> pkt(this->get_address());
  pkt.payload.command = M2M2_FILE_SYS_CMD_DELETE_CONFIG_FILE_REQ;
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  return pkt.payload.status;

}

/*!
 \brief To open (create) or close the file in the filesystem
 */
uint8_t fs_application::fs_config_log(bool enable) {
  m2m2_pkt<m2m2_file_sys_cmd_t> pkt(this->get_address());
  if(enable){
    pkt.payload.command = M2M2_FILE_SYS_CMD_DCFG_START_LOG_REQ;
  }else{
    pkt.payload.command = M2M2_FILE_SYS_CMD_DCFG_STOP_LOG_REQ;
  }
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  return pkt.payload.status;
}

/*!
 \brief writing the buffer into the config file
 */

uint8_t fs_application::fs_config_write(std::vector<uint8_t> buffer) {

    uint8_t status = ret::SDK_ERR;
    uint16_t buffer_length = buffer.size();
    int reading_pos = 0;
    uint16_t config_buffer_len = 0;

    while (true) {
		m2m2_pkt<m2m2_file_sys_user_config_data> pkt(this->get_address());
		pkt.payload.command = M2M2_FILE_SYS_CMD_LOG_USER_CONFIG_DATA_REQ;
	    config_buffer_len = sizeof(pkt.payload.byte_configstream);

        if (buffer_length <= config_buffer_len) {

            pkt.payload.status = M2M2_FILE_SYS_END_OF_FILE;
            for(unsigned int i = 0; i < buffer_length;i++ ){
                pkt.payload.byte_configstream[i] = buffer[reading_pos++];
            }
            pkt.payload.len_configstream = buffer_length;

            auto resp = this->sync_send(pkt.pack());
            pkt.unpack(resp);
            status = pkt.payload.status;
			break;

        } else {
            pkt.payload.status = M2M2_FILE_SYS_STATUS_OK;
            for(unsigned int i = 0; i < config_buffer_len ;i++ ){
                pkt.payload.byte_configstream[i] = buffer[reading_pos++];
            }
            buffer_length = buffer_length - config_buffer_len;
            pkt.payload.len_configstream = config_buffer_len;
            auto resp = this->sync_send(pkt.pack());
            pkt.unpack(resp);
           if (pkt.payload.status != M2M2_APP_COMMON_STATUS_OK) {
               status = pkt.payload.status;
			   break;
            }

        }
    }

	return status;
}

/*!
 \brief mount file system
 */
uint8_t fs_application::fs_mount() {
  m2m2_pkt<m2m2_file_sys_cmd_t> pkt(this->get_address());
  pkt.payload.command = M2M2_FILE_SYS_CMD_MOUNT_REQ;
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);

  return pkt.payload.status;
}

/*!
 \brief write reference HR data into file system
 */
void fs_application::fs_refhr(uint16_t refhr, sys_date_time_t *time_info) {
  sdk->is_fs_refhr_cmd = true;
  m2m2_pkt<m2m2_file_sys_app_ref_hr_stream_t> pkt(this->get_address());
  pkt.payload.command = M2M2_SENSOR_COMMON_CMD_STREAM_DATA;
  pkt.payload.refhr = refhr;
  sys_date_time_t *sys_date_time = &time_info[0];
  pkt.payload.year = sys_date_time->year;
  pkt.payload.month = sys_date_time->month;
  pkt.payload.day = sys_date_time->day;
  pkt.payload.hour = sys_date_time->hour;
  pkt.payload.minute = sys_date_time->minute;
  pkt.payload.second = sys_date_time->second;
  pkt.payload.TZ_sec = sys_date_time->TZ_sec;

  this->async_send(pkt.pack());

  sdk->is_fs_refhr_cmd = false;
}

M2M2_ADDR_ENUM_t fs_application::getAddrStream(ADDR_ENUM_STREAM stream)
{

	switch (stream)
	{
	//case ADDR_ENUM_STREAM::ADPD_Stream:
	//	return  M2M2_ADDR_ENUM_t::M2M2_ADDR_SENSOR_ADPD_STREAM;
	case ADDR_ENUM_STREAM::ADXL_Stream:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_SENSOR_ADXL_STREAM;
	case ADDR_ENUM_STREAM::PPG_Stream:
		return M2M2_ADDR_ENUM_t::M2M2_ADDR_MED_PPG_STREAM;
	case ADDR_ENUM_STREAM::SYNCPPG_Stream:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_MED_SYNC_ADPD_ADXL_STREAM;
	case ADDR_ENUM_STREAM::ECG_Stream:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_MED_ECG_STREAM;
	case ADDR_ENUM_STREAM::EDA_Stream:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_MED_EDA_STREAM;
	case ADDR_ENUM_STREAM::TEMPERATURE_Stream:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_MED_TEMPERATURE_STREAM;
	case ADDR_ENUM_STREAM::PEDOMETER_Stream:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_MED_PED_STREAM;
	case ADDR_ENUM_STREAM::BCM_Stream:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_MED_BCM_STREAM;
	//case ADDR_ENUM_STREAM::HRV_Stream:
	//	return  M2M2_ADDR_ENUM_t::M2M2_ADDR_SYS_HRV_STREAM;
	case ADDR_ENUM_STREAM::BATTERY_Stream:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_SYS_BATT_STREAM;
	case ADDR_ENUM_STREAM::AGC_Stream:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_SYS_AGC_STREAM;
	case ADDR_ENUM_STREAM::ADPD1_STREAM:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_SENSOR_ADPD_STREAM1;
	case ADDR_ENUM_STREAM::ADPD2_STREAM:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_SENSOR_ADPD_STREAM2;
	case ADDR_ENUM_STREAM::ADPD3_STREAM:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_SENSOR_ADPD_STREAM3;
	case ADDR_ENUM_STREAM::ADPD4_STREAM:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_SENSOR_ADPD_STREAM4;
	case ADDR_ENUM_STREAM::ADPD5_STREAM:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_SENSOR_ADPD_STREAM5;
	case ADDR_ENUM_STREAM::ADPD6_STREAM:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_SENSOR_ADPD_STREAM6;
	case ADDR_ENUM_STREAM::ADPD7_STREAM:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_SENSOR_ADPD_STREAM7;
	case ADDR_ENUM_STREAM::ADPD8_STREAM:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_SENSOR_ADPD_STREAM8;
	case ADDR_ENUM_STREAM::ADPD9_STREAM:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_SENSOR_ADPD_STREAM9;
	case ADDR_ENUM_STREAM::ADPD10_STREAM:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_SENSOR_ADPD_STREAM10;
	case ADDR_ENUM_STREAM::ADPD11_STREAM:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_SENSOR_ADPD_STREAM11;
	case ADDR_ENUM_STREAM::ADPD12_STREAM:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_SENSOR_ADPD_STREAM12;
	case ADDR_ENUM_STREAM::ADPD_OPTIONAL_STREAM:
		return  M2M2_ADDR_ENUM_t::M2M2_ADDR_SENSOR_ADPD_OPTIONAL_BYTES_STREAM;
	}
  return  M2M2_ADDR_ENUM_t::M2M2_ADDR_SENSOR_ADPD_OPTIONAL_BYTES_STREAM;
}

uint32_t fs_application::fs_get_number_of_bad_blocks(void)
{
	uint32_t num_of_blocks;
	m2m2_pkt<m2m2_file_sys_get_bad_blocks_cmd_t> pkt(this->get_address());
	pkt.payload.command = M2M2_FILE_SYS_CMD_GET_BAD_BLOCKS_REQ;
	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);
	num_of_blocks = pkt.payload.bad_blocks;
	return num_of_blocks;
}

uint8_t fs_application::fs_abort(void)
{
	uint8_t status;
	m2m2_pkt<m2m2_file_sys_cmd_t> pkt(this->get_address());
	pkt.payload.command = M2M2_FILE_SYS_CMD_FORCE_STOP_LOG_REQ;
	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);
	status = pkt.payload.status;
	return status;
}

