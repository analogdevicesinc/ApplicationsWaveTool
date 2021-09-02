import 'dart:io';

import 'package:flutter/foundation.dart';
import 'package:vsm/business_logic/utils/enumerations.dart';

const int adpdFreq = 100; //Hz
const int ppgFreq = 50; //Hz
const int ecgFreq = 100; //Hz
const int edaFreq = 4;
const int tempFreq = 1;
const int tapCount = 8;
const int duration = 3; //sec

const int adpd100Hz = 0x2710;
const int ppg50Hz = 0x4E20;

class RegisterModel {
  int regAddress;
  int regValue;
  RegisterModel({@required this.regAddress, @required this.regValue});
}

class SettingsModel {
  String selectedLoadDcfg;
  File loadDcfgFile;
  List<RegisterModel> lstRegModel;
  ADPDSettingsModel adpdConfig;
  PPGSettingsModel ppgConfig;
  ECGSettingsModel ecgConfig;
  EDASettingsModel edaConfig;
  TempSettingsModel tempConfig;
  double adpdScaleFactor;
  double ppgScaleFactor;
  double ecgScaleFactor;
  double edaScaleFactor;
  double tempScaleFactor;

  SettingsModel() {
    lstRegModel = new List<RegisterModel>();
    selectedLoadDcfg = "No File Selected";
    adpdConfig = ADPDSettingsModel(
      sampleRate: adpdFreq,
      filterType: Filters.FIR,
      tapCount: tapCount,
      duration: duration,
    );
    updateScaleFactor(Sensors.ADPD, adpdFreq);

    ppgConfig = PPGSettingsModel(
      sampleRate: ppgFreq,
      duration: duration,
    );
    updateScaleFactor(Sensors.PPG, ppgFreq);

    ecgConfig = ECGSettingsModel(
      sampleRate: ecgFreq,
      filterType: Filters.ButterWorth,
      tapCount: tapCount,
      duration: duration,
    );
    updateScaleFactor(Sensors.ECG, ecgFreq);

    edaConfig = EDASettingsModel(duration: duration);
    updateScaleFactor(Sensors.EDA, edaFreq);

    tempConfig = TempSettingsModel(duration: 30);
    updateScaleFactor(Sensors.TEMP, tempFreq);
  }

  void updateScaleFactor(Sensors sensor, int sampleRate) {
    switch (sensor) {
      case Sensors.ADPD:
        adpdConfig.sampleRate = sampleRate;
        adpdScaleFactor = (1 / adpdConfig.sampleRate);
        break;
      case Sensors.PPG:
        ppgConfig.sampleRate = sampleRate;
        ppgScaleFactor = (1 / ppgConfig.sampleRate);
        break;
      case Sensors.ECG:
        ecgConfig.sampleRate = sampleRate;
        ecgScaleFactor = (1 / ecgConfig.sampleRate);
        break;
      case Sensors.EDA:
        edaConfig.sampleRate = sampleRate;
        edaScaleFactor = (1 / edaConfig.sampleRate);
        break;
      case Sensors.TEMP:
        tempConfig.sampleRate = sampleRate;
        tempScaleFactor = (1 / tempConfig.sampleRate);
        break;
      default:
    }
  }

  Future<bool> parseDcfgFile() async {
    try {
      if (loadDcfgFile != null) {
        lstRegModel.clear();
        await loadDcfgFile.readAsLines().then((lines) => lines.forEach((l) {
              if (!l.startsWith('#')) {
                var lstWords = l.split(' ');
                if (lstWords.length >= 2) {
                  lstRegModel.add(
                    RegisterModel(
                      regAddress: int.parse('0x${lstWords[0]}'),
                      regValue: int.parse('0x${lstWords[1]}'),
                    ),
                  );
                }
              }
            }));
        return true;
      }
    } catch (e) {
      print(e);
    }
    return false;
  }

  void resetLoadConfig() {
    selectedLoadDcfg = "No File Selected";
    loadDcfgFile = null;
    lstRegModel.clear();
  }

  void resetSettingToDefault() {
    adpdConfig.duration = duration;
    ppgConfig.duration = duration;
    ecgConfig.duration = duration;
    edaConfig.duration = duration;
  }
}

class ADPDSettingsModel {
  int sampleRate;
  Filters filterType;
  int tapCount;
  int duration;
  ADPDSettingsModel({
    @required this.sampleRate,
    @required this.filterType,
    @required this.tapCount,
    @required this.duration,
  });
}

class PPGSettingsModel {
  int sampleRate;
  int duration;
  PPGSettingsModel({
    @required this.sampleRate,
    @required this.duration,
  });
}

class ECGSettingsModel {
  int sampleRate;
  Filters filterType;
  int tapCount;
  int duration;
  ECGSettingsModel({
    @required this.sampleRate,
    @required this.filterType,
    @required this.tapCount,
    @required this.duration,
  });
}

class EDASettingsModel {
  int duration;
  int sampleRate;
  EDASettingsModel({
    this.sampleRate,
    @required this.duration,
  });
}

class TempSettingsModel {
  int duration;
  int sampleRate;
  TempSettingsModel({
    this.sampleRate,
    this.duration,
  });
}
