#pragma once

#include "m2m2_application.hpp"
#include "m2m2_data_stream.hpp"
#include "m2m2/cpp/bia_application_interface.hpp"

/*!
    \brief A class representing the BIA application.

    The bia application manages the bia sensors on the watch.
*/

class bia_application : public m2m2_application {
  friend watch;
public:

	enum AD5940_DCB_DATA_FLAG {
		DCB_AD5940_LCFG_ENABLE = M2M2_AD5940_DCB_DATA_FLAG_t::ADI_DCB_AD5940_LCFG_ENABLE,   //if '1' its lcfg dcb being written
		DCB_AD5940_DCFG_ENABLE = M2M2_AD5940_DCB_DATA_FLAG_t::ADI_DCB_AD5940_DCFG_ENABLE,   //if '2' its dcfg dcb being written
		DCB_MAX_AD5940_BLOCK_IDX = M2M2_AD5940_DCB_DATA_FLAG_t::ADI_DCB_MAX_AD5940_BLOCK_IDX,
		DCB_INVALID = M2M2_AD5940_DCB_DATA_FLAG_t::ADI_DCB_INVALID,
	};

	enum AD5940_DCB_CONFIG_BLOCK_INDEX {
		DCB_BIA_LCFG_BLOCK_IDX = M2M2_DCB_CONFIG_BLOCK_INDEX_t::ADI_DCB_BIA_LCFG_BLOCK_IDX,   //if '0' its lcfg dcb being written
		DCB_BIA_DCFG_BLOCK_IDX = M2M2_DCB_CONFIG_BLOCK_INDEX_t::ADI_DCB_BIA_DCFG_BLOCK_IDX,   //if '1' its dcfg dcb being written
	};

	struct fds_status_info_resp {
		uint16_t  dirty_records;
		uint16_t  open_records;
		uint16_t  valid_records;
		uint16_t  pages_available;
		uint16_t  num_blocks;
		uint16_t  blocks_free;
	};

	bia_application(watch *sdk = NULL);

  ~bia_application(void);

  m2m2_data_stream<bia_stream_callback> bia_stream; //!< bia data stream
  m2m2_data_stream<bia_algo_stream_callback> bia_algo_stream; //!< bia data stream

  std::string get_name(void);

  M2M2_ADDR_ENUM_t get_address(void);

  ret::sdk_status set_dft_num(uint32_t dft_num);

  ret::sdk_status enable_or_disable_sweep_frequency(bool enable);

  ret::sdk_status set_hsrtia_cal(uint16_t value);

  std::vector<std::pair<uint8_t, uint32_t>> lcfg_read(std::vector<uint8_t> addresses);

  std::vector<std::pair<uint8_t, uint32_t>> lcfg_write(std::vector<std::pair<uint8_t, uint32_t>> addr_vals);

  ret::sdk_status get_fds_status(fds_status_info_resp *statusInfo);

  ret::sdk_status dcb_set_lcfg(void);

  std::vector<std::pair<uint32_t, uint32_t>> register_read(std::vector<uint32_t> addresses);
  std::vector<std::pair<uint32_t, uint32_t>> register_write(std::vector<std::pair<uint32_t, uint32_t>> addr_vals);
  ret::sdk_status load_cfg(void);

  ret::sdk_status write_dcb_lcfg_config(std::vector<std::pair<uint32_t, uint32_t>> addr_vals);
  ret::sdk_status write_dcb_dcfg_config(std::vector<std::pair<uint32_t, uint32_t>> addr_vals);

  std::vector<std::pair<uint32_t, uint32_t>> read_dcb_config(AD5940_DCB_DATA_FLAG state);

  ret::sdk_status  dcb_delete_config(AD5940_DCB_DATA_FLAG state);

};
