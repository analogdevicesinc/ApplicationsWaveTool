import 'package:flutter/material.dart';
import 'package:mp_chart/mp/chart/line_chart.dart';
import 'package:mp_chart/mp/controller/line_chart_controller.dart';
import 'package:provider/provider.dart';
import 'package:vsm/business_logic/view_model/chart_viewmodel.dart';
import 'package:vsm/services/service_locator.dart';
import 'package:toggle_switch/toggle_switch.dart';
import 'package:smooth_page_indicator/smooth_page_indicator.dart';
import 'package:wakelock/wakelock.dart';

class ChartView extends StatefulWidget {
  @override
  _ChartViewState createState() => _ChartViewState();
}

class _ChartViewState extends State<ChartView> {
  ChartViewModel _viewModel = serviceLocator<ChartViewModel>();
  Widget _adpdChart;
  Widget _ppgChart;
  Widget _ecgChart;
  Widget _edaChart;
  Widget _tempChart;
  Widget _selectedChart;
  int _selectedIndex = 0;
  PageController controller;

  @override
  void initState() {
    super.initState();
    print('Chartview Init state called');
    Wakelock.enable();

    _viewModel?.initState();
    controller = PageController(initialPage: 0, viewportFraction: 0.95);
    _adpdChart = ShowChart(
        chartName: 'SpO2', chartControl: _viewModel.chartControllerADPD);
    _ppgChart = getPPGChartLayout();
    _ecgChart = ShowChart(
        chartName: 'ECG', chartControl: _viewModel.chartControllerECG);
    _edaChart = ShowChart(
        chartName: 'EDA', chartControl: _viewModel.chartControllerEDA);
    _tempChart = ShowChart(
        chartName: 'Temp', chartControl: _viewModel.chartControllerTemp);

    _selectedIndex = _viewModel.getChartIndex();
    _selectedChart = switchChart(_selectedIndex);
  }

  @override
  void deactivate() {
    print('Chartview Deactivate called');
    Wakelock.disable();
    super.deactivate();
  }

  Widget switchChart(int pageIndex) {
    switch (pageIndex) {
      case 0:
        return _ppgChart;
        break;
      case 1:
        return _ecgChart;
        break;
      case 2:
        return _edaChart;
        break;
      case 3:
        return _tempChart;
        break;
      case 4:
        return _adpdChart;
        break;
      default:
        return Text('wrong index received');
    }
  }

  Widget getPPGChartLayout() {
    return Directionality(
      textDirection: TextDirection.ltr,
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: <Widget>[
          Expanded(
            child: PageView(
              controller: controller,
              children: List.generate(
                2,
                (index) => Stack(
                  children: [
                    Card(
                      shape: RoundedRectangleBorder(
                          borderRadius: BorderRadius.circular(8)),
                      child: index == 0
                          ? LineChart(_viewModel.chartControllerSyncPPG)
                          : LineChart(_viewModel.chartControllerADXL),
                    ),
                    Align(
                      alignment: Alignment.topCenter,
                      child: Padding(
                        padding: const EdgeInsets.only(top: 12.0),
                        child: Text(
                          index == 0 ? 'PPG chart' : 'ADXL Chart',
                          style: TextStyle(fontSize: 18.0, color: Colors.black),
                        ),
                      ),
                    ),
                    Align(
                      alignment: Alignment.bottomCenter,
                      child: Padding(
                        padding: const EdgeInsets.only(bottom: 8.0),
                        child: Text(
                          'Seconds',
                          style: TextStyle(fontSize: 18.0, color: Colors.black),
                        ),
                      ),
                    ),
                  ],
                ),
              ),
            ),
          ),
          SizedBox(height: 5),
          Container(
            child: SmoothPageIndicator(
              controller: controller,
              count: 2,
              textDirection: TextDirection.ltr,
              effect: WormEffect(),
            ),
          ),
          SizedBox(height: 8),
        ],
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    var colorScheme = Theme.of(context).colorScheme;

    return ChangeNotifierProvider<ChartViewModel>(
      create: (context) => _viewModel,
      child: Consumer<ChartViewModel>(
        builder: (context, _viewmodel, child) => Scaffold(
          appBar: AppBar(
            backgroundColor: colorScheme.primary,
            title: Center(
              child: Text(
                'Charts',
                style: TextStyle(color: Colors.white),
                textAlign: TextAlign.center,
              ),
            ),
          ),
          body:
              Column(crossAxisAlignment: CrossAxisAlignment.stretch, children: [
            Expanded(
              flex: 1,
              child: Container(
                alignment: Alignment.center,
                child: ToggleSwitch(
                  minWidth: 250.0,
                  fontSize: 16.0,
                  iconSize: 20.0,
                  initialLabelIndex: _selectedIndex,
                  cornerRadius: 15.0,
                  activeFgColor: Colors.white,
                  inactiveBgColor: Colors.grey,
                  inactiveFgColor: Colors.white,
                  labels: ['PPG', 'ECG', 'EDA', 'Temp', 'SpO2'],
                  activeBgColors: [
                    Colors.blue[700],
                    Colors.pink[800],
                    Colors.purple[800],
                    Colors.teal,
                    Colors.purple[800],
                  ],
                  onToggle: (tappedIndex) {
                    print('switched to: $tappedIndex');
                    setState(() {
                      _selectedIndex = tappedIndex;
                      _selectedChart = switchChart(tappedIndex);
                    });
                  },
                ),
              ),
            ),
            Expanded(
              flex: 10,
              child: Stack(children: [
                _selectedChart,
                Align(
                  alignment: Alignment.centerLeft,
                  child: Wrap(direction: Axis.vertical, children: [
                    RotatedBox(
                      quarterTurns: -1,
                      child: Padding(
                        padding: const EdgeInsets.only(top: 5.0),
                        child: Text(
                          'Amplitude',
                          style: TextStyle(
                            fontSize: 14.0,
                            color: Colors.black,
                          ),
                        ),
                      ),
                    ),
                  ]),
                ),
              ]),
            ),
          ]),
        ),
      ),
    );
  }
}

class ShowChart extends StatelessWidget {
  final String chartName;
  final LineChartController chartControl;
  ShowChart({@required this.chartName, @required this.chartControl});

  @override
  Widget build(BuildContext context) {
    return Consumer<ChartViewModel>(
      builder: (context, _viewmodel, child) => Padding(
        padding: const EdgeInsets.only(
            left: 10.0, top: 5.0, right: 5.0, bottom: 5.0),
        child: Stack(children: [
          Container(
            color: Colors.grey,
            child: LineChart(chartControl),
          ),
          Align(
            alignment: Alignment.topCenter,
            child: Padding(
              padding: const EdgeInsets.only(top: 12.0),
              child: Text(
                '$chartName chart',
                style: TextStyle(fontSize: 18.0, color: Colors.black),
              ),
            ),
          ),
          Align(
            alignment: Alignment.bottomCenter,
            child: Padding(
              padding: const EdgeInsets.only(bottom: 8.0),
              child: Text(
                'Seconds',
                style: TextStyle(fontSize: 18.0, color: Colors.black),
              ),
            ),
          ),
        ]),
      ),
    );
  }
}

class PPGChart extends StatelessWidget {
  final String chartName;
  final LineChartController ppgChartControl;
  final LineChartController adxlChartControl;

  PPGChart(
      {@required this.chartName,
      @required this.ppgChartControl,
      @required this.adxlChartControl});

  @override
  Widget build(BuildContext context) {
    final controller = PageController(viewportFraction: 0.95);
    return Directionality(
      textDirection: TextDirection.rtl,
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: <Widget>[
          Expanded(
            child: PageView(
              controller: controller,
              children: List.generate(
                2,
                (index) => Card(
                  shape: RoundedRectangleBorder(
                      borderRadius: BorderRadius.circular(16)),
                  child: index == 1
                      ? LineChart(ppgChartControl)
                      : LineChart(adxlChartControl),
                ),
              ),
            ),
          ),
          SizedBox(height: 5),
          Container(
            child: SmoothPageIndicator(
              controller: controller,
              count: 2,
              textDirection: TextDirection.rtl,
              effect: WormEffect(),
            ),
          ),
          SizedBox(height: 8),
        ],
      ),
    );
  }
}
