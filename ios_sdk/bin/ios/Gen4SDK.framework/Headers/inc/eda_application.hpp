#pragma once

#include "m2m2_application.hpp"
#include "m2m2_data_stream.hpp"
#include "m2m2/cpp/sensor_adxl_application_interface.hpp"

/*!
    \brief A class reperesenting the EDA application

    The EDA application manages the ElectroDermal Activity sensors on the watch.
*/

class eda_application : public m2m2_application {
  friend watch;
public:
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
  ret::sdk_status write_dcb_config(std::vector<std::pair<uint16_t, uint16_t>> addr_vals);

  std::vector<std::pair<uint16_t, uint16_t>> read_dcb_config(void);

  ret::sdk_status  dcb_delete_config(void);

  ret::sdk_status calibrate_rtia(uint16_t minScale, uint16_t maxScale, uint16_t lprtiasel);
};
