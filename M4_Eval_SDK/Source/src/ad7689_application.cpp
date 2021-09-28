#include "ad7689_application.hpp"

ad7689_application::~ad7689_application(void) {

}

ad7689_application::ad7689_application(watch *sdk) :
    m2m2_application(sdk) {}

std::string ad7689_application::get_name(void) {
  return "ad7689 sensor application";
}

/*!
   \brief Fetches the address of the application.
*/
M2M2_ADDR_ENUM_t ad7689_application::get_address(void) {
  return M2M2_ADDR_SENSOR_AD7689;
}

ret::sdk_status ad7689_application::start(void) {
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

ret::sdk_status ad7689_application::stop(void) {
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
void ad7689_application::get_status(uint8_t &status, //!< Current status of the stream
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
