import 'package:flutter/material.dart';
import 'package:flutter_easyloading/flutter_easyloading.dart';
import 'package:location/location.dart';
import 'package:provider/provider.dart';
import 'package:vsm/business_logic/view_model/main_viewmodel.dart';
import 'package:vsm/services/service_locator.dart';
import 'package:vsm/user_interface/views/connection_view.dart';
import 'dart:io';

class MainView extends StatefulWidget {
  static const String id = "main_view";
  @override
  _MainViewState createState() => _MainViewState();
}

class _MainViewState extends State<MainView> {
  final MainViewModel _viewModel = serviceLocator<MainViewModel>();
  HomePage _homePage = HomePage();
  ConnectionView _connectionView;

  Widget getView(ViewCategory viewCategory) {
    switch (viewCategory) {
      case ViewCategory.HomePage:
        return _homePage;
      case ViewCategory.ConnectionPage:
        _connectionView = ConnectionView();
        return _connectionView;
      default:
        return Container(
          child: Center(
            child: Text('getView received invalid view type'),
          ),
        );
    }
  }

  @override
  Widget build(BuildContext context) {
    return ChangeNotifierProvider<MainViewModel>(
      create: (context) => _viewModel,
      child: Consumer<MainViewModel>(
        builder: (context, _viewmodel, child) => SafeArea(
          child: getView(_viewModel.viewCategory),
        ),
      ),
    );
  }
}

// ignore: must_be_immutable
class HomePage extends StatelessWidget {
  final Location location = new Location();

  bool _serviceEnabled;
  PermissionStatus _permissionGranted;
  //LocationData _locationData;

  Future<bool> checkLocationState() async {
    print('get location');
    _serviceEnabled = await location.serviceEnabled();
    if (!_serviceEnabled) {
      _serviceEnabled = await location.requestService();
      if (!_serviceEnabled) {
        return false;
      }
    }

    _permissionGranted = await location.hasPermission();
    if (_permissionGranted == PermissionStatus.denied) {
      _permissionGranted = await location.requestPermission();
      if (_permissionGranted != PermissionStatus.granted) {
        return false;
      }
    }

    //_locationData = await location.getLocation();

    print('is Location enabled : $_serviceEnabled');

    return _serviceEnabled;
  }

  @override
  Widget build(BuildContext context) {
    return Consumer<MainViewModel>(
      builder: (context, _viewmodel, child) => Scaffold(
        appBar: AppBar(
          title: Text('Health Wearable'),
          centerTitle: true,
        ),
        body: SafeArea(
          child: Center(
            child: _buildWaitingScanningBackground(_viewmodel),
          ),
        ),
      ),
    );
  }

  Widget _buildWaitingScanningBackground(MainViewModel viewModel) {
    return SizedBox.expand(
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        crossAxisAlignment: CrossAxisAlignment.center,
        children: <Widget>[
          _buildScanButton(viewModel),
          SizedBox(height: 20),
          new Text("Press the button to scan",
              style: TextStyle(fontSize: 22.0, fontWeight: FontWeight.bold)),
        ],
      ),
    );
  }

  Widget _buildScanButton(MainViewModel viewModel) {
    return Container(
      child: Ink.image(
        image: AssetImage("assets/images/bluetooth_icon.png"),
        fit: BoxFit.fill,
        child: InkWell(onTap: () async {
          if(Platform.isAndroid) {
            var isLocationON = await checkLocationState();
            if (isLocationON) {
              viewModel.updateView(ViewCategory.ConnectionPage);
            } else {
              EasyLoading.showInfo(
                  'Please turn ON the device location to proceed further',
                  duration: Duration(seconds: 3));
            }
          }
          else
            {
              viewModel.updateView(ViewCategory.ConnectionPage);

            }
        }),
      ),
      height: 250,
      width: 250,
    );
  }
}

class ScanButton extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Consumer<MainViewModel>(
      builder: (context, _viewmodel, child) => RawMaterialButton(
        onPressed: () {
          _viewmodel.updateView(ViewCategory.ConnectionPage);          
        },
        elevation: 2.0,
        fillColor: Colors.white,
        child: Icon(
          Icons.bluetooth,
          size: 60.0,
        ),
        padding: EdgeInsets.all(15.0),
        shape: CircleBorder(),
      ),
    );
  }
}
