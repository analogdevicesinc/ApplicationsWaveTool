import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:mp_chart/mp/controller/line_chart_controller.dart';
import 'package:vsm/business_logic/model/graph_data_model.dart';
import 'package:vsm/business_logic/utils/enumerations.dart';
import 'package:vsm/services/chart/chart_service.dart';
import 'package:vsm/services/chart/mp_chart_manager.dart';
import 'package:vsm/services/sdk_lib/sdk_service.dart';
import 'package:vsm/services/service_locator.dart';

class ChartViewModel extends ChangeNotifier {
  final SdkWrapperService _sdkService = serviceLocator<SdkWrapperService>();
  final ChartService _chartService = serviceLocator<ChartService>();
  LineChartController get chartControllerADPD =>
      _chartService.getChartController(Sensors.ADPD);
  LineChartController get chartControllerADXL =>
      _chartService.getChartController(Sensors.ADXL);
  LineChartController get chartControllerSyncPPG =>
      _chartService.getChartController(Sensors.PPG);
  LineChartController get chartControllerECG =>
      _chartService.getChartController(Sensors.ECG);
  LineChartController get chartControllerEDA =>
      _chartService.getChartController(Sensors.EDA);
  LineChartController get chartControllerTemp =>
      _chartService.getChartController(Sensors.TEMP);

  bool _isCh1Checked = true;
  bool get isCh1Checked => _isCh1Checked;

  void notifyCallback() {
    notifyListeners();
  }

  void channelCheckedChanged(bool status) {
    _isCh1Checked = MPChartManager.isCh1Checked = status;
    notifyListeners();
    print("Ch1 checked changed : $status");
  }

  Future<void> initState() {
    _sdkService.adpdDataCallback = adpdCallback;
    _sdkService.syncppgDataCallback = syncppgCallback;
    _sdkService.ecgDataCallback = ecgCallback;
    _sdkService.edaDataCallback = edaCallback;
    _sdkService.edaDataCallback = edaCallback;
    _sdkService.tempDataCallback = tempCallback;
    _sdkService.notifyCallback = notifyCallback;
    return null;
  }

  void adpdCallback(ADPDdataModel dataModel) {
    _chartService.updateChartValues(Sensors.ADPD, dataModel);
  }

  void syncppgCallback(SyncPPGdataModel dataModel) {
    _chartService.updateChartValues(Sensors.PPG, dataModel);
  }

  void ecgCallback(ECGdataModel dataModel) {
    _chartService.updateChartValues(Sensors.ECG, dataModel);
  }

  void edaCallback(EDAdataModel dataModel) {
    _chartService.updateChartValues(Sensors.EDA, dataModel);
  }

  void tempCallback(TempdataModel dataModel) {
    _chartService.updateChartValues(Sensors.TEMP, dataModel);
  }

  int getChartIndex() {
    if (_sdkService.isPPGRunning) {
      return 0;
    } else if (_sdkService.isECGRunning) {
      return 1;
    } else if (_sdkService.isEDARunning) {
      return 2;
    } else if (_sdkService.isTempRunning) {
      return 3;
    } else if (_sdkService.isAdpdRedLEDRunning ||
        _sdkService.isAdpdGreenLEDRunning ||
        _sdkService.isAdpdBlueLEDRunning ||
        _sdkService.isAdpdIRLEDRunning) {
      return 4;
    } else {
      return 0;
    }
  }

  @override
  void dispose() {
    print('delink adpd callback');
    _sdkService.adpdDataCallback = null;
    _sdkService.syncppgDataCallback = null;
    _sdkService.ecgDataCallback = null;
    _sdkService.edaDataCallback = null;
    _sdkService.tempDataCallback = null;
    super.dispose();
  }
}
