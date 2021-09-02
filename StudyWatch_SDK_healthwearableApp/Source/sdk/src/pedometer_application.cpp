
#include "pedometer_application.hpp"

pedometer_application::~pedometer_application(void) {

}

pedometer_application::pedometer_application(watch *sdk) :
    m2m2_application(sdk),
    pedometer_stream(M2M2_ADDR_MED_PED_STREAM, this) {}

/*!
 \brief Fetches a human-readable string describing the application.
 */
std::string pedometer_application::get_name(void) {
  return "Pedometer";
}

/*!
 \brief Fetches the address of the application.
 */
M2M2_ADDR_ENUM_t pedometer_application::get_address(void) {
  return M2M2_ADDR_MED_PED;
}

/*!
 \brief Fetches the algorithm vendor version info of the Pedometer application.
 */
void pedometer_application::get_algo_vendor_version(common_app_version_t *app_version) {
  m2m2_pkt<_m2m2_app_common_cmd_t> pkt(this->get_address());
  pkt.payload.command = M2M2_PED_APP_CMD_GET_ALGO_VENDOR_VERSION_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;

  auto resp = this->sync_send(pkt.pack());

  m2m2_pkt<m2m2_app_common_version_t> respkt;
  respkt.unpack(resp);

  app_version->major = respkt.payload.major;
  app_version->minor = respkt.payload.minor;
  app_version->patch = respkt.payload.patch;
  app_version->verstr = std::string(reinterpret_cast<char *>(respkt.payload.verstr),
                                    strlen((const char *) respkt.payload.verstr));
  app_version->vendstr = std::string(reinterpret_cast<char *>(respkt.payload.str),
                                     strlen((const char *) respkt.payload.str));

}
