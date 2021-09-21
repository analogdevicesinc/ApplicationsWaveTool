#pragma once

#include "m2m2_application.hpp"
#include "m2m2_data_stream.hpp"
#include "m2m2/cpp/sqi_application_interface.hpp"

/*!
 \brief A class representing the SyncPPG application.
 
 */
class sqi_application : public m2m2_application {
  friend watch;
public:
	sqi_application(watch *sdk = NULL);

  ~sqi_application(void);

  m2m2_data_stream<sqi_stream_callback> sqi_stream;

  std::string get_name(void);

  M2M2_ADDR_ENUM_t get_address(void);

  ret::sdk_status set_slot(uint16_t  slot_id);
};
