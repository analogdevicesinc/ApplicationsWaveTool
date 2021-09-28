#include "ad5940_application.hpp"

ad5940_application::~ad5940_application(void) {

}

ad5940_application::ad5940_application(watch *sdk) :
    m2m2_application(sdk) {}

std::string ad5940_application::get_name(void) {
  return "ad5940 sensor application";
}

/*!
   \brief Fetches the address of the application.
*/
M2M2_ADDR_ENUM_t ad5940_application::get_address(void) {
  return M2M2_ADDR_SENSOR_AD5940;
}

ret::sdk_status ad5940_application::set_wg_freq(uint32_t wg_freq) {
  m2m2_pkt<m2m2_sensor_ad5940_set_wg_freq_resp_t> pkt(this->get_address());
  pkt.payload.command = M2M2_SENSOR_AD5940_COMMAND_SET_WG_FREQ_REQ;
  pkt.payload.status = APP_COMMON_STATUS_OK;
  pkt.payload.wg_freq = wg_freq;
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  if (pkt.payload.status == APP_COMMON_STATUS_OK) {
    return ret::SDK_OK;
  }
  return ret::SDK_ERR;
}

ret::sdk_status ad5940_application::set_datarate(SENSOR_AD5940_DATARATE_ENUM datarate) {
  m2m2_pkt<m2m2_sensor_ad5940_set_odr_resp_t> pkt(this->get_address());
  pkt.payload.command = M2M2_SENSOR_AD5940_COMMAND_SET_ODR_REQ;
  pkt.payload.status = APP_COMMON_STATUS_OK;
  pkt.payload.datarate = static_cast<M2M2_SENSOR_AD5940_DATARATE_ENUM_t> (datarate);
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  if (pkt.payload.status == APP_COMMON_STATUS_OK) {
    return ret::SDK_OK;
  }
  return ret::SDK_ERR;
}


/*!
   \brief Read from a set of registers.

   This function takes a vector of addresses to read, and returns a vector of
   (address, value) pairs which contain the addresses and values read.
   \note If an invalid list of addresses is given (i.e. an empty list), or an
   error occured on the device, a vector of size 1 with a (address, value) pair
   of 0 is returned.
   \note The SDK does not perform any validation on the correctness of the addresses
   given.
   \see ad5940_application::register_write
*/
std::vector<std::pair<uint16_t, uint32_t>> ad5940_application::register_read(
    std::vector<uint16_t> addresses //!< A vector of register addresses to be read.
) {
  if (addresses.size() == 0) {
    // return a single pair of zeroes
    return std::vector<std::pair<uint16_t, uint32_t>> {{0, 0}};
  }
  // Assemble this packet the old school way to fill out the variable-length list of register operations
  uint16_t pkt_size = offsetof(m2m2_hdr_t, data)
                      + offsetof(m2m2_sensor_common_reg_op_32_hdr_t, ops)
                      + addresses.size() * sizeof(m2m2_sensor_common_reg_op_32_t);
  std::vector<uint8_t> pkt(pkt_size);
  m2m2_hdr_t *p_hdr = (m2m2_hdr_t *) &pkt[0];
  m2m2_sensor_common_reg_op_32_hdr_t *p_ops_hdr = (m2m2_sensor_common_reg_op_32_hdr_t *) &p_hdr->data[0];
  m2m2_sensor_common_reg_op_32_t *p_ops = (m2m2_sensor_common_reg_op_32_t *) &p_ops_hdr->ops[0];
  p_hdr->length = pkt_size;
  p_ops_hdr->command = M2M2_SENSOR_COMMON_CMD_READ_REG_32_REQ;
  p_ops_hdr->num_ops = addresses.size();
  for (unsigned int i = 0; i < addresses.size(); i++) {
    p_ops[i].address = addresses[i];
    p_ops[i].value = 0x0000;
  }
  auto resp = this->sync_send(pkt);

  std::vector<std::pair<uint16_t, uint32_t>> ret_vals;
  p_hdr = (m2m2_hdr_t *) &resp[0];
  p_ops_hdr = (m2m2_sensor_common_reg_op_32_hdr_t *) &p_hdr->data[0];
  if (p_ops_hdr->status != M2M2_APP_COMMON_STATUS_OK) {
    // return a single pair of zeroes
    return std::vector<std::pair<uint16_t, uint32_t>> {{0, 0}};
  }
  p_ops = (m2m2_sensor_common_reg_op_32_t *) &p_ops_hdr->ops[0];
  for (unsigned int i = 0; i < p_ops_hdr->num_ops; i++) {
    std::pair<uint16_t, uint16_t> v;
    v.first = p_ops[i].address;
    v.second = p_ops[i].value;
    ret_vals.push_back(v);
  }
  return ret_vals;
}

/*!
   \brief Write to a set of registers.
   This function takes a vector of (address, value) pairs to write, and returns
   a vector of (address, value) pairs which contain the addresses and values written.
   \note If an invalid list of addresses is given (i.e. an empty list), or an
   error occured on the device, a vector of size 1 with a (address, value) pair
   of 0 is returned.
   \note The SDK does not perform any validation on the correctness of the addresses
   or values given.
   \see ad5940_application::register_read
*/
std::vector<std::pair<uint16_t, uint32_t>> ad5940_application::register_write(
    std::vector<std::pair<uint16_t, uint32_t>> addr_vals //!< A vector of register (address,value) pairs to be written.
) {
  if (addr_vals.size() == 0) {
    // return a single pair of zeroes
    return std::vector<std::pair<uint16_t, uint32_t>> {{0, 0}};
  }
  // Assemble this packet the old school way to fill out the variable-length list of register operations
  uint16_t pkt_size = offsetof(m2m2_hdr_t, data)
                      + offsetof(m2m2_sensor_common_reg_op_32_hdr_t, ops)
                      + addr_vals.size() * sizeof(m2m2_sensor_common_reg_op_32_t);
  std::vector<uint8_t> pkt(pkt_size);
  m2m2_hdr_t *p_hdr = (m2m2_hdr_t *) &pkt[0];
  m2m2_sensor_common_reg_op_32_hdr_t *p_ops_hdr = (m2m2_sensor_common_reg_op_32_hdr_t *) &p_hdr->data[0];
  m2m2_sensor_common_reg_op_32_t *p_ops = (m2m2_sensor_common_reg_op_32_t *) &p_ops_hdr->ops[0];
  p_hdr->length = pkt_size;
  p_ops_hdr->command = M2M2_SENSOR_COMMON_CMD_WRITE_REG_32_REQ;
  p_ops_hdr->num_ops = addr_vals.size();
  for (unsigned int i = 0; i < addr_vals.size(); i++) {
    p_ops[i].address = addr_vals[i].first;
    p_ops[i].value = addr_vals[i].second;
  }
  auto resp = this->sync_send(pkt);

  std::vector<std::pair<uint16_t, uint32_t>> ret_vals;
  p_hdr = (m2m2_hdr_t *) &resp[0];
  p_ops_hdr = (m2m2_sensor_common_reg_op_32_hdr_t *) &p_hdr->data[0];
  if (p_ops_hdr->status != M2M2_APP_COMMON_STATUS_OK) {
    // return a single pair of zeroes
    return std::vector<std::pair<uint16_t, uint32_t>> {{0, 0}};
  }
  p_ops = (m2m2_sensor_common_reg_op_32_t *) &p_ops_hdr->ops[0];
  for (unsigned int i = 0; i < p_ops_hdr->num_ops; i++) {
    std::pair<uint16_t, uint16_t> v;
    v.first = p_ops[i].address;
    v.second = p_ops[i].value;
    ret_vals.push_back(v);
  }
  return ret_vals;
}

ret::sdk_status ad5940_application::start(void) {
  m2m2_pkt<m2m2_app_common_sub_op_t> pkt(this->get_address());
  pkt.payload.command = M2M2_APP_COMMON_CMD_STREAM_START_REQ;
  pkt.payload.stream = this->get_address();
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  if (pkt.payload.status == (M2M2_APP_COMMON_STATUS_ERROR || M2M2_APP_COMMON_STATUS_STREAM_NOT_STARTED)) {
    return ret::SDK_ERR_DEVICE_ERR;
  }
  return ret::SDK_OK;
}

ret::sdk_status ad5940_application::stop(void) {
  m2m2_pkt<m2m2_app_common_sub_op_t> pkt(this->get_address());
  pkt.payload.command = M2M2_APP_COMMON_CMD_STREAM_STOP_REQ;
  pkt.payload.stream = this->get_address();
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  if (pkt.payload.status == (M2M2_APP_COMMON_STATUS_ERROR || M2M2_APP_COMMON_STATUS_STREAM_NOT_STOPPED)) {
    return ret::SDK_ERR_DEVICE_ERR;
  }
  return ret::SDK_OK;
}

/*!
   \brief Get the status of the stream.
   This function fetches the status of the stream. The status contains information
   like whether the stream has been started, stopped, etc.
   \note
   The stream may still be running even if the SDK has requested that it stop.
   The application on the device that manages the stream will only stop the stream
   once all subscribers have unsubscribed.
   \see m2m2_data_stream::subscribe
   \see m2m2_data_stream::unsubscribe
   \see m2m2_data_stream::start
*/
void ad5940_application::get_status(uint8_t &status, //!< Current status of the stream
                uint8_t &num_subscribers, //!< Number of applications (internal to the device or external) that have subscribed to the stream. If a stream has no subscribers, it will not produce data.
                uint8_t &num_start_requests //!< Number of start requests a stream has received; a stream will not stop producing data until it has received the same number of stop requests as start requests.
) {
  m2m2_pkt<m2m2_app_common_status_t> pkt(this->get_address());
  pkt.payload.command = M2M2_APP_COMMON_CMD_SENSOR_STATUS_QUERY_REQ;
  pkt.payload.stream = this->get_address();
  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  status = pkt.payload.status;
  num_subscribers = pkt.payload.num_subscribers;
  num_start_requests = pkt.payload.num_start_reqs;
}
