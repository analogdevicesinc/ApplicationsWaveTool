
#ifdef __cplusplus
extern "C" {
#endif
	#include <stdint.h>
	#include <stdbool.h>
	#include "dart_api_dl.h"


	/*=========================================================================================
	|								Enums used for sdk C wrapper							   |
	===========================================================================================*/
	typedef enum _SENSORS {
		PM = 0, ADXL, ADPD, ADPD4000, PPG, SYNCPPG, ECG, EDA, AD5940, TEMPERATURE, BCM, PED, HRV, AGC
	}SENSORS;

	typedef enum _sdkPlatform : uint8_t {
		python_usb = 0, windows = 1, android = 2, ios = 3, python_ble = 4
	} sdkPlatform;

	typedef enum _SDK_DEVICEID : uint8_t {
		ADPD107WATCH = 0, ADPD185WATCH, ADPD188WATCH, ADPD107CHESTSTRAP, SENSORBOARD3, OPTICAL_BOARD_103, OPTICAL_BOARD_108, OPTICAL_BOARD_4000, OPTICAL_BOARD_4100, STUDY_WATCH
	} SDK_DEVICEID;

	typedef enum _ADPD4XXX_SOURCE {
		STREAM1 = 1, STREAM2, STREAM3, STREAM4, STREAM5, STREAM6, STREAM7, STREAM8, STREAM9, STREAM10, STREAM11, STREAM12
	}ADPD4XXX_SOURCE;

	typedef enum _SENSOR_ADXL_DEVICE_ID {
		ADXL_LCFG_ID_362 = 362,
	}SENSOR_ADXL_DEVICE_ID;

	typedef enum _SENSOR_ADPD4000_DEVICE_ID {
		ADPD4000_DEVICE_4000_G = 40,
		ADPD4000_DEVICE_4000_R = 41,
		ADPD4000_DEVICE_4000_IR = 42,
		ADPD4000_DEVICE_4000_B = 43,
	}SENSOR_ADPD4000_DEVICE_ID;

	typedef enum _SENSOR_ADPD_DEVICE_ID {
		ADPD_DEVICE_107 = 2,
		ADPD_DEVICE_185,
		ADPD_DEVICE_ECG_PPG_185,
		ADPD_DEVICE_ECG_185,
		ADPD_DEVICE_105,
		ADPD_DEVICE_188,
		ADPD_DEVICE_108,
		ADPD_DEVICE_188F,
		ADPD_DEVICE_ECG_PPG_188,
		ADPD_DEVICE_ECG_188,
		ADPDCL_DEVICE_4000_G = 40,
		ADPDCL_DEVICE_4000_R ,
		ADPDCL_DEVICE_4000_IR ,
		ADPDCL_DEVICE_4000_B,
		ADPDCL_DEVICE_4000_G_R_IR_B,
	}SENSOR_ADPD_DEVICE_ID;

	typedef enum _ADPD_SLOT_MODE_ENUM
	{
		SLOTMODE_DISABLED = 0,
		SLOTMODE_4CH_16b = 1,
		SLOTMODE_4CH_32b = 4,
		SLOTMODE_SUM16b = 17,
		SLOTMODE_SUM32b = 20,
	}ADPD_SLOT_MODE_ENUM;

	typedef enum _SDK_STATUS {
		SDK_OK = 0,
		SDK_ERR = 1,
		SDK_ERR_INVALID,
		SDK_DEVICE_ERR,
		SDK_PACKET_TIMEOUT,
	}SDK_STATUS;


	typedef enum _SENSOR_PPG_LCFG_ID {
		PPG_LCFG_ID_ADPD107 = 107,
		PPG_LCFG_ID_ADPD185 = 185,
		PPG_LCFG_ID_ADPD108 = 108,
		PPG_LCFG_ID_ADPD188 = 188,
		PPG_LCFG_ID_ADPD4000 = 40,
	}SENSOR_PPG_LCFG_ID;

	typedef enum _PM_SYS_DG2502_SELECT_ENUM :uint8_t {
		PM_SYS_DG2502_8233_SW = 0,
		PM_SYS_DG2502_5940_SW = 1,
		PM_SYS_DG2502_4K_SW = 2,
	}PM_SYS_DG2502_SELECT_ENUM;

	typedef enum _PM_SYS_CHIP_ID_ENUM :uint8_t {
		PM_SYS_ADXL362 = 1,
		PM_SYS_ADPD4K = 2,
		PM_SYS_ADP5360 = 3,
		PM_SYS_AD5940 = 4,
		PM_SYS_NAND_FLASH = 5,
		PM_SYS_AD7156 = 6,
		//M2M2_PM_SYS_LCD_DISPLAY = 3,
		//M2M2_PM_SYS_AD8233 = 3,
	}PM_SYS_CHIP_ID_ENUM;

	typedef enum  _DISPLAY_SET_COMMAND_ENUM {
		WHITE_DISPLAY = 0,
		BLACK_DISPLAY = 1,
		RED_DISPLAY = 2,
		GREEN_DISPLAY = 3,
		BLUE_DISPLAY = 4,
	}DISPLAY_SET_COMMAND_ENUM;


	typedef enum _SDK_ERR_STATUS {
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
	}SDK_ERR_STATUS;


	typedef enum _ADDRS_ENUM_STREAM {
		ADPD_Stream = 0,
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

	}ADDRS_ENUM_STREAM;

	typedef enum _FS_SYS_FILE_TYPE_ENUM : uint8_t {
		FS_SYS_IS_DIR = 0,
		FS_SYS_IS_FILE = 1,
		FS_SYS_INVALID_TYPE = 2,
	}FS_SYS_FILE_TYPE_ENUM;

	typedef enum _FILE_SYSTEM_STATUS_ENUM {
		FILE_SYS_ERR_LOWEST = 64,
		FILE_SYS_STATUS_OK = 65,
		FILE_SYS_STATUS_ERROR = 66,
		FILE_SYS_END_OF_FILE = 67,
		FILE_SYS_END_OF_DIR = 68,
		FILE_SYS_ERR_INVALID = 69,
		FILE_SYS_ERR_ARGS = 70,
		FILE_SYS_ERR_FORMAT = 71,
		FILE_SYS_ERR_MEMORY_FULL = 72,
		FILE_SYS_ERR_LOG_FORCE_STOPPED = 73,
		FILE_SYS_ERR_MAX_FILE_COUNT = 74,
		FILE_SYS_CONFIG_FILE_FOUND = 75,
		FILE_SYS_CONFIG_FILE_NOT_FOUND = 76,
		FILE_SYS_STATUS_LOGGING_STOPPED = 77,
		FILE_SYS_STATUS_LOGGING_IN_PROGRESS = 78,
		FILE_SYS_STATUS_LOGGING_ERROR = 79,
		FILE_SYS_STATUS_LOGGING_NOT_STARTED = 80,
		FILE_SYS_ERR_BATTERY_LOW = 81,
		FILE_SYS_ERR_POWER_STATE_SHUTDOWN = 82,
		FILE_SYS_ERR_CONFIG_FILE_POSITION = 83,
		FILE_SYS_STATUS_BLOCKS_WRITE_ERROR = 84,
		FILE_SYS_ERR_NOT_CHKD = 255,
	}FILE_SYSTEM_STATUS_ENUM;


	/*=========================================================================================
	|								Structures used for sdk C wrapper						   |
	===========================================================================================*/
//#pragma pack(1)
	typedef struct _registers
	{
		int *address;
		int *value;
		short length;
	}registers;

	typedef struct _agcCtrl
	{
		int *agcType;
		int *agc_ctrlValue;
		short length;
	}agcCtrl;

	typedef struct _adxl_stream_t
	{
		uint16_t sequence_number;
		double timestamp;
		int16_t x;
		int16_t y;
		int16_t z;
	}adxl_stream_t;

	typedef struct _common_app_version_t {
		uint16_t major;
		uint16_t minor;
		uint16_t patch;
		char *verstr;
		char *vendstr;
	}common_app_version;

		typedef struct _app_version_t {
    		char *verstr;
    	}app_version;

	typedef struct _sys_date_time {
		uint16_t year;
		uint8_t month;
		uint8_t day;
		uint8_t hour;
		uint8_t minute;
		uint8_t second;
		uint32_t TZ_sec;
	}sys_date_time;

	typedef struct _pm_sys_info {
		uint16_t version;
		char *mac_addr;
		uint32_t device_id;
		uint32_t model_number;
		uint16_t hw_id;
		uint16_t bom_id;
		uint8_t batch_id;
		uint32_t date;
	}pm_sys_info;

	typedef struct _pm_sys_bat_info {
		double timestamp;
		uint8_t bat_chrg_stat;
		uint8_t bat_lvl;
		uint16_t bat_mv;
		uint16_t bat_temp;
	}pm_sys_bat_info;

	typedef struct _eeprom_info {
		uint32_t manufacture_id;
		uint16_t hw_id;
		uint16_t bom_id;
		uint8_t batch_id;
		uint32_t date;
		uint32_t additional_detail;

	}eeprom_info;

	typedef struct _get_slot_info {
		uint8_t  slot_num;
		uint8_t  slot_enable;
		uint16_t slot_format;
		uint8_t  channel_num;
	}get_slot_info;

	typedef struct _adpd4000_stream_cb_data {
		uint16_t seq_num;
		ADPD4XXX_SOURCE src;
		uint8_t  dark;
		uint8_t  signal;
		uint8_t  impulse;
		uint8_t  lit;
		uint8_t  channel_num;
		double  timestamp;
		int adpddata_d[24];
		short adpd_d_length;
		int adpddata_s[24];
		short adpd_s_length;
		short adpd_l_length;
		int adpddata_l[24];
	}adpd4000_stream_cb_data;

	typedef struct _adpd4000_interrupt_stream_cb_data_t {
		uint16_t seq_num;
		double  timestamp;
		uint16_t  data_int;
		uint16_t  level0_int;
		uint16_t  level1_int;
		uint16_t  tia_ch1_int;
		uint16_t  tia_ch2_int;
	}adpd4000_interrupt_stream_cb_data_t;

	typedef struct _ppg_stream_cb_data {
		uint16_t seqnum;
		double timestamp;    //!< System timestamp in milliseconds
		uint16_t adpdlibstate; //!< Internal ADPDlib state
		float hr;           //!< PPG-derived heart rate
		float confidence;   //!< Heart rate confidence level
		uint16_t hr_type;      //!< Internal HR type
		uint16_t rr_interval;  //!< R-R peak interval
	}ppg_stream_cb_data;

	typedef struct _syncppg_stream_cb_data {
		uint16_t seqnum;
		double timestamp;
		uint32_t ppg;
		int16_t x;
		int16_t y;
		int16_t z;
	}syncppg_stream_cb_data;


	typedef struct _adpd_stream_cb_data_t_sum {
		uint16_t seqnum;
		double    timestamp;  //!< System timestamp in milliseconds
		uint32_t  adpd_data;  //!< ADPD data value
	}adpd_stream_cb_data_t_sum;

	typedef struct _adpd_stream_cb_data_t_4ch {
		uint16_t seqnum;
		double    timestamp;  //!< System timestamp in milliseconds
		uint32_t  ch_data[4];  //!< ADPD data value
	}adpd_stream_cb_data_t_4ch;

	typedef struct _eda_stream_cb_data {
		uint16_t seqnum;
		double timestamp;  //!< System timestamp in milliseconds
		double admittance_real;
		double admittance_img;
		double impedance_real;
		double impedance_img;
		double admittance_magnitude;
		double admittance_phase;
		double impedance_magnitude;
		double impedance_phase;
	}eda_stream_cb_data;

	typedef struct _ecg_stream_cb_data {
		uint16_t seqnum;
		double timestamp;  //!< System timestamp in milliseconds
		uint8_t datatype;   //!< ECG device type 0 - sport, 1 - monitor
		bool leadsoff;   //!< ECG lead detection
		uint8_t HR;         //!< ECG algorithm heart rate
		uint16_t ecg_data;   //!< ECG data value
	}ecg_stream_cb_data;

	typedef struct _temperature_stream_cb_data {
		double timestamp;    //!< System timestamp in milliseconds
		float temp_skin;    //!< Skin temperature in degrees Celcius
		float temp_ambient; //!< Ambient temperature in degrees Celcius
	}temperature_stream_cb_data;


	typedef struct _bcm_stream_cb_data {
		double timestamp;
		int32_t impedance_real;
		int32_t impedance_img;
		uint8_t is_finger_on_leads;
		uint8_t signal_stability;
		double impedance_magnitude;
		double impedance_phase;
		double admittance_magnitude;
		double admittance_phase;
	}bcm_stream_cb_data;

	typedef struct _hrv_stream_cb_data {
		double timestamp;
		int16_t rr_interval;
		uint16_t is_gap;
	}hrv_stream_cb_data;


	typedef struct _agc_stream_cb_data {
		double timestamp;
		uint16_t  mts0;
		uint16_t  mts1;
		uint16_t  mts2;
		uint16_t  mts3;
		uint16_t  mts4;
		uint16_t  mts5;
		uint16_t  setting0;
		uint16_t  setting1;
		uint16_t  setting2;
		uint16_t  setting3;
		uint16_t  setting4;
		uint16_t  setting5;
		uint16_t  setting6;
		uint16_t  setting7;
		uint16_t  setting8;
		uint16_t  setting9;
	}agc_stream_cb_data;

	typedef struct _pedometer_stream_cb_data {
		int32_t NumSteps;
		uint16_t AlgoStatus;
		double TimeStamp;
		int8_t Reserved;
	}pedometer_stream_cb_data;

	typedef struct _ping_stream_cb_data {
		uint16_t pktSize;
	}ping_stream_cb_data;

	typedef struct _get_agc_info {
		uint8_t led_index;
		uint32_t led_ch1[10];
		uint32_t led_ch2[10];
		uint16_t DC0_LEDcurrent;
		uint16_t TIA_ch1_i;
		uint16_t TIA_ch2_i;
	}get_agc_info;

	typedef struct _fs_ls_data {
		uint8_t  status;
		char *filename;
		uint8_t  filetype;
		uint32_t  filesize;
	}fs_ls_data;

	typedef struct _fs_stream_cb_data {
		uint8_t  status;
		uint16_t  length_Stream;
		uint8_t  byte_stream[512];
		uint16_t  crc16;
	}fs_stream_cb_data;

//#pragma pack()
	/*=========================================================================================
	|									Callbacks											   |
	===========================================================================================*/

	typedef void( *Tx_Callback)(char *pBytes, int nLength);
	typedef void( *adxl_stream_Cb)(adxl_stream_t *pData);
//	typedef void( *adpd_stream_sum)(adpd_stream_cb_data_t_sum *pData);
//	typedef void( *adpd_stream_4ch)(adpd_stream_cb_data_t_4ch *pData);
	typedef void( *battery_stream_cb)(pm_sys_bat_info *pBatInfo);
	typedef void( *adpd4xxx_stream_cb)(adpd4000_stream_cb_data *pData);
	typedef void( *ppg_stream_cb)(ppg_stream_cb_data *pData);
	typedef void( *syncppg_stream_cb)(syncppg_stream_cb_data *pData);
	typedef void( *ecg_stream_cb)(ecg_stream_cb_data *pData);
	typedef void( *eda_stream_cb)(eda_stream_cb_data *pData);
	typedef void( *adpd4xxx_interrupt_stream_cb)(adpd4000_interrupt_stream_cb_data_t *pData);
	typedef void(*temperature_stream_cb)(temperature_stream_cb_data *pData);

	typedef void(*bcm_stream_cb)(bcm_stream_cb_data *pData);
	typedef void(*pedometer_stream_cb)(pedometer_stream_cb_data *pData);
	typedef void(*agc_stream_cb)(agc_stream_cb_data *pData);
//	typedef void(*hrv_stream_cb)(hrv_stream_cb_data *pData);

	typedef void(*fs_ls_cb)(fs_ls_data *pData);
	typedef void(*fs_stream_cb)(fs_stream_cb_data *pData);

    typedef void (*sdkAlertCB)(int);
	typedef void(*pingStreamCB)(ping_stream_cb_data *pData);
	/*=========================================================================================
	|									SDK TX & RX											   |
	===========================================================================================*/

	 void  initializesdk(sdkPlatform platform);
	 void  dispatch_SDK(unsigned char *pBytes, int nLength);


	/*=========================================================================================
	|									PM													   |
	===========================================================================================*/
	 void  pm_get_system_info(pm_sys_info *pSysInfo);
	 void  pm_get_mcu_version(uint8_t *pMCU_Type);
	 SDK_STATUS  pm_set_date_time(sys_date_time *pTimeInfo);
	 void  pm_get_date_time(sys_date_time *pTimeInfo);
	 void  pm_get_battery_info(pm_sys_bat_info *pSysBatInfo);
	 SDK_STATUS  pm_set_battery_threshold(uint8_t nLowLvl, uint8_t nCriticalLvl);
	 SDK_STATUS  pm_set_usb_power(bool enable, bool action);
	 SDK_STATUS  pm_set_power_state(uint8_t nPowerState);
	 void  pm_ping(uint16_t nTimes, uint16_t pktsize, pingStreamCB pingcb);
	 SDK_STATUS  pm_enable_or_disable_battery_charger(bool enable);
	 SDK_STATUS  pm_enable_or_disable_thermistor_state_change(bool enable);
	 SDK_STATUS  pm_powerboost_m3(uint8_t nEnableState, uint8_t nOVPValue);
	 uint8_t  pm_powerboost_m4(uint8_t nEnableState);
	 uint8_t  pm_boost_m4(uint8_t nEnableState, uint8_t  nBoostVoltage);
	 uint8_t  pm_read_EEPROM(eeprom_info *pEEPROM_Info);
	 void  pm_subscribe_battery(battery_stream_cb battery_cb);
	 void  pm_unsubscribe_battery();
	 uint8_t pm_get_low_touch_status();
	 uint8_t pm_enable_config_log(bool enable);

	 SDK_STATUS pm_flash_reset();
	 SDK_STATUS pm_system_reset();
	 SDK_STATUS pm_system_hardware_reset();
	 SDK_STATUS pm_dg502_software_control(PM_SYS_DG2502_SELECT_ENUM sw_name, bool enable);
	 SDK_STATUS pm_ldo_control(uint8_t ldo_num, bool ldo_enable);
	 uint16_t pm_get_chipID(PM_SYS_CHIP_ID_ENUM chipname);
	 SDK_STATUS pm_enter_bootloader();
	 SDK_STATUS pm_cap_sense_test(bool enable);
	 SDK_STATUS pm_setMaxPktCombineCnt(uint8_t count);

	/*=========================================================================================
	|									ADXL												   |
	===========================================================================================*/

	 void  subscribe_adxl(adxl_stream_Cb adxl_cb);
	 SDK_STATUS adxl_loadcfg();
	 SDK_STATUS adxl_selfTest();


	/*=========================================================================================
	|									ADPD												   |
	===========================================================================================*/


//   SDK_STATUS  adpd_loadConfig(SENSOR_ADPD_DEVICE_ID device_id);
//	 SDK_STATUS  adpd_clockCalibration(void);
//	 void  adpd_darkoffset(uint32_t *pValue1, uint32_t *pValue2, uint32_t *pValue3, uint32_t *pValue4);
//	 void  adpd_floatConfig(uint32_t *pValue1, uint32_t *pValue2, uint32_t *pValue3);
//	 SDK_STATUS  adpd_getctrvalue(uint32_t *pValue1, uint32_t *pValue2, uint32_t *pValue3);
//	 void  adpd_set_slot(ADPD_SLOT_MODE_ENUM slot_a, ADPD_SLOT_MODE_ENUM slot_b);
//	 void  subscribe_adpd(adpd_stream_sum adpd_sum_cb, adpd_stream_4ch adpd_4ch_cb);


	/*=========================================================================================
	|									ADPD4XXX											   |
	===========================================================================================*/

	 SDK_STATUS  adpd4xxx_load_cfg(SENSOR_ADPD4000_DEVICE_ID deviceid);
	 SDK_STATUS  adpd4xxx_set_slot(uint8_t  nSlotNum, uint8_t  nSlotEnableState, uint16_t  nSlotFormat, uint8_t  nChannelNum);
	 SDK_STATUS  adpd4xxx_get_slot(uint8_t nSlotNum, get_slot_info *pSlotInfo);
	 SDK_STATUS  adpd4xxx_set_slot_active(uint8_t nSlotNum, uint8_t nSlotActvState);
	 SDK_STATUS  adpd4xxx_get_slot_active(uint8_t nSlotNum, uint8_t *pSlotNum, uint8_t *pSlotActvState);
	 SDK_STATUS  adpd4xxx_calibrate_clock(uint8_t nClockCalibID);
	 uint8_t adpd4xxx_getComMode();
	 SDK_STATUS adpd4xxx_setPause(bool enable);
	 void adpd4xxx_write_ecg4kreg(SENSORS sensor, registers *pReg);
	 void adpd4xxx_read_ecg4kreg(SENSORS sensor, unsigned short *pAddress, int nLength, registers *pReg);
	 void adpd4xxx_create_ecg4kreg(SENSORS sensor, registers *pReg);
	 SDK_STATUS adpd4xxx_setadpd4k_fs(uint16_t odr);
	 SDK_STATUS adpd4xxx_disable_Adpd4k_Slots(void);
	 SDK_STATUS adpd4xxx_agc_info(uint8_t led_index, get_agc_info *agc_info);


	 void write_agcCtrl(agcCtrl *pData);
	 void  subscribe_adpd4xxx(ADPD4XXX_SOURCE src);
	 void  unsubscribe_adpd4xxx(ADPD4XXX_SOURCE src);
	 void  subscribe_adpd4xxx_interrupt(adpd4xxx_interrupt_stream_cb adpd4xxx_cb);
	 void  unsubscribe_adpd4xxx_interrupt();

	/*=========================================================================================
	|									PPG													   |
	===========================================================================================*/
	 SDK_STATUS ppg_load_lcfg(SENSOR_PPG_LCFG_ID lcfg_id);
	 void subscribe_ppg(ppg_stream_cb ppg_cb);
	 void subscribe_syncppg(syncppg_stream_cb syncppg_cb);
	void getPPGAlgo(app_version *pVersion);
	void ppg_getLastState(uint8_t *statesarr, uint8_t *length);

	/*=========================================================================================
	|									EDA													   |
	===========================================================================================*/
	 void subscribe_ecg(ecg_stream_cb ecg_cb);
	 SDK_STATUS eda_dcb_set_lcfg();
	 SDK_STATUS eda_dynamic_scaling(bool isEnable, uint16_t minScale, uint16_t maxScale, uint16_t lprtiasel);
	 SDK_STATUS eda_set_dft_num(uint8_t dftnum);


	/*=========================================================================================
	|									ECG													   |
	===========================================================================================*/
	void subscribe_eda(eda_stream_cb eda_cb);
	SDK_STATUS ecg_dcb_set_lcfg();

	/*=========================================================================================
	|									TEMPERATURE											   |
	===========================================================================================*/
	void subscribe_temperature(temperature_stream_cb temp_cb);


	/*=========================================================================================
	|										BCM												   |
	===========================================================================================*/
	void subscribe_bcm(bcm_stream_cb bcm_cb);
	SDK_STATUS bcm_set_dft_num(uint32_t dft_num);
	SDK_STATUS bcm_enable_or_disable_sweep_frequency(bool enable);
	SDK_STATUS bcm_set_hsrtia_cal(uint16_t value);

	/*=========================================================================================
	|									Pedometer											   |
	===========================================================================================*/
	void subscribe_pedometer(pedometer_stream_cb ped_cb);


	/*=========================================================================================
	|									 HRV												  |
	===========================================================================================*/
//	void subscribe_hrv(hrv_stream_cb hrv_cb);


	/*=========================================================================================
	|									AGC													   |
	===========================================================================================*/
	void subscribe_agc(agc_stream_cb agc_cb);

	/*=========================================================================================
	|									Display												   |
	===========================================================================================*/
	SDK_STATUS display_set_colour(DISPLAY_SET_COMMAND_ENUM display_color);
	SDK_STATUS display_backlight_control(bool enable);
	SDK_STATUS display_key_press_test(bool enable);


	/*=========================================================================================
	|									FS													   |
	===========================================================================================*/

	SDK_STATUS fileSystem_format(void);
	void fileSystem_volume_info(uint32_t* total_Memory,
		uint32_t* used_Memory,
		uint16_t* free_Memory);

	void fileSystem_debug_info(ADDRS_ENUM_STREAM stream,
		uint32_t& packets_received,
		uint32_t& packets_missed);
	uint8_t fileSystem_stream_subscription_status(ADDRS_ENUM_STREAM stream);
	SDK_STATUS fileSystem_keyvaluepair(char* value_id);
	uint8_t fileSystem_status(void);
	SDK_STATUS fileSystem_subscribe(ADDRS_ENUM_STREAM stream);
	SDK_STATUS fileSystem_unsubscribe(ADDRS_ENUM_STREAM stream);
	void fileSystem_get_file_count(uint16_t& maxFileCount);
	SDK_STATUS fileSystem_find_config_file(void);

	uint8_t fileSystem_delete_config_file(void);
	uint8_t fileSystem_config_log(bool enable);

	SDK_STATUS fileSystem_logStart(void);
	SDK_STATUS fileSystem_logStop(void);
	uint8_t fileSystem_mount(void);
	void fileSystem_refhr(uint16_t refhr, sys_date_time *ptime_info);
	uint32_t fileSystem_get_number_of_bad_blocks(void);
	uint8_t fileSystem_abort(void);
	void fileSystem_ls(fs_ls_cb fs_ls_cb);
	void fileSystem_get(char *filename,
		fs_stream_cb fs_stream_cb);

	void fileSystem_get_stream_chunk(uint8_t roll_over, uint16_t chunk_num, char *filename, fs_stream_cb fs_stream_cb);

	/*=========================================================================================
	|							Common APPlication(ADPD/ADXL/ADPD4000/PPG)					   |
	===========================================================================================*/

	void get_version(SENSORS sensor, common_app_version *pVersion);							// ( ADPD,ADXL,ADPD4000,PPG,PM ,ECG, EDA, BCM, PED	)
	void read_registers(SENSORS sensor, unsigned short *pAddress, int nLength, registers *pReg);	// (ADPD,ADXL,ADPD4000)
	void get_dcfg(SENSORS sensor, registers *pReg);												// (ADPD,ADXL,ADPD4000)
	void write_register(SENSORS sensor, registers *pReg);										// (ADPD,ADXL,ADPD4000)
	void start_stream(SENSORS sensor);															// (ADPD,ADXL,ADPD4000,PPG,ECG, EDA,AD5940, BCM, PED, AGC, HRV)
	void stop_stream(SENSORS sensor);
	void subscribe_stream(SENSORS sensor);															// (ADPD,ADXL,ADPD4000,PPG,ECG, EDA,AD5940, BCM, PED, AGC, HRV)
	void unsubscribe_stream(SENSORS sensor);													// (ADPD,ADXL,PPG,ECG, EDA, BCM, PED, AGC, HRV)
	void setTimeout(SENSORS sensor, uint16_t time);												// ( ADPD,ADXL,ADPD4000,PPG,PM ,ECG, EDA, BCM, PED)
	uint16_t getTimeout(SENSORS sensor);														// ( ADPD,ADXL,ADPD4000,PPG,Syncppg,PM, ECG, EDA, BCM, PED)
	void stream_status(SENSORS sensor, uint8_t *nSubreq, uint8_t *nStartreq);					// ( ADPD,ADXL,ADPD4000,PPG,SyncPPG , ECG, EDA, BCM, PED, AGC, HRV)
	void get_algo_vendor_version(SENSORS sensor, common_app_version *pVersion);					// (PPG,ECG, PED)
	void  get_lcfg(SENSORS sensor, registers *pReg);												// (PPG,ECG)
	void write_lcfgreg(SENSORS sensor, registers *pReg);										// (PPG,ECG, EDA, BCM)
	void read_lfcgreg(SENSORS sensor, unsigned short *pAddress, int nLength, registers *pReg);  // (PPG,ECG, EDA, BCM)
	SDK_STATUS dcb_deleteConfig(SENSORS sensor);														// (ADXL, ADPD4000, ECG)
	void write_dcgreg(SENSORS sensor, registers *pReg);										// (ADXL, ADPD4000, ECG, PPG)
	void read_dcb_registers(SENSORS sensor, registers *pReg);	// (ADXL, ADPD4000, ECG, PPG)


	/*=========================================================================================
	|									LEVEL 2 API											   |
	===========================================================================================*/

	SDK_ERR_STATUS startADXL();
	SDK_ERR_STATUS stopADXL();

//	SDK_ERR_STATUS startADPD(adpd_stream_sum adpd_sum_cb, adpd_stream_4ch adpd_4ch_cb);
//	SDK_ERR_STATUS stopADPD();

	SDK_ERR_STATUS startPPG();
	SDK_ERR_STATUS stopPPG();

	SDK_ERR_STATUS startADPD4xxx();
	SDK_ERR_STATUS stopADPD4xxx();

	SDK_ERR_STATUS startECG();
	SDK_ERR_STATUS stopECG();

	SDK_ERR_STATUS startEDA();
	SDK_ERR_STATUS stopEDA();

	SDK_ERR_STATUS  startTEMPARATURE();
	SDK_ERR_STATUS  stopTEMPARATURE();

	SDK_ERR_STATUS startADPD4000_g();
	SDK_ERR_STATUS stopADPD4000_g();


	SDK_ERR_STATUS startADPD4000_r();
	SDK_ERR_STATUS stopADPD4000_r();


	SDK_ERR_STATUS startADPD4000_b();
	SDK_ERR_STATUS stopADPD4000_b();


	SDK_ERR_STATUS startADPD4000_ir();
	SDK_ERR_STATUS stopADPD4000_ir();


	SDK_ERR_STATUS startBCM(bcm_stream_cb bcm_cb);
	SDK_ERR_STATUS stopBCM();

//	SDK_ERR_STATUS startHRV(hrv_stream_cb hrv_cb);
//	SDK_ERR_STATUS stopHRV();

	SDK_ERR_STATUS startPEDOMETER(pedometer_stream_cb ped_cb);
	SDK_ERR_STATUS stopPEDOMETER();

	SDK_ERR_STATUS startBATTERY(battery_stream_cb battery_cb);
	SDK_ERR_STATUS stopBATTERY();


	SDK_ERR_STATUS start_logADXL(void);
	SDK_ERR_STATUS stop_logADXL(void);

//	SDK_ERR_STATUS start_logADPD(void);
//	SDK_ERR_STATUS stop_logADPD(void);

	SDK_ERR_STATUS start_logSYNCPPG(void);
	SDK_ERR_STATUS stop_logSYNCPPG(void);

	SDK_ERR_STATUS start_logECG(void);
	SDK_ERR_STATUS stop_logECG(void);

	SDK_ERR_STATUS start_logTemperature(void);
	SDK_ERR_STATUS stop_logTemperature(void);

	SDK_ERR_STATUS start_logEDA(void);
	SDK_ERR_STATUS stop_logEDA(void);

	SDK_ERR_STATUS start_logPedometer(void);
	SDK_ERR_STATUS stop_logPedometer(void);

	SDK_ERR_STATUS start_logBCM(void);
	SDK_ERR_STATUS stop_logBCM(void);

//	SDK_ERR_STATUS start_logHRV(void);
//	SDK_ERR_STATUS stop_logHRV(void);

	SDK_ERR_STATUS start_logBattery(void);
	SDK_ERR_STATUS stop_logBattery(void);

	SDK_ERR_STATUS start_logADPD4000_g(void);
	SDK_ERR_STATUS stop_logADPD4000_g(void);

	SDK_ERR_STATUS start_logADPD4000_r(void);
	SDK_ERR_STATUS stop_logADPD4000_r(void);

	SDK_ERR_STATUS start_logADPD4000_ir(void);
	SDK_ERR_STATUS stop_logADPD4000_ir(void);

	SDK_ERR_STATUS start_logADPD4000_b(void);
	SDK_ERR_STATUS stop_logADPD4000_b(void);


	/*=========================================================================================
	|									WATCH												   |
	===========================================================================================*/

	SDK_DEVICEID getDeviceID();

		/*=========================================================================================
    	|									DART												   |
    	===========================================================================================*/
	DART_EXPORT intptr_t InitDartApiDL(void* data);
	DART_EXPORT void RegisterSendPort(Dart_Port send_port);
	DART_EXPORT void RegisterADPDPort(Dart_Port adpd_port);
	DART_EXPORT void RegisterADXLPort(Dart_Port adxl_port);
	DART_EXPORT void RegisterSyncPPGPort(Dart_Port syncPPG_port);
	DART_EXPORT void RegisterPPGPort(Dart_Port ppg_port);
	DART_EXPORT void RegisterECGPort(Dart_Port ecg_port);
	DART_EXPORT void RegisterEDAPort(Dart_Port eda_port);
	DART_EXPORT void RegisterTempPort(Dart_Port temp_port);

	
#ifdef __cplusplus
}
#endif
