import 'package:flutter/material.dart';
import 'package:flutter_switch/flutter_switch.dart';
import 'package:font_awesome_flutter/font_awesome_flutter.dart';
import 'package:provider/provider.dart';
import 'package:vsm/business_logic/utils/images.dart';
import 'package:vsm/business_logic/view_model/logging_viewmodel.dart';
import 'package:vsm/services/service_locator.dart';

class LoggingView extends StatefulWidget {
  static const String id = "logging_view";
  @override
  _LoggingViewState createState() => _LoggingViewState();
}

class _LoggingViewState extends State<LoggingView> {
  final LoggingViewModel _viewModel = serviceLocator<LoggingViewModel>();

  @override
  void initState() {
    print('Logging Init state called');
    _viewModel?.initState();
    super.initState();
  }

  @override
  void deactivate() {
    print('Logging Deactivate called');
    super.deactivate();
  }

  @override
  Widget build(BuildContext context) {
    return ChangeNotifierProvider<LoggingViewModel>(
      create: (context) => _viewModel,
      child: Consumer<LoggingViewModel>(
        builder: (context, _viewmodel, child) => SafeArea(
          child: Scaffold(
            appBar: AppBar(
              title: Text(
                'Logging',
                style: TextStyle(color: Colors.white),
              ),
              centerTitle: true,
            ),
            body: Column(children: [
              Card(
                child: SizedBox(
                  height: 65,
                  child: Row(
                    children: <Widget>[
                      Expanded(
                        flex: 2,
                        child: Padding(
                          padding: const EdgeInsets.only(left: 8.0),
                          child: Icon(
                            FontAwesomeIcons.fileCsv,
                            color: Colors.green[700],
                            size: 36,
                          ),
                        ),
                      ),
                      Expanded(
                        flex: 15,
                        child: Padding(
                          padding:
                              const EdgeInsets.fromLTRB(20.0, 0.0, 2.0, 0.0),
                          child: Text(
                            'Logging',
                            style: TextStyle(color: Colors.black, fontSize: 36),
                          ),
                        ),
                      ),
                      Expanded(
                        flex: 4,
                        child: FlutterSwitch(
                          width: 60.0,
                          height: 30.0,
                          valueFontSize: 14.0,
                          toggleSize: 22.0,
                          showOnOff: true,
                          activeColor: Colors.lightBlue[700],
                          activeTextColor: Colors.white,
                          value: _viewmodel.isLogSwitched,
                          onToggle: (isLogON) async {
                            if (isLogON) {
                              // var fsInfo = await _viewModel.getVolInfo();
                              // if (fsInfo.freeMemory < 5) {
                              //   print('Less than 5');
                              //   return;
                              // } else {
                              //   if (fsInfo.freeMemory < 25) {
                              //     print('less than 25');
                              //   }

                              final String participantId =
                                  await _asyncInputDialog(context);
                              print("Current Log Session Id is $participantId");
                              _viewModel.onLoggingStateChanged(
                                  isLogON, participantId);
                              //}
                            } else {
                              _viewModel.onLoggingStateChanged(isLogON, '');
                            }
                          },
                        ),
                      )
                    ],
                  ),
                ),
              ),
              SizedBox(
                height: 15.0,
              ),
              CustomListTile(
                icon: Image.asset(
                  ImageIcons.ppgImage,
                  fit: BoxFit.fill,
                ),
                itemName: 'PPG',
                switchValue: _viewmodel.isPPGSwitched,
                switchCallback: _viewmodel.isLogSwitched
                    ? null
                    : _viewModel.onPPGStateChanged,
              ),
              CustomListTile(
                icon: Image.asset(
                  ImageIcons.ecgImage,
                  fit: BoxFit.fill,
                ),
                itemName: 'ECG',
                switchValue: _viewmodel.isECGSwitched,
                switchCallback: _viewmodel.isLogSwitched
                    ? null
                    : _viewModel.onECGStateChanged,
              ),
              CustomListTile(
                icon: Icon(FontAwesomeIcons.handHoldingHeart,
                    color: Colors.blue[600]),
                itemName: 'EDA',
                switchValue: _viewmodel.isEDASwitched,
                switchCallback: _viewmodel.isLogSwitched
                    ? null
                    : _viewModel.onEDAStateChanged,
              ),
              CustomListTile(
                icon: Icon(
                  FontAwesomeIcons.thermometerHalf,
                  color: Colors.orange[700],
                  size: 32,
                ),
                itemName: 'Temp',
                switchValue: _viewmodel.isTempSwitched,
                switchCallback: _viewmodel.isLogSwitched
                    ? null
                    : _viewModel.onTempStateChanged,
              ),
              CustomListTile(
                icon: Image.asset(
                  ImageIcons.spo2Image,
                  fit: BoxFit.fill,
                ),
                itemName: 'Spo2',
                switchValue: _viewmodel.isADPDSwitched,
                switchCallback: _viewmodel.isLogSwitched
                    ? null
                    : _viewModel.onADPDStateChanged,
              ),
              CustomListTile(
                icon: Image.asset(
                  ImageIcons.adxlImage,
                  fit: BoxFit.fill,
                ),
                itemName: 'Accelerometer',
                switchValue: _viewmodel.isADXLSwitched,
                switchCallback: _viewmodel.isLogSwitched
                    ? null
                    : _viewModel.onADXLStateChanged,
              ),
            ]),
          ),
        ),
      ),
    );
  }
}

Future _asyncInputDialog(BuildContext context) async {
  String teamName = '';
  return await showDialog(
    context: context,
    barrierDismissible:
        false, // dialog is dismissible with a tap on the barrier
    builder: (BuildContext context) {
      return AlertDialog(
        title: Text(
          'Enter Log Session Id',
          style: TextStyle(color: Colors.black),
        ),
        content: new Row(
          children: [
            new Expanded(
                child: new TextField(
              autofocus: true,
              decoration: new InputDecoration(
                  labelText: 'Participant Id', hintText: 'eg. person101'),
              onChanged: (value) {
                teamName = value;
              },
            ))
          ],
        ),
        actions: [
          FlatButton(
            child: Text('Ok'),
            onPressed: () {
              Navigator.of(context).pop(teamName);
            },
          ),
        ],
      );
    },
  );
}

class CustomListTile extends StatelessWidget {
  final String itemName;
  final Widget icon;
  final bool switchValue;
  final Function(bool) switchCallback;
  CustomListTile(
      {@required this.itemName,
      @required this.icon,
      @required this.switchValue,
      @required this.switchCallback});

  @override
  Widget build(BuildContext context) {
    return Card(
      child: SizedBox(
        height: 55,
        child: Row(
          children: <Widget>[
            Expanded(
              flex: 2,
              child: Padding(
                padding: const EdgeInsets.only(left: 8.0),
                child: icon,
              ),
            ),
            Expanded(
              flex: 15,
              child: Padding(
                padding: const EdgeInsets.fromLTRB(20.0, 0.0, 2.0, 0.0),
                child: Text(
                  '$itemName',
                  style: TextStyle(color: Colors.black, fontSize: 22),
                ),
              ),
            ),
            Expanded(
              flex: 4,
              child: FlutterSwitch(
                width: 53.0,
                height: 28.0,
                valueFontSize: 12.0,
                toggleSize: 18.0,
                showOnOff: true,
                activeColor: Colors.lightBlue[700],
                activeTextColor: Colors.white,
                value: switchValue,
                onToggle: switchCallback,
              ),
            )
          ],
        ),
      ),
    );
  }
}
