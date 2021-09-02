import 'dart:async';
import 'dart:io';
import 'dart:typed_data';
import 'dart:ffi';
import 'package:ffi/ffi.dart';
import 'package:flutter_easyloading/flutter_easyloading.dart';
import 'package:vsm/business_logic/model/graph_data_model.dart';
import 'package:vsm/business_logic/model/settings_model.dart';
import 'package:vsm/business_logic/utils/enumerations.dart';
import 'package:vsm/services/network/communication_service.dart';
import 'package:vsm/services/service_locator.dart';
import 'fl_sdk_wrapper.dart';
import 'dart:isolate';
import 'dart:collection';

class SdkWrapperService {
  FlSdkWrapper sdkInstance = FlSdkWrapper.instance;
  static CommunicationService bledatainstance =
      serviceLocator<CommunicationService>();
  final SettingsModel _settingsModel = serviceLocator<SettingsModel>();

  static bool isInitDone = false;
  LogInfo logInfo = LogInfo(loggingAppList: [], sessionId: 'default');

  Queue<DataModel> ch1Queue = Queue<DataModel>();
  Queue<DataModel> ch2Queue = Queue<DataModel>();

  static StreamController<int> _ppgHRStreamController;
  StreamSubscription<int> ppgHRStreamSubscription;
  Timer timerPPGHR;

  static StreamController<SyncPPGdataModel> _syncppgDataStreamController;
  StreamSubscription<SyncPPGdataModel> syncppgStreamSubscription;
  Timer timerSyncPPG;

  static StreamController<ECGdataModel> _ecgStreamController;
  StreamSubscription<ECGdataModel> ecgStreamSubscription;
  Timer timerECG;

  static StreamController<EDAdataModel> _edaDataStreamController;
  StreamSubscription<EDAdataModel> edaDataStreamSubscription;
  Timer timerEDA;

  static StreamController<TempdataModel> _tempDataStreamController;
  StreamSubscription<TempdataModel> tempDataStreamSubscription;
  Timer timerTemp;

  static StreamController<ADPDdataModel> _adpdStreamController;
  StreamSubscription<ADPDdataModel> adpdStreamSubscription;
  Timer timerADPD;

  static String logSessionId = 'TestSession';
  var statusTimer;

  void _initRxLister() {
    bledatainstance.dispatchStream.listen((List<int> value) {
      sdkInstance.dispatchData(value);
    });
    print('Initialize dispatch stream Listner');
  }

  Future<void> startTimer(int timeout, String message) async {
    if (statusTimer == null) {
      statusTimer = Timer(Duration(seconds: timeout), () {
        EasyLoading.dismiss();
        EasyLoading.showInfo(message + " Failed",
            duration: Duration(seconds: 2));
        statusTimer?.cancel();
        statusTimer = null;
        //Kill the isolate
      });
    }
  }

  Future<void> cancelTimer() async {
    statusTimer?.cancel();
    statusTimer = null;
  }

  Future<void> initSDK() async {
    ReceivePort receivePort = ReceivePort();

    if (initializeApi(NativeApi.initializeApiDLData) != 0) {
      throw "Failed to initialize Dart API";
    }

    commandPortListner();
    adpdPortListener();
    adxlPortListner();
    syncPPGPortListener();
    ppgPortListener();
    ecgPortListener();
    edaPortListener();
    tempPortListener();

    startTimer(7, "Initial Commands");
    Isolate.spawn(callSDK, receivePort.sendPort);

    receivePort.listen((data) {
      cancelTimer();
      EasyLoading.dismiss();
    });
  }

  void commandPortListner() {
    final interactiveCppRequests = ReceivePort()
      ..listen((data) {
        List<String> receivedStrData = data.split(":");
        receivedStrData.removeLast();

        List<int> cmdPacket =
            receivedStrData.map((data) => int.parse(data)).toList();
        print("Receive Port : $cmdPacket");
        bledatainstance.writeData(cmdPacket);
      });
    final int nativePort = interactiveCppRequests.sendPort.nativePort;

    registerSendPort(nativePort);
  }

  void adxlPortListner() {
    final interactiveadxlCppRequests = ReceivePort()
      ..listen((data) {
        print("ADXL DATA $data");
      });
    final int adxlnativePort = interactiveadxlCppRequests.sendPort.nativePort;

    registerADXLPort(adxlnativePort);
  }

  void adpdPortListener() {
    final interactiveAdpdCppRequests = ReceivePort()
      ..listen((data) {
        List<String> receivedStrData = data.split(",");

        double _timestamp = double.parse(receivedStrData[0]);
        double _channelNum = double.parse(receivedStrData[1]);
        double _dataLength = double.parse(receivedStrData[2]);

        for (int i = 3; i < _dataLength + 3; i++) {
          if (_channelNum == 1) {
            ch1Queue.addLast(DataModel(
              timeStamp: _timestamp,
              value: double.parse(receivedStrData[i]),
            ));
          } else {
            ch2Queue.addLast(DataModel(
              timeStamp: _timestamp,
              value: double.parse(receivedStrData[i]),
            ));
          }
        }

        while (ch1Queue.length > 0 && ch2Queue.length > 0) {
          var ch1Data = ch1Queue.removeFirst();
          var ch2Data = ch2Queue.removeFirst();
          if (_adpdStreamController?.hasListener == true) {
            _adpdStreamController?.add(new ADPDdataModel(
              timeStamp: ch1Data.timeStamp,
              ch1Data: ch1Data.value,
              ch2Data: ch2Data.value,
            ));
          }
        }
      });

    final int adpdNativePort = interactiveAdpdCppRequests.sendPort.nativePort;
    registerADPDPort(adpdNativePort);
  }

  void syncPPGPortListener() {
    final interactiveSyncPPGCppRequests = ReceivePort()
      ..listen((data) {
        List<String> receivedStrData = data.split(",");

        double _timestamp = double.parse(receivedStrData[0]);
        double _ppgvalue = double.parse(receivedStrData[1]);
        double _xValue = double.parse(receivedStrData[2]);
        double _yValue = double.parse(receivedStrData[3]);
        double _zValue = double.parse(receivedStrData[4]);

        if (_syncppgDataStreamController?.hasListener == true) {
          _syncppgDataStreamController?.add(new SyncPPGdataModel(
            timeStamp: _timestamp,
            ppgData: _ppgvalue,
            xValue: _xValue,
            yValue: _yValue,
            zValue: _zValue,
          ));
        }
      });

    final int syncPPGNativePort =
        interactiveSyncPPGCppRequests.sendPort.nativePort;

    registerSyncPPGPort(syncPPGNativePort);
  }

  void ppgPortListener() {
    final interactivePPGCppRequests = ReceivePort()
      ..listen((data) {
        List<String> receivedStrData = data.split(",");
        double ppgHR = double.parse(receivedStrData[1]);

        if (_ppgHRStreamController?.hasListener == true) {
          _ppgHRStreamController?.add(ppgHR.toInt());
        }
      });
    final int ppgnativePort = interactivePPGCppRequests.sendPort.nativePort;

    registerPPGPort(ppgnativePort);
  }

  void ecgPortListener() {
    final interactiveECGCppRequests = ReceivePort()
      ..listen((data) {
        List<String> receivedStrData = data.split(",");

        double _timestamp = double.parse(receivedStrData[0]);
        double _ecgvalue = double.parse(receivedStrData[1]);
        double _bpm = double.parse(receivedStrData[2]);

        if (_ecgStreamController?.hasListener == true) {
          _ecgStreamController?.add(new ECGdataModel(
            timeStamp: _timestamp,
            ecgData: _ecgvalue,
            leadData: 0.0,
            bpm: _bpm,
          ));
        }
      });
    final int ecgNativePort = interactiveECGCppRequests.sendPort.nativePort;

    registerECGPort(ecgNativePort);
  }

  void edaPortListener() {
    final interactiveEDACppRequests = ReceivePort()
      ..listen((data) {
        List<String> receivedStrData = data.split(",");

        double _timestamp = double.parse(receivedStrData[0]);
        double _impPhase = double.parse(receivedStrData[1]);
        double _impMagnitide = double.parse(receivedStrData[2]);
        double _admPhase = double.parse(receivedStrData[3]);
        double _admMagnitide = double.parse(receivedStrData[4]);

        if (_edaDataStreamController?.hasListener == true) {
          _edaDataStreamController?.add(EDAdataModel(
            timeStamp: _timestamp,
            impedancePhase: _impPhase,
            impedanceMagnitude: _impMagnitide,
            admittancePhase: _admPhase,
            admittanceMagnitude: _admMagnitide,
          ));
        }
      });

    final int edaNativePort = interactiveEDACppRequests.sendPort.nativePort;
    registerEDAPort(edaNativePort);
  }

  void tempPortListener() {
    final interactiveTempCppRequests = ReceivePort()
      ..listen((data) {
        List<String> receivedStrData = data.split(",");

        double _timestamp = double.parse(receivedStrData[0]);
        double _tempSkin = double.parse(receivedStrData[1]);
        double _tempAmbient = double.parse(receivedStrData[2]);

        if (_tempDataStreamController?.hasListener == true) {
          _tempDataStreamController?.add(new TempdataModel(
            timeStamp: _timestamp,
            tempSkin: _tempSkin / 10,
            tempAmbient: _tempAmbient / 10,
          ));
        }
      });

    final int tempNativePort = interactiveTempCppRequests.sendPort.nativePort;
    registerTempPort(tempNativePort);
  }

  static void callSDK(SendPort sendPort) {
    final int platform = Platform.isAndroid
        ? SDK_PLATFORM.android.index
        : SDK_PLATFORM.ios.index;
    initializeSDK(platform);
    setMaxPktCount(1);
    l1LoadADPDcfg(SENSOR_ADPD4000_DEVICE_ID.ADPD4000_DEVICE_4000_G.index + 40);
    doClockCalibration([0x08]); //chipIdRegAddress = 0x08
    l1SetPPGLcfg(ppgLcfgId[PPG_LCFG_ID.ADPD4000]);

    // ECG
    List<int> regNames = [0];
    List<int> regValues = [100];
    lcfgWrite(regNames, regValues, SENSORS_SDK.ECG.index);

    //EDA
    regNames = [0];
    regValues = [4];
    lcfgWrite(regNames, regValues, SENSORS_SDK.EDA.index);

    sendPort.send('completed');
  }

  static void lcfgWrite(
      List<int> adpdRegNames, List<int> adpdRegValues, int index) {
    Pointer<Int32> regAddrsDataptr =
        allocate<Int32>(count: adpdRegNames.length);
    Pointer<Int32> regValueDataptr =
        allocate<Int32>(count: adpdRegValues.length);

    for (int i = 0; i < adpdRegNames.length; i++) {
      regAddrsDataptr[i] = adpdRegNames[i];
      regValueDataptr[i] = adpdRegValues[i];
    }
    Pointer<Registers> registersStruct = allocate();
    registersStruct.ref.address = regAddrsDataptr;
    registersStruct.ref.value = regValueDataptr;
    registersStruct.ref.length = adpdRegNames.length;

    l1LcfgRegWrite(index, registersStruct);
    free(registersStruct);
  }

  Future initializeGen3SDK() async {
    print('Initialize SDK');
    _initRxLister();
    print('calling future delay for 2 seconds');
    await Future.delayed(Duration(seconds: 2));
    print('2 second delay completed');
    initSDK();
  }

  void getEEPROM() {
    sdkInstance.getEEPROM();
  }

  void initStreams(Sensors sensor) {
    switch (sensor) {
      case Sensors.ADPD:
        initADPDStream();
        break;
      case Sensors.PPG:
        initPPGHRStream();
        initSyncPPGStream();
        break;
      case Sensors.ECG:
        initECGStream();
        break;
      case Sensors.EDA:
        initEDAStream();
        break;
      case Sensors.TEMP:
        initTempStream();
        break;
      default:
        print('initStream failed - invalid sensor type received');
    }
  }

  void initPPGHRStream() {
    _ppgHRStreamController = StreamController<int>();
    ppgHRStreamSubscription = _ppgHRStreamController?.stream?.listen((event) {
      appOutputCallback?.call(event.toString(), Sensors.PPG);
    }, onError: (error) {
      print(error);
    }, onDone: () async {
      appOutputCallback?.call('--', Sensors.PPG);
      print('OnDone called');
    });
  }

  void initSyncPPGStream() {
    _syncppgDataStreamController = StreamController<SyncPPGdataModel>();
    syncppgStreamSubscription =
        _syncppgDataStreamController?.stream?.listen((event) {
      syncppgDataCallback?.call(event);
    }, onError: (error) {
      print(error);
    }, onDone: () async {
      syncppgDataCallback?.call(null);
      print('OnDone called');
    });
  }

  void initECGStream() {
    _ecgStreamController = StreamController<ECGdataModel>();
    ecgStreamSubscription = _ecgStreamController?.stream?.listen((event) {
      ecgDataCallback?.call(event);
      appOutputCallback?.call(event.bpm.toStringAsFixed(0), Sensors.ECG);
      print('ECG HR : ${event.bpm}');
    }, onError: (error) {
      print(error);
    }, onDone: () async {
      ecgDataCallback?.call(null);
      appOutputCallback?.call('--', Sensors.ECG);
      print('OnDone called');
    });
  }

  void initEDAStream() {
    _edaDataStreamController = StreamController<EDAdataModel>();
    edaDataStreamSubscription =
        _edaDataStreamController?.stream?.listen((event) {
      edaDataCallback?.call(event);
    }, onError: (error) {
      print(error);
    }, onDone: () async {
      edaDataCallback?.call(null);
      print('OnDone called');
    });
  }

  void initTempStream() {
    _tempDataStreamController = StreamController<TempdataModel>();
    tempDataStreamSubscription =
        _tempDataStreamController?.stream?.listen((event) {
      appOutputCallback?.call(event.tempSkin.toStringAsFixed(1), Sensors.TEMP);
      tempDataCallback?.call(event);
    }, onError: (error) {
      print(error);
    }, onDone: () async {
      appOutputCallback?.call('--', Sensors.TEMP);
      tempDataCallback?.call(null);
      print('OnDone called');
    });
  }

  void initADPDStream() {
    _adpdStreamController = StreamController<ADPDdataModel>();
    adpdStreamSubscription = _adpdStreamController?.stream?.listen((event) {
      adpdDataCallback?.call(event);
    }, onError: (error) {
      print(error);
    }, onDone: () async {
      adpdDataCallback?.call(null);
      print('OnDone adpd called');
    });
  }

  void deinitStreams(Sensors sensor) {
    switch (sensor) {
      case Sensors.ADPD:
        deinitADPDStream();
        break;
      case Sensors.PPG:
        deinitPPGStream();
        break;
      case Sensors.ECG:
        deinitECGStream();
        break;
      case Sensors.EDA:
        deinitEDAStream();
        break;
      case Sensors.TEMP:
        deinitTempStream();
        break;
      default:
        print('deinit stream failed - invalid sensor type received');
    }
  }

  void deinitPPGStream() {
    ppgHRStreamSubscription?.cancel();
    ppgHRStreamSubscription = null;
    _ppgHRStreamController?.close();
    _ppgHRStreamController = null;
    //-----------------
    syncppgStreamSubscription?.cancel();
    syncppgStreamSubscription = null;
    _syncppgDataStreamController?.close();
    _syncppgDataStreamController = null;
  }

  void deinitECGStream() {
    ecgStreamSubscription?.cancel();
    ecgStreamSubscription = null;
    _ecgStreamController?.close();
    _ecgStreamController = null;
  }

  void deinitADPDStream() {
    adpdStreamSubscription?.cancel();
    adpdStreamSubscription = null;
    _adpdStreamController?.close();
    _adpdStreamController = null;

    ch1Queue.clear();
    ch2Queue.clear();
  }

  void deinitEDAStream() {
    edaDataStreamSubscription?.cancel();
    edaDataStreamSubscription = null;
    _edaDataStreamController?.close();
    _edaDataStreamController = null;
  }

  void deinitTempStream() {
    tempDataStreamSubscription?.cancel();
    tempDataStreamSubscription = null;
    _tempDataStreamController?.close();
    _tempDataStreamController = null;
  }

  static void doClockCalibration(List<int> adpdRegNames) {
    Pointer<Uint16> regAddrsDataptr =
        allocate<Uint16>(count: adpdRegNames.length);

    for (int i = 0; i < adpdRegNames.length; i++) {
      regAddrsDataptr[i] = adpdRegNames[i];
    }

    Pointer<Registers> regPtr = allocate();
    regPtr.ref.address = allocate<Int32>(count: adpdRegNames.length);
    regPtr.ref.value = allocate<Int32>(count: adpdRegNames.length);

    l1RegRead(SENSORS_SDK.ADPD4000.index, regAddrsDataptr, adpdRegNames.length,
        regPtr);

    var chipId = (regPtr.ref.value[0] & 0x00FF);

    print("ChipId : $chipId");
    if (chipId == 0x00C0) {
      l1ClockCalib(6);
    } else {
      l1ClockCalib(2);
    }
  }

  static void fsLogStart(Params param) {
    var _logInfo = param.message as LogInfo;
    fsSetLogSessionId(_logInfo.sessionId);

    for (var sensor in _logInfo.loggingAppList) {
      if (sensor == ADDRS_ENUM_STREAM.ADPD6_STREAM) {
        //enum index starts from 40 in sdk enum.cannot assign value to dart enum
        l1LoadADPDcfg(
            SENSOR_ADPD4000_DEVICE_ID.ADPD4000_DEVICE_4000_G.index + 40);

        doClockCalibration([0x08]); //chipIdRegAddress = 0x08

        print('AGC Ctrl Write 1:1');
        List<int> agcTypes = [1];
        List<int> agcCtrlValues = [1];
        Pointer<Int32> agcTypeptr = allocate<Int32>(count: agcTypes.length);
        Pointer<Int32> agcCtrlValueptr =
            allocate<Int32>(count: agcCtrlValues.length);

        for (int i = 0; i < agcTypes.length; i++) {
          agcTypeptr[i] = agcTypes[i];
          agcCtrlValueptr[i] = agcCtrlValues[i];
        }

        Pointer<AgcCtrl> agcCtrlStruct = allocate();
        agcCtrlStruct.ref.agcType = agcTypeptr;
        agcCtrlStruct.ref.agcCtrlValue = agcCtrlValueptr;
        agcCtrlStruct.ref.length = agcTypes.length;

        l1AgcCtrl(agcCtrlStruct);
        free(agcCtrlStruct);

        l1FSSubscribe(sensor.index);
        l1StreamStart(SENSORS_SDK.ADPD4000.index);
      } else if (sensor == ADDRS_ENUM_STREAM.ADXL_Stream) {
        l1FSSubscribe(sensor.index);
        l1StreamStart(SENSORS_SDK.ADXL.index);
      } else if (sensor == ADDRS_ENUM_STREAM.PPG_Stream) {
        //enum index starts from 40 in sdk enum.cannot assign value to dart enum
        l1LoadADPDcfg(
            SENSOR_ADPD4000_DEVICE_ID.ADPD4000_DEVICE_4000_G.index + 40);
        doClockCalibration([0x08]); //chipIdRegAddress = 0x08
        l1SetPPGLcfg(ppgLcfgId[PPG_LCFG_ID.ADPD4000]);
        l1FSSubscribe(sensor.index);
        l1StreamStart(SENSORS_SDK.PPG.index);
      } else if (sensor == ADDRS_ENUM_STREAM.ECG_Stream) {
        List<int> adpdRegNames = [0];
        List<int> adpdRegValues = [100];
        Pointer<Int32> regAddrsDataptr =
            allocate<Int32>(count: adpdRegNames.length);
        Pointer<Int32> regValueDataptr =
            allocate<Int32>(count: adpdRegValues.length);

        for (int i = 0; i < adpdRegNames.length; i++) {
          regAddrsDataptr[i] = adpdRegNames[i];
          regValueDataptr[i] = adpdRegValues[i];
        }
        Pointer<Registers> registersStruct = allocate();
        registersStruct.ref.address = regAddrsDataptr;
        registersStruct.ref.value = regValueDataptr;
        registersStruct.ref.length = adpdRegNames.length;
        l1LcfgRegWrite(SENSORS_SDK.ECG.index, registersStruct);
        free(registersStruct);
        l1FSSubscribe(sensor.index);
        l1StreamStart(SENSORS_SDK.ECG.index);
      } else if (sensor == ADDRS_ENUM_STREAM.EDA_Stream) {
        l1FSSubscribe(sensor.index);
        l1StreamStart(SENSORS_SDK.EDA.index);
      } else if (sensor == ADDRS_ENUM_STREAM.TEMPERATURE_Stream) {
        l1LoadADPDcfg(
            SENSOR_ADPD4000_DEVICE_ID.ADPD4000_DEVICE_4000_G.index + 40);
        l1FSSubscribe(sensor.index);
        l1StreamStart(SENSORS_SDK.TEMPERATURE.index);
      }
    }

    print('FS Log Start');
    l1FSLogStart();

    param.sendPort.send('Log Start Completed');
  }

  static void fsLogStop(Params param) {
    var _logInfo = param.message as LogInfo;
    for (var sensor in _logInfo.loggingAppList) {
      if (sensor == ADDRS_ENUM_STREAM.ADPD6_STREAM) {
        l1StreamStop(SENSORS_SDK.ADPD4000.index);
        l1FSUnSubscribe(sensor.index);
      } else if (sensor == ADDRS_ENUM_STREAM.ADXL_Stream) {
        l1StreamStop(SENSORS_SDK.ADXL.index);
        l1FSUnSubscribe(sensor.index);
      } else if (sensor == ADDRS_ENUM_STREAM.PPG_Stream) {
        l1StreamStop(SENSORS_SDK.PPG.index);
        l1FSUnSubscribe(sensor.index);
      } else if (sensor == ADDRS_ENUM_STREAM.ECG_Stream) {
        l1StreamStop(SENSORS_SDK.ECG.index);
        l1FSUnSubscribe(sensor.index);
      } else if (sensor == ADDRS_ENUM_STREAM.EDA_Stream) {
        l1StreamStop(SENSORS_SDK.EDA.index);
        l1FSUnSubscribe(sensor.index);
      } else if (sensor == ADDRS_ENUM_STREAM.TEMPERATURE_Stream) {
        l1StreamStop(SENSORS_SDK.TEMPERATURE.index);
        l1FSUnSubscribe(sensor.index);
      }
    }
    print('FS Log Stop');
    l1FSLogStop();

    param.sendPort.send('Log Stop Completed');
  }

  static void fsSetLogSessionId(String logSessionId) {
    Pointer<Utf8> logId = Utf8.toUtf8(logSessionId);
    l1FsLogSessionId(logId);
  }

  static void fsGetVolInfo(SendPort port) {
    Pointer<Uint32> totalMemory = allocate();
    Pointer<Uint32> usedMemory = allocate();
    Pointer<Uint16> freeMemory = allocate();
    l1FsVolumeInfo(totalMemory, usedMemory, freeMemory);
    port.send(FsVolInfo(
        totalMemory: totalMemory.value,
        usedMemory: usedMemory.value,
        freeMemory: freeMemory.value));
  }

  Future<FsVolInfo> getFlashMemory() async {
    var receivePort = new ReceivePort();
    Isolate.spawn(fsGetVolInfo, receivePort.sendPort);
    await for (var retMsg in receivePort) {
      return retMsg as FsVolInfo;
    }
    return null;
  }

  Future<bool> controlLogging(bool isLogON, String participantId) async {
    ReceivePort receivePort;
    if (isLogON) {
      receivePort = ReceivePort();
      logInfo.sessionId = participantId;
      EasyLoading.show(
        status: 'Starting Log...',
        maskType: EasyLoadingMaskType.black,
      );
      startTimer(7, "Start Log");
      Isolate.spawn(fsLogStart, Params(logInfo, receivePort.sendPort));
      receivePort.listen((data) {
        cancelTimer();
        EasyLoading.dismiss();
      });
    } else {
      receivePort = ReceivePort();
      EasyLoading.show(
        status: 'Stopping Log...',
        maskType: EasyLoadingMaskType.black,
      );
      startTimer(5, "Stop Log");
      Isolate.spawn(fsLogStop, Params(logInfo, receivePort.sendPort));
      receivePort.listen((data) {
        cancelTimer();
        EasyLoading.dismiss();
      });
      logInfo.loggingAppList.clear();
    }
    _isLoggingON = isLogON;
    return true;
  }

//----------------------------------
  Future<bool> startSendorData(Sensors sensor,
      {AdpdLED adpdLED = AdpdLED.Red}) async {
    switch (sensor) {
      case Sensors.ADPD:
        startADPD(adpdLED);
        break;
      case Sensors.PPG:
        startPPG();
        startSyncPPG();
        break;
      case Sensors.ECG:
        startECG();
        break;
      case Sensors.EDA:
        startEDA();
        break;
      case Sensors.TEMP:
        startTemp();
        break;
      default:
        print('Start Sensor Data failed - Invalid Sensor type received');
    }
    return true;
  }

  static void quickStartAdpdGreenLED(Params param) {
    var lstRegModel = param.message as List<RegisterModel>;
    setMaxPktCount(4);
    l1LoadADPDcfg(SENSOR_ADPD4000_DEVICE_ID.ADPD4000_DEVICE_4000_G.index + 40);

    if (lstRegModel != null) {
      loadDcfgRegisters(lstRegModel);
    }

    writeADPDFreq(adpd100Hz);
    writeAGCRegister([1], [1]);
    l1StreamStart(SENSORS_SDK.ADPD4000.index);
    l1SubscribeAdpd(ADPD4XXX_SOURCE.STREAM6.index + 1);
    param.sendPort.send('Start ADPD Completed');
  }

  static void quickStartAdpdRedLED(Params param) {
    var lstRegModel = param.message as List<RegisterModel>;
    setMaxPktCount(4);
    l1LoadADPDcfg(SENSOR_ADPD4000_DEVICE_ID.ADPD4000_DEVICE_4000_R.index + 40);

    if (lstRegModel != null) {
      loadDcfgRegisters(lstRegModel);
    }

    writeADPDFreq(adpd100Hz);
    writeAGCRegister([2], [1]);
    l1StreamStart(SENSORS_SDK.ADPD4000.index);
    l1SubscribeAdpd(ADPD4XXX_SOURCE.STREAM7.index + 1);
    param.sendPort.send('Start ADPD Completed');
  }

  static void quickStartAdpdIRLED(Params param) {
    var lstRegModel = param.message as List<RegisterModel>;
    setMaxPktCount(4);
    l1LoadADPDcfg(SENSOR_ADPD4000_DEVICE_ID.ADPD4000_DEVICE_4000_IR.index + 40);

    if (lstRegModel != null) {
      loadDcfgRegisters(lstRegModel);
    }

    writeADPDFreq(adpd100Hz);
    writeAGCRegister([3], [1]);
    l1StreamStart(SENSORS_SDK.ADPD4000.index);
    l1SubscribeAdpd(ADPD4XXX_SOURCE.STREAM8.index + 1);
    param.sendPort.send('Start ADPD Completed');
  }

  static void quickStartAdpdBlueLED(Params param) {
    var lstRegModel = param.message as List<RegisterModel>;
    setMaxPktCount(4);
    l1LoadADPDcfg(SENSOR_ADPD4000_DEVICE_ID.ADPD4000_DEVICE_4000_B.index + 40);

    if (lstRegModel != null) {
      loadDcfgRegisters(lstRegModel);
    }

    writeADPDFreq(adpd100Hz);
    writeAGCRegister([4], [1]);
    l1StreamStart(SENSORS_SDK.ADPD4000.index);
    l1SubscribeAdpd(ADPD4XXX_SOURCE.STREAM9.index + 1);
    param.sendPort.send('Start ADPD Completed');
  }

  static void writeAGCRegister(List<int> agcTypes, List<int> agcCtrlValues) {
    Pointer<Int32> agcTypeptr = allocate<Int32>(count: agcTypes.length);
    Pointer<Int32> agcCtrlValueptr =
        allocate<Int32>(count: agcCtrlValues.length);

    for (int i = 0; i < agcTypes.length; i++) {
      agcTypeptr[i] = agcTypes[i];
      agcCtrlValueptr[i] = agcCtrlValues[i];
    }

    Pointer<AgcCtrl> agcCtrlStruct = allocate();
    agcCtrlStruct.ref.agcType = agcTypeptr;
    agcCtrlStruct.ref.agcCtrlValue = agcCtrlValueptr;
    agcCtrlStruct.ref.length = agcTypes.length;

    l1AgcCtrl(agcCtrlStruct);
    free(agcCtrlStruct);
  }

  Future<bool> startADPD(AdpdLED adpdLED) async {
    ReceivePort receivePort;

    if (adpdLED == AdpdLED.Red) {
      if (!_isAdpdRedLEDRunning) {
        receivePort = ReceivePort();
        EasyLoading.show(
          status: 'Starting SpO2...',
          maskType: EasyLoadingMaskType.black,
        );
        startTimer(4, "Red Led Start");

        Isolate.spawn(quickStartAdpdRedLED,
            Params(_settingsModel.lstRegModel, receivePort.sendPort));

        receivePort.listen((data) {
          cancelTimer();
          EasyLoading.dismiss();
        });
        _isAdpdRedLEDRunning = true;
      }
    } else if (adpdLED == AdpdLED.Green) {
      if (!_isAdpdGreenLEDRunning) {
        receivePort = ReceivePort();
        EasyLoading.show(
          status: 'Starting SpO2...',
          maskType: EasyLoadingMaskType.black,
        );
        startTimer(4, "Green Led Start");

        Isolate.spawn(quickStartAdpdGreenLED,
            Params(_settingsModel.lstRegModel, receivePort.sendPort));

        receivePort.listen((data) {
          cancelTimer();
          EasyLoading.dismiss();
        });
        _isAdpdGreenLEDRunning = true;
      }
    } else if (adpdLED == AdpdLED.Blue) {
      if (!_isAdpdBlueLEDRunning) {
        receivePort = ReceivePort();
        EasyLoading.show(
          status: 'Starting SpO2...',
          maskType: EasyLoadingMaskType.black,
        );
        startTimer(4, "Blue Led Start");

        Isolate.spawn(quickStartAdpdBlueLED,
            Params(_settingsModel.lstRegModel, receivePort.sendPort));

        receivePort.listen((data) {
          cancelTimer();
          EasyLoading.dismiss();
        });

        _isAdpdBlueLEDRunning = true;
      }
    } else if (adpdLED == AdpdLED.IR) {
      if (!_isAdpdIRLEDRunning) {
        receivePort = ReceivePort();
        EasyLoading.show(
          status: 'Starting SpO2...',
          maskType: EasyLoadingMaskType.black,
        );
        startTimer(4, "IR Start");

        Isolate.spawn(quickStartAdpdIRLED,
            Params(_settingsModel.lstRegModel, receivePort.sendPort));

        receivePort.listen((data) {
          cancelTimer();
          EasyLoading.dismiss();
        });
        _isAdpdIRLEDRunning = true;
      }
    }

    return true;
  }

  static void quickStartPPG(Params param) {
    var lstRegmodel = param.message as List<RegisterModel>;
    setMaxPktCount(2);
    l1LoadADPDcfg(SENSOR_ADPD4000_DEVICE_ID.ADPD4000_DEVICE_4000_G.index + 40);

    if (lstRegmodel != null) {
      loadDcfgRegisters(lstRegmodel);
    }

    l1StreamStart(SENSORS_SDK.PPG.index);
    l1StreamSubscribe(SENSORS_SDK.PPG.index);
    param.sendPort.send('PPG_Start Completed');
  }

  Future<bool> startPPG() async {
    if (!_isPPGRunning) {
      ReceivePort receivePort = ReceivePort();
      print('start PPG timer');
      EasyLoading.show(
        status: 'Starting PPG...',
        maskType: EasyLoadingMaskType.black,
      );
      startTimer(4, "PPG Start");

      Isolate.spawn(quickStartPPG,
          Params(_settingsModel.lstRegModel, receivePort.sendPort));

      receivePort.listen((data) {
        cancelTimer();
        EasyLoading.dismiss();
      });

      _isPPGRunning = true;
    }
    return true;
  }

  Future<bool> startSyncPPG() async {
    if (!_isSyncPPGRunning) {
      _isSyncPPGRunning = true;
    }
    return true;
  }

  static void quickStartECG(Params param) {
    setMaxPktCount(2);
    l1StreamStart(SENSORS_SDK.ECG.index);
    l1StreamSubscribe(SENSORS_SDK.ECG.index);
    param.sendPort.send('ECG_Start Completed');
  }

  Future<bool> startECG() async {
    if (!_isECGRunning) {
      ReceivePort receivePort = ReceivePort();
      EasyLoading.show(
        status: 'Starting ECG...',
        maskType: EasyLoadingMaskType.black,
      );
      startTimer(4, "ECG Start");

      Isolate.spawn(quickStartECG, Params('ECG Start', receivePort.sendPort));

      receivePort.listen((data) {
        cancelTimer();
        EasyLoading.dismiss();
      });

      _isECGRunning = true;
    }
    return true;
  }

  static void quickStartEDA(Params param) {
    setMaxPktCount(2);
    l1StreamStart(SENSORS_SDK.EDA.index);
    l1StreamSubscribe(SENSORS_SDK.EDA.index);
    param.sendPort.send('EDA_Start Completed');
  }

  Future<bool> startEDA() async {
    if (!_isEDARunning) {
      ReceivePort receivePort = ReceivePort();
      EasyLoading.show(
        status: 'Starting EDA...',
        maskType: EasyLoadingMaskType.black,
      );
      startTimer(9, "EDA Start");

      Isolate.spawn(quickStartEDA, Params('EDA Start', receivePort.sendPort));

      receivePort.listen((data) {
        cancelTimer();
        EasyLoading.dismiss();
      });

      _isEDARunning = true;
    }
    return true;
  }

  static void quickStartTemp(Params param) {
    setMaxPktCount(4);
    l1StreamStart(SENSORS_SDK.TEMPERATURE.index);
    l1StreamSubscribe(SENSORS_SDK.TEMPERATURE.index);
    param.sendPort.send('Temp_Start Completed');
  }

  Future<bool> startTemp() async {
    if (!_isTempRunning) {
      ReceivePort receivePort = ReceivePort();
      EasyLoading.show(
        status: 'Starting Temp...',
        maskType: EasyLoadingMaskType.black,
      );
      startTimer(4, "Temperature Start");

      Isolate.spawn(quickStartTemp, Params('Temp Start', receivePort.sendPort));

      receivePort.listen((data) {
        cancelTimer();
        EasyLoading.dismiss();
      });

      _isTempRunning = true;
    }
    return true;
  }

  Future<bool> stopSendorData(Sensors sensor,
      {AdpdLED adpdLED = AdpdLED.Red}) async {
    switch (sensor) {
      case Sensors.ADPD:
        await stopADPDsensor(adpdLED);
        break;
      case Sensors.PPG:
        await stopPPGSensor();
        await stopSyncPPGSensor();
        break;
      case Sensors.ECG:
        await stopECGSensor();
        break;
      case Sensors.EDA:
        await stopEDASensor();
        break;
      case Sensors.TEMP:
        await stopTempSensor();
        break;
      default:
        print('stopSensorData failed - received invalid sensor type');
    }
    return true;
  }

  static void quickStopAdpdRedLED(Params param) {
    l2stopadpdRedLED();
    param.sendPort.send('Stop ADPD Red Completed');
  }

  static void quickStopAdpdGreenLED(Params param) {
    l2stopadpdGreenLED();
    param.sendPort.send('Stop ADPD Red Completed');
  }

  static void quickStopAdpdBlueLED(Params param) {
    l2stopadpdBlueLED();
    param.sendPort.send('Stop ADPD Red Completed');
  }

  static void quickStopAdpdIRLED(Params param) {
    l2stopadpdIRLED();
    param.sendPort.send('Stop ADPD Red Completed');
  }

  static void loadDcfgRegisters(List<RegisterModel> lstRegModel) {
    const int chuckCount = 20;
    var currLen = 0;
    var remLen = lstRegModel.length;
    var lstMarker = 0;

    try {
      while (remLen > 0) {
        currLen = remLen < chuckCount ? remLen : chuckCount;
        Pointer<Registers> reg = allocate();
        reg.ref.address = allocate<Int32>(count: currLen);
        reg.ref.value = allocate<Int32>(count: currLen);
        try {
          for (int index = 0; index < currLen; index++) {
            reg.ref.address[index] = lstRegModel[lstMarker].regAddress;
            reg.ref.value[index] = lstRegModel[lstMarker].regValue;
            var strRegAddr =
                lstRegModel[lstMarker].regAddress.toRadixString(16);
            var strRegValue = lstRegModel[lstMarker].regValue.toRadixString(16);
            print('${strRegAddr.toUpperCase()} ${strRegValue.toUpperCase()}');
            lstMarker += 1;
          }
        } catch (e) {
          print('Exception in load DCFG for loop $e');
        }

        reg.ref.length = currLen;
        l1RegWrite(SENSORS_SDK.ADPD4000.index, reg);

        free(reg.ref.address);
        free(reg.ref.value);
        free(reg);

        remLen -= currLen;
      }
    } catch (e) {
      print('Exception in Load DCFG $e');
    }
  }

  static void writeADPDFreq(int freqVal) {
    Pointer<Registers> reg = allocate();
    reg.ref.address = allocate<Int32>(count: 1);
    reg.ref.value = allocate<Int32>(count: 1);
    reg.ref.address[0] = 0x0D;
    reg.ref.value[0] = freqVal;
    reg.ref.length = 1;

    l1RegWrite(SENSORS_SDK.ADPD4000.index, reg);

    free(reg.ref.address);
    free(reg.ref.value);
    free(reg);
  }

  //StreamSubscription listner;
  Future<bool> stopADPDsensor(AdpdLED adpdLED) async {
    ReceivePort receivePort = ReceivePort();

    receivePort.listen((data) {
      cancelTimer();
      EasyLoading.dismiss();
    });

    if (adpdLED == AdpdLED.Red) {
      if (_isAdpdRedLEDRunning) {
        EasyLoading.show(
          status: 'Stopping ADPD...',
          maskType: EasyLoadingMaskType.black,
        );
        startTimer(4, "Red Led Stop");

        Isolate.spawn(
            quickStopAdpdRedLED, Params('ADPD R Stop', receivePort.sendPort));
        _isAdpdRedLEDRunning = false;
      }
    } else if (adpdLED == AdpdLED.Green) {
      if (_isAdpdGreenLEDRunning) {
        EasyLoading.show(
          status: 'Stopping ADPD...',
          maskType: EasyLoadingMaskType.black,
        );
        startTimer(4, "Green Led Stop");

        Isolate.spawn(
            quickStopAdpdGreenLED, Params('ADPD G Stop', receivePort.sendPort));
        _isAdpdGreenLEDRunning = false;
      }
    } else if (adpdLED == AdpdLED.Blue) {
      if (_isAdpdBlueLEDRunning) {
        EasyLoading.show(
          status: 'Stopping ADPD...',
          maskType: EasyLoadingMaskType.black,
        );
        startTimer(4, "Blue Led Stop");

        Isolate.spawn(
            quickStopAdpdBlueLED, Params('ADPD B Stop', receivePort.sendPort));
        _isAdpdBlueLEDRunning = false;
      }
    } else if (adpdLED == AdpdLED.IR) {
      if (_isAdpdIRLEDRunning) {
        EasyLoading.show(
          status: 'Stopping ADPD...',
          maskType: EasyLoadingMaskType.black,
        );
        startTimer(4, "IR Stop");

        Isolate.spawn(
            quickStopAdpdIRLED, Params('ADPD IR Stop', receivePort.sendPort));
        _isAdpdIRLEDRunning = false;
      }
    }

    return true;
  }

  static void quickStopPPG(Params param) {
    l2stopppg();
    param.sendPort.send('PPG Stop Completed');
  }

  Future<bool> stopPPGSensor() async {
    if (_isPPGRunning) {
      ReceivePort receivePort = ReceivePort();
      EasyLoading.show(
        status: 'Stopping PPG...',
        maskType: EasyLoadingMaskType.black,
      );
      startTimer(4, "PPG Stop");

      Isolate.spawn(quickStopPPG, Params('PPG Start', receivePort.sendPort));

      receivePort.listen((data) {
        cancelTimer();
        EasyLoading.dismiss();
      });

      appOutputCallback?.call('--', Sensors.PPG);

      _isPPGRunning = false;
    }
    return true;
  }

  Future<bool> stopSyncPPGSensor() async {
    if (_isSyncPPGRunning) {
      print('stop Syncppg timer');
      _isSyncPPGRunning = false;
    }
    return true;
  }

  static void quickStopECG(Params param) {
    l2stopecg();
    param.sendPort.send('ECG Stop Completed');
  }

  Future<bool> stopECGSensor() async {
    if (_isECGRunning) {
      ReceivePort receivePort = ReceivePort();
      EasyLoading.show(
        status: 'Stopping ECG...',
        maskType: EasyLoadingMaskType.black,
      );
      startTimer(4, "ECG Stop");

      Isolate.spawn(quickStopECG, Params('ECG Stop', receivePort.sendPort));

      receivePort.listen((data) {
        cancelTimer();
        EasyLoading.dismiss();
      });

      _isECGRunning = false;
    }
    return true;
  }

  static void quickStopEDA(Params param) {
    l2stopeda();
    param.sendPort.send('EDA Stop Completed');
  }

  Future<bool> stopEDASensor() async {
    if (_isEDARunning) {
      ReceivePort receivePort = ReceivePort();
      EasyLoading.show(
        status: 'Stopping EDA...',
        maskType: EasyLoadingMaskType.black,
      );
      startTimer(6, "EDA Stop");

      Isolate.spawn(quickStopEDA, Params('EDA Stop', receivePort.sendPort));

      receivePort.listen((data) {
        cancelTimer();
        EasyLoading.dismiss();
      });

      _isEDARunning = false;
    }
    return true;
  }

  static void quickStopTemp(Params param) {
    l2stopTemp();
    param.sendPort.send('Temp Stop Completed');
  }

  Future<bool> stopTempSensor() async {
    if (_isTempRunning) {
      ReceivePort receivePort = ReceivePort();
      EasyLoading.show(
        status: 'Stopping Temp...',
        maskType: EasyLoadingMaskType.black,
      );
      startTimer(4, "Temperature Stop");

      Isolate.spawn(quickStopTemp, Params('Temp Stop', receivePort.sendPort));

      receivePort.listen((data) {
        cancelTimer();
        EasyLoading.dismiss();
      });

      _isTempRunning = false;
    }
    return true;
  }

  void Function(String p1, Sensors p2) appOutputCallback;
  void Function(ADPDdataModel p1) adpdDataCallback;
  void Function(ADXLdataModel p1) adxlDataCallback;
  void Function(ECGdataModel p1) ecgDataCallback;
  void Function(SyncPPGdataModel p1) syncppgDataCallback;
  void Function(EDAdataModel p1) edaDataCallback;
  void Function(TempdataModel p1) tempDataCallback;

  static bool _isLoggingON = false;
  bool get isLoggingON => _isLoggingON;

  static bool isAnyStreamRunning = false;

  static bool _isPPGRunning = false;
  bool get isPPGRunning => _isPPGRunning;

  static bool _isSyncPPGRunning = false;
  bool get isSyncPPGRunning => _isSyncPPGRunning;

  static bool _isECGRunning = false;
  bool get isECGRunning => _isECGRunning;

  static bool _isEDARunning = false;
  bool get isEDARunning => _isEDARunning;

  static bool _isTempRunning = false;
  bool get isTempRunning => _isTempRunning;

  static bool _isAdpdRedLEDRunning = false;
  bool get isAdpdRedLEDRunning => _isAdpdRedLEDRunning;

  static bool _isAdpdGreenLEDRunning = false;
  bool get isAdpdGreenLEDRunning => _isAdpdGreenLEDRunning;

  static bool _isAdpdBlueLEDRunning = false;
  bool get isAdpdBlueLEDRunning => _isAdpdBlueLEDRunning;

  static bool _isAdpdIRLEDRunning = false;
  bool get isAdpdIRLEDRunning => _isAdpdIRLEDRunning;

  Function notifyCallback;
}
