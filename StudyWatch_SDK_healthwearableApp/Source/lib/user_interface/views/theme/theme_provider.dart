import 'package:flutter/material.dart';
import 'package:vsm/user_interface/views/theme/theme_preferences.dart';

class ThemeProvider extends ChangeNotifier {
  DarkThemePreference darkThemePreference = DarkThemePreference();

  bool _isDarkThemeEnabled = false;
  bool get isDarkThemeEnabled => _isDarkThemeEnabled;

  set isDarkThemeEnabled(bool value) {
    _isDarkThemeEnabled = value;
    darkThemePreference.setDarkTheme(value);
    notifyListeners();
  }
}
