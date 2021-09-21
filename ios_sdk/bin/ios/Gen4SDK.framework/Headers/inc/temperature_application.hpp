#pragma once

#include "m2m2_application.hpp"
#include "m2m2_data_stream.hpp"
#include "m2m2/cpp/temperature_application_interface.hpp"

/*!
    \brief A class representing the Temperature application.

    The temperature application manages the temperature sensors on the watch.
*/
class temperature_application : public m2m2_application {
  friend watch;
public:
  temperature_application(watch *sdk = NULL);

  ~temperature_application(void);

  m2m2_data_stream<temperature_stream_callback> temperature_stream; //!< Temperature data stream
  std::string get_name(void);

  M2M2_ADDR_ENUM_t get_address(void);

  std::vector<std::pair<uint16_t, uint16_t>> get_dcfg();

};
