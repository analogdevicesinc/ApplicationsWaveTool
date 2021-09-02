import 'dart:io';

import 'package:flutter/foundation.dart';
import 'package:vsm/business_logic/model/settings_model.dart';
import 'package:vsm/business_logic/utils/enumerations.dart';
import 'package:vsm/services/sdk_lib/sdk_service.dart';
import 'package:vsm/services/service_locator.dart';
import 'package:enum_to_string/enum_to_string.dart';

class SettingsViewModel extends ChangeNotifier {
  final SdkWrapperService _sdkService = serviceLocator<SdkWrapperService>();
  final SettingsModel _settingsModel = serviceLocator<SettingsModel>();

  String get loadDcfgFileName => _settingsModel.selectedLoadDcfg;
  set loadDcfgFileName(String selectedDcfg) {
    _settingsModel.selectedLoadDcfg = selectedDcfg;
  }

  File get loadDcfgFile => _settingsModel.loadDcfgFile;

  set loadDcfgFile(File dcfgFile) {
    _settingsModel.loadDcfgFile = dcfgFile;
    _settingsModel.parseDcfgFile();
  }

  String get adpdSampleRate =>
      _settingsModel.adpdConfig.sampleRate.toStringAsFixed(0);

  set adpdSampleRate(String newValue) {
    _settingsModel.adpdConfig.sampleRate = int.tryParse(newValue);
    print('Sample rate changed to ${_settingsModel.adpdConfig.sampleRate}');
  }

  String get adpdFilter =>
      EnumToString.convertToString(_settingsModel.adpdConfig.filterType);

  set adpdFilter(String newValue) {
    _settingsModel.adpdConfig.filterType =
        EnumToString.fromString(Filters.values, newValue);
    print('Filter type changed to ${_settingsModel.adpdConfig.filterType}');
  }

  int get adpdTapCount => _settingsModel.adpdConfig.tapCount;

  set adpdTapCount(int newValue) {
    _settingsModel.adpdConfig.tapCount = newValue;
    print('Tap Count changed to ${_settingsModel.adpdConfig.tapCount}');
  }

  int get adpdDuration => _settingsModel.adpdConfig.duration;

  set adpdDuration(int newValue) {
    _settingsModel.adpdConfig.duration = newValue;
    print('Duration changed to ${_settingsModel.adpdConfig.duration}');
  }

  String get adxlSampleRate =>
      _settingsModel.ppgConfig.sampleRate.toStringAsFixed(0);

  set adxlSampleRate(String newValue) {
    _settingsModel.ppgConfig.sampleRate = int.tryParse(newValue);
    print('Sample rate changed to ${_settingsModel.ppgConfig.sampleRate}');
  }

  int get ppgDuration => _settingsModel.ppgConfig.duration;

  set ppgDuration(int newValue) {
    _settingsModel.ppgConfig.duration = newValue;
    print('Duration changed to ${_settingsModel.ppgConfig.duration}');
  }

  String get ecgSampleRate =>
      _settingsModel.ecgConfig.sampleRate.toStringAsFixed(0);

  set ecgSampleRate(String newValue) {
    _settingsModel.ecgConfig.sampleRate = int.tryParse(newValue);
    print('Sample rate changed to ${_settingsModel.ecgConfig.sampleRate}');
  }

  String get ecgFilter =>
      EnumToString.convertToString(_settingsModel.ecgConfig.filterType);

  set ecgFilter(String newValue) {
    _settingsModel.ecgConfig.filterType =
        EnumToString.fromString(Filters.values, newValue);
    print('Filter type changed to ${_settingsModel.ecgConfig.filterType}');
  }

  int get ecgTapCount => _settingsModel.ecgConfig.tapCount;

  set ecgTapCount(int newValue) {
    _settingsModel.ecgConfig.tapCount = newValue;
    print('Tap Count changed to ${_settingsModel.ecgConfig.tapCount}');
  }

  int get ecgDuration => _settingsModel.ecgConfig.duration;

  set ecgDuration(int newValue) {
    _settingsModel.ecgConfig.duration = newValue;
    print('Duration changed to ${_settingsModel.ecgConfig.duration}');
  }

  int get edaDuration => _settingsModel.edaConfig.duration;

  set edaDuration(int newValue) {
    _settingsModel.edaConfig.duration = newValue;
    print('Duration changed to ${_settingsModel.edaConfig.duration}');
  }

  void resetLoadSettings() {
    _settingsModel.resetLoadConfig();
    notifyListeners();
  }
}
