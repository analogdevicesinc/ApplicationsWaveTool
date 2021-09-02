import 'dart:async';
import 'dart:typed_data';
import 'package:flutter/services.dart';
import 'dart:ffi'; // For FFI
import 'dart:io';
import 'package:ffi/ffi.dart';

/*=========================================================================================
	|								Enums used for sdk C wrapper							   |
	===========================================================================================*/
enum SENSORS {
  PM,
  ADXL,
  ADPD,
  ADPD4000,
  PPG,
  SYNCPPG,
  ECG,
  EDA,
  AD5940,
  TEMPERATURE
}

enum SDK_STATUS {
  SDK_OK,
  SDK_ERR,
  SDK_ERR_INVALID,
  SDK_DEVICE_ERR,
  SDK_PACKET_TIMEOUT,
}

enum SDK_PLATFORM
{
  python_usb,
  windows,
  android,
  ios,
  python_ble,
}

/*=========================================================================================
	|								Structures used for sdk C wrapper						   |
	===========================================================================================*/

class AdxlStream extends Struct {
  @Uint16()
  int sequenceNumber;

  @Double()
  double timestamp;

  @Int16()
  int x;

  @Int16()
  int y;

  @Int16()
  int z;

  factory AdxlStream.allocate(
          int sequenceNumber, double timestamp, int x, int y, int z) =>
      allocate<AdxlStream>().ref
        ..sequenceNumber = sequenceNumber
        ..timestamp = timestamp
        ..x = x
        ..y = y
        ..z = z;
}

class SystemDateTime extends Struct {
  @Uint16()
  int year;

  @Uint8()
  int month;

  @Uint8()
  int day;

  @Uint8()
  int hour;

  @Uint8()
  int minute;

  @Uint8()
  int second;

  @Uint32()
  int tzSec;

  factory SystemDateTime.allocate(int year, int month, int day, int hour,
          int minute, int second, int tzSec) =>
      allocate<SystemDateTime>().ref
        ..year = year
        ..month = month
        ..day = day
        ..hour = hour
        ..minute = minute
        ..second = second
        ..tzSec = tzSec;
}

class PMSystemInfo extends Struct {
  @Uint16()
  int version;

  Pointer<Utf8> macAddress;

  @Uint32()
  int deviceId;

  @Uint32()
  int modelNumber;

  @Uint16()
  int hwId;

  @Uint16()
  int bomId;

  @Uint8()
  int batchId;

  @Uint32()
  int date;

  factory PMSystemInfo.allocate(
          int version,
          Pointer<Utf8> macAddress,
          int deviceId,
          int modelNumber,
          int hwId,
          int bomId,
          int batchId,
          int date) =>
      allocate<PMSystemInfo>().ref
        ..version = version
        ..macAddress = macAddress
        ..deviceId = deviceId
        ..modelNumber = modelNumber
        ..hwId = hwId
        ..bomId = bomId
        ..batchId = batchId
        ..date = date;
}

class PMSystemBatteryInfo extends Struct {
  @Double()
  double timestamp;

  @Uint8()
  int chargeStatus;

  @Uint8()
  int batteryLevel;

  @Uint16()
  int batteryMilliVolt;

  @Uint16()
  int batteryTemp;

  factory PMSystemBatteryInfo.allocate(double timestamp, int _chargeStatus,
          int _batteryLevel, int _batteryMilliVolt, int _batteryTemp) =>
      allocate<PMSystemBatteryInfo>().ref
        ..timestamp = timestamp
        ..chargeStatus = _chargeStatus
        ..batteryLevel = _batteryLevel
        ..batteryMilliVolt = _batteryMilliVolt
        ..batteryTemp = _batteryTemp;
}

class EEPROMInfo extends Struct {
  @Uint32()
  int manufactureId;

  @Uint16()
  int hwId;

  @Uint16()
  int bomId;

  @Uint8()
  int batchId;

  @Uint32()
  int date;

  @Uint32()
  int additionalDetail;

  factory EEPROMInfo.allocate(int manufactureId, int hwId, int bomId,
          int batchId, int date, int _additionalDetail) =>
      allocate<EEPROMInfo>().ref
        ..manufactureId = manufactureId
        ..hwId = hwId
        ..bomId = bomId
        ..batchId = batchId
        ..date = date
        ..additionalDetail = _additionalDetail;
}

class Registers extends Struct {
  Pointer<Int32> address;

  Pointer<Int32> value;

  @Int16()
  int length;

  factory Registers.allocate(
          Pointer<Int32> _address, Pointer<Int32> _value, int _length) =>
      allocate<Registers>().ref
        ..address = _address
        ..value = _value
        ..length = _length;
}

class GetSlotInfo extends Struct {
  @Uint8()
  int slotNum;

  @Uint8()
  int slotEnable;

  @Uint16()
  int slotFormat;

  @Uint8()
  int channelNum;

  factory GetSlotInfo.allocate(
          int _slotNum, int _slotEnable, int _slotFormat, int _channelNum) =>
      allocate<GetSlotInfo>().ref
        ..slotNum = _slotNum
        ..slotEnable = _slotEnable
        ..slotFormat = _slotFormat
        ..channelNum = _channelNum;
}

class Adpd4000InterruptStreamData extends Struct {
  @Uint16()
  int sequenceNumber;

  @Double()
  double timestamp;

  @Uint16()
  int dataInt;

  @Uint16()
  int level0Int;

  @Uint16()
  int level1Int;

  @Uint16()
  int tiaCh1Int;

  @Uint16()
  int tiaCh2Int;

  factory Adpd4000InterruptStreamData.allocate(
          int _sequenceNumber,
          double timestamp,
          int _dataInt,
          int _level0Int,
          int _level1Int,
          int _tiaCh1Int,
          int _tiaCh2Int) =>
      allocate<Adpd4000InterruptStreamData>().ref
        ..sequenceNumber = _sequenceNumber
        ..timestamp = timestamp
        ..dataInt = _dataInt
        ..level0Int = _level0Int
        ..level1Int = _level1Int
        ..tiaCh1Int = _tiaCh1Int
        ..tiaCh2Int = _tiaCh2Int;
}

class PPGStreamData extends Struct {
  @Uint16()
  int seqnum;

  @Double()
  double timestamp;

  @Uint16()
  int adpdlibstate;

  @Float()
  double hr;

  @Float()
  double confidence;

  @Uint16()
  int hrType;

  @Uint16()
  int rrInterval;

  factory PPGStreamData.allocate(int seqnum, double timestamp, int adpdlibstate,
          double hr, double confidence, int _hrType, int _rrInterval) =>
      allocate<PPGStreamData>().ref
        ..seqnum = seqnum
        ..timestamp = timestamp
        ..adpdlibstate = adpdlibstate
        ..hr = hr
        ..confidence = confidence
        ..hrType = _hrType
        ..rrInterval = _rrInterval;
}

class SyncPPGStreamData extends Struct {
  @Uint16()
  int seqnum;

  @Double()
  double timestamp;

  @Uint32()
  int ppg;

  @Int16()
  int x;

  @Int16()
  int y;

  @Int16()
  int z;

  factory SyncPPGStreamData.allocate(
          int seqnum, double timestamp, int ppg, int x, int y, int z) =>
      allocate<SyncPPGStreamData>().ref
        ..seqnum = seqnum
        ..timestamp = timestamp
        ..ppg = ppg
        ..x = x
        ..y = y
        ..z = z;
}

class AdpdStreamDataSum extends Struct {
  @Uint16()
  int seqnum;

  @Double()
  double timestamp;

  @Uint32()
  int adpdData;

  factory AdpdStreamDataSum.allocate(
          int seqnum, double timestamp, int _adpdData) =>
      allocate<AdpdStreamDataSum>().ref
        ..seqnum = seqnum
        ..timestamp = timestamp
        ..adpdData = _adpdData;
}

class EdaStreamData extends Struct {
  @Uint16()
  int seqnum;

  @Double()
  double timestamp;

  @Double()
  double admittanceReal;

  @Double()
  double admittanceImg;

  @Double()
  double impedanceReal;

  @Double()
  double impedanceImg;

  @Double()
  double admittanceMagnitude;

  @Double()
  double admittancePhase;

  @Double()
  double impedanceMagnitude;

  @Double()
  double impedancePhase;

  factory EdaStreamData.allocate(
          int seqnum,
          double timestamp,
          double _admittanceReal,
          double _admittanceImg,
          double _impedanceReal,
          double _impedanceImg,
          double _admittanceMagnitude,
          double _admittancePhase,
          double _impedanceMagnitude,
          double _impedancePhase) =>
      allocate<EdaStreamData>().ref
        ..seqnum = seqnum
        ..timestamp = timestamp
        ..admittanceReal = _admittanceReal
        ..admittanceImg = _admittanceImg
        ..impedanceReal = _impedanceReal
        ..impedanceImg = _impedanceImg
        ..admittanceMagnitude = _admittanceMagnitude
        ..admittancePhase = _admittancePhase
        ..impedanceMagnitude = _impedanceMagnitude
        ..impedancePhase = _impedancePhase;
}

class EcgStreamData extends Struct {
  @Uint16()
  int seqnum;

  @Double()
  double timestamp;

  @Uint8()
  int datatype;

  @Uint8()
  int leadsoff;

  @Uint8()
  int hr;

  @Uint16()
  int ecgData;

  factory EcgStreamData.allocate(int seqnum, double timestamp, int datatype,
          int leadsoff, int _hr, int _ecgData) =>
      allocate<EcgStreamData>().ref
        ..seqnum = seqnum
        ..timestamp = timestamp
        ..datatype = datatype
        ..leadsoff = leadsoff
        ..hr = _hr
        ..ecgData = _ecgData;
}

class TemperatureStreamData extends Struct {
  @Double()
  double timestamp;

  @Float()
  double tempSkin;

  @Float()
  double tempAmbient;

  factory TemperatureStreamData.allocate(
          double timestamp, double _tempSkin, double _tempAmbient) =>
      allocate<TemperatureStreamData>().ref
        ..timestamp = timestamp
        ..tempSkin = _tempSkin
        ..tempAmbient = _tempAmbient;
}

class CommonAppVersion extends Struct {
  @Uint16()
  int major;

  @Uint16()
  int minor;

  @Uint16()
  int patch;

  // @size(20)
  Pointer<Utf8> verstr;

  // @InlineArray(80)
  Pointer<Utf8> vendstr;

  factory CommonAppVersion.allocate(int major, int minor, int patch,
          Pointer<Utf8> verstr, Pointer<Utf8> vendstr) =>
      allocate<CommonAppVersion>().ref
        ..major = major
        ..minor = minor
        ..patch = patch
        ..verstr = verstr
        ..vendstr = vendstr;
}

class AppVersion extends Struct {
//  @Uint16()
//  int major;
//
//  @Uint16()
//  int minor;
//
//  @Uint16()
//  int patch;

  Pointer<Utf8> verstr;

  factory AppVersion.allocate(Pointer<Utf8> verstr) =>
      allocate<AppVersion>().ref..verstr = verstr;
}

class AgcCtrl extends Struct {
  Pointer<Int32> agcType; //[4];

  Pointer<Int32> agcCtrlValue; //[4];

  @Int16()
  int length;

  factory AgcCtrl.allocate(
          Pointer<Int32> _agcType, Pointer<Int32> _agcCtrlValue, int _length) =>
      allocate<AgcCtrl>().ref
        ..agcType = _agcType
        ..agcCtrlValue = _agcCtrlValue
        ..length = _length;
}

/*=========================================================================================
	|							CLASSES			   |
	===========================================================================================*/

final DynamicLibrary nativeGen3SDK = Platform.isAndroid
    ? DynamicLibrary.open("libgen4sdk.so")
    : DynamicLibrary.process();

typedef tx_length = Void Function();

typedef tx_callback = Void Function(Pointer<Uint8> pBytes, Int32 nLength);
typedef adxl_callback = Void Function(Pointer<AdxlStream> pAdxlcb);

typedef sdkAlert_callback = Void Function(Int32 nLength);

typedef TX_FUNC = Void Function(Pointer a, Pointer b);
typedef tx_Func = void Function(Pointer a, Pointer b);

typedef EMPTY_func = Void Function();
typedef empty = void Function();

typedef DISPATCH_FUNC = Void Function(Pointer<Uint8> a, Int32 nLength);
typedef dispatch_Func = void Function(Pointer<Uint8> a, int nLength);

typedef EEPROM_INFO_FUNC = Uint8 Function(Pointer<EEPROMInfo> pEEPROMInfo);
typedef eeprom_info_func = int Function(Pointer<EEPROMInfo> pEEPROMInfo);

typedef L1_FS_KeyValuePair = Void Function(Pointer<Utf8> userId);
typedef l1_fs_KeyValuePair = void Function(Pointer<Utf8> userId);

typedef L1_FS_VolumeInfo = Void Function(Pointer<Uint32> totalMemory,
    Pointer<Uint32> usedMemory, Pointer<Uint16> freeMemory);
typedef l1_fs_volumeInfo = void Function(Pointer<Uint32> totalMemory,
    Pointer<Uint32> usedMemory, Pointer<Uint16> freeMemory);

typedef L1_RegRead_Func = Void Function(Uint8 sensor,
    Pointer<Uint16> regAddress, Int32 length, Pointer<Registers> pRegisters);
typedef l1_regread_func = void Function(int sensor, Pointer<Uint16> regAddress,
    int length, Pointer<Registers> pRegisters);

typedef L1_RegWrite_Func = Void Function(
    Uint8 sensor, Pointer<Registers> pRegisters);
typedef l1_regwrite_func = void Function(
    int sensor, Pointer<Registers> pRegisters);

typedef L1_Write_AgcCtrl = Void Function(Pointer<AgcCtrl> pAgcCtrl);
typedef l1_write_agcctrl = void Function(Pointer<AgcCtrl> pAgcCtrls);

typedef L1_Func = Uint8 Function(Uint8 stream);
typedef l1_func = int Function(int stream);

typedef CB_SUB = Void Function(Pointer adxlcb);
typedef cb_sub = void Function(Pointer adxlcb);

typedef L2_START_PPG = Void Function(Pointer ppgcb, Pointer syncppg);
typedef l2_start_ppg = void Function(Pointer ppgcb, Pointer syncppg);

typedef STARTFUNC = Void Function(Uint8 sensor);
typedef startfunc = void Function(int sensor);

typedef APP_VERSION_FUNC = Void Function(
    Uint8 sts, Pointer<CommonAppVersion> pAppInfo);
typedef app_version_func = void Function(
    int sts, Pointer<CommonAppVersion> pAppInfo);

final initializeSDK =
    nativeGen3SDK.lookupFunction<Void Function(Uint8 platform), void Function(int platform)>("initializesdk");

final l1FsLogSessionId =
    nativeGen3SDK.lookupFunction<L1_FS_KeyValuePair, l1_fs_KeyValuePair>(
        "fileSystem_keyvaluepair");

final l1FsVolumeInfo =
    nativeGen3SDK.lookupFunction<L1_FS_VolumeInfo, l1_fs_volumeInfo>(
        "fileSystem_volume_info");

final l1RegRead = nativeGen3SDK
    .lookupFunction<L1_RegRead_Func, l1_regread_func>("read_registers");

final l1RegWrite = nativeGen3SDK
    .lookupFunction<L1_RegWrite_Func, l1_regwrite_func>("write_register");

final l1LcfgRegWrite = nativeGen3SDK
    .lookupFunction<L1_RegWrite_Func, l1_regwrite_func>("write_lcfgreg");

final l1SetPPGLcfg =
    nativeGen3SDK.lookupFunction<L1_Func, l1_func>("ppg_load_lcfg");

final l1LoadADPDcfg =
    nativeGen3SDK.lookupFunction<L1_Func, l1_func>("adpd4xxx_load_cfg");

final l1ClockCalib =
    nativeGen3SDK.lookupFunction<L1_Func, l1_func>("adpd4xxx_calibrate_clock");

final l1AgcCtrl = nativeGen3SDK
    .lookupFunction<L1_Write_AgcCtrl, l1_write_agcctrl>("write_agcCtrl");

final l1StreamSubscribe =
    nativeGen3SDK.lookupFunction<L1_Func, l1_func>("subscribe_stream");

final l1StreamUnSubscribe =
    nativeGen3SDK.lookupFunction<L1_Func, l1_func>("unsubscribe_stream");

final l1SubscribeAdpd =
    nativeGen3SDK.lookupFunction<L1_Func, l1_func>("subscribe_adpd4xxx");
final l1UnsubscribeAdpd =
    nativeGen3SDK.lookupFunction<L1_Func, l1_func>("unsubscribe_adpd4xxx");

final l1StreamStart =
    nativeGen3SDK.lookupFunction<L1_Func, l1_func>("start_stream");

final l1StreamStop =
    nativeGen3SDK.lookupFunction<L1_Func, l1_func>("stop_stream");

final l1FSSubscribe =
    nativeGen3SDK.lookupFunction<L1_Func, l1_func>("fileSystem_subscribe");

final l1FSUnSubscribe =
    nativeGen3SDK.lookupFunction<L1_Func, l1_func>("fileSystem_unsubscribe");

final l1FSLogStart =
    nativeGen3SDK.lookupFunction<EMPTY_func, empty>("fileSystem_logStart");

final l1FSLogStop =
    nativeGen3SDK.lookupFunction<EMPTY_func, empty>("fileSystem_logStop");

final l2startadpdRedLED =
    nativeGen3SDK.lookupFunction<EMPTY_func, empty>("startADPD4000_r");
final l2stopadpdRedLED =
    nativeGen3SDK.lookupFunction<EMPTY_func, empty>("stopADPD4000_r");

final l2startadpdGreenLED =
    nativeGen3SDK.lookupFunction<EMPTY_func, empty>("startADPD4000_g");
final l2stopadpdGreenLED =
    nativeGen3SDK.lookupFunction<EMPTY_func, empty>("stopADPD4000_g");

final l2startadpdBlueLED =
    nativeGen3SDK.lookupFunction<EMPTY_func, empty>("startADPD4000_b");
final l2stopadpdBlueLED =
    nativeGen3SDK.lookupFunction<EMPTY_func, empty>("stopADPD4000_b");

final l2startadpdIRLED =
    nativeGen3SDK.lookupFunction<EMPTY_func, empty>("startADPD4000_ir");
final l2stopadpdIRLED =
    nativeGen3SDK.lookupFunction<EMPTY_func, empty>("stopADPD4000_ir");

final l2startadxl = nativeGen3SDK.lookupFunction<EMPTY_func, empty>("startADXL");
final l2stopadxl = nativeGen3SDK.lookupFunction<EMPTY_func, empty>("stopADXL");

final l2startppg =
    nativeGen3SDK.lookupFunction<EMPTY_func, empty>("startPPG");
final l2stopppg = nativeGen3SDK.lookupFunction<EMPTY_func, empty>("stopPPG");

final l2startecg = nativeGen3SDK.lookupFunction<EMPTY_func, empty>("startECG");
final l2stopecg = nativeGen3SDK.lookupFunction<EMPTY_func, empty>("stopECG");

final l2starteda = nativeGen3SDK.lookupFunction<EMPTY_func, empty>("startEDA");
final l2stopeda = nativeGen3SDK.lookupFunction<EMPTY_func, empty>("stopEDA");

final l2startTemp =
    nativeGen3SDK.lookupFunction<EMPTY_func, empty>("startTEMPARATURE");
final l2stopTemp =
    nativeGen3SDK.lookupFunction<EMPTY_func, empty>("stopTEMPARATURE");

final subscribeADXL =
    nativeGen3SDK.lookupFunction<EMPTY_func, empty>("subscribe_adxl");

final startStream =
    nativeGen3SDK.lookupFunction<STARTFUNC, startfunc>("start_stream");

final stopStream =
    nativeGen3SDK.lookupFunction<STARTFUNC, startfunc>("stop_stream");

final unsubscribeStream =
    nativeGen3SDK.lookupFunction<STARTFUNC, startfunc>("unsubscribe_stream");

final initializeApi = nativeGen3SDK.lookupFunction<
    IntPtr Function(Pointer<Void>),
    int Function(Pointer<Void>)>("InitDartApiDL");

final registerSendPort = nativeGen3SDK.lookupFunction<
    Void Function(Int64 sendPort),
    void Function(int sendPort)>('RegisterSendPort');

final registerADPDPort = nativeGen3SDK.lookupFunction<
    Void Function(Int64 sendPort),
    void Function(int sendPort)>('RegisterADPDPort');

final registerADXLPort = nativeGen3SDK.lookupFunction<
    Void Function(Int64 sendPort),
    void Function(int sendPort)>('RegisterADXLPort');

final registerSyncPPGPort = nativeGen3SDK.lookupFunction<
    Void Function(Int64 sendPort),
    void Function(int sendPort)>('RegisterSyncPPGPort');

final registerPPGPort = nativeGen3SDK.lookupFunction<
    Void Function(Int64 sendPort),
    void Function(int sendPort)>('RegisterPPGPort');

final registerECGPort = nativeGen3SDK.lookupFunction<
    Void Function(Int64 sendPort),
    void Function(int sendPort)>('RegisterECGPort');

final registerEDAPort = nativeGen3SDK.lookupFunction<
    Void Function(Int64 sendPort),
    void Function(int sendPort)>('RegisterEDAPort');

final registerTempPort = nativeGen3SDK.lookupFunction<
    Void Function(Int64 sendPort),
    void Function(int sendPort)>('RegisterTempPort');

final setMaxPktCount = nativeGen3SDK.lookupFunction<Uint8 Function(Uint8 count),
    int Function(int count)>('pm_setMaxPktCombineCnt');

class FlSdkWrapper {
  // Singleton Class
  FlSdkWrapper._privateConstructor();
  static final FlSdkWrapper _instance = FlSdkWrapper._privateConstructor();
  static FlSdkWrapper get instance => _instance;

  static const MethodChannel _channel = const MethodChannel('fl_gen3sdk');

  static Future<String> get platformVersion async {
    final String version = await _channel.invokeMethod('getPlatformVersion');
    return version;
  }

  // static void _tx_length() {
  //   print("Callback occured");
  // }

  void getEEPROM() {
    var eepromInfoCall = nativeGen3SDK
        .lookupFunction<EEPROM_INFO_FUNC, eeprom_info_func>("pm_read_EEPROM");

    Pointer<EEPROMInfo> eepromStruct = allocate();

    eepromInfoCall(eepromStruct);

    print(
        "EEPROM INFO ${eepromStruct.ref.hwId} ${eepromStruct.ref.bomId} ${eepromStruct.ref.batchId} ${eepromStruct.ref.manufactureId}");

    free(eepromStruct);
  }

  void dispatchData(List<int> bytes) {
    if (bytes.length == 0) {
      return;
    }

    Pointer<Uint8> rcvdDataptr = allocate<Uint8>(count: bytes.length);

    for (int i = 0; i < bytes.length; i++) {
      rcvdDataptr[i] = bytes[i];
    }

    var dispatchSDK = nativeGen3SDK
        .lookupFunction<DISPATCH_FUNC, dispatch_Func>("dispatch_SDK");

    dispatchSDK(rcvdDataptr, bytes.length);

    free(rcvdDataptr);
  }
}
