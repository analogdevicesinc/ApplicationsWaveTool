
#include "syncppg_application.hpp"

syncppg_application::~syncppg_application(void) {

}

syncppg_application::syncppg_application(watch *sdk) :
    m2m2_application(sdk),
    syncppg_stream(M2M2_ADDR_MED_SYNC_ADPD_ADXL_STREAM, this) {}

/*!
 \brief Fetches a human-readable string describing the application.
 */
std::string syncppg_application::get_name(void) {
  return "Synchronized PPG and ADXL application";
}

/*!
 \brief Fetches the address of the application.
 */
M2M2_ADDR_ENUM_t syncppg_application::get_address(void) {
  return M2M2_ADDR_MED_SYNC_ADPD_ADXL;
}


