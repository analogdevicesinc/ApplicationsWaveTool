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
  m2m2_pkt<bcm_app_set_dft_num_t> pkt(this->get_address());
  pkt.payload.command = M2M2_BCM_APP_CMD_SET_DFT_NUM_REQ;
  pkt.payload.status = APP_COMMON_STATUS_OK;
  pkt.payload.dftnum = dft_num;
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


ret::sdk_status bcm_application::set_hsrtia_cal(uint16_t value)
{
	m2m2_pkt<bcm_app_hs_rtia_sel_t> pkt(this->get_address());
	pkt.payload.command = M2M2_BCM_APP_CMD_SET_HS_RTIA_CAL_REQ;
	pkt.payload.status = APP_COMMON_STATUS_OK;
	pkt.payload.hsritasel = value;
	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);
	if (pkt.payload.status == APP_COMMON_STATUS_OK) {
		return ret::SDK_OK;
	}
	return ret::SDK_ERR;
}


/*!
   \brief Read from a set of BCM LCFGs.

   This function takes a vector of addresses to read, and returns a vector of
   (field, value) pairs which contain the addresses and values read.
   \note If an invalid list of addresses is given (i.e. an empty list), or an
   error occured on the device, a vector of size 1 with a (address, value) pair
   of 0 is returned.
   \note The SDK does not perform any validation on the correctness of the addresses
   given.
   \see ppg_application::lcfg_write
   \see ppg_application::load_lcfg
*/
std::vector<std::pair<uint8_t, uint32_t>> bcm_application::lcfg_read(
	std::vector<uint8_t> addresses //!< A vector of lcfg addresses to be read.
) {
	if (addresses.size() == 0) {
		// return a single pair of zeroes
		return std::vector<std::pair<uint8_t, uint32_t>> { {0, 0}};
	}
	// Assemble this packet the old school way to fill out the variable-length list of lcfg operations
	uint16_t pkt_size = (offsetof(m2m2_hdr_t, data)) + offsetof(bcm_app_lcfg_op_hdr_t, ops) +
		addresses.size() * sizeof(bcm_app_lcfg_op_t);
	std::vector<uint8_t> pkt(pkt_size);
	m2m2_hdr_t *p_hdr = (m2m2_hdr_t *)&pkt[0];
	bcm_app_lcfg_op_hdr_t *p_ops_hdr = (bcm_app_lcfg_op_hdr_t *)&p_hdr->data[0];
	bcm_app_lcfg_op_t *p_ops = (bcm_app_lcfg_op_t *)&p_ops_hdr->ops[0];
	p_hdr->length = pkt_size;
	p_ops_hdr->command = M2M2_APP_COMMON_CMD_READ_LCFG_REQ;
	p_ops_hdr->num_ops = addresses.size();

	for (unsigned int i = 0; i < addresses.size(); i++) {
		p_ops[i].field = addresses[i];
		p_ops[i].value = 0x0000;
	}

	auto resp = this->sync_send(pkt);

	std::vector<std::pair<uint8_t, uint32_t>> ret_vals;
	p_hdr = (m2m2_hdr_t *)&resp[0];
	p_ops_hdr = (bcm_app_lcfg_op_hdr_t *)&p_hdr->data[0];
	if (p_ops_hdr->status != M2M2_APP_COMMON_STATUS_OK) {
		// return a single pair of zeroes
		return std::vector<std::pair<uint8_t, uint32_t>> { {0, 0}};
	}
	p_ops = (bcm_app_lcfg_op_t *)&p_ops_hdr->ops[0];

	for (unsigned int i = 0; i < p_ops_hdr->num_ops; i++) {
		std::pair<uint8_t, uint32_t> v;
		v.first = p_ops[i].field;
		v.second = p_ops[i].value;
		ret_vals.push_back(v);
	}

	return ret_vals;
}

/*!
   \brief Write to a set of lcfgs.
   This function takes a vector of (field, value) pairs to write, and returns
   a vector of (field, value) pairs which contain the field and values written.
   \note If an invalid list of addresses is given (i.e. an empty list), or an
   error occured on the device, a vector of size 1 with a (address, value) pair
   of 0 is returned.
   \note The SDK does not perform any validation on the correctness of the addresses
   or values given.
   \see ppg_application::lcfg_read
   \see ppg_application::load_lcfg
*/

std::vector<std::pair<uint8_t, uint32_t>> bcm_application::lcfg_write(
	std::vector<std::pair<uint8_t, uint32_t>> addr_vals //!< A vector of lcfg (field,value) pairs to be written.
) {
	if (addr_vals.size() == 0) {
		// return a single pair of zeroes
		return std::vector<std::pair<uint8_t, uint32_t>> { {0, 0}};
	}

	// Assemble this packet the old school way to fill out the variable-length list of register operations
	uint16_t pkt_size = offsetof(m2m2_hdr_t, data)
		+ offsetof(bcm_app_lcfg_op_hdr_t, ops)
		+ addr_vals.size() * sizeof(bcm_app_lcfg_op_t);
	std::vector<uint8_t> pkt(pkt_size);
	m2m2_hdr_t *p_hdr = (m2m2_hdr_t *)&pkt[0];
	bcm_app_lcfg_op_hdr_t *p_ops_hdr = (bcm_app_lcfg_op_hdr_t *)&p_hdr->data[0];
	bcm_app_lcfg_op_t *p_ops = (bcm_app_lcfg_op_t *)&p_ops_hdr->ops[0];
	p_hdr->length = pkt_size;
	p_ops_hdr->command = M2M2_APP_COMMON_CMD_WRITE_LCFG_REQ;
	p_ops_hdr->num_ops = addr_vals.size();
	for (unsigned int i = 0; i < addr_vals.size(); i++) {
		p_ops[i].field = addr_vals[i].first;
		p_ops[i].value = addr_vals[i].second;
	}
	auto resp = this->sync_send(pkt);

	std::vector<std::pair<uint8_t, uint32_t>> ret_vals;
	p_hdr = (m2m2_hdr_t *)&resp[0];
	p_ops_hdr = (bcm_app_lcfg_op_hdr_t *)&p_hdr->data[0];
	if (p_ops_hdr->status != M2M2_APP_COMMON_STATUS_OK) {
		// return a single pair of zeroes
		return std::vector<std::pair<uint8_t, uint32_t>> { {0, 0}};
	}
	p_ops = (bcm_app_lcfg_op_t *)&p_ops_hdr->ops[0];
	for (unsigned int i = 0; i < p_ops_hdr->num_ops; i++) {
		std::pair<uint8_t, uint32_t> v;
		v.first = p_ops[i].field;
		v.second = p_ops[i].value;
		ret_vals.push_back(v);
	}
	return ret_vals;
}

