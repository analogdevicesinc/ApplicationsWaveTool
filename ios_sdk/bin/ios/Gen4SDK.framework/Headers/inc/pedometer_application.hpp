#pragma once

#include "m2m2_application.hpp"
#include "m2m2_data_stream.hpp"
#include "m2m2/cpp/pedometer_application_interface.hpp"

/*!
 \brief A class representing the SyncPPG application.
 
 */

class pedometer_application : public m2m2_application {
  friend watch;
public:
  pedometer_application(watch *sdk = NULL);

  ~pedometer_application(void);

  m2m2_data_stream<pedometer_stream_callback> pedometer_stream;

  void get_algo_vendor_version(common_app_version_t *app_version);

  std::string get_name(void);

  M2M2_ADDR_ENUM_t get_address(void);

};

