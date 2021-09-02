import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:responsive_framework/responsive_framework.dart';
import 'package:vsm/services/network/communication_service.dart';
import 'package:vsm/services/sdk_lib/sdk_service.dart';
import 'package:vsm/services/service_locator.dart';
import 'package:vsm/user_interface/views/chart_view.dart';
import 'package:vsm/user_interface/views/dashboard_view.dart';
import 'package:curved_navigation_bar/curved_navigation_bar.dart';
import 'package:vsm/user_interface/views/logging_view.dart';
import 'package:vsm/user_interface/views/main_view.dart';
import 'package:vsm/user_interface/views/settings_view.dart';
import 'package:vsm/user_interface/views/theme/app_theme.dart';
import 'package:vsm/user_interface/views/theme/theme_provider.dart';
import 'package:flutter_easyloading/flutter_easyloading.dart';
import 'package:bot_toast/bot_toast.dart';
import 'package:vsm/user_interface/widgets/app_retain_widget.dart';

import 'business_logic/view_model/main_viewmodel.dart';

const bool kReleaseMode = true;

void main() {
  setupServiceLocator();
  runApp(StarterApp());
  // DevicePreview(
  //   enabled: !kReleaseMode,
  //   plugins: [
  //     const ScreenshotPlugin(),
  //   ],
  //   builder: (context) => StarterApp(),
  // ),
}

class StarterApp extends StatefulWidget {
  const StarterApp();

  @override
  _StarterAppState createState() => _StarterAppState();
}

class _StarterAppState extends State<StarterApp> {
  ThemeProvider themeProvider = ThemeProvider();

  Widget responsiveFrameworkSetup(BuildContext context, Widget child) {
    child = ResponsiveWrapper.builder(
      BouncingScrollWrapper.builder(context, child),
      maxWidth: 1200,
      minWidth: 450,
      defaultScale: true,
      breakpoints: [
        ResponsiveBreakpoint.resize(450, name: MOBILE),
        ResponsiveBreakpoint.autoScale(800, name: TABLET),
        ResponsiveBreakpoint.autoScale(1000, name: TABLET),
        ResponsiveBreakpoint.resize(1200, name: DESKTOP),
        ResponsiveBreakpoint.autoScale(2460, name: "4K"),
      ],
      background: Container(color: Color(0xFFF5F5F5)),
    );
    return child;
  }

  @override
  void initState() {
    super.initState();
    getCurrentAppTheme();
  }

  void getCurrentAppTheme() async {
    themeProvider.isDarkThemeEnabled =
        await themeProvider.darkThemePreference.getTheme();
  }

  @override
  Widget build(BuildContext context) {
    return ChangeNotifierProvider<ThemeProvider>(
      create: (context) => themeProvider,
      child: Consumer<ThemeProvider>(builder: (context, themeProvider, child) {
        return MaterialApp(
          builder: EasyLoading.init(builder: responsiveFrameworkSetup),
          navigatorObservers: [BotToastNavigatorObserver()],
          theme: AppTheme.lightTheme,
          darkTheme: AppTheme.darkTheme,
          themeMode: themeProvider.isDarkThemeEnabled
              ? ThemeMode.dark
              : ThemeMode.light,
          home: AppRetainWidget(
            child: MainView(),
          ),
          debugShowCheckedModeBanner: false,
        );
      }),
    );
  }
}

class AppNavigator extends StatefulWidget {
  @override
  _AppNavigatorState createState() => _AppNavigatorState();
}

class _AppNavigatorState extends State<AppNavigator> {
  final SdkWrapperService _sdkService = serviceLocator<SdkWrapperService>();
  final CommunicationService bledatainstance =
      serviceLocator<CommunicationService>();
  int _currentIndex = 0;
  GlobalKey _bottomNavigationKey = GlobalKey();
  final DashboardPortrait _dashboard = DashboardPortrait();
  final ChartView _chartView = ChartView();
  final LoggingView _loggingView = LoggingView();
  final SettingsView _settingView = SettingsView();
  Widget _showPage;

  @override
  void initState() {
    super.initState();
    print('Homepage init');
    EasyLoading.show(
      status: 'Initializing...',
      maskType: EasyLoadingMaskType.black,
    );

    EasyLoading.addStatusCallback((status) {
      print('EasyLoading Status $status');
    });

    _showPage = _dashboard;

    _sdkService.initializeGen3SDK();
  }

  @override
  void deactivate() {
    print('Homepage deactivate');
    super.deactivate();
  }

  @override
  void dispose() {
    super.dispose();
  }

  Widget switchPage(int pageIndex) {
    switch (pageIndex) {
      case 0:
        return _dashboard;
      case 1:
        return _chartView;
      case 2:
        return _loggingView;
      case 3:
        return _settingView;
      default:
        return Text('wrong index received');
    }
  }

  @override
  Widget build(BuildContext context) {
    print('On build');
    var colorScheme = Theme.of(context).colorScheme;
    return Consumer<MainViewModel>(
      builder: (context, _mainViewModel, child) => Scaffold(
        bottomNavigationBar: CurvedNavigationBar(
          key: _bottomNavigationKey,
          index: _currentIndex,
          height: 50.0,
          items: <Widget>[
            Icon(Icons.dashboard, size: 30, color: Colors.white),
            Icon(Icons.stacked_line_chart, size: 30, color: Colors.white),
            Icon(Icons.insert_drive_file, size: 30, color: Colors.white),
            Icon(Icons.settings, size: 30, color: Colors.white),
          ],
          color: colorScheme.primary,
          buttonBackgroundColor: colorScheme.primary,
          backgroundColor: colorScheme.surface,
          animationCurve: Curves.easeInOut,
          animationDuration: Duration(milliseconds: 600),
          onTap: (tappedIndex) {
            setState(() {
              _showPage = switchPage(tappedIndex);
            });
          },
        ),
        body: Container(
          color: colorScheme.primary,
          child: Center(
            child: _showPage,
          ),
        ),
      ),
    );
  }
}
