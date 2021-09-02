import 'package:flutter/foundation.dart';
import 'package:vsm/business_logic/model/graph_data_model.dart';
import 'package:vsm/business_logic/utils/enumerations.dart';
import 'package:vsm/services/sdk_lib/sdk_service.dart';
import 'package:vsm/services/service_locator.dart';

class LoggingViewModel extends ChangeNotifier {
  final SdkWrapperService _sdkService = serviceLocator<SdkWrapperService>();

  bool _isLogSwitched = false;
  bool get isLogSwitched => _isLogSwitched;

  bool _isADPDSwitched = false;
  bool get isADPDSwitched => _isADPDSwitched;

  bool _isADXLSwitched = false;
  bool get isADXLSwitched => _isADXLSwitched;

  bool _isPPGSwitched = false;
  bool get isPPGSwitched => _isPPGSwitched;

  bool _isECGSwitched = false;
  bool get isECGSwitched => _isECGSwitched;

  bool _isEDASwitched = false;
  bool get isEDASwitched => _isEDASwitched;

  bool _isTempSwitched = false;
  bool get isTempSwitched => _isTempSwitched;

  Future<void> initState() {
    if (_sdkService.isLoggingON) {
      _isLogSwitched = true;
      for (var item in _sdkService.logInfo.loggingAppList) {
        if (item == ADDRS_ENUM_STREAM.ADPD6_STREAM) {
          _isADPDSwitched = true;
        } else if (item == ADDRS_ENUM_STREAM.ADXL_Stream) {
          _isADXLSwitched = true;
        } else if (item == ADDRS_ENUM_STREAM.PPG_Stream) {
          _isPPGSwitched = true;
        } else if (item == ADDRS_ENUM_STREAM.ECG_Stream) {
          _isECGSwitched = true;
        } else if (item == ADDRS_ENUM_STREAM.EDA_Stream) {
          _isEDASwitched = true;
        } else if (item == ADDRS_ENUM_STREAM.TEMPERATURE_Stream) {
          _isTempSwitched = true;
        }
      }
      notifyListeners();
    }

    return null;
  }

  Future<FsVolInfo> getVolInfo() {
    return _sdkService.getFlashMemory();
  }

  void onLoggingStateChanged(bool isSwitchedOn, String logSessionId) async {
    _sdkService.controlLogging(isSwitchedOn, logSessionId);
    _isLogSwitched = isSwitchedOn;
    if (!isSwitchedOn) {
      _isADPDSwitched = _isADXLSwitched = _isPPGSwitched =
          _isECGSwitched = _isEDASwitched = _isTempSwitched = false;
    }
    notifyListeners();
  }

  void onADPDStateChanged(bool isSwitchedOn) async {
    if (isSwitchedOn) {
      _sdkService.logInfo.loggingAppList.add(ADDRS_ENUM_STREAM.ADPD6_STREAM);
    } else {
      _sdkService.logInfo.loggingAppList.remove(ADDRS_ENUM_STREAM.ADPD6_STREAM);
    }
    _isADPDSwitched = isSwitchedOn;
    notifyListeners();
  }

  void onADXLStateChanged(bool isSwitchedOn) async {
    if (isSwitchedOn) {
      _sdkService.logInfo.loggingAppList.add(ADDRS_ENUM_STREAM.ADXL_Stream);
    } else {
      _sdkService.logInfo.loggingAppList.remove(ADDRS_ENUM_STREAM.ADXL_Stream);
    }
    _isADXLSwitched = isSwitchedOn;
    notifyListeners();
  }

  void onPPGStateChanged(bool isSwitchedOn) async {
    if (isSwitchedOn) {
      _sdkService.logInfo.loggingAppList.add(ADDRS_ENUM_STREAM.PPG_Stream);
      _sdkService.logInfo.loggingAppList.add(ADDRS_ENUM_STREAM.SYNCPPG_Stream);
    } else {
      _sdkService.logInfo.loggingAppList.remove(ADDRS_ENUM_STREAM.PPG_Stream);
      _sdkService.logInfo.loggingAppList
          .remove(ADDRS_ENUM_STREAM.SYNCPPG_Stream);
    }
    _isPPGSwitched = isSwitchedOn;
    notifyListeners();
  }

  void onECGStateChanged(bool isSwitchedOn) async {
    if (isSwitchedOn) {
      _sdkService.logInfo.loggingAppList.add(ADDRS_ENUM_STREAM.ECG_Stream);
    } else {
      _sdkService.logInfo.loggingAppList.remove(ADDRS_ENUM_STREAM.ECG_Stream);
    }
    _isECGSwitched = isSwitchedOn;
    notifyListeners();
  }

  void onEDAStateChanged(bool isSwitchedOn) async {
    if (isSwitchedOn) {
      _sdkService.logInfo.loggingAppList.add(ADDRS_ENUM_STREAM.EDA_Stream);
    } else {
      _sdkService.logInfo.loggingAppList.remove(ADDRS_ENUM_STREAM.EDA_Stream);
    }
    _isEDASwitched = isSwitchedOn;
    notifyListeners();
  }

  void onTempStateChanged(bool isSwitchedOn) async {
    if (isSwitchedOn) {
      _sdkService.logInfo.loggingAppList
          .add(ADDRS_ENUM_STREAM.TEMPERATURE_Stream);
    } else {
      _sdkService.logInfo.loggingAppList
          .remove(ADDRS_ENUM_STREAM.TEMPERATURE_Stream);
    }
    _isTempSwitched = isSwitchedOn;
    notifyListeners();
  }

  @override
  void dispose() {
    super.dispose();
  }
}
