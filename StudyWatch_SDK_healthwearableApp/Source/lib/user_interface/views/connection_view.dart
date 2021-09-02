import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:provider/provider.dart';
import 'package:vsm/business_logic/utils/enumerations.dart';
import 'package:vsm/business_logic/view_model/connection_viewmodel.dart';
import 'package:vsm/business_logic/view_model/main_viewmodel.dart';
import 'package:vsm/main.dart';
import 'package:vsm/services/service_locator.dart';

class ConnectionView extends StatefulWidget {
  @override
  _ConnectionViewState createState() => _ConnectionViewState();
}

class _ConnectionViewState extends State<ConnectionView> {
  ConnectionViewModel _viewModel = serviceLocator<ConnectionViewModel>();
  AppNavigator _appNavigator = AppNavigator();

  @override
  void initState() {
    super.initState();
    print('Ble view initState...isConnected ${_viewModel.isConnected}');
  }

  @override
  void deactivate() {
    super.deactivate();
    print('Ble view deactivate...');
    _viewModel.stopScan();
    _viewModel.deviceList.clear();
  }

  @override
  Widget build(BuildContext context) {
    return ChangeNotifierProvider<ConnectionViewModel>(
      create: (context) => _viewModel,
      child: Consumer<ConnectionViewModel>(
        builder: (context, _viewmodel, child) => Consumer<MainViewModel>(
          builder: (context, _mainViewModel, child) => Scaffold(
            appBar: _viewmodel.connectionStatus != ConnectStatus.Connected
                ? AppBar(
                    title: Text('Health Wearable'),
                    centerTitle: true,
                  )
                : null,
            body: StreamBuilder<BluetoothState>(
                stream: FlutterBlue.instance.state,
                initialData: BluetoothState.unknown,
                builder: (c, snapshot) {
                  final state = snapshot.data;
                  if (state == BluetoothState.on) {
                    _viewmodel.scanDevices();

                    var body =
                        _buildBodySection(_viewmodel, _mainViewModel, context);

                    return body ?? _appNavigator;
                  } else {
                    _viewmodel.stopScan();
                    _viewmodel.deviceList.clear();
                    return BluetoothOffScreen(state: state);
                  }
                }),
            floatingActionButton: _viewmodel.isBluetoothON
                ? _buildFloatingActionButton(_viewmodel, _mainViewModel)
                : Container(),
          ),
        ),
      ),
    );
  }
}

Widget _buildFloatingActionButton(
    ConnectionViewModel _viewmodel, MainViewModel _mainviewmodel) {
  Widget fab;
  if (_viewmodel.connectionStatus == ConnectStatus.Connected) {
    fab = Container();
  } else if (_viewmodel.connectionStatus ==
      ConnectStatus.ConnectionInProgress) {
    fab = Container();
  } else {
    fab = FloatingActionButton.extended(
      onPressed: () {
        _viewmodel.stopScan();
        _mainviewmodel.updateView(ViewCategory.HomePage);
      },
      icon: Icon(Icons.cancel),
      label: Text('Stop Scan'),
    );
  }

  return fab;
}

Widget _buildBodySection(ConnectionViewModel _viewmodel,
    MainViewModel _mainviewmodel, BuildContext context) {
  Widget _bodyWidget;

  if (_viewmodel.connectionStatus == ConnectStatus.Connected) {
    _bodyWidget = null;
  } else if (_viewmodel.connectionStatus == ConnectStatus.ScanningInProgress) {
    if (_viewmodel.deviceList.length > 0) {
      _bodyWidget = ListView.builder(
        padding: EdgeInsets.symmetric(vertical: 5, horizontal: 2),
        itemCount: _viewmodel.deviceList.length,
        itemBuilder: (_, index) {
          return DeviceInfo(
              _viewmodel.deviceList[index], _viewmodel.connectDevice);
        },
      );
    } else {
      _bodyWidget = Stack(
        children: <Widget>[
          LinearProgressIndicator(
            minHeight: 2.0,
          ),
          _buildScanningBackground(),
        ],
      );
    }
  } else if (_viewmodel.connectionStatus ==
      ConnectStatus.ConnectionInProgress) {
    _bodyWidget = Center(
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        crossAxisAlignment: CrossAxisAlignment.center,
        children: <Widget>[
          CircularProgressIndicator(
            backgroundColor: Colors.grey,
            strokeWidth: 8.0,
          ),
          SizedBox(height: 20),
          Text(
            "Connecting to ADI Watch...",
            style: TextStyle(fontSize: 22.0, fontWeight: FontWeight.bold),
          ),
        ],
      ),
    );
  } else if (_viewmodel.connectionStatus == ConnectStatus.ConnectionFailed) {
    _bodyWidget = Center(
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        crossAxisAlignment: CrossAxisAlignment.center,
        children: [
          Text(
            "Connection Failed.\nPlease check the device bluetooth/Watch power state and try again..",
            style: TextStyle(
                fontSize: 18.0,
                color: Colors.black,
                fontWeight: FontWeight.bold),
            textAlign: TextAlign.center,
          ),
          SizedBox(
            height: 30,
          ),
          RaisedButton(
            padding: const EdgeInsets.all(8.0),
            textColor: Colors.white,
            color: Colors.blue[900],
            child: Text('Ok'),
            onPressed: () {
              _viewmodel.stopScan();
              _mainviewmodel.updateView(ViewCategory.HomePage);
            },
          ),
        ],
      ),
    );
  } else {
    //Disconnected
    print('Else case : Disconnected state');
    _viewmodel.stopScan();
    _mainviewmodel.updateView(ViewCategory.HomePage);
  }

  return _bodyWidget;
}

Widget _buildScanningBackground() {
  return SizedBox.expand(
      child: Column(
    mainAxisAlignment: MainAxisAlignment.center,
    crossAxisAlignment: CrossAxisAlignment.center,
    children: <Widget>[
      Image.asset("assets/images/bluetooth_scanning.png", height: 250),
      SizedBox(height: 20),
      Text("Scanning...",
          style: TextStyle(fontSize: 22.0, fontWeight: FontWeight.bold)),
      SizedBox(height: 20),
      Text("Make sure you are in a close range to your ADI Watch",
          style: TextStyle(fontSize: 16.0, fontWeight: FontWeight.bold)),
    ],
  ));
}

class BluetoothOffScreen extends StatelessWidget {
  const BluetoothOffScreen({Key key, this.state}) : super(key: key);

  final BluetoothState state;

  @override
  Widget build(BuildContext context) {
    var textTheme = Theme.of(context).textTheme;
    return Scaffold(
      body: Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: <Widget>[
            Icon(
              Icons.bluetooth_disabled,
              size: 200.0,
              color: Color(0xE1085692),
            ),
            Text(
              'Bluetooth Adapter is ${state != null ? state.toString().substring(15) : 'not available'}.',
              style: textTheme.headline3,
            ),
            SizedBox(
              height: 20,
            ),
            Text(
              'Please turn ON the bluetooth to scan devices ',
              style: textTheme.headline4,
            ),
          ],
        ),
      ),
    );
  }
}

class GotoDashboard extends StatefulWidget {
  @override
  _GotoDashboardState createState() => _GotoDashboardState();
}

class _GotoDashboardState extends State<GotoDashboard> {
  @override
  void initState() {
    // TODO: implement initState
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return Container();
  }
}

class ConnectedView extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    var textTheme = Theme.of(context).textTheme;
    //var colorScheme = Theme.of(context).colorScheme;

    return Consumer<ConnectionViewModel>(
      builder: (context, _viewmodel, child) => Consumer<MainViewModel>(
        builder: (context, _mainViewModel, child) => Container(
          // color: Colors.red,
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            crossAxisAlignment: CrossAxisAlignment.stretch,
            children: [
              Center(
                child: Text(
                  '${_viewmodel.connectedDeviceName} Connected',
                  style: textTheme.headline3,
                  textAlign: TextAlign.center,
                ),
              ),
              SizedBox(
                height: 100.0,
              ),
              RawMaterialButton(
                onPressed: () {
                  _viewmodel.disconnectDevice();
                  _mainViewModel.updateView(ViewCategory.HomePage);
                },
                elevation: 2.0,
                fillColor: Colors.white,
                child: Icon(
                  Icons.bluetooth_disabled,
                  size: 60.0,
                ),
                padding: EdgeInsets.all(15.0),
                shape: CircleBorder(),
              )
            ],
          ),
        ),
      ),
    );
  }
}

class DeviceInfo extends StatelessWidget {
  final BluetoothDevice device;
  final Function(Object) connectCallback;
  DeviceInfo(this.device, this.connectCallback);

  @override
  Widget build(BuildContext context) {
    return ListTile(
      leading: Icon(
        Icons.watch,
        size: 40.0,
        color: Color(0xE1085692),
      ),
      title: Text(
        '${device.name}',
        style: TextStyle(fontSize: 20),
      ),
      onTap: () {
        connectCallback(device);
      },
    );
  }
}
