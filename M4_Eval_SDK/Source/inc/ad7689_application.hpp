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
class ad7689_application : public m2m2_application {
  friend watch;
public:
  ad7689_application(watch *sdk = NULL);

  ~ad7689_application(void);

  std::string get_name(void);

  M2M2_ADDR_ENUM_t get_address(void);

  ret::sdk_status start(void);

  ret::sdk_status stop(void);

  void get_status(uint8_t &status, uint8_t &num_subscribers, uint8_t &num_start_requests);
};
