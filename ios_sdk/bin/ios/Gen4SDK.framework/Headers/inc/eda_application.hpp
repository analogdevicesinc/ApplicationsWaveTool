#pragma once

#include "m2m2_application.hpp"
#include "m2m2_data_stream.hpp"
#include "m2m2/cpp/sensor_adxl_application_interface.hpp"


struct eda_app_set_baseline_imp {
	float    imp_real_dft16;
	float    imp_img_dft16;
	float    imp_real_dft8;
	float    imp_img_dft8;
	uint32_t resistor_baseline;
};

struct eda_rtia_cal_table {
	uint16_t  minscale;
	uint16_t  maxscale;
	uint16_t  lowpowerrtia;
	uint16_t  num_calibrated_values;
	std::vector<uint32_t> calibratedvalue;
	std::vector<uint32_t> actualvalue;
};

struct eda_app_get_baseline_imp {
	int16_t  eda_user_baseline_imp_set;
	float    imp_real_dft16;
	float    imp_img_dft16;
	float    imp_real_dft8;
	float    imp_img_dft8;
	uint32_t resistor_baseline;
};

/*!
    \brief A class reperesenting the EDA application

    The EDA application manages the ElectroDermal Activity sensors on the watch.
*/

class eda_application : public m2m2_application {
  friend watch;
public:

	enum AD5940_DCB_DATA_FLAG {
		DCB_AD5940_LCFG_ENABLE = M2M2_AD5940_DCB_DATA_FLAG_t::ADI_DCB_AD5940_LCFG_ENABLE,   //if '0' its lcfg dcb being written
		DCB_AD5940_DCFG_ENABLE = M2M2_AD5940_DCB_DATA_FLAG_t::ADI_DCB_AD5940_DCFG_ENABLE,   //if '1' its dcfg dcb being written
		DCB_MAX_AD5940_BLOCK_IDX = M2M2_AD5940_DCB_DATA_FLAG_t ::ADI_DCB_MAX_AD5940_BLOCK_IDX,
		DCB_INVALID = M2M2_AD5940_DCB_DATA_FLAG_t::ADI_DCB_INVALID,
	};

	enum AD5940_DCB_CONFIG_BLOCK_INDEX {
		DCB_EDA_LCFG_BLOCK_IDX = M2M2_DCB_CONFIG_BLOCK_INDEX_t::ADI_DCB_EDA_LCFG_BLOCK_IDX,   //if '0' its lcfg dcb being written
		DCB_EDA_DCFG_BLOCK_IDX = M2M2_DCB_CONFIG_BLOCK_INDEX_t::ADI_DCB_EDA_DCFG_BLOCK_IDX,   //if '1' its dcfg dcb being written
	};

  eda_application(watch *sdk = NULL);

  ~eda_application(void);



  m2m2_data_stream<eda_stream_callback> eda_stream;

  std::string get_name(void);

  M2M2_ADDR_ENUM_t get_address(void);

  std::vector<std::pair<uint8_t, uint32_t>> lcfg_read(std::vector<uint8_t> addresses);

  std::vector<std::pair<uint8_t, uint32_t>> lcfg_write(std::vector<std::pair<uint8_t, uint32_t>> addr_vals);
  ret::sdk_status dcb_set_lcfg(void);

  ret::sdk_status dynamic_scaling(bool isEnable, uint16_t minScale, uint16_t maxScale, uint16_t lprtiasel);

  ret::sdk_status set_dft_num(uint8_t dftnum);
  ret::sdk_status write_dcb_lcfg_config(std::vector<std::pair<uint32_t, uint32_t>> addr_vals);
  ret::sdk_status write_dcb_dcfg_config(std::vector<std::pair<uint32_t, uint32_t>> addr_vals);

  std::vector<std::pair<uint32_t, uint32_t>> read_dcb_config(AD5940_DCB_DATA_FLAG state);

  ret::sdk_status  dcb_delete_config(AD5940_DCB_DATA_FLAG state);

  ret::sdk_status calibrate_rtia(uint16_t minScale, uint16_t maxScale, uint16_t lprtiasel);

  void getRTIACalTable(uint8_t table_val, eda_rtia_cal_table* ptable);

  ret::sdk_status setBaselineImpedance(eda_app_set_baseline_imp *baseInfo);

  ret::sdk_status resetBaselineImpedance(void);

  ret::sdk_status getBaselineImpedance(eda_app_get_baseline_imp* baseInfo);

  std::vector<std::pair<uint32_t, uint32_t>> register_read(std::vector<uint32_t> addresses);
  std::vector<std::pair<uint32_t, uint32_t>> register_write(std::vector<std::pair<uint32_t, uint32_t>> addr_vals);
  ret::sdk_status load_cfg(void);

  ret::sdk_status deteleRTIA_Table(void);

};
