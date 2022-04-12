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
//#include "adpd_application.hpp"
#include "callback.hpp"
#include "m2m2/cpp/file_system_interface.hpp"
#include "m2m2/cpp/common_application_interface.hpp"
#include "m2m2_application.hpp"

class fs_application:public m2m2_application{
  friend watch;
public:
	enum ADDR_ENUM_STREAM {
		ADPD_Stream = 0,
		ADXL_Stream,
		PPG_Stream,
		SYNCPPG_Stream,
		ECG_Stream,
		EDA_Stream,
		TEMPERATURE_Stream,
		PEDOMETER_Stream,
		BIA_Stream,
		BIA_ALGO_Stream,
		HRV_Stream,
		BATTERY_Stream,
		STATIC_AGC_Stream,
		DYNAMIC_AGC_Stream,
		SQI_Stream,
		AD7156_Stream,
		ADPD1_STREAM,
		ADPD2_STREAM,
		ADPD3_STREAM,
		ADPD4_STREAM,
		ADPD5_STREAM,
		ADPD6_STREAM,
		ADPD7_STREAM,
		ADPD8_STREAM,
		ADPD9_STREAM,
		ADPD10_STREAM,
		ADPD11_STREAM,
		ADPD12_STREAM,
		ADPD_OPTIONAL_STREAM,
		TEMP1_STREAM,
		TEMP2_STREAM,
		TEMP3_STREAM,
		TEMP4_STREAM,
		TEMP5_STREAM,
		TEMP6_STREAM,
		TEMP7_STREAM,
		TEMP8_STREAM,
		TEMP9_STREAM,
		TEMP10_STREAM,
		TEMP11_STREAM,
		TEMP12_STREAM,

	};
  enum FS_SYS_FILE_TYPE_ENUM_t : uint8_t {
    FS_SYS_IS_DIR = FILE_TYPE_ENUM_t::M2M2_FILE_SYS_IS_CONFIG_FILE,
    FS_SYS_IS_FILE = FILE_TYPE_ENUM_t::M2M2_FILE_SYS_IS_DATA_FILE,
    FS_SYS_INVALID_TYPE = FILE_TYPE_ENUM_t::M2M2_FILE_SYS_INVALID_TYPE,
  };
    
  enum FILE_SYS_STATUS_ENUM {
    FILE_SYS_STATUS_OK = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_STATUS_OK,
    FILE_SYS_STATUS_ERROR = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_STATUS_ERROR,
    FILE_SYS_END_OF_FILE = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_END_OF_FILE,
    FILE_SYS_END_OF_DIR = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_END_OF_DIR,
    FILE_SYS_ERR_INVALID = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_ERR_INVALID,
    FILE_SYS_ERR_ARGS = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_ERR_ARGS,
    FILE_SYS_ERR_FORMAT = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_ERR_FORMAT,
    FILE_SYS_ERR_MEMORY_FULL = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_ERR_MEMORY_FULL,
    FILE_SYS_ERR_LOG_FORCE_STOPPED = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_ERR_LOG_FORCE_STOPPED,
    FILE_SYS_ERR_MAX_FILE_COUNT = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_ERR_MAX_FILE_COUNT,
    FILE_SYS_CONFIG_FILE_FOUND = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_CONFIG_FILE_FOUND,
    FILE_SYS_CONFIG_FILE_NOT_FOUND = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_CONFIG_FILE_NOT_FOUND,
    FILE_SYS_ERR_CONFIG_FILE_POSITION = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_ERR_CONFIG_FILE_POSITION,
    FILE_SYS_ERR_NOT_CHKD = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_ERR_NOT_CHKD,
	FILE_SYS_STATUS_LOGGING_STOPPED = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_STATUS_LOGGING_STOPPED,
	FILE_SYS_STATUS_LOGGING_IN_PROGRESS = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_STATUS_LOGGING_IN_PROGRESS,
	FILE_SYS_STATUS_LOGGING_ERROR = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_STATUS_LOGGING_ERROR,
	FILE_SYS_STATUS_LOGGING_NOT_STARTED = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_STATUS_LOGGING_NOT_STARTED,
	FILE_SYS_ERR_LOWEST = M2M2_FILE_SYS_STATUS_ENUM_t::_M2M2_FILE_SYS_STATUS_ENUM_t__M2M2_FILE_SYS_ERR_LOWEST,
	FILE_SYS_ERR_BATTERY_LOW = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_ERR_BATTERY_LOW,
	FILE_SYS_ERR_POWER_STATE_SHUTDOWN = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_ERR_POWER_STATE_SHUTDOWN,
	FILE_SYS_STATUS_BLOCKS_WRITE_ERROR = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_STATUS_BLOCKS_WRITE_ERROR,
	FILE_SYS_NO_FILE_TO_APPEND = M2M2_FILE_SYS_STATUS_ENUM_t::M2M2_FILE_SYS_NO_FILE_TO_APPEND,
  };

  enum FILE_PAGE_CHUNK_RETRANSMIT_TYPE_ENUM {
	  FILE_SYS_PAGE_CHUNK_CRC_ERROR = FILE_PAGE_CHUNK_RETRANSMIT_TYPE_ENUM_t::M2M2_FILE_SYS_PAGE_CHUNK_CRC_ERROR,
	  FILE_SYS_PAGE_CHUNK_LOST = FILE_PAGE_CHUNK_RETRANSMIT_TYPE_ENUM_t::M2M2_FILE_SYS_PAGE_CHUNK_LOST,
  };

  struct file_name {
    uint8_t namefile;
  };


  struct file_sys_format_debug_info_resp_t {
	  uint8_t  erase_failed_due_bad_block_check;
	  uint8_t  wrap_around_cond;
	  uint8_t  nothing_is_written_to_erase_error;
	  uint8_t  mem_full_in_partial_erase;
	  uint8_t  toc_mem_erased_flag;
	  uint8_t  succesfull_erase_flag;
	  uint16_t  num_blocks_erased_in_mem_full_partial_erase;
	  uint16_t  num_blocks_erased_in_partial_erase_1;
	  uint16_t  num_blocks_erased_in_partial_erase_2;
	  uint16_t  num_times_format_failed_due_bad_blocks_1;
	  uint16_t  num_times_format_failed_due_bad_blocks_2;
	  uint32_t  format_src_blk_ind;
	  uint32_t  format_dest_blk_ind_1;
	  uint32_t  format_dest_blk_ind_2;
  };

  struct file_sys_page_read_test_resp_pkt_t {
	  uint32_t  page_num;
	  uint8_t  ecc_zone_status;
	  uint32_t  next_page;
	  uint8_t  occupied;
	  uint16_t  num_bytes_written;
	  uint8_t  data_region_status;
	  std::vector<uint8_t>  sample_data;
	  uint8_t  num_bytes;
  };

    
  fs_application(watch *sdk=NULL);
  ~fs_application(void);
  m2m2_data_stream<fs_stream_callback> fs_stream;
  m2m2_data_stream<fs_ls_callback> fs_ls_response;
  std::string get_name(void);
  M2M2_ADDR_ENUM_t get_address(void);
  ret::sdk_status fs_format(void);
  void fs_volume_info(uint32_t& total_Memory,
                      uint32_t& used_Memory,
                      uint16_t& free_Memory);
  void fs_ls(fs_ls_callback &cb);
  void fs_get(std::string filename,
              fs_stream_callback &cb);

  ret::sdk_status fs_get_stream_chunk(FILE_PAGE_CHUNK_RETRANSMIT_TYPE_ENUM type, uint8_t roll_over, uint16_t chunk_num, uint16_t pagenum, std::string filename, fs_stream_cb_data_t  *pstreamData);
  void fs_debug_info(ADDR_ENUM_STREAM stream,
                     uint32_t& packets_received,
                     uint32_t& packets_missed);
  uint8_t fs_stream_subscription_status(ADDR_ENUM_STREAM stream);
  ret::sdk_status fs_keyvaluepair(std::string value_id);
  uint8_t fs_status(void);
  ret::sdk_status fs_subscribe(ADDR_ENUM_STREAM stream);
  ret::sdk_status fs_unsubscribe(ADDR_ENUM_STREAM stream);
  void fs_get_file_count(uint16_t& maxFileCount);
  ret::sdk_status fs_find_config_file(void);
  
  uint8_t delete_config_file(void);
  uint8_t fs_config_log(bool enable);
  uint8_t fs_config_write(std::vector<uint8_t> buffer, bool isEOF);

  fs_application::FILE_SYS_STATUS_ENUM fs_logStart(void);
  fs_application::FILE_SYS_STATUS_ENUM fs_logStop(void);
  uint8_t fs_mount(void);

  void fs_refhr(uint16_t refhr, sys_date_time_t *time_info);

  uint32_t fs_get_number_of_bad_blocks(void);

  uint8_t fs_abort(void);
  ret::sdk_status fs_blockErase(uint16_t blockNo);
  ret::sdk_status fs_getFormatInfo(file_sys_format_debug_info_resp_t *debugInfo);
  fs_application::FILE_SYS_STATUS_ENUM fs_appendFileReq(void);
  ret::sdk_status fs_fileReadTest(uint32_t  start_page_ind, uint32_t  end_page_ind, file_sys_page_read_test_resp_pkt_t *readInfo);
  ret::sdk_status fs_writeRandomDataToBlk(std::vector<uint32_t> data);
  fs_application::FILE_SYS_STATUS_ENUM fs_pattern_write(uint32_t file_size, uint8_t scale_type, uint16_t scale_factor, uint16_t base, uint16_t num_files);

private:
	M2M2_ADDR_ENUM_t getAddrStream(ADDR_ENUM_STREAM stream);
};
