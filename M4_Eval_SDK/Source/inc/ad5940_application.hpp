#pragma once

#include "m2m2_application.hpp"
#include "m2m2_data_stream.hpp"
#include "sdk_err.hpp"

#include "m2m2/cpp/ad7689_application_interface.hpp"

/*!
   \brief A class representing the AD7689 application.

   This application manages the AD7689 ADC on the watch. This application forms
   the data source for several other applications which process raw ADC
   measurements into usable biomedical signals.
*/
class ad5940_application : public m2m2_application {
  friend watch;
public:

  enum SENSOR_AD5940_DATARATE_ENUM : uint8_t {
    SENSOR_AD5940_DATARATE_25 = M2M2_SENSOR_AD5940_DATARATE_ENUM_t::M2M2_SENSOR_AD5940_DATARATE_25,
    SENSOR_AD5940_DATARATE_50 = M2M2_SENSOR_AD5940_DATARATE_ENUM_t::M2M2_SENSOR_AD5940_DATARATE_50,
    SENSOR_AD5940_DATARATE_100 = M2M2_SENSOR_AD5940_DATARATE_ENUM_t::M2M2_SENSOR_AD5940_DATARATE_100,
    SENSOR_AD5940_DATARATE_200 = M2M2_SENSOR_AD5940_DATARATE_ENUM_t::M2M2_SENSOR_AD5940_DATARATE_200,
    SENSOR_AD5940_DATARATE_500 = M2M2_SENSOR_AD5940_DATARATE_ENUM_t::M2M2_SENSOR_AD5940_DATARATE_500,
  };

  ad5940_application(watch *sdk = NULL);

  ~ad5940_application(void);

  std::string get_name(void);

  M2M2_ADDR_ENUM_t get_address(void);

  ret::sdk_status set_wg_freq(uint32_t wg_freq);

  ret::sdk_status set_datarate(SENSOR_AD5940_DATARATE_ENUM datarate);

  std::vector<std::pair<uint16_t, uint32_t>> register_read(std::vector<uint16_t> addresses);

  std::vector<std::pair<uint16_t, uint32_t>> register_write(std::vector<std::pair<uint16_t, uint32_t>> addr_vals);

  ret::sdk_status start(void);

  ret::sdk_status stop(void);

  void get_status(uint8_t &status, uint8_t &num_subscribers, uint8_t &num_start_requests);
};
