#pragma once

#include "m2m2_application.hpp"
#include "m2m2_data_stream.hpp"
#include "m2m2/cpp/user0_config_application_interface.hpp"

/*!
 \brief A class representing the User0 config application.
 
 */
class user0_config_application : public m2m2_application {
  friend watch;
public:

	enum ID_OPERATION_MODE_ENUM :uint8_t {
		OPERATION_MODE_READ = 0,
		OPERATION_MODE_WRITE = 1,
		OPERATION_MODE_DELETE = 2,
	};

   user0_config_application(watch *sdk = NULL);

  ~user0_config_application(void);

  std::string get_name(void);

  M2M2_ADDR_ENUM_t get_address(void);


  ret::sdk_status  dcb_delete_config(void);
  ret::sdk_status write_dcb_config(std::vector<std::pair<uint16_t, uint32_t>> addr_vals);
  std::vector<std::pair<uint16_t, uint32_t>> read_dcb_config(void);
  std::vector<std::pair<uint8_t, uint16_t>> register_write(std::vector<std::pair<uint8_t, uint16_t>> addr_vals);
  std::vector<std::pair<uint8_t, uint16_t>> register_read(std::vector<uint8_t> addresses);

  ret::sdk_status set_config_app_state(uint8_t state);
  uint8_t get_config_app_state(void);
  ret::sdk_status set_exp_id(ID_OPERATION_MODE_ENUM mode, uint16_t &num);
  ret::sdk_status set_hw_id(ID_OPERATION_MODE_ENUM mode, uint16_t &num);


  std::vector<std::pair<uint8_t, uint32_t>> lcfg_read(std::vector<uint8_t> addresses);

  std::vector<std::pair<uint8_t, uint32_t>> lcfg_write(std::vector<std::pair<uint8_t, uint32_t>> addr_vals);
};
