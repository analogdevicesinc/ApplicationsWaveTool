#pragma once

#include "m2m2_application.hpp"
#include "m2m2_data_stream.hpp"
#include "m2m2/cpp/ppg_application_interface.hpp"
#include "sdk_err.hpp"

/*!
   \brief A class representing the PPG application.

   The PPG application manages the PhotoPlethysmoGraphy algorithm on the watch.
*/
class ppg_application : public m2m2_application {
  friend watch;
public:
	enum SENSOR_PPG_LCFG_ID {
		PPG_LCFG_ID_ADPD107 = M2M2_SENSOR_PPG_LCFG_ID_ENUM_t::M2M2_SENSOR_PPG_LCFG_ID_ADPD107,
		PPG_LCFG_ID_ADPD185 = M2M2_SENSOR_PPG_LCFG_ID_ENUM_t::M2M2_SENSOR_PPG_LCFG_ID_ADPD185,
		PPG_LCFG_ID_ADPD108 = M2M2_SENSOR_PPG_LCFG_ID_ENUM_t::M2M2_SENSOR_PPG_LCFG_ID_ADPD108,
		PPG_LCFG_ID_ADPD188 = M2M2_SENSOR_PPG_LCFG_ID_ENUM_t::M2M2_SENSOR_PPG_LCFG_ID_ADPD188,
		PPG_LCFG_ID_ADPD4000 = M2M2_SENSOR_PPG_LCFG_ID_ENUM_t::M2M2_SENSOR_PPG_LCFG_ID_ADPD4000
  };
  ppg_application(watch *sdk = NULL);

  ~ppg_application(void);

  m2m2_data_stream<ppg_stream_callback> ppg_stream; //!< PPG data stream
  
  m2m2_data_stream<hrv_stream_callback> hrv_stream; //!< hrv data stream

  m2m2_data_stream<agc_stream_callback> agc_stream; //!< agc data stream
  std::string get_name(void);

  M2M2_ADDR_ENUM_t get_address(void);

  void get_algo_vendor_version(common_app_version_t *app_version);

  std::vector<std::pair<uint8_t, uint32_t>> lcfg_read(std::vector<uint8_t> addresses);

  std::vector<std::pair<uint8_t, uint32_t>> lcfg_write(std::vector<std::pair<uint8_t, uint32_t>> addr_vals);

  ret::sdk_status load_lcfg(SENSOR_PPG_LCFG_ID lcfg_id = PPG_LCFG_ID_ADPD107);

  std::vector<uint32_t> get_lcfg(void);
};
