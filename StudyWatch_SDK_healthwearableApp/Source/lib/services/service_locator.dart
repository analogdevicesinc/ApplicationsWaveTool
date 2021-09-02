import 'package:get_it/get_it.dart';
import 'package:vsm/business_logic/model/settings_model.dart';
import 'package:vsm/business_logic/view_model/chart_viewmodel.dart';
import 'package:vsm/business_logic/view_model/connection_viewmodel.dart';
import 'package:vsm/business_logic/view_model/dashboard_viewmodel.dart';
import 'package:vsm/business_logic/view_model/logging_viewmodel.dart';
import 'package:vsm/business_logic/view_model/main_viewmodel.dart';
import 'package:vsm/business_logic/view_model/settings_viewmodel.dart';
import 'package:vsm/services/chart/mp_chart_manager.dart';
import 'package:vsm/services/network/communication_service.dart';
import 'package:vsm/services/sdk_lib/sdk_service.dart';
import 'chart/chart_service.dart';
import 'network/ble_service.dart';

// Using GetIt is a convenient way to provide services and view models
// anywhere we need them in the app.
GetIt serviceLocator = GetIt.instance;

void setupServiceLocator() {
  //Services
  serviceLocator
      .registerLazySingleton<CommunicationService>(() => BleService());
  serviceLocator.registerLazySingleton<ChartService>(() => MPChartManager());
  serviceLocator
      .registerLazySingleton<SdkWrapperService>(() => SdkWrapperService());

  // view models
  serviceLocator.registerFactory<MainViewModel>(() => MainViewModel());
  serviceLocator
      .registerFactory<ConnectionViewModel>(() => ConnectionViewModel());
  serviceLocator
      .registerFactory<DashboardViewModel>(() => DashboardViewModel());
  serviceLocator.registerFactory<ChartViewModel>(() => ChartViewModel());

  serviceLocator.registerFactory<LoggingViewModel>(() => LoggingViewModel());

  serviceLocator.registerFactory<SettingsViewModel>(() => SettingsViewModel());

  //Settings Model
  serviceLocator.registerLazySingleton<SettingsModel>(() => SettingsModel());
}
