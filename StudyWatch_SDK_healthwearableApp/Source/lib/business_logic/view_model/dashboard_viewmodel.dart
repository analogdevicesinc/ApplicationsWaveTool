import 'package:flutter/foundation.dart';
import 'package:flutter_easyloading/flutter_easyloading.dart';
import 'package:vsm/business_logic/utils/enumerations.dart';
import 'package:vsm/services/chart/chart_service.dart';
import 'package:vsm/services/sdk_lib/sdk_service.dart';
import 'package:vsm/services/service_locator.dart';

class DashboardViewModel extends ChangeNotifier {
  final SdkWrapperService _sdkService = serviceLocator<SdkWrapperService>();
  final ChartService _chartService = serviceLocator<ChartService>();

  bool get isAnySensorON => SdkWrapperService.isAnyStreamRunning;
  set isAnySensorON(bool status) {
    SdkWrapperService.isAnyStreamRunning = status;
  }

//----------------------PPG------------------------------
  String _ppgHR = '--';
  String get ppgHR => _ppgHR;

  bool _isPPGSwitched = false;
  bool get isPPGSwitched => _isPPGSwitched;

//----------------------ECG------------------------------
  String _ecgHR = '--';
  String get ecgHR => _ecgHR;

  bool _isECGSwitched = false;
  bool get isECGSwitched => _isECGSwitched;

//----------------------EDA------------------------------

  bool _isEDASwitched = false;
  bool get isEDASwitched => _isEDASwitched;

//----------------------TEMP------------------------------
  String _tempValue = "--";
  String get tempValue => _tempValue;

  bool _isTempSwitched = false;
  bool get isTempSwitched => _isTempSwitched;

//---------------------ADPD LEDs--------------------------

  bool _isAdpdRedLEDSwitched = false;
  bool get isAdpdRedLEDSwitched => _isAdpdRedLEDSwitched;

  bool _isAdpdGreenLEDSwitched = false;
  bool get isAdpdGreenLEDSwitched => _isAdpdGreenLEDSwitched;

  bool _isAdpdBlueLEDSwitched = false;
  bool get isAdpdBlueLEDSwitched => _isAdpdBlueLEDSwitched;

  bool _isAdpdIRLEDSwitched = false;
  bool get isAdpdIRLEDSwitched => _isAdpdIRLEDSwitched;

  Future<void> initState() {
    bool isAdpdRunning = false;

    if (_sdkService.isPPGRunning) {
      print('PPG running');
      _isPPGSwitched = _sdkService.isPPGRunning;
      notifyListeners();
    } else {
      _sdkService.initStreams(Sensors.PPG);
    }

    if (_sdkService.isECGRunning) {
      _isECGSwitched = _sdkService.isECGRunning;
      notifyListeners();
    } else {
      _sdkService.initStreams(Sensors.ECG);
    }

    if (_sdkService.isEDARunning) {
      _isEDASwitched = _sdkService.isEDARunning;
      notifyListeners();
    } else {
      _sdkService.initStreams(Sensors.EDA);
    }

    if (_sdkService.isTempRunning) {
      _isTempSwitched = _sdkService.isTempRunning;
      notifyListeners();
    } else {
      _sdkService.initStreams(Sensors.TEMP);
    }

    if (_sdkService.isAdpdRedLEDRunning) {
      isAdpdRunning = _isAdpdRedLEDSwitched = _sdkService.isAdpdRedLEDRunning;
      notifyListeners();
    }

    if (_sdkService.isAdpdGreenLEDRunning) {
      isAdpdRunning =
          _isAdpdGreenLEDSwitched = _sdkService.isAdpdGreenLEDRunning;
      notifyListeners();
    }

    if (_sdkService.isAdpdBlueLEDRunning) {
      isAdpdRunning = _isAdpdBlueLEDSwitched = _sdkService.isAdpdBlueLEDRunning;
      notifyListeners();
    }

    if (_sdkService.isAdpdIRLEDRunning) {
      isAdpdRunning = _isAdpdIRLEDSwitched = _sdkService.isAdpdIRLEDRunning;
      notifyListeners();
    }

    if (!isAdpdRunning) {
      _sdkService.initStreams(Sensors.ADPD);
    }

    _sdkService.appOutputCallback = updateAppOutput;

    return null;
  }

  void updateAppOutput(String data, Sensors app) {
    if (app == Sensors.PPG) {
      if (_sdkService.isPPGRunning) {
        _ppgHR = data;
      } else {
        _ppgHR = '--';
      }
    } else if (app == Sensors.ECG) {
      if (_sdkService.isECGRunning) {
        _ecgHR = data;
      } else {
        _ecgHR = '--';
      }
    } else if (app == Sensors.TEMP) {
      if (_sdkService.isTempRunning) {
        _tempValue = data;
      } else {
        _tempValue = '--';
      }
    }
    notifyListeners();
  }

  void onADPDStateChanged(bool isSwitchedOn, AdpdLED _adpdLED) async {
    if (isSwitchedOn) {
      if (!isAnySensorON) {
        isAnySensorON = isSwitchedOn;
      } else {
        EasyLoading.showToast('Cannot start more than one stream',
            duration: Duration(milliseconds: 1500),
            toastPosition: EasyLoadingToastPosition.bottom);
        return;
      }
      await _sdkService.startSendorData(Sensors.ADPD, adpdLED: _adpdLED);
    } else {
      if (isAnySensorON) {
        isAnySensorON = isSwitchedOn;
      }
      await _sdkService.stopSendorData(Sensors.ADPD, adpdLED: _adpdLED);
      _chartService.deinitChartController(Sensors.ADPD);
    }
    if (_adpdLED == AdpdLED.Red) {
      _isAdpdRedLEDSwitched = isSwitchedOn;
    } else if (_adpdLED == AdpdLED.Green) {
      _isAdpdGreenLEDSwitched = isSwitchedOn;
    } else if (_adpdLED == AdpdLED.Blue) {
      _isAdpdBlueLEDSwitched = isSwitchedOn;
    } else {
      _isAdpdIRLEDSwitched = isSwitchedOn;
    }

    notifyListeners();
  }

  void onPPGStateChanged(bool isSwitchedOn) async {
    if (isSwitchedOn) {
      print('PPG ON');
      if (!isAnySensorON) {
        isAnySensorON = isSwitchedOn;
      } else {
        EasyLoading.showToast('Cannot start more than one stream',
            duration: Duration(milliseconds: 1500),
            toastPosition: EasyLoadingToastPosition.bottom);
        return;
      }
      await _sdkService.startSendorData(Sensors.PPG);
    } else {
      print('PPG OFF');
      if (isAnySensorON) {
        isAnySensorON = isSwitchedOn;
      }
      await _sdkService.stopSendorData(Sensors.PPG);
      _chartService.deinitChartController(Sensors.PPG);
    }

    _isPPGSwitched = isSwitchedOn;
    notifyListeners();
  }

  void onECGStateChanged(bool isSwitchedOn) async {
    if (isSwitchedOn) {
      print('ECG ON');
      if (!isAnySensorON) {
        isAnySensorON = isSwitchedOn;
      } else {
        EasyLoading.showToast('Cannot start more than one stream',
            duration: Duration(milliseconds: 1500),
            toastPosition: EasyLoadingToastPosition.bottom);
        return;
      }
      await _sdkService.startSendorData(Sensors.ECG);
    } else {
      print('ECG OFF');
      if (isAnySensorON) {
        isAnySensorON = isSwitchedOn;
      }
      await _sdkService.stopSendorData(Sensors.ECG);
      _chartService.deinitChartController(Sensors.ECG);
    }
    _isECGSwitched = isSwitchedOn;
    notifyListeners();
  }

  void onEDAStateChanged(bool isSwitchedOn) async {
    if (isSwitchedOn) {
      print('EDA ON');
      if (!isAnySensorON) {
        isAnySensorON = isSwitchedOn;
      } else {
        EasyLoading.showToast('Cannot start more than one stream',
            duration: Duration(milliseconds: 1500),
            toastPosition: EasyLoadingToastPosition.bottom);
        return;
      }
      await _sdkService.startSendorData(Sensors.EDA);
    } else {
      print('EDA OFF');
      if (isAnySensorON) {
        isAnySensorON = isSwitchedOn;
      }
      await _sdkService.stopSendorData(Sensors.EDA);
      _chartService.deinitChartController(Sensors.EDA);
    }
    _isEDASwitched = isSwitchedOn;
    notifyListeners();
  }

  void onTempStateChanged(bool isSwitchedOn) async {
    if (isSwitchedOn) {
      print('Temp ON');
      if (!isAnySensorON) {
        isAnySensorON = isSwitchedOn;
      } else {
        EasyLoading.showToast('Cannot start more than one stream',
            duration: Duration(milliseconds: 1500),
            toastPosition: EasyLoadingToastPosition.bottom);
        return;
      }
      await _sdkService.startSendorData(Sensors.TEMP);
    } else {
      print('Temp OFF');
      if (isAnySensorON) {
        isAnySensorON = isSwitchedOn;
      }
      await _sdkService.stopSendorData(Sensors.TEMP);
      _chartService.deinitChartController(Sensors.TEMP);
    }
    _isTempSwitched = isSwitchedOn;
    notifyListeners();
  }

  @override
  void dispose() {
    print('delink PPGHR callback');

    if (!_sdkService.isAdpdRedLEDRunning &&
        !_sdkService.isAdpdGreenLEDRunning &&
        !_sdkService.isAdpdBlueLEDRunning &&
        !_sdkService.isAdpdIRLEDRunning) {
      _sdkService.deinitStreams(Sensors.ADPD);
    }

    if (!_sdkService.isPPGRunning) {
      _sdkService.deinitStreams(Sensors.PPG);
    }

    if (!_sdkService.isECGRunning) {
      _sdkService.deinitStreams(Sensors.ECG);
    }

    if (!_sdkService.isEDARunning) {
      _sdkService.deinitStreams(Sensors.EDA);
    }

    if (!_sdkService.isTempRunning) {
      _sdkService.deinitStreams(Sensors.TEMP);
    }
    _sdkService.appOutputCallback = null;
    super.dispose();
  }
}
