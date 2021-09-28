%module(directors="13") gen3_sdk
%{
#include "watch.hpp"
#include "callback.hpp"
#include "m2m2_application.hpp"
#include "adxl_application.hpp"
#include "temperature_application.hpp"
#include "eda_application.hpp"
#include "sdk_err.hpp"
#include "adpd_application.hpp"
#include "adpd4000_application.hpp"
#include "ecg_application.hpp"
#include "syncppg_application.hpp"
#include "pm_application.hpp"
#include "ppg_application.hpp"
#include "ps_application.hpp"
#include "pedometer_application.hpp"
#include "fs_application.hpp"
#include "ad7689_application.hpp"
#include "ad5940_application.hpp"
#include "bcm_application.hpp"
%}


%include "std_string.i"
%include "stdint.i"
%include "std_vector.i"
%include "std_pair.i"

/*
Tell SWIG to instantiate the std::vector<uint8_t> template so that proper
bindings can be generated
*/

namespace std {
  %template(vector_uint8_t) std::vector<uint8_t>;
  %template(vector_uint16_t) std::vector<uint16_t>;
  %template(vector_uint32_t) std::vector<uint32_t>;
  %template(vector_int32_t) std::vector<int32_t>;
};


// #############################################################################
// ## WATCH CALLBACKS                                                         ##
// #############################################################################
/* Turn on director wrapping watch_callback so that the watch_callback class
  can be extended by scripting languages to implement callbacks that are called
  from c++ */
%include "watch.hpp"
%feature("director") watch_phy_callback;


%include "m2m2_application.hpp"

// Tell SWIG how to wrap passing-by-reference
%apply uint8_t& OUTPUT { uint8_t& };
%apply uint16_t& OUTPUT { uint16_t& };
%apply uint32_t& OUTPUT { uint32_t& };
%apply int32_t& OUTPUT { int32_t& };
// Tell swig to handle both a std::pair of uint16_t, and a std::vector of std::pair of uint16_t
%template() std::pair<uint16_t,uint16_t>;
%template() std::pair<uint8_t,uint32_t>;
%template() std::pair<uint8_t,uint16_t>;
%template() std::pair<uint16_t,uint32_t>;
%template(PairVector) std::vector<std::pair<uint16_t,uint16_t>>;
%template() std::vector<std::pair<uint16_t,uint32_t>>;
%template(PairVector_ppg) std::vector<std::pair<uint8_t,uint32_t>>;
%template(PairVector_ecg) std::vector<std::pair<uint8_t,uint16_t>>;


%include "led_application.hpp"
%include "ecg_application.hpp"
%include "adxl_application.hpp"
%include "eda_application.hpp"
%include "m2m2_data_stream.hpp"
%include "ad7689_application.hpp"
%include "temperature_application.hpp"
%include "adpd_application.hpp"
%include "adpd4000_application.hpp"
%include "ppg_application.hpp"
%include "syncppg_application.hpp"
%include "pm_application.hpp"
%include "ps_application.hpp"
%include "m2m2_application.hpp"
%include "pedometer_application.hpp"
%include "fs_application.hpp"
%include "ad7689_application.hpp"
%include "ad5940_application.hpp"
%include "bcm_application.hpp"

%include "sdk_err.hpp"


/*
Tell SWIG to:
 - Generate a director for each callback class type (so they can be instantiated and passed into the c++ library)
 - Generate a specialized vector class for the callback type (so a list of the callback data type can be passed out)
 - Generate a specialized instance of a m2m2_data_stream<>, so that the stream objects can be called.
*/

// #############################################################################
// ## ADXL STREAM CALLBACKS                                                   ##
// #############################################################################
%feature("director") adxl_stream_callback;
%template(vector_adxl_stream_cb_data_t) std::vector<adxl_stream_cb_data_t>;
%template(adxl_stream_cb) m2m2_data_stream<adxl_stream_callback>;


// #############################################################################
// ## ECG STREAM CALLBACKS                                                   ##
// #############################################################################
%feature("director") ecg_stream_callback;
%template(vector_ecg_stream_cb_data_t) std::vector<ecg_stream_cb_data_t>;
%template(ecg_stream_cb) m2m2_data_stream<ecg_stream_callback>;

// #############################################################################
// ## EDA STREAM CALLBACKS                                                   ##
// #############################################################################
%feature("director") eda_stream_callback;
%template(vector_eda_stream_cb_data_t) std::vector<eda_stream_cb_data_t>;
%template(eda_stream_cb) m2m2_data_stream<eda_stream_callback>;

// #############################################################################
// ## TEMPERATURE STREAM CALLBACKS                                            ##
// #############################################################################
%feature("director") temperature_stream_callback;
%template(vector_temperature_stream_cb_data_t) std::vector<temperature_stream_cb_data_t>;
%template(temperature_stream_cb) m2m2_data_stream<temperature_stream_callback>;
// #############################################################################
// ## PPG STREAM CALLBACKS                                                   ##
// #############################################################################
%feature("director") ppg_stream_callback;
%template(vector_ppg_stream_cb_data_t) std::vector<ppg_stream_cb_data_t>;
%template(ppg_stream_cb) m2m2_data_stream<ppg_stream_callback>;
// #############################################################################
// ## ADPD STREAM CALLBACKS                                                  ##
// #############################################################################
%feature("director") adpd_stream_callback;
%template(vector_adpd_stream_cb_data_t_sum32) std::vector<adpd_stream_cb_data_t_sum32>;
%template(vector_adpd_stream_cb_data_t_sum16) std::vector<adpd_stream_cb_data_t_sum16>;
%template(vector_adpd_stream_cb_data_t_4CH16) std::vector<adpd_stream_cb_data_t_4CH16>;
%template(vector_adpd_stream_cb_data_t_4CH32) std::vector<adpd_stream_cb_data_t_4CH32>;

%template(adpd_stream_cb) m2m2_data_stream<adpd_stream_callback>;

// #############################################################################
// ## Sync PPG STREAM CALLBACKS                                               ##
// #############################################################################
%feature("director") syncppg_stream_callback;
%template(vector_syncppg_stream_cb_data_t) std::vector<syncppg_stream_cb_data_t>;
%template(syncppg_stream_cb) m2m2_data_stream<syncppg_stream_callback>;
// #############################################################################
// ## ADPD4000 STREAM CALLBACKS                                                  ##
// #############################################################################
%feature("director") adpd4000_stream_callback;
%template(vector_adpd4000_stream_cb_data_t) std::vector<adpd4000_stream_cb_data_t>;
%template(vector_adpd4000_optional_stream_cb_data_t) std::vector<adpd4000_optional_stream_cb_data_t>;
%template(adpd4000_stream_cb) m2m2_data_stream<adpd4000_stream_callback>;
// #############################################################################
// ## PEDOMETER STREAM CALLBACKS                                               ##
// #############################################################################
%feature("director") pedometer_stream_callback;
%template(vector_pedometer_stream_cb_data_t) std::vector<pedometer_stream_cb_data_t>;
%template(pedometer_stream_cb) m2m2_data_stream<pedometer_stream_callback>;

// #############################################################################
// ## FS-LS RESPONSE CALLBACKS                                               ##
// #############################################################################
%feature("director") fs_ls_callback;
%template(vector_fs_ls_data_t) std::vector<fs_ls_data_t>;
%template(fs_ls_cb) m2m2_data_stream<fs_ls_callback>;

// #############################################################################
// ## FS STREAM CALLBACKS                                               ##
// #############################################################################
%feature("director") fs_stream_callback;
%template(vector_fs_stream_cb_data_t) std::vector<fs_stream_cb_data_t>;
%template(fs_stream_cb) m2m2_data_stream<fs_stream_callback>;

// #############################################################################
// ## BATTERY STREAM CALLBACKS                                                ##
// #############################################################################
%feature("director") battery_stream_callback;
%template(vector_battery_stream_cb_data_t) std::vector<battery_stream_cb_data_t>;
%template(Battery_stream_cb) m2m2_data_stream<battery_stream_callback>;

// #############################################################################
// ## BCM STREAM CALLBACKS                                                ##
// #############################################################################
%feature("director") bcm_stream_callback;
%template(vector_bcm_stream_cb_data_t) std::vector<bcm_stream_cb_data_t>;
%template(bcm_stream_cb) m2m2_data_stream<bcm_stream_callback>;

// #############################################################################
// ## HRV STREAM CALLBACKS                                                ##
// #############################################################################
%feature("director") hrv_stream_callback;
%template(vector_hrv_stream_cb_data_t) std::vector<hrv_stream_cb_data_t>;
%template(hrv_stream_cb) m2m2_data_stream<hrv_stream_callback>;

// #############################################################################
// ## AGC STREAM CALLBACKS                                                    ##
// #############################################################################
%feature("director") agc_stream_callback;
%template(vector_agc_stream_cb_data_t) std::vector<agc_stream_cb_data_t>;
%template(agc_stream_cb) m2m2_data_stream<agc_stream_callback>;


// This needs to be included after all the callback structures mappings are declared
%include "callback.hpp"
