#pragma once
#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace ret {
enum sdk_status {
  SDK_OK =  0,
  SDK_ERR = 1,
  SDK_ERR_INVALID_VALUE,
  SDK_ERR_DEVICE_ERR,
  SDk_ERR_Packet_TimedOut,
};

enum device_identifiers : uint8_t {
	ADPD107WATCH = 0, ADPD185WATCH, ADPD188WATCH, ADPD107CHESTSTRAP, SENSORBOARD3, OPTICAL_BOARD_103, OPTICAL_BOARD_108, OPTICAL_BOARD_4000, OPTICAL_BOARD_4100,STUDY_WATCH
};
enum err_status : uint8_t {
	OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE = 0,
	UNKNOWN_DEVICE = 1,
	LOAD_CFG_FAILED = 2,
	CLOCK_CALIBRATE_FAILED,
	SET_SLOT_FAILED,
	SUBSCRIBE_FAILED,
	SUBSCRIBED,
	STREAM_START_FAILED,
	STREAM_STARTED,
	STREAM_STOP_FAILED,
	STREAM_STOPPED,
	UNSUBSCRIBE_FAILED,
	UNSUBSCRIBED,
	PPG_LOAD_LCFG_FAILED,
	LCFG_WRITE_FAILED,
	AD7689_START_FAILED,
	AD7689_STOP_FAILED,
	AD5940_START_FAILED,
	AD5940_STOP_FAILED,
	SET_FREQ_FAILED,
	FS_SUBSCRIBE_FAILED,
	FS_UNSUBSCRIBE_FAILED,
	FS_STATUS_ERROR,
	APP_COMMON_ERROR,
	FS_ERR_INVALID,
	FS_ERR_CONFIG_FILE_POSITION,
	FS_ERR_MAX_FILE_COUNT,
	LOGGING_NOT_STARTED,
	LOGGING_STARTED,
	LOGGING_STOPPED,
	FILE_NOT_CLOSED,
	FILE_NOT_OPENED,
	STREAM_IN_PROGRESS,
	LOGGING_ERROR
};

}
#endif
