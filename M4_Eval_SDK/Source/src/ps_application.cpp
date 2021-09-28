#include "ps_application.hpp"
#include "pm_application.hpp"

ps_application::~ps_application(void) {}

ps_application::ps_application(watch *sdk) : m2m2_application(sdk) {}

/*!
   \brief Fetches a human-readable string describing the application.
*/
std::string ps_application::get_name() {
  return "PS application";
}

/*!
   \brief Fetches the address of the application.
*/
M2M2_ADDR_ENUM_t ps_application::get_address(void) {
  return M2M2_ADDR_SYS_PS;
}


/*!
   \brief To set power state HIBERNATE or FLEXI or SHUTDOWN
*/
ret::sdk_status ps_application::set_power_state(uint8_t state) {
  m2m2_pkt<m2m2_ps_sys_pwr_state_t> pkt(this->get_address());
  pkt.payload.command = M2M2_PS_SYS_COMMAND_SET_PWR_STATE_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;
  if (state >= 0 && state <= 3) {
    pkt.payload.state = static_cast<M2M2_PS_SYS_PWR_STATE_ENUM_t>(state);
  } else {
    std::cout << "ADI SDK:: Error! Power State Input Value:" << state << "Invalid";
  }

  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);
  if (pkt.payload.status != M2M2_PM_SYS_STATUS_OK) {
    return ret::SDK_ERR;
  }
  return ret::SDK_OK;
}

/*!
   \brief ping returns whether the application is alive or not
*/
ret::sdk_status ps_application::ping() {
  m2m2_pkt<m2m2_app_common_ping_t> pkt(this->get_address());
  m2m2_pkt<m2m2_app_common_ping_t> respkt(this->get_address());

  pkt.payload.command = M2M2_APP_COMMON_CMD_PING_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;
  pkt.payload.sequence_num = 1;
  auto resp = this->sync_send(pkt.pack());

  respkt.unpack(resp);

  if (respkt.payload.sequence_num != pkt.payload.sequence_num + 1) {
    return ret::SDK_ERR;
  }
  return ret::SDK_OK;
}

/*!
   \brief set local time information to PS application
*/
ret::sdk_status ps_application::set_date_time(sys_date_time_t *time_info) {

  m2m2_pkt<m2m2_ps_sys_date_time_req_t> pkt(this->get_address());
  pkt.payload.command = M2M2_PS_SYS_COMMAND_SET_DATE_TIME_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;

  sys_date_time_t *sys_date_time = &time_info[0];

  pkt.payload.year = sys_date_time->year;
  pkt.payload.month = sys_date_time->month;
  pkt.payload.day = sys_date_time->day;
  pkt.payload.hour = sys_date_time->hour;
  pkt.payload.minute = sys_date_time->minute;
  pkt.payload.second = sys_date_time->second;
  pkt.payload.TZ_sec = sys_date_time->TZ_sec;

  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);

  if (pkt.payload.status != M2M2_PS_SYS_STATUS_OK) {
    return ret::SDK_ERR;
  }
  return ret::SDK_OK;
}


uint8_t ps_application::activate_touch_sensor() {
  m2m2_pkt<m2m2_ps_sys_cmd_t> pkt(this->get_address());
  pkt.payload.command = M2M2_PS_SYS_COMMAND_ACTIVATE_TOUCH_SENSOR_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;

  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);

  return pkt.payload.status;
}

uint8_t ps_application::deactivate_touch_sensor() {
  m2m2_pkt<m2m2_ps_sys_cmd_t> pkt(this->get_address());
  pkt.payload.command = M2M2_PS_SYS_COMMAND_DEACTIVATE_TOUCH_SENSOR_REQ;
  pkt.payload.status = M2M2_APP_COMMON_STATUS_OK;

  auto resp = this->sync_send(pkt.pack());
  pkt.unpack(resp);

  return pkt.payload.status;
}