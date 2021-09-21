#pragma once

#include "m2m2_application.hpp"
#include "m2m2_data_stream.hpp"
#include "m2m2/cpp/sensor_ad7156_application_interface.hpp"

/*!
   \brief A class representing the AD7156 application.

*/

class ad7156_application:public m2m2_application {
  friend watch;
public:
	
  ad7156_application(watch *sdk=NULL);
  ~ad7156_application(void);
  m2m2_data_stream<adxl_stream_callback> ad7156_stream; 
  std::string get_name(void);
  M2M2_ADDR_ENUM_t get_address(void);
  std::vector<std::pair<uint16_t, uint16_t>> register_read(std::vector<uint16_t> addresses);
  std::vector<std::pair<uint16_t, uint16_t>> register_write(std::vector<std::pair<uint16_t, uint16_t>> addr_vals);
  
  ret::sdk_status load_cfg(void);

  ret::sdk_status  dcb_delete_config(void);
  ret::sdk_status write_dcb_config(std::vector<std::pair<uint16_t, uint16_t>> addr_vals);
  std::vector<std::pair<uint16_t, uint16_t>> read_dcb_config(void);

};
