
#include "sqi_application.hpp"

sqi_application::~sqi_application(void) {

}

sqi_application::sqi_application(watch *sdk) :
    m2m2_application(sdk),
    sqi_stream(M2M2_ADDR_MED_SQI_STREAM, this) {}

/*!
 \brief Fetches a human-readable string describing the application.
 */
std::string sqi_application::get_name(void) {
  return "SQI application";
}

/*!
 \brief Fetches the address of the application.
 */
M2M2_ADDR_ENUM_t sqi_application::get_address(void) {
  return M2M2_ADDR_MED_SQI;
}


ret::sdk_status sqi_application::set_slot(uint16_t  slot_id)
{
	m2m2_pkt<sqi_app_set_slot_t> pkt(this->get_address());
	pkt.payload.command = M2M2_SQI_APP_CMD_SET_SLOT_REQ;
	pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;

	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);
	if (pkt.payload.status != M2M2_PM_SYS_STATUS_OK || pkt.payload.status != M2M2_APP_COMMON_STATUS_OK) {
		return ret::SDK_ERR;
	}
	return ret::SDK_OK;
}

