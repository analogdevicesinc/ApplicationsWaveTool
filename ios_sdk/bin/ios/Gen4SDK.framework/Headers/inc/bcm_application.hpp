#pragma once

#include "m2m2_application.hpp"
#include "m2m2_data_stream.hpp"
#include "m2m2/cpp/bcm_application_interface.hpp"

/*!
    \brief A class representing the BCM application.

    The bcm application manages the bcm sensors on the watch.
*/

class bcm_application : public m2m2_application {
  friend watch;
public:
  bcm_application(watch *sdk = NULL);

  ~bcm_application(void);

  m2m2_data_stream<bcm_stream_callback> bcm_stream; //!< bcm data stream

  std::string get_name(void);

  M2M2_ADDR_ENUM_t get_address(void);

  ret::sdk_status set_dft_num(uint32_t dft_num);

  ret::sdk_status enable_or_disable_sweep_frequency(bool enable);

  ret::sdk_status set_hsrtia_cal(uint16_t value);

  std::vector<std::pair<uint8_t, uint32_t>> lcfg_read(std::vector<uint8_t> addresses);

  std::vector<std::pair<uint8_t, uint32_t>> lcfg_write(std::vector<std::pair<uint8_t, uint32_t>> addr_vals);
};
