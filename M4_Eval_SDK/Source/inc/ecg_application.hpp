#pragma once

#include "m2m2_application.hpp"
#include "m2m2_data_stream.hpp"
#include "m2m2/cpp/ecg_application_interface.hpp"

/*!
  /brief A class representing the ECG application.

  The ECG application manages the ElectroCardioGram sensors and algorithms on the watch.
*/

class ecg_application : public m2m2_application {
  friend watch;
public:
  ecg_application(watch *sdk = NULL);

  ~ecg_application(void);

  m2m2_data_stream<ecg_stream_callback> ecg_stream;

  std::string get_name(void);

  M2M2_ADDR_ENUM_t get_address(void);

  std::vector<uint32_t> get_lcfg(void);

  void get_algo_vendor_version(common_app_version_t *app_version);

  std::vector<std::pair<uint8_t, uint16_t>> lcfg_read(std::vector<uint8_t> addresses);

  std::vector<std::pair<uint8_t, uint16_t>> lcfg_write(std::vector<std::pair<uint8_t, uint16_t>> addr_vals);
};
