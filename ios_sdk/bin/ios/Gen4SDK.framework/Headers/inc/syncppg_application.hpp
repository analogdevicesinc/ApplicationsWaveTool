#pragma once

#include "m2m2_application.hpp"
#include "m2m2_data_stream.hpp"
#include "m2m2/cpp/sync_data_application_interface.hpp"

/*!
 \brief A class representing the SyncPPG application.
 
 */
class syncppg_application : public m2m2_application {
  friend watch;
public:
  syncppg_application(watch *sdk = NULL);

  ~syncppg_application(void);

  m2m2_data_stream<syncppg_stream_callback> syncppg_stream;

  std::string get_name(void);

  M2M2_ADDR_ENUM_t get_address(void);
};
