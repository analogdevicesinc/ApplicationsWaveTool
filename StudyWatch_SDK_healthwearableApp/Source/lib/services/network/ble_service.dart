import 'dart:async';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:vsm/services/network/communication_service.dart';

class BleService implements CommunicationService {
  final FlutterBlue bleInstance = FlutterBlue.instance;
  final List<BluetoothDevice> devicesList = new List<BluetoothDevice>();
  final Guid serviceCharacteristic =
      Guid("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
  final Guid writecharacteristic = Guid("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
  final Guid readcharacteristic = Guid("6e400003-b5a3-f393-e0a9-e50e24dcca9e");
  List<BluetoothService> _services;
  StreamController<BluetoothDevice> _scanController;
  @override
  Stream<BluetoothDevice> get bleDeviceStream => _scanController?.stream;

  StreamController<List<int>> _rxController;
  @override
  Stream<List<int>> get dispatchStream => _rxController?.stream;

  StreamSubscription _readDataSubscription;
  StreamSubscription _scanDevicesSubscription;

  bool _isBleConnected = false;
  @override
  bool get deviceConnectionStatus => _isBleConnected;

  String _deviceName = "No device connected";
  @override
  String get deviceName => _deviceName;

  BluetoothDevice activeDevice;

  @override
  void Function(BluetoothDevice p1) scanResultCallback;

  void initControllers() {
    _scanController = StreamController<BluetoothDevice>();
    _scanDevicesSubscription = _scanController?.stream?.listen((event) {
      scanResultCallback?.call(event);
    }, onError: (error) {
      print(error);
    }, onDone: () async {
      print('OnDone called');
    });

    _rxController = new StreamController<List<int>>.broadcast();
  }

  void deinitControllers() {
    bleInstance.stopScan();
    _scanController?.close();
    _scanController = null;
    _rxController?.close();
    _rxController = null;

    _scanDevicesSubscription?.cancel();
    _scanDevicesSubscription = null;
    _readDataSubscription?.cancel();
    _readDataSubscription = null;
  }

  @override
  Future<bool> connectDevice(Object device) async {
    BluetoothDevice bleDevice;
    _isBleConnected = false;
    try {
      bleDevice = device as BluetoothDevice;
    } catch (e) {
      throw 'Invalid Bluetooth device';
    }

    try {
      if (bleDevice != null) {
        _isBleConnected = await _connectToDevice(bleDevice, 5);
      }
      if (!_isBleConnected) {
        return false;
      }
    } catch (e) {
      if (e.code != 'already_connected') {
        await bleDevice?.disconnect();
        print('Device Already connected : $e');
        return false;
      }
      print('Exception in connectDevice API : $e');
    } finally {
      if (_isBleConnected) {
        _services = await bleDevice?.discoverServices();
        for (BluetoothService service in _services) {
          var characteristics = service.characteristics;
          for (BluetoothCharacteristic c in characteristics) {
            if (c.uuid == writecharacteristic) {
              //print('\nble data instance  Write Characteristic\n');
            }
            if (c.uuid == readcharacteristic) {
              _setNotifyforReadBleData(c);
            }
          }
        }
        activeDevice = bleDevice;
        _deviceName = bleDevice.name;

        var mtu = await bleDevice.mtu.first;
        print('Default MTU $mtu');
        try {
          await bleDevice.requestMtu(247);
        } catch (e) {
          print('requestMtu Failed : $e');
        }
        print('After set MTU $mtu');
      }
    }

    return _isBleConnected;
  }

  Future<bool> _connectToDevice(BluetoothDevice device, int timeout) async {
    Future<bool> returnValue;
    await device.connect(autoConnect: false).timeout(Duration(seconds: timeout),
        onTimeout: () {
      print('timeout occured');
      returnValue = Future.value(false);
      device?.disconnect();
    }).then((data) {
      if (returnValue == null) {
        print('connection successful');
        returnValue = Future.value(true);
      }
    });

    return returnValue;
  }

  @override
  Future<bool> disconnectDevice() async {
    if (activeDevice != null) {
      await activeDevice.disconnect();
      deinitControllers();
      _isBleConnected = false;
      activeDevice = null;
      devicesList.clear();
      _deviceName = "No device connected";
    } else {
      print('activeDevice is null in bleService');
    }
    return true;
  }

  @override
  Future scanDevices() async {
    if (!_isBleConnected) {
      bleInstance.stopScan();
      devicesList.clear();

      initControllers();

      print('Listening devices...');
      _scanDevicesSubscription =
          bleInstance.scanResults.listen((List<ScanResult> results) {
        for (ScanResult result in results) {
          _addDeviceTolist(result.device);
        }
      });

      print('Scanning...');
      bleInstance.startScan(withServices: [serviceCharacteristic]);
    }
  }

  @override
  Future stopScanningDevices() async {
    print('stopping scan');
    await bleInstance.stopScan();
    devicesList.clear();
    _scanDevicesSubscription?.cancel();
    _scanDevicesSubscription = null;
  }

  void _addDeviceTolist(final BluetoothDevice device) {
    if (!devicesList.contains(device)) {
      if (_scanController?.hasListener == true) {
        if (device.name != '') {
          devicesList.add(device);
          _scanController.add(device);
          print('Adding ${device.name}');
        } else {
          //print('device.name is empty');
        }
      } else {
        //print('_controller has no listener');
      }
    } else {
      print('${device.name} already in list');
    }
  }

  void _setNotifyforReadBleData(BluetoothCharacteristic charc) async {
    await charc.setNotifyValue(true);
    _readDataSubscription = charc.value.listen((value) {
      print('r:$value');
      if (_rxController.hasListener) {
        _rxController.add(value);
      } else {
        //print('rxListner not registered');
      }
    });
  }

  @override
  Future writeData(List<int> value) async {
    for (BluetoothService service in _services) {
      var characteristics = service.characteristics;
      for (BluetoothCharacteristic c in characteristics) {
        if (c.uuid == writecharacteristic) {
          await c.write(value, withoutResponse: true);
        }
      }
    }
  }
}
