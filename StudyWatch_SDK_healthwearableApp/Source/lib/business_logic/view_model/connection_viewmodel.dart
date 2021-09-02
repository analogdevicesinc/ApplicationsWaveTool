import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:vsm/business_logic/utils/enumerations.dart';
import 'package:vsm/services/network/communication_service.dart';
import 'package:vsm/services/service_locator.dart';

class ConnectionViewModel extends ChangeNotifier {
  CommunicationService bledatainstance = serviceLocator<CommunicationService>();
  bool get isConnected => bledatainstance.deviceConnectionStatus;
  List<BluetoothDevice> deviceList = [];
  Stream<BluetoothDevice> get deviceStream => bledatainstance.bleDeviceStream;
  String get connectedDeviceName => bledatainstance.deviceName;
  bool isScanning = false;
  bool isBluetoothON = false;
  ConnectStatus _connectionStatus = ConnectStatus.Disconnected;
  ConnectStatus get connectionStatus => _connectionStatus;

  ConnectionViewModel() {
    bledatainstance = serviceLocator<CommunicationService>();
    bledatainstance.scanResultCallback = updateDeviceList;
    if (isConnected) {
      _connectionStatus = ConnectStatus.Connected;
    } else {
      _connectionStatus = ConnectStatus.Disconnected;
    }
    print('isDevice connected ${bledatainstance.deviceConnectionStatus}');
  }

  void connectDevice(Object device) async {
    print('connecting...');
    _connectionStatus = ConnectStatus.ConnectionInProgress;
    notifyListeners();

    await bledatainstance?.connectDevice(device);
    if (isConnected) {
      _connectionStatus = ConnectStatus.Connected;
      print('device connected');
    } else {
      print('Connection failed');
      deviceList.clear();
      _connectionStatus = ConnectStatus.ConnectionFailed;
    }
    notifyListeners();
  }

  void disconnectDevice() async {
    print('disconnecting...');
    var isdisconnected = await bledatainstance?.disconnectDevice();
    if (isdisconnected) {
      _connectionStatus = ConnectStatus.Disconnected;
      print('Device Disconnected');
    } else {
      print('Disconnection Failed');
      _connectionStatus = ConnectStatus.DisconnectionFailed;
    }
  }

  Future scanDevices() async {
    if (!isConnected && !isScanning) {
      print('calling scanDevices');
      isBluetoothON = true;
      deviceList.clear();
      _connectionStatus = ConnectStatus.ScanningInProgress;
      isScanning = true;
      await bledatainstance?.scanDevices();
      notifyListeners();
    }
  }

  void stopScan() {
    if (isScanning) {
      isBluetoothON = false;
      bledatainstance?.stopScanningDevices();
      isScanning = false;
    }
  }

  void updateDeviceList(BluetoothDevice newDevice) {
    if (deviceList.contains(newDevice) == false) {
      deviceList.add(newDevice);
      notifyListeners();
    }
  }
}
