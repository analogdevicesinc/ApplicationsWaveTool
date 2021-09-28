#include "led_application.hpp"
#include <iostream>

led_0_application::~led_0_application(void) {}

led_0_application::led_0_application(watch *sdk) : m2m2_application(sdk) {}

/*!
   \brief Fetches a human-readable string describing the application.
*/
std::string led_0_application::get_name(void) {
  return "LED application";
}

/*!
   \brief Fetches the address of the application.
*/
M2M2_ADDR_ENUM_t led_0_application::get_address(void) {
  return M2M2_ADDR_SYS_LED_0;
}


/*!
   \brief Sets the red/green LED pattern.

   \note
   The LED application works by defining a pattern for each colour. Every 1/4s,
   the application runs, and sets each LED on or off depending on the value of the
   LSB of the pattern for that LED colour. The patterns are all then circularly
   shifted by one bit. In this way, simple patterns of LED colours are defined.
   For example:
   - A pattern of b10000000 would cause the LED to be on for 1/4s, then off for 7/4s.
   - A pattern of b11111111 would cause the LED to be on solid.
   - A pattern of b10101010 would cause the LED to blink at a rate of 2Hz.

   \see led_0_application::get_led
*/
void led_0_application::set_led(uint8_t r_pattern,  //!< The red LED pattern to be set.
                                uint8_t g_pattern,  //!< The green LED pattern to be set.
                                uint8_t priority    //!< Priority of the command. Shouldn't normally be used.
) {
  m2m2_pkt<m2m2_led_ctrl_t> pkt(this->get_address());
  pkt.payload.command = M2M2_LED_COMMAND_SET;
  pkt.payload.priority = static_cast<M2M2_LED_PRIORITY_ENUM_t>(priority);
  pkt.payload.r_pattern = static_cast<M2M2_LED_PATTERN_ENUM_t>(r_pattern);
  pkt.payload.b_pattern = M2M2_LED_PATTERN_OFF;
  pkt.payload.g_pattern = static_cast<M2M2_LED_PATTERN_ENUM_t>(g_pattern);
  this->async_send(pkt.pack());
}

/*!
   \brief Fetches the current pattern for each LED.

   \note
   The patterns follow the same formatting as led_0_application::set_led.
   \see led_0_application::set_led
*/
void led_0_application::get_led(uint8_t &r_pattern,   //!< The current pattern of the red LED.
                                uint8_t &g_pattern    //!< The current pattern of the green LED.
) {
  m2m2_pkt<m2m2_led_ctrl_t> pkt(this->get_address());
  pkt.payload.command = M2M2_LED_COMMAND_GET;
  auto resp = this->sync_send(pkt.pack());

  pkt.unpack(resp);
  r_pattern = pkt.payload.r_pattern;
  g_pattern = pkt.payload.g_pattern;

  return;
}
