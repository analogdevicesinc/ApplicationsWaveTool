import 'package:flutter/material.dart';

enum ViewCategory {
  HomePage,
  ConnectionPage,
  AppNavigatorPage,
}

class MainViewModel extends ChangeNotifier {
  ViewCategory _viewCategory = ViewCategory.HomePage;
  ViewCategory get viewCategory => _viewCategory;

  void updateView(ViewCategory view) {
    _viewCategory = view;
    notifyListeners();
  }
}
