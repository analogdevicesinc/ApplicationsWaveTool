import 'dart:async';

import 'package:flutter_blue/flutter_blue.dart';

abstract class CommunicationService {
  bool get deviceConnectionStatus;
  String get deviceName;
  Future<bool> connectDevice(Object device);
  Future<bool> disconnectDevice();
  Future scanDevices();
  Future stopScanningDevices();
  Future writeData(List<int> data);
  Stream<List<int>> get dispatchStream;
  Stream<BluetoothDevice> get bleDeviceStream;
  void Function(BluetoothDevice p1) scanResultCallback;
}
