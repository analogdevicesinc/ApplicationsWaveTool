#include "ecg_application.hpp"

ecg_application::~ecg_application(void) {

}

ecg_application::ecg_application(watch *sdk) :
    m2m2_application(sdk),
    ecg_stream(M2M2_ADDR_MED_ECG_STREAM, this) {}

/*!
   \brief Fetches a human-readable string describing the application.
*/
std::string ecg_application::get_name(void) {
  return "ECG application";
}

/*!
    \brief Fetches the address of the application.
*/
M2M2_ADDR_ENUM_t ecg_application::get_address(void) {
  return M2M2_ADDR_MED_ECG;
}

/*!
    \brief gets lcfg values from ECG Library Config
*/
std::vector<uint32_t> ecg_application::get_lcfg(void) {
  uint8_t firstbyte;
  uint8_t secondbyte;
  uint32_t value;
  std::vector<uint32_t> vectorlcfgdata;

  m2m2_pkt<m2m2_app_lcfg_data_t> pkt(this->get_address());
  pkt.payload.command = M2M2_APP_COMMON_CMD_GET_LCFG_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;
  auto resp = this->sync_send(pkt.pack());

  pkt.unpack(resp);

  for (int i = 0; i < (pkt.payload.size); i++) {
    firstbyte = pkt.payload.lcfgdata[2 * i];
    secondbyte = pkt.payload.lcfgdata[2 * i + 1];
    value = (firstbyte | (secondbyte << 8));
    vectorlcfgdata.push_back(value);
  }
  return vectorlcfgdata;
}

/*!
   \brief Fetches the algorithm vendor version info of the ECG application.
*/
void ecg_application::get_algo_vendor_version(common_app_version_t *app_version) {
  m2m2_pkt<_m2m2_app_common_cmd_t> pkt(this->get_address());
  pkt.payload.command = M2M2_ECG_APP_CMD_GET_ALGO_VENDOR_VERSION_REQ;
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

/*!
 \brief Read from a set of ECG LCFGs.
 
 This function takes a vector of addresses to read, and returns a vector of
 (field, value) pairs which contain the addresses and values read.
 \note If an invalid list of addresses is given (i.e. an empty list), or an
 error occured on the device, a vector of size 1 with a (address, value) pair
 of 0 is returned.
 \note The SDK does not perform any validation on the correctness of the addresses
 given.
 \see ECG_application::lcfg_write
 \see ECG_application::load_lcfg
 */
std::vector<std::pair<uint8_t, uint16_t>> ecg_application::lcfg_read(
    std::vector<uint8_t> addresses //!< A vector of lcfg addresses to be read.
) {
  if (addresses.size() == 0) {
    // return a single pair of zeroes
    return std::vector<std::pair<uint8_t, uint16_t>> {{0, 0}};
  }
  // Assemble this packet the old school way to fill out the variable-length list of lcfg operations
  uint16_t pkt_size = (offsetof(m2m2_hdr_t, data)) + offsetof(ecg_app_lcfg_op_hdr_t, ops) +
                      addresses.size() * sizeof(ecg_app_lcfg_op_t);
  std::vector<uint8_t> pkt(pkt_size);
  m2m2_hdr_t *p_hdr = (m2m2_hdr_t *) &pkt[0];
  ecg_app_lcfg_op_hdr_t *p_ops_hdr = (ecg_app_lcfg_op_hdr_t *) &p_hdr->data[0];
  ecg_app_lcfg_op_t *p_ops = (ecg_app_lcfg_op_t *) &p_ops_hdr->ops[0];
  p_hdr->length = pkt_size;
  p_ops_hdr->command = M2M2_APP_COMMON_CMD_READ_LCFG_REQ;
  p_ops_hdr->num_ops = addresses.size();

  for (unsigned int i = 0; i < addresses.size(); i++) {
    p_ops[i].field = addresses[i];
    p_ops[i].value = 0x0000;
  }

  auto resp = this->sync_send(pkt);

  std::vector<std::pair<uint8_t, uint16_t>> ret_vals;
  p_hdr = (m2m2_hdr_t *) &resp[0];
  p_ops_hdr = (ecg_app_lcfg_op_hdr_t *) &p_hdr->data[0];
  if (p_ops_hdr->status != M2M2_APP_COMMON_STATUS_OK) {
    // return a single pair of zeroes
    return std::vector<std::pair<uint8_t, uint16_t>> {{0, 0}};
  }
  p_ops = (ecg_app_lcfg_op_t *) &p_ops_hdr->ops[0];

  for (unsigned int i = 0; i < p_ops_hdr->num_ops; i++) {
    std::pair<uint8_t, uint16_t> v;
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
 \see ECG_application::lcfg_read
 \see ECG_application::load_lcfg
 */
std::vector<std::pair<uint8_t, uint16_t>> ecg_application::lcfg_write(
    std::vector<std::pair<uint8_t, uint16_t>> addr_vals //!< A vector of lcfg (field,value) pairs to be written.
) {
  if (addr_vals.size() == 0) {
    // return a single pair of zeroes
    return std::vector<std::pair<uint8_t, uint16_t>> {{0, 0}};
  }

  // Assemble this packet the old school way to fill out the variable-length list of register operations
  uint16_t pkt_size = offsetof(m2m2_hdr_t, data)
                      + offsetof(ecg_app_lcfg_op_hdr_t, ops)
                      + addr_vals.size() * sizeof(ecg_app_lcfg_op_t);
  std::vector<uint8_t> pkt(pkt_size);
  m2m2_hdr_t *p_hdr = (m2m2_hdr_t *) &pkt[0];
  ecg_app_lcfg_op_hdr_t *p_ops_hdr = (ecg_app_lcfg_op_hdr_t *) &p_hdr->data[0];
  ecg_app_lcfg_op_t *p_ops = (ecg_app_lcfg_op_t *) &p_ops_hdr->ops[0];
  p_hdr->length = pkt_size;
  p_ops_hdr->command = M2M2_APP_COMMON_CMD_WRITE_LCFG_REQ;
  p_ops_hdr->num_ops = addr_vals.size();
  for (unsigned int i = 0; i < addr_vals.size(); i++) {
    p_ops[i].field = addr_vals[i].first;
    p_ops[i].value = addr_vals[i].second;
  }
  auto resp = this->sync_send(pkt);

  std::vector<std::pair<uint8_t, uint16_t>> ret_vals;
  p_hdr = (m2m2_hdr_t *) &resp[0];
  p_ops_hdr = (ecg_app_lcfg_op_hdr_t *) &p_hdr->data[0];
  if (p_ops_hdr->status != M2M2_APP_COMMON_STATUS_OK) {
    // return a single pair of zeroes
    return std::vector<std::pair<uint8_t, uint16_t>> {{0, 0}};
  }
  p_ops = (ecg_app_lcfg_op_t *) &p_ops_hdr->ops[0];
  for (unsigned int i = 0; i < p_ops_hdr->num_ops; i++) {
    std::pair<uint8_t, uint16_t> v;
    v.first = p_ops[i].field;
    v.second = p_ops[i].value;
    ret_vals.push_back(v);
  }
  return ret_vals;
}




ret::sdk_status ecg_application::dcb_set_lcfg(void ) {
	m2m2_pkt<_ecg_app_dcb_lcfg_t> pkt(this->get_address());
	pkt.payload.command = M2M2_APP_COMMON_CMD_SET_LCFG_REQ;
	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);
	if (pkt.payload.status == M2M2_APP_COMMON_STATUS_OK) {
		return ret::SDK_OK;
	}
	else {
		return ret::SDK_ERR;
	}
}


ret::sdk_status ecg_application::write_dcb_config(std::vector<std::pair<uint16_t, uint16_t>> addr_vals)
{
	if (addr_vals.size() == 0 || addr_vals.size() > MAXECGDCBSIZE) {
		// return a single pair of zeroes
		return ret::SDK_ERR;
	}
	uint8_t addrs;
	uint16_t value;
	uint32_t  dcb_data_value;
	m2m2_pkt<m2m2_dcb_ecg_data_t> pkt(this->get_address());
	pkt.payload.command = M2M2_DCB_COMMAND_WRITE_CONFIG_REQ;
	pkt.payload.size = addr_vals.size();
	for (unsigned int i = 0; i < addr_vals.size(); i++) {
		addrs = addr_vals[i].first;
		value = addr_vals[i].second;
		dcb_data_value = static_cast<uint32_t>((addrs) | (value << 16));
		pkt.payload.dcbdata[i] = dcb_data_value;
	}

	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);

	if (pkt.payload.status == M2M2_DCB_STATUS_OK) {
		return ret::SDK_OK;
	}
	else {
		return ret::SDK_ERR;
	}

}

std::vector<std::pair<uint16_t, uint16_t>> ecg_application::read_dcb_config(void)
{
	uint32_t  dcb_data_value;
	m2m2_pkt<m2m2_dcb_ecg_data_t> pkt(this->get_address());
	pkt.payload.command = M2M2_DCB_COMMAND_READ_CONFIG_REQ;
	auto resp = this->sync_send(pkt.pack());
	pkt.unpack(resp);

	std::vector<std::pair<uint16_t, uint16_t>> ret_vals;
	for (unsigned int i = 0; i < pkt.payload.size; i++) {
		std::pair<uint16_t, uint16_t> v;

		dcb_data_value = pkt.payload.dcbdata[i];
		v.first = dcb_data_value & 0xffff;
		v.second = (dcb_data_value >> 16) & 0xffff;
		ret_vals.push_back(v);
	}
	return ret_vals;
}

ret::sdk_status  ecg_application::dcb_delete_config(void)
{
	m2m2_pkt<m2m2_dcb_ecg_data_t> pkt(this->get_address());
	pkt.payload.command = M2M2_DCB_COMMAND_ERASE_CONFIG_REQ;
	pkt.payload.status = M2M2_DCB_STATUS_ERR_NOT_CHKD;

	auto resp = this->sync_send(pkt.pack());

	pkt.unpack(resp);

	if (pkt.payload.status == M2M2_DCB_STATUS_OK) {
#if Debug
		std::cout << "ret::SDK_OK" << std::endl;
#endif
		return ret::SDK_OK;
	}
	else {
#if Debug
		std::cout << "ret::SDK_ERR_DEVICE_ERR" << std::endl;
#endif
		return ret::SDK_ERR_DEVICE_ERR;
	}
}