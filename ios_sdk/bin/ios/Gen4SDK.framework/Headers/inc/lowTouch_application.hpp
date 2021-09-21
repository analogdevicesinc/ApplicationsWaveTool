#pragma once

#include "m2m2_application.hpp"
#include "m2m2_data_stream.hpp"
#include "m2m2/cpp/low_touch_application_interface.hpp"

/*!
 \brief A class representing the low Touch application.
 
 */
class lowTouch_application : public m2m2_application {
  friend watch;
public:
	lowTouch_application(watch *sdk = NULL);

  ~lowTouch_application(void);

  m2m2_data_stream<pedometer_stream_callback> lowTouch_stream;

  std::string get_name(void);
  M2M2_ADDR_ENUM_t get_address(void);

  std::vector<std::pair<uint8_t, uint16_t>> lcfg_read(std::vector<uint8_t> addresses);
  std::vector<std::pair<uint8_t, uint16_t>> lcfg_write(std::vector<std::pair<uint8_t, uint16_t>> addr_vals);
  
  ret::sdk_status  write_dcb_config(std::vector<std::pair<uint16_t, uint16_t>> addr_vals);
  std::vector<std::pair<uint16_t, uint16_t>> read_dcb_config(void);
  ret::sdk_status  dcb_delete_config(void);

};
