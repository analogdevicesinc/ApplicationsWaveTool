#pragma once

#include "m2m2_application.hpp"
#include "m2m2_data_stream.hpp"
#include "m2m2/cpp/temperature_application_interface.hpp"

/*!
    \brief A class representing the Temperature application.

    The temperature application manages the temperature sensors on the watch.
*/
class temperature_application : public m2m2_application {
  friend watch;
public:
  temperature_application(watch *sdk = NULL);

  ~temperature_application(void);

  m2m2_data_stream<temperature_stream_callback> temperature_stream; //!< Temperature data stream

  m2m2_data_stream<temperature_stream_callback> temperature_stream1; //!< ADPDCL data stream
  m2m2_data_stream<temperature_stream_callback> temperature_stream2;
  m2m2_data_stream<temperature_stream_callback> temperature_stream3;
  m2m2_data_stream<temperature_stream_callback> temperature_stream4;
  m2m2_data_stream<temperature_stream_callback> temperature_stream5;
  m2m2_data_stream<temperature_stream_callback> temperature_stream6;
  m2m2_data_stream<temperature_stream_callback> temperature_stream7;
  m2m2_data_stream<temperature_stream_callback> temperature_stream8;
  m2m2_data_stream<temperature_stream_callback> temperature_stream9;
  m2m2_data_stream<temperature_stream_callback> temperature_stream10;
  m2m2_data_stream<temperature_stream_callback> temperature_stream11;
  m2m2_data_stream<temperature_stream_callback> temperature_stream12;


  std::string get_name(void);

  M2M2_ADDR_ENUM_t get_address(void);

  std::vector<std::pair<uint16_t, uint16_t>> get_dcfg();

  ret::sdk_status write_dcb_config(std::vector<std::pair<uint16_t, uint32_t>> addr_vals, uint16_t num_packets);

  std::vector<std::pair<uint16_t, uint32_t>> read_dcb_config(void);

  ret::sdk_status  dcb_delete_config(void);
  ret::sdk_status dcb_set_lcfg(void);

  ret::sdk_status write_lcfg(std::vector<std::pair<uint16_t, uint32_t>> addr_vals, uint8_t field);

  std::vector<std::pair<uint16_t, uint32_t>> read_lcfg(uint8_t field);
};
