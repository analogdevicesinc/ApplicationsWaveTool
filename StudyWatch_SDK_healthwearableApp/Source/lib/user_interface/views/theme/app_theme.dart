import 'package:flutter/material.dart';

class AppTheme {
  AppTheme._();
  static Color _iconColor = Colors.blueAccent.shade200;
  static const Color _lightBGColor = Color(0XE1F2F2f7);
  static const Color _lightSurfaceColor = Colors.white;
  static const Color _lightPrimaryColor = Color(0xFF165A9E);
  static const Color _lightPrimaryVariantColor = Color(0xFF07385e);
  static const Color _lightSecondaryColor = Color(0xFF03C6FB);
  static const Color _lightOnPrimaryColor = Colors.black;

  static const Color _darkBGColor = Color(0xFF1D3046);
  static const Color _darkSurfaceColor = Color(0xFF0A1521);
  static const Color _darkPrimaryColor = Color(0xFF07385e);
  //static const Color _darkPrimaryVariantColor = Colors.black;
  static const Color _darkSecondaryColor = Color(0xFF03C6FB);
  static const Color _darkOnPrimaryColor = Colors.white;
  static const Color cardColor = Colors.white;

  static final ThemeData lightTheme = ThemeData(
    scaffoldBackgroundColor: _lightSurfaceColor,
    appBarTheme: AppBarTheme(
      color: _lightPrimaryColor,
      iconTheme: IconThemeData(color: _lightOnPrimaryColor),
    ),
    secondaryHeaderColor: Colors.lightBlue[900],
    cardColor: _lightBGColor,
    colorScheme: ColorScheme.light(
      background: _lightBGColor,
      surface: _lightSurfaceColor,
      primary: _lightPrimaryColor,
      primaryVariant: _lightPrimaryVariantColor,
      secondary: _lightSecondaryColor,
      onPrimary: _lightOnPrimaryColor,
    ),
    iconTheme: IconThemeData(
      color: _iconColor,
    ),
    inputDecorationTheme: InputDecorationTheme(
      labelStyle: TextStyle(color: _lightPrimaryColor), // style for labels
    ),
    textTheme: _lightTextTheme,
  );

  static final ThemeData darkTheme = ThemeData(
    scaffoldBackgroundColor: _darkSurfaceColor,
    appBarTheme: AppBarTheme(
      color: _darkPrimaryColor,
      iconTheme: IconThemeData(color: _darkOnPrimaryColor),
    ),
    secondaryHeaderColor: Color(0xFF056DAC),
    cardColor: _darkBGColor,
    colorScheme: ColorScheme.dark(
      background: _darkBGColor,
      surface: _darkSurfaceColor,
      primary: _darkPrimaryColor,
      //primaryVariant: _darkPrimaryVariantColor,
      secondary: _darkSecondaryColor,
      onPrimary: _darkOnPrimaryColor,
    ),
    iconTheme: IconThemeData(
      color: _iconColor,
    ),
    inputDecorationTheme: InputDecorationTheme(
      labelStyle: TextStyle(color: _darkSecondaryColor), // style for labels
    ),
    textTheme: _darkTextTheme,
  );

  static final TextTheme _lightTextTheme = TextTheme(
    headline1: _lightScreenHeadingTextStyle,
    headline2: _lightNumberTextStyle,
    headline3: _lightUnitTextStyle,
    headline4: _lightScreenLoadConfigTextStyle,
    headline6: TextStyle(color: Colors.white),
    bodyText1: _lightScreenTaskNameTextStyle,
    bodyText2: _lightScreenTaskDurationTextStyle,
  );

  static final TextTheme _darkTextTheme = TextTheme(
    headline1: _darkScreenHeadingTextStyle,
    headline2: _darkNumberTextStyle,
    headline3: _darkUnitTextStyle,
    headline4: _darkScreenLoadConfigTextStyle,
    bodyText1: _darkScreenTaskNameTextStyle,
    bodyText2: _darkScreenTaskDurationTextStyle,
  );

  static final TextStyle _lightScreenHeadingTextStyle =
      TextStyle(fontSize: 48.0, color: _lightOnPrimaryColor);
  static final TextStyle _lightScreenTaskNameTextStyle =
      TextStyle(fontSize: 26.0, color: _lightOnPrimaryColor);
  static final TextStyle _lightScreenLoadConfigTextStyle =
      TextStyle(fontSize: 16.0, color: _lightOnPrimaryColor);
  static final TextStyle _lightScreenTaskDurationTextStyle =
      TextStyle(fontSize: 16.0, color: Colors.grey);

  static final TextStyle _lightNumberTextStyle = TextStyle(
    fontSize: 36.0,
    fontWeight: FontWeight.w900,
    color: Color(0xFF085692),
  );

  static final TextStyle _darkNumberTextStyle =
      _lightNumberTextStyle.copyWith(color: _darkSecondaryColor);

  static final TextStyle _lightUnitTextStyle = TextStyle(
    fontSize: 26.0,
    color: Color(0xE1085692),
  );

  static final TextStyle _darkUnitTextStyle =
      _lightUnitTextStyle.copyWith(color: Color(0xAF03C6FB));

  static final TextStyle _darkScreenHeadingTextStyle =
      _lightScreenHeadingTextStyle.copyWith(color: _darkOnPrimaryColor);

  static final TextStyle _darkScreenTaskNameTextStyle =
      _lightScreenTaskNameTextStyle.copyWith(color: _darkOnPrimaryColor);

  static final TextStyle _darkScreenLoadConfigTextStyle =
      _lightScreenLoadConfigTextStyle.copyWith(color: _darkOnPrimaryColor);

  static final TextStyle _darkScreenTaskDurationTextStyle =
      _lightScreenTaskDurationTextStyle;
}
