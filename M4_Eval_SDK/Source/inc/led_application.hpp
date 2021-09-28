#pragma once

#include "m2m2_application.hpp"
#include "m2m2/cpp/led_interface.hpp"

/*!
   \brief A class representing the LED application.

   The LED application manages the Red/Green LED on the watch.
*/
class led_0_application : public m2m2_application {
  friend watch;
public:
  led_0_application(watch *);

  ~led_0_application(void);

  std::string get_name(void);

  M2M2_ADDR_ENUM_t get_address(void);

  void set_led(uint8_t r_pattern,
               uint8_t g_pattern,
               uint8_t priority = M2M2_LED_PRIORITY_LOW);

  void get_led(uint8_t &r_pattern, uint8_t &g_pattern);
};
