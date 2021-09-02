enum Sensors {
  ADPD,
  ADXL,
  PPG,
  ECG,
  EDA,
  TEMP,
}

enum AdpdLED {
  Red,
  Green,
  Blue,
  IR,
}

enum Filters {
  Moving_Average,
  BPF,
  ButterWorth,
  FIR,
  IIR,
}

enum SettingsType {
  ADPD,
  ADXL,
  ECG,
  EDA,
}

enum ConnectStatus {
  Connected,
  ConnectionFailed,
  Disconnected,
  DisconnectionFailed,
  ConnectionInProgress,
  ScanningInProgress,
}

enum ADDRS_ENUM_STREAM {
  ADPD_Stream,
  ADXL_Stream,
  PPG_Stream,
  SYNCPPG_Stream,
  ECG_Stream,
  EDA_Stream,
  TEMPERATURE_Stream,
  PEDOMETER_Stream,
  BCM_Stream,
  HRV_Stream,
  BATTERY_Stream,
  AGC_Stream,
  ADPD1_STREAM,
  ADPD2_STREAM,
  ADPD3_STREAM,
  ADPD4_STREAM,
  ADPD5_STREAM,
  ADPD6_STREAM,
  ADPD7_STREAM,
  ADPD8_STREAM,
  ADPD9_STREAM,
  ADPD10_STREAM,
  ADPD11_STREAM,
  ADPD12_STREAM,
  ADPD_OPTIONAL_STREAM,
}

enum SENSORS_SDK {
  PM,
  ADXL,
  ADPD,
  ADPD4000,
  PPG,
  SYNCPPG,
  ECG,
  EDA,
  AD5940,
  TEMPERATURE,
  BCM,
  PED,
  HRV,
  AGC
}

//index + 40
enum SENSOR_ADPD4000_DEVICE_ID {
  ADPD4000_DEVICE_4000_G,
  ADPD4000_DEVICE_4000_R,
  ADPD4000_DEVICE_4000_IR,
  ADPD4000_DEVICE_4000_B,
}

enum PPG_LCFG_ID {
  ADPD107,
  ADPD185,
  ADPD108,
  ADPD188,
  ADPD4000,
}

//index + 1
enum ADPD4XXX_SOURCE {
  STREAM1,
  STREAM2,
  STREAM3,
  STREAM4,
  STREAM5,
  STREAM6,
  STREAM7,
  STREAM8,
  STREAM9,
  STREAM10,
  STREAM11,
  STREAM12
}

const Map<PPG_LCFG_ID, int> ppgLcfgId = {
  PPG_LCFG_ID.ADPD107: 107,
  PPG_LCFG_ID.ADPD185: 185,
  PPG_LCFG_ID.ADPD108: 108,
  PPG_LCFG_ID.ADPD188: 188,
  PPG_LCFG_ID.ADPD4000: 40,
};
