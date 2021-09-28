#include "bcm_application.hpp"

bcm_application::~bcm_application(void) {

}

bcm_application::bcm_application(watch *sdk) :
    m2m2_application(sdk),
    bcm_stream(M2M2_ADDR_MED_BCM_STREAM, this) {}

/*!
  \brief Fetches a human-readable string describing the application.
*/
std::string bcm_application::get_name(void) {
  return "BCM application";
}

/*!
   \brief Fetches the address of the application.
*/
M2M2_ADDR_ENUM_t bcm_application::get_address(void) {
  return M2M2_ADDR_MED_BCM;
}

ret::sdk_status bcm_application::set_dft_num(uint32_t dft_num) {
  m2m2_pkt<m2m2_sensor_ad5940_set_dft_num_resp_t> pkt(this->get_address());
  pkt.payload.command = M2M2_BCM_APP_CMD_SET_DFT_NUM_REQ;
  pkt.payload.status = APP_COMMON_STATUS_OK;
  pkt.payload.dft_num = dft_num;
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  if (pkt.payload.status == APP_COMMON_STATUS_OK) {
    return ret::SDK_OK;
  }
  return ret::SDK_ERR;
}

ret::sdk_status bcm_application::enable_or_disable_sweep_frequency(bool enable) {
  m2m2_pkt<m2m2_bcm_app_sweep_freq_resp_t> pkt(this->get_address());
  if (enable)
    pkt.payload.command = M2M2_BCM_APP_CMD_SWEEP_FREQ_ENABLE_REQ;
  else
    pkt.payload.command = M2M2_BCM_APP_CMD_SWEEP_FREQ_DISABLE_REQ;
  pkt.payload.status = APP_COMMON_STATUS_OK;
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  if (pkt.payload.status == APP_COMMON_STATUS_OK) {
    return ret::SDK_OK;
  }
  return ret::SDK_ERR;
}


