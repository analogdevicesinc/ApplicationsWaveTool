import 'package:auto_size_text/auto_size_text.dart';
import 'package:flutter/material.dart';
import 'package:flutter_switch/flutter_switch.dart';
import 'package:font_awesome_flutter/font_awesome_flutter.dart';
import 'package:provider/provider.dart';
import 'package:vsm/business_logic/utils/enumerations.dart';
import 'package:vsm/business_logic/utils/images.dart';
import 'package:vsm/business_logic/view_model/dashboard_viewmodel.dart';
import 'package:vsm/business_logic/view_model/main_viewmodel.dart';
import 'package:vsm/services/network/communication_service.dart';
import 'package:vsm/services/service_locator.dart';
import 'package:vsm/user_interface/views/theme/theme_provider.dart';

class DashboardPortrait extends StatefulWidget {
  static const String id = "dashboard_view";
  @override
  _DashboardPortraitState createState() => _DashboardPortraitState();
}

class _DashboardPortraitState extends State<DashboardPortrait> {
  final CommunicationService bledatainstance =
      serviceLocator<CommunicationService>();
  final DashboardViewModel _viewModel = serviceLocator<DashboardViewModel>();

  @override
  void initState() {
    print('Dashboard Init state called');
    _viewModel?.initState();
    super.initState();
  }

  @override
  void deactivate() {
    print('Dashboard Deactivate called');
    super.deactivate();
  }

  @override
  Widget build(BuildContext context) {
    var colorScheme = Theme.of(context).colorScheme;

    return ChangeNotifierProvider<DashboardViewModel>(
      create: (context) => _viewModel,
      child: Consumer<DashboardViewModel>(
        builder: (context, _viewmodel, child) => Consumer<ThemeProvider>(
          builder: (context, themeProvider, child) => Consumer<MainViewModel>(
            builder: (context, _mainViewModel, child) => SafeArea(
              child: Scaffold(
                appBar: AppBar(
                  leading: Icon(
                    Icons.bluetooth,
                    color: Colors.white,
                    size: 34.0,
                  ),
                  backgroundColor: colorScheme.primary,
                  title: Text(
                    'Dashboard',
                    style: TextStyle(color: Colors.white),
                    textAlign: TextAlign.center,
                  ),
                  centerTitle: true,
                ),
                body: SingleChildScrollView(
                  child: Column(children: [
                    SizedBox(
                      height: 10.0,
                    ),
                    Row(children: [
                      Expanded(
                        child: ReusableCard2(
                          icon: Image.asset(
                            ImageIcons.ppgImage,
                            height: 40,
                          ),
                          title: 'PPG',
                          value: '${_viewModel.ppgHR}',
                          unit: 'bpm',
                          switchValue: _viewModel.isPPGSwitched,
                          switchCallback: _viewModel.onPPGStateChanged,
                        ),
                      ),
                      Expanded(
                        child: ReusableCard2(
                          icon: Image.asset(
                            ImageIcons.ecgImage,
                            height: 40,
                          ),
                          title: 'ECG',
                          value: '${_viewModel.ecgHR}',
                          unit: ' bpm',
                          switchValue: _viewModel.isECGSwitched,
                          switchCallback: _viewModel.onECGStateChanged,
                        ),
                      ),
                    ]),
                    Row(children: [
                      Expanded(
                        child: ReusableCard2(
                          icon: Icon(
                            FontAwesomeIcons.handHoldingHeart,
                            color: Colors.blue.shade500,
                            size: 40.0,
                          ),
                          title: 'EDA',
                          value: '--',
                          unit: '    ',
                          switchValue: _viewModel.isEDASwitched,
                          switchCallback: _viewModel.onEDAStateChanged,
                        ),
                      ),
                      Expanded(
                        child: ReusableCard2(
                          icon: Icon(
                            FontAwesomeIcons.thermometerHalf,
                            color: Colors.orange,
                            size: 35.0,
                          ),
                          title: 'Temp',
                          value: '${_viewModel.tempValue}',
                          unit: ' ℃  ',
                          switchValue: _viewModel.isTempSwitched,
                          switchCallback: _viewModel.onTempStateChanged,
                        ),
                      ),
                    ]),
                    ADPD4x00Card(),
                  ]),
                ),
                floatingActionButton: FloatingActionButton.extended(
                  onPressed: () async {
                    await bledatainstance.disconnectDevice();
                    _mainViewModel.updateView(ViewCategory.HomePage);
                  },
                  icon: Icon(Icons.bluetooth_disabled),
                  label: Text('Disconnect'),
                ),
              ),
            ),
          ),
        ),
      ),
    );
  }
}

class ReusableCard2 extends StatelessWidget {
  ReusableCard2(
      {@required this.icon,
      this.title,
      this.value,
      this.unit,
      this.switchValue,
      this.switchCallback});

  final Widget icon;
  final String title;
  final String value;
  final String unit;
  final Function(bool) switchCallback;
  final bool switchValue;

  @override
  Widget build(BuildContext context) {
    var textTheme = Theme.of(context).textTheme;
    var colorScheme = Theme.of(context).colorScheme;
    return Card(
      color: colorScheme.background,
      elevation: 4,
      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(15)),
      margin: const EdgeInsets.symmetric(vertical: 10.0, horizontal: 10.0),
      child: Padding(
        padding: const EdgeInsets.symmetric(vertical: 20.0),
        child: Row(
          mainAxisAlignment: MainAxisAlignment.spaceAround,
          children: [
            Expanded(
              flex: 2,
              child: Container(
                child: icon,
              ),
            ),
            Expanded(
              flex: 6,
              child: Container(
                child: Column(children: [
                  Padding(
                    padding: const EdgeInsets.only(bottom: 16.0),
                    child: Row(
                      children: [
                        Expanded(
                          flex: 4,
                          child: Container(
                            alignment: Alignment.center,
                            child: AutoSizeText(
                              title,
                              style: textTheme.bodyText1,
                              maxLines: 1,
                            ),
                          ),
                        ),
                        Expanded(
                          flex: 3,
                          child: Container(
                            alignment: Alignment.center,
                            child: FlutterSwitch(
                              width: 50.0,
                              height: 25.0,
                              valueFontSize: 10.0,
                              toggleSize: 15.0,
                              showOnOff: true,
                              activeColor: Colors.lightBlue[700],
                              activeTextColor: Colors.white,
                              value: switchValue,
                              onToggle: (val) {
                                switchCallback(val);
                              },
                            ),
                          ),
                        ),
                      ],
                    ),
                  ),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.start,
                    crossAxisAlignment: CrossAxisAlignment.end,
                    children: <Widget>[
                      Expanded(
                        flex: 5,
                        child: Container(
                          alignment: Alignment.center,
                          child: AutoSizeText(
                            '$value',
                            style: textTheme.headline2,
                            maxLines: 1,
                          ),
                        ),
                      ),
                      Expanded(
                        flex: 4,
                        child: Padding(
                          padding: const EdgeInsets.only(bottom: 4.0),
                          child: Container(
                            alignment: Alignment.topLeft,
                            child: AutoSizeText(
                              '$unit',
                              style: textTheme.headline3,
                            ),
                          ),
                        ),
                      )
                    ],
                  ),
                ]),
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class ADPD4x00Card extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    var textTheme = Theme.of(context).textTheme;
    var colorScheme = Theme.of(context).colorScheme;
    return Consumer<DashboardViewModel>(
      builder: (context, _viewmodel, child) => Card(
        color: colorScheme.background,
        elevation: 4,
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(15)),
        margin: const EdgeInsets.symmetric(vertical: 10.0, horizontal: 10.0),
        child: Padding(
            padding: const EdgeInsets.symmetric(vertical: 20.0),
            child: Column(
              children: [
                Row(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    Container(
                      child: Image.asset(
                        ImageIcons.spo2Image,
                        height: 35,
                      ),
                    ),
                    Padding(
                      padding: const EdgeInsets.only(left: 8.0),
                      child: Text(
                        'SpO2',
                        style: textTheme.bodyText1,
                      ),
                    ),
                  ],
                ),
                LedControl(
                    ledName: 'Red',
                    ledColor: Colors.red,
                    adpdLED: AdpdLED.Red,
                    switchValue: _viewmodel.isAdpdRedLEDSwitched,
                    switchCallback: _viewmodel.onADPDStateChanged
                    // _viewmodel.isAnySensorON &&
                    //         !_viewmodel.isAdpdRedLEDSwitched
                    //     ? null
                    //     : _viewmodel.onADPDStateChanged,
                    ),
                LedControl(
                  ledName: 'Green',
                  ledColor: Colors.green,
                  adpdLED: AdpdLED.Green,
                  switchValue: _viewmodel.isAdpdGreenLEDSwitched,
                  switchCallback: _viewmodel.onADPDStateChanged,
                  // _viewmodel.isAnySensorON &&
                  //         !_viewmodel.isAdpdGreenLEDSwitched
                  //     ? null
                  //     : _viewmodel.onADPDStateChanged,
                ),
                LedControl(
                  ledName: 'Blue ',
                  ledColor: Colors.blue,
                  adpdLED: AdpdLED.Blue,
                  switchValue: _viewmodel.isAdpdBlueLEDSwitched,
                  switchCallback: _viewmodel.onADPDStateChanged,
                  // _viewmodel.isAnySensorON &&
                  //         !_viewmodel.isAdpdBlueLEDSwitched
                  //     ? null
                  //     : _viewmodel.onADPDStateChanged,
                ),
                LedControl(
                    ledName: 'IR   ',
                    ledColor: Colors.black,
                    adpdLED: AdpdLED.IR,
                    switchValue: _viewmodel.isAdpdIRLEDSwitched,
                    switchCallback: _viewmodel.onADPDStateChanged
                    // _viewmodel.isAnySensorON &&
                    //         !_viewmodel.isAdpdIRLEDSwitched
                    //     ? null
                    //     : _viewmodel.onADPDStateChanged,
                    ),
              ],
            )),
      ),
    );
  }
}

class LedControl extends StatelessWidget {
  final String ledName;
  final Color ledColor;
  final AdpdLED adpdLED;
  final Function(bool, AdpdLED) switchCallback;
  final bool switchValue;

  LedControl({
    this.ledName,
    this.ledColor,
    this.switchValue,
    this.adpdLED,
    this.switchCallback,
  });

  @override
  Widget build(BuildContext context) {
    var textTheme = Theme.of(context).textTheme;
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 10.0),
      child: Row(
        mainAxisAlignment: MainAxisAlignment.start,
        children: [
          Expanded(
            flex: 2,
            child: Container(
              alignment: Alignment.centerRight,
              child: Icon(
                Icons.circle,
                color: ledColor, // Theme.of(context).iconTheme.color,
                size: 28.0,
              ),
            ),
          ),
          Expanded(
            flex: 3,
            child: Container(
              alignment: Alignment.centerLeft,
              //color: Colors.blueGrey,
              child: Padding(
                padding: const EdgeInsets.only(left: 8.0),
                child: Text(
                  '$ledName',
                  style: textTheme.bodyText1,
                ),
              ),
            ),
          ),
          Expanded(
            flex: 5,
            child: Container(
              //color: Colors.blue,
              alignment: Alignment.center,
              child: FlutterSwitch(
                width: 65.0,
                height: 30.0,
                valueFontSize: 14.0,
                toggleSize: 25.0,
                showOnOff: true,
                activeColor: Colors.lightBlue[700],
                // activeToggleColor: ledColor,
                // inactiveToggleColor: ledColor,
                activeTextColor: Colors.white,
                value: switchValue,
                onToggle: (isSwitched) {
                  switchCallback(isSwitched, adpdLED);
                },
              ),
            ),
          ),
        ],
      ),
    );
  }
}
