#include "temperature_application.hpp"

temperature_application::~temperature_application(void) {

}

temperature_application::temperature_application(watch *sdk) :
    m2m2_application(sdk),
    temperature_stream(M2M2_ADDR_MED_TEMPERATURE_STREAM, this) {}

/*!
  \brief Fetches a human-readable string describing the application.
*/
std::string temperature_application::get_name(void) {
  return "Temperature application";
}

/*!
   \brief Fetches the address of the application.
*/
M2M2_ADDR_ENUM_t temperature_application::get_address(void) {
  return M2M2_ADDR_MED_TEMPERATURE;
}


