import 'dart:isolate';

import 'package:flutter/foundation.dart';
import 'package:vsm/business_logic/utils/enumerations.dart';

class ADPDdataModel {
  double timeStamp;
  double ch1Data;
  double ch2Data;
  ADPDdataModel({
    @required this.timeStamp,
    @required this.ch1Data,
    @required this.ch2Data,
  });
}

class ADXLdataModel {
  double timeStamp;
  double xValue;
  double yValue;
  double zValue;
  ADXLdataModel(
      {@required this.timeStamp,
      @required this.xValue,
      @required this.yValue,
      @required this.zValue});
}

class SyncPPGdataModel {
  double timeStamp;
  double ppgData;
  double xValue;
  double yValue;
  double zValue;
  SyncPPGdataModel(
      {@required this.timeStamp,
      @required this.ppgData,
      @required this.xValue,
      @required this.yValue,
      @required this.zValue});
}

class ECGdataModel {
  double timeStamp;
  double ecgData;
  double leadData;
  double bpm;
  ECGdataModel({
    @required this.timeStamp,
    @required this.ecgData,
    @required this.leadData,
    @required this.bpm,
  });
}

class EDAdataModel {
  double timeStamp;
  double admittancePhase;
  double admittanceMagnitude;
  double impedancePhase;
  double impedanceMagnitude;
  EDAdataModel({
    @required this.timeStamp,
    @required this.admittancePhase,
    @required this.admittanceMagnitude,
    @required this.impedancePhase,
    @required this.impedanceMagnitude,
  });
}

class TempdataModel {
  double timeStamp;
  double tempSkin;
  double tempAmbient;
  TempdataModel({
    @required this.timeStamp,
    @required this.tempSkin,
    @required this.tempAmbient,
  });
}

class DataModel {
  double timeStamp;
  double value;
  DataModel({
    @required this.timeStamp,
    @required this.value,
  });
}

class FsVolInfo {
  int totalMemory;
  int usedMemory;
  int freeMemory;
  FsVolInfo({this.totalMemory, this.usedMemory, this.freeMemory});
}

class LogInfo {
  String sessionId;
  List<ADDRS_ENUM_STREAM> loggingAppList;
  LogInfo({this.sessionId, this.loggingAppList});
}

class Params {
  Object message;
  SendPort sendPort;

  Params(this.message, this.sendPort);
}
