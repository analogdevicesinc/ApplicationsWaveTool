import 'package:flutter/material.dart';
import 'package:mp_chart/mp/core/data/line_data.dart';
import 'package:mp_chart/mp/core/data_interfaces/i_line_data_set.dart';
import 'package:mp_chart/mp/core/data_set/line_data_set.dart';
import 'package:mp_chart/mp/core/description.dart';
import 'package:mp_chart/mp/core/entry/entry.dart';
import 'package:mp_chart/mp/core/enums/axis_dependency.dart';
import 'package:mp_chart/mp/core/enums/legend_form.dart';
import 'package:mp_chart/mp/core/enums/legend_horizontal_alignment.dart';
import 'package:mp_chart/mp/core/enums/legend_orientation.dart';
import 'package:mp_chart/mp/core/enums/legend_vertical_alignment.dart';
import 'package:mp_chart/mp/core/enums/mode.dart';
import 'package:mp_chart/mp/core/enums/x_axis_position.dart';
import 'package:vsm/business_logic/model/graph_data_model.dart';
import 'package:vsm/business_logic/model/settings_model.dart';
import 'package:vsm/business_logic/utils/enumerations.dart';
import 'package:mp_chart/mp/controller/line_chart_controller.dart';
import 'package:vsm/services/chart/chart_service.dart';
import 'package:vsm/services/service_locator.dart';

class MPChartManager implements ChartService {
  final SettingsModel _settingsModel = serviceLocator<SettingsModel>();
  double rollingBufferSize = 250;
  double rollingBufferSizeADXL = 250;
  double rollingBufferSizeEDA = 50;
  double rollingBufferSizeTemp = 25;

  static bool isCh1Checked = true;

  LineChartController _controllerADPD;
  LineChartController _controllerADXL;
  LineChartController _controllerSyncPPG;
  LineChartController _controllerECG;
  LineChartController _controllerEDA;
  LineChartController _controllerTemp;

  ILineDataSet ch1LineDataSetList;
  ILineDataSet ch2LineDataSetList;
  ILineDataSet xLineDataSetList;
  ILineDataSet yLineDataSetList;
  ILineDataSet zLineDataSetList;
  ILineDataSet syncLineDataSetList;
  ILineDataSet ecgLineDataSetList;
  ILineDataSet edaPhaseLineDataSetList;
  ILineDataSet edaModuleLineDataSetList;
  ILineDataSet tempLineDataSetList;

  double _removalCounterADPD = 0;
  double _removalCounterADXL = 0;
  double _removalCounterSyncPPG = 0;
  double _removalCounterECG = 0;
  double _removalCounterEDA = 0;
  double _removalCounterTemp = 0;

  MPChartManager() {
    initChartController(Sensors.ADPD);
    initChartController(Sensors.ADXL);
    initChartController(Sensors.PPG);
    initChartController(Sensors.ECG);
    initChartController(Sensors.EDA);
    initChartController(Sensors.TEMP);
  }

  @override
  LineChartController getChartController(Sensors sensor) {
    if (sensor == Sensors.ADPD) {
      return _controllerADPD;
    } else if (sensor == Sensors.ADXL) {
      return _controllerADXL;
    } else if (sensor == Sensors.PPG) {
      return _controllerSyncPPG;
    } else if (sensor == Sensors.ECG) {
      return _controllerECG;
    } else if (sensor == Sensors.EDA) {
      return _controllerEDA;
    } else if (sensor == Sensors.TEMP) {
      return _controllerTemp;
    }
    return null;
  }

  void _setChartController(Sensors sensor, LineChartController controller) {
    if (sensor == Sensors.ADPD) {
      _controllerADPD = controller;
    } else if (sensor == Sensors.ADXL) {
      _controllerADXL = controller;
    } else if (sensor == Sensors.PPG) {
      _controllerSyncPPG = controller;
    } else if (sensor == Sensors.ECG) {
      _controllerECG = controller;
    } else if (sensor == Sensors.EDA) {
      _controllerEDA = controller;
    } else if (sensor == Sensors.TEMP) {
      _controllerTemp = controller;
    }
  }

  List<ILineDataSet> _getLineItem(Sensors sensor) {
    List<ILineDataSet> dataSetList = [];
    if (sensor == Sensors.ADPD) {
      dataSetList.add(ch1LineDataSetList);
      dataSetList.add(ch2LineDataSetList);
    } else if (sensor == Sensors.ADXL) {
      dataSetList.add(xLineDataSetList);
      dataSetList.add(yLineDataSetList);
      dataSetList.add(zLineDataSetList);
    } else if (sensor == Sensors.PPG) {
      dataSetList.add(syncLineDataSetList);
    } else if (sensor == Sensors.ECG) {
      dataSetList.add(ecgLineDataSetList);
    } else if (sensor == Sensors.EDA) {
      dataSetList.add(edaPhaseLineDataSetList);
      dataSetList.add(edaModuleLineDataSetList);
    } else if (sensor == Sensors.TEMP) {
      dataSetList.add(tempLineDataSetList);
    }
    return dataSetList;
  }

  @override
  void updateChartValues(Sensors sensor, Object dataModel) {
    var controller = getChartController(sensor);
    LineData data = controller?.data;
    if (data == null) {
      data = LineData();
      controller.data = data;
    }

    var dataSetList = _getLineItem(sensor);

    if (sensor == Sensors.ADPD) {
      updateChartValueADPD(controller, dataSetList, dataModel);
    } else if (sensor == Sensors.PPG) {
      updateChartValueSyncPPG(controller, dataSetList, dataModel);
      //---------------ADXL------------------
      controller = getChartController(Sensors.ADXL);
      LineData data = controller?.data;
      if (data == null) {
        data = LineData();
        controller.data = data;
      }
      dataSetList = _getLineItem(Sensors.ADXL);
      updateChartValueADXL(controller, dataSetList, dataModel);
    } else if (sensor == Sensors.ECG) {
      updateChartValueECG(controller, dataSetList, dataModel);
    } else if (sensor == Sensors.EDA) {
      updateChartValueEDA(controller, dataSetList, dataModel);
    } else if (sensor == Sensors.TEMP) {
      updateChartValueTemp(controller, dataSetList, dataModel);
    }
  }

  void updateChartValueADPD(LineChartController controller,
      List<ILineDataSet> dataSetList, Object dataModel) {
    int index = 0;
    var _dataModel = dataModel as ADPDdataModel;
    LineData data = controller?.data;
    ILineDataSet _dataSet;

    //--------------ch1-----------------
    _dataSet = data.getDataSetByIndex(index);
    if (_dataSet == null) {
      _dataSet = dataSetList[index];
      data.addDataSet(_dataSet);
    }

    //--------------ch2-----------------
    _dataSet = data.getDataSetByIndex(index + 1);
    if (_dataSet == null) {
      _dataSet = dataSetList[index + 1];
      data.addDataSet(_dataSet);
    }

    var numSamples =
        () => (_dataSet.getEntryCount() * _settingsModel.adpdScaleFactor);
    var duration = _settingsModel.adpdConfig.duration.toDouble();

    double _x = (numSamples() + _removalCounterADPD);

    if (isCh1Checked) {
      data.addEntry(Entry(x: _x, y: _dataModel.ch1Data), index);
    }
    data.addEntry(Entry(x: _x, y: _dataModel.ch2Data), index + 1);

    if (numSamples() > duration) {
      data.removeEntry2(_removalCounterADPD.toDouble(), index);
      data.removeEntry2(_removalCounterADPD.toDouble(), index + 1);
      _removalCounterADPD += _settingsModel.adpdScaleFactor;
    }
    data?.notifyDataChanged();

    double minCount = numSamples() < duration ? numSamples() : duration;
    controller?.setVisibleXRangeMinimum(minCount);
    controller?.setVisibleXRangeMaximum(duration);
    controller?.moveViewToX(numSamples());
    controller?.state?.setStateIfNotDispose();
  }

  void updateChartValueADXL(LineChartController controller,
      List<ILineDataSet> dataSetList, Object dataModel) {
    int index = 0;
    var _dataModel = dataModel as SyncPPGdataModel;
    LineData data = controller?.data;
    ILineDataSet _dataSet;

    //------------X Value------------------
    _dataSet = data.getDataSetByIndex(index);
    if (_dataSet == null) {
      _dataSet = dataSetList[index];
      data.addDataSet(_dataSet);
    }

    //-----------Y Value-------------------
    _dataSet = data.getDataSetByIndex(index + 1);
    if (_dataSet == null) {
      _dataSet = dataSetList[index + 1];
      data.addDataSet(_dataSet);
    }

    //-----------Z Value-------------------
    _dataSet = data.getDataSetByIndex(index + 2);
    if (_dataSet == null) {
      _dataSet = dataSetList[index + 2];
      data.addDataSet(_dataSet);
    }

    var numSamples =
        () => (_dataSet.getEntryCount() * _settingsModel.ppgScaleFactor);
    var duration = _settingsModel.ppgConfig.duration.toDouble();

    double _x = (numSamples() + _removalCounterADXL).toDouble();

    data.addEntry(Entry(x: _x, y: _dataModel.xValue), index);
    data.addEntry(Entry(x: _x, y: _dataModel.yValue), index + 1);
    data.addEntry(Entry(x: _x, y: _dataModel.zValue), index + 2);

    if (numSamples() > duration) {
      data.removeEntry2(_removalCounterADXL.toDouble(), index);
      data.removeEntry2(_removalCounterADXL.toDouble(), index + 1);
      data.removeEntry2(_removalCounterADXL.toDouble(), index + 2);
      _removalCounterADXL += _settingsModel.ppgScaleFactor;
    }

    data?.notifyDataChanged();

    double minCount = numSamples() < duration ? numSamples() : duration;
    controller?.setVisibleXRangeMinimum(minCount);
    controller?.setVisibleXRangeMaximum(duration);
    controller?.moveViewToX(numSamples());
    controller?.state?.setStateIfNotDispose();
  }

  void updateChartValueSyncPPG(LineChartController controller,
      List<ILineDataSet> dataSetList, Object dataModel) {
    int index = 0;
    var _dataModel = dataModel as SyncPPGdataModel;
    LineData data = controller?.data;
    ILineDataSet _dataSet;

    _dataSet = data.getDataSetByIndex(index);
    if (_dataSet == null) {
      _dataSet = dataSetList[index];
      data.addDataSet(_dataSet);
    }

    double _x = ((_dataSet.getEntryCount() * _settingsModel.ppgScaleFactor) +
            _removalCounterSyncPPG)
        .toDouble();

    data.addEntry(Entry(x: _x, y: _dataModel.ppgData), index);

    if ((_dataSet.getEntryCount() * _settingsModel.ppgScaleFactor) >
        (_settingsModel.ppgConfig.duration)) {
      data.removeEntry2(_removalCounterSyncPPG, 0);
      _removalCounterSyncPPG += _settingsModel.ppgScaleFactor;
    }

    data?.notifyDataChanged();

    var count =
        (data.getEntryCount().toDouble() * _settingsModel.ppgScaleFactor);
    double minCount = count < (_settingsModel.ppgConfig.duration)
        ? count
        : (_settingsModel.ppgConfig.duration.toDouble());

    controller?.setVisibleXRangeMinimum(minCount);
    controller
        ?.setVisibleXRangeMaximum(_settingsModel.ppgConfig.duration.toDouble());
    controller?.moveViewToX(count);
    controller?.state?.setStateIfNotDispose();
  }

  void updateChartValueECG(LineChartController controller,
      List<ILineDataSet> dataSetList, Object dataModel) {
    int index = 0;
    var _dataModel = dataModel as ECGdataModel;
    LineData data = controller?.data;
    ILineDataSet _dataSet;

    _dataSet = data.getDataSetByIndex(index);
    if (_dataSet == null) {
      _dataSet = dataSetList[index];
      data.addDataSet(_dataSet);
    }

    var getNumSamples =
        () => (_dataSet.getEntryCount() * _settingsModel.ecgScaleFactor);
    var duration = _settingsModel.ecgConfig.duration;

    double _x = (getNumSamples() + _removalCounterECG);

    data.addEntry(Entry(x: _x, y: _dataModel.ecgData), index);

    if (getNumSamples() > duration) {
      data.removeEntry2(_removalCounterECG.toDouble(), 0);
      _removalCounterECG += _settingsModel.ecgScaleFactor;
    }

    data?.notifyDataChanged();

    double minCount =
        getNumSamples() < duration ? getNumSamples() : duration.toDouble();
    controller?.setVisibleXRangeMinimum(minCount);
    controller?.setVisibleXRangeMaximum(duration.toDouble());
    controller?.moveViewToX(getNumSamples());
    controller?.state?.setStateIfNotDispose();
  }

  void updateChartValueEDA(LineChartController controller,
      List<ILineDataSet> dataSetList, Object dataModel) {
    int index = 0;
    var _dataModel = dataModel as EDAdataModel;
    LineData data = controller?.data;
    ILineDataSet _dataSet;

    //------------Phase------------------
    _dataSet = data.getDataSetByIndex(index);
    if (_dataSet == null) {
      _dataSet = dataSetList[index];
      data.addDataSet(_dataSet);
    }

    //-----------Module-------------------
    _dataSet = data.getDataSetByIndex(index + 1);
    if (_dataSet == null) {
      _dataSet = dataSetList[index + 1];
      data.addDataSet(_dataSet);
    }

    var getNumSamples =
        () => (_dataSet.getEntryCount() * _settingsModel.edaScaleFactor);
    var duration = _settingsModel.edaConfig.duration;

    double _x = (getNumSamples() + _removalCounterEDA);

    data.addEntry(Entry(x: _x, y: _dataModel.impedancePhase), index);
    print('x :$_x impPhase:${_dataModel.impedancePhase}');

    data.addEntry(Entry(x: _x, y: _dataModel.impedanceMagnitude), index + 1);
    print('x :$_x impMaginitude:${_dataModel.impedanceMagnitude}');

    if (getNumSamples() > duration) {
      data.removeEntry2(_removalCounterEDA.toDouble(), index);
      data.removeEntry2(_removalCounterEDA.toDouble(), index + 1);
      _removalCounterEDA += _settingsModel.edaScaleFactor;
    }

    data?.notifyDataChanged();

    double minCount =
        getNumSamples() < duration ? getNumSamples() : duration.toDouble();
    controller?.setVisibleXRangeMinimum(minCount);
    controller?.setVisibleXRangeMaximum(duration.toDouble());
    controller?.moveViewToX(getNumSamples());
    controller?.state?.setStateIfNotDispose();
  }

  void updateChartValueTemp(LineChartController controller,
      List<ILineDataSet> dataSetList, Object dataModel) {
    int index = 0;
    var _dataModel = dataModel as TempdataModel;
    LineData data = controller?.data;
    ILineDataSet _dataSet;

    _dataSet = data.getDataSetByIndex(index);
    if (_dataSet == null) {
      _dataSet = dataSetList[index];
      data.addDataSet(_dataSet);
    }

    var numSamples =
        () => (_dataSet.getEntryCount() * _settingsModel.tempScaleFactor);
    var duration = _settingsModel.tempConfig.duration.toDouble();

    double _x = (numSamples() + _removalCounterTemp);

    data.addEntry(Entry(x: _x, y: _dataModel.tempSkin), index);
    print('x :$_x TempSkin:${_dataModel.tempSkin}');

    if (numSamples() > duration) {
      data.removeEntry2(_removalCounterTemp.toDouble(), 0);
      _removalCounterTemp += _settingsModel.tempScaleFactor;
    }

    data?.notifyDataChanged();

    double minCount = numSamples() < duration ? numSamples() : duration;
    controller?.setVisibleXRangeMinimum(minCount);
    controller?.setVisibleXRangeMaximum(duration);
    controller?.moveViewToX(numSamples());
    controller?.state?.setStateIfNotDispose();
  }

  @override
  void initChartController(Sensors sensor) {
    var desc = Description()..enabled = false;
    var _controller = LineChartController(
        axisLeftSettingFunction: (axisLeft, controller) {
          if (sensor == Sensors.ADPD) {
            axisLeft..textColor = (Colors.blue[900]);
          } else if (sensor == Sensors.EDA) {
            axisLeft..textColor = (Colors.indigo);
          } else {
            axisLeft..textColor = (Colors.black);
          }
          axisLeft.drawGridLines = (false);
        },
        axisRightSettingFunction: (axisRight, controller) {
          axisRight.drawGridLines = (false);
          if (sensor == Sensors.ADPD) {
            axisRight..textColor = (Colors.orange);
            axisRight.enabled = (true);
          } else if (sensor == Sensors.EDA) {
            axisRight..textColor = (Colors.deepOrange);
            axisRight.enabled = (true);
          } else {
            axisRight.enabled = (false);
          }
        },
        legendSettingFunction: (legend, controller) {
          legend
            ..shape = (LegendForm.LINE)
            ..textSize = (11)
            ..verticalAlignment = (LegendVerticalAlignment.BOTTOM)
            ..horizontalAlignment = (LegendHorizontalAlignment.LEFT)
            ..orientation = (LegendOrientation.HORIZONTAL)
            ..drawInside = (false);
        },
        xAxisSettingFunction: (xAxis, controller) {
          xAxis
            ..position = (XAxisPosition.BOTTOM)
            ..drawGridLines = (false)
            ..drawAxisLine = (true);
        },
        backgroundColor: Colors.grey[50],
        gridBackColor: Colors.grey[200],
        drawGridBackground: false,
        dragXEnabled: true,
        dragYEnabled: true,
        scaleXEnabled: true,
        scaleYEnabled: true,
        pinchZoomEnabled: false,
        description: desc);

    LineData data = _controller?.data;

    if (data == null) {
      data = LineData();
      _controller.data = data;
    }

    _setChartController(sensor, _controller);
    initChartLineItems(sensor);
  }

  @override
  void initChartLineItems(Sensors sensor) {
    if (sensor == Sensors.ADPD) {
      ch1LineDataSetList = _createSet(lineTitle: 'Ch1', lineColor: Colors.blue);
      ch2LineDataSetList =
          _createSet(lineTitle: 'Ch2', lineColor: Colors.orange);
    } else if (sensor == Sensors.ADXL) {
      xLineDataSetList =
          _createSet(lineTitle: 'X', lineColor: Colors.blue[900]);
      yLineDataSetList =
          _createSet(lineTitle: 'Y', lineColor: Colors.green[700]);
      zLineDataSetList = _createSet(lineTitle: 'Z', lineColor: Colors.black);
    } else if (sensor == Sensors.PPG) {
      syncLineDataSetList = _createSet(lineTitle: 'PPG', lineColor: Colors.red);
    } else if (sensor == Sensors.ECG) {
      ecgLineDataSetList =
          _createSet(lineTitle: 'ECG', lineColor: Colors.green);
    } else if (sensor == Sensors.EDA) {
      edaPhaseLineDataSetList =
          _createSet(lineTitle: 'Phase', lineColor: Colors.indigo);
      edaModuleLineDataSetList =
          _createSet(lineTitle: 'Module', lineColor: Colors.deepOrange);
    } else if (sensor == Sensors.TEMP) {
      tempLineDataSetList =
          _createSet(lineTitle: 'Temp', lineColor: Colors.teal);
    }
  }

  LineDataSet _createSet({String lineTitle, Color lineColor}) {
    LineDataSet set = LineDataSet(null, lineTitle);
    if (lineTitle == 'Module' || lineTitle == 'Ch1') {
      set.setAxisDependency(AxisDependency.RIGHT);
    } else {
      set.setAxisDependency(AxisDependency.LEFT);
    }
    set.setLineWidth(2.5);
    set.setDrawCircles(false);
    set.setColor1(lineColor);
    set.setDrawCircles(false);
    set.setMode(Mode.LINEAR);
    set.setDrawFilled(false);
    set.setDrawValues(false);
    return set;
  }

  @override
  void deinitChartLineItems(Sensors sensor) {
    if (sensor == Sensors.ADPD) {
      xLineDataSetList.clear();
      xLineDataSetList = null;
    }
  }

  @override
  void deinitChartController(Sensors sensor) {
    if (sensor == Sensors.ADPD) {
      _removalCounterADPD = 0;
      _controllerADPD.data?.clearValues();
      initChartLineItems(sensor); //Re-init the line item for next run
      _controllerADPD.state?.setStateIfNotDispose();
    } else if (sensor == Sensors.ADXL) {
      _removalCounterADXL = 0;
      _controllerADXL.data?.clearValues();
      initChartLineItems(sensor); //Re-init the line item for next run
      _controllerADXL.state?.setStateIfNotDispose();
    } else if (sensor == Sensors.PPG) {
      _removalCounterSyncPPG = 0;
      _controllerSyncPPG.data?.clearValues();
      initChartLineItems(sensor); //Re-init the line item for next run
      _controllerSyncPPG.state?.setStateIfNotDispose();

      _removalCounterADXL = 0;
      _controllerADXL.data?.clearValues();
      initChartLineItems(Sensors.ADXL); //Re-init the line item for next run
      _controllerADXL.state?.setStateIfNotDispose();
    } else if (sensor == Sensors.ECG) {
      _removalCounterECG = 0;
      _controllerECG.data?.clearValues();
      initChartLineItems(sensor); //Re-init the line item for next run
      _controllerECG.state?.setStateIfNotDispose();
    } else if (sensor == Sensors.EDA) {
      _removalCounterEDA = 0;
      _controllerEDA.data?.clearValues();
      initChartLineItems(sensor); //Re-init the line item for next run
      _controllerEDA.state?.setStateIfNotDispose();
    } else if (sensor == Sensors.TEMP) {
      _removalCounterTemp = 0;
      _controllerTemp.data?.clearValues();
      initChartLineItems(sensor); //Re-init the line item for next run
      _controllerTemp.state?.setStateIfNotDispose();
    }
  }
}
