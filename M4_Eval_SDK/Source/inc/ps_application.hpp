#pragma once

#include "m2m2_application.hpp"
#include "m2m2/cpp/ps_application_interface.hpp"
#include "sdk_err.hpp"

class ps_application : public m2m2_application {
  friend watch;
public:

  ps_application(watch *sdk = NULL);

  ~ps_application(void);

  std::string get_name(void);

  M2M2_ADDR_ENUM_t get_address(void);

  ret::sdk_status set_date_time(sys_date_time_t *time_info);

  ret::sdk_status set_power_state(uint8_t state);

  ret::sdk_status ping();

  uint8_t activate_touch_sensor();

  uint8_t deactivate_touch_sensor();
};
