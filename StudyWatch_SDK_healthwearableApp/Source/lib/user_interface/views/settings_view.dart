import 'dart:io';
import 'package:auto_size_text/auto_size_text.dart';
import 'package:file_picker/file_picker.dart';
import 'package:flutter/material.dart';
import 'package:card_settings/card_settings.dart';
import 'package:provider/provider.dart';
import 'package:vsm/business_logic/utils/enumerations.dart';
import 'package:vsm/business_logic/view_model/settings_viewmodel.dart';
import 'package:vsm/services/service_locator.dart';

class SettingsView extends StatefulWidget {
  static const String id = "settings_view";
  @override
  _SettingsViewState createState() => _SettingsViewState();
}

class _SettingsViewState extends State<SettingsView> {
  final SettingsViewModel _viewModel = serviceLocator<SettingsViewModel>();

  final GlobalKey<FormState> _formKey = GlobalKey<FormState>();
  AutovalidateMode _autoValidateMode = AutovalidateMode.onUserInteraction;
  final GlobalKey<FormState> _adpdFreqKey = GlobalKey<FormState>();
  final GlobalKey<FormState> _adxlFreqKey = GlobalKey<FormState>();
  final GlobalKey<FormState> _ecgFreqKey = GlobalKey<FormState>();

  final GlobalKey<FormState> _adpdFilterKey = GlobalKey<FormState>();
  final GlobalKey<FormState> _ecgFilterKey = GlobalKey<FormState>();

  final GlobalKey<FormState> _adpdTapCountKey = GlobalKey<FormState>();
  final GlobalKey<FormState> _ecgTapCountKey = GlobalKey<FormState>();

  final GlobalKey<FormState> _adpdDurationKey = GlobalKey<FormState>();
  final GlobalKey<FormState> _adxlDurationKey = GlobalKey<FormState>();
  final GlobalKey<FormState> _ecgDurationKey = GlobalKey<FormState>();
  final GlobalKey<FormState> _edaDurationKey = GlobalKey<FormState>();

  final FocusNode _adpdDurationNode = FocusNode();
  final FocusNode _adxldurationNode = FocusNode();
  final FocusNode _ecgDurationNode = FocusNode();
  final FocusNode _edaDurationNode = FocusNode();

  String adpdSampleRate = '50';
  String adxlSampleRate = '50';
  String ecgSampleRate = '300';

  String adpdFilter = 'FIR';
  String ecgFilter = 'BandPass';

  int adpdTapCount = 8;
  int ecgTapCount = 8;

  int adpdDuration = 3;
  int adxlDuration = 3;
  int ecgDuration = 3;
  int edaDuration = 3;

  var sampleRateOptions = <String>[
    '50 Hz',
    '100 Hz',
    '150 Hz',
    '200 Hz',
    '250 Hz',
    '300 Hz'
  ];
  var sampleRateValues = <String>['50', '100', '150', '200', '250', '300'];

  var durationOptions = <String>[
    '1 Sec',
    '2 Sec',
    '3 Sec',
    '4 Sec',
    '5 Sec',
  ];
  var durationValues = <String>['1', '2', '3', '4', '5'];

  var filterOptions = <String>[
    'Moving_Average',
    'BPF',
    'ButterWorth',
    'FIR',
    'IIR'
  ];

  bool loaded = false;
  String fileName = "No file Selected";

  @override
  void initState() {
    super.initState();
    setState(() => loaded = true);
  }

  @override
  Widget build(BuildContext context) {
    var colorScheme = Theme.of(context).colorScheme;
    var textTheme = Theme.of(context).textTheme;
    return ChangeNotifierProvider<SettingsViewModel>(
      create: (context) => _viewModel,
      child: Consumer<SettingsViewModel>(
        builder: (context, _viewmodel, child) => Scaffold(
          appBar: AppBar(
            backgroundColor: colorScheme.primary,
            title: Center(
              child: Text(
                'Settings',
                style: TextStyle(color: Colors.white),
                textAlign: TextAlign.center,
              ),
            ),
          ),
          body: loaded
              ? Form(
                  key: _formKey,
                  child: SingleChildScrollView(
                    child: Padding(
                      padding: const EdgeInsets.all(2.0),
                      child: Column(
                        crossAxisAlignment: CrossAxisAlignment.start,
                        children: [
                          Card(
                            elevation: 4,
                            shape: RoundedRectangleBorder(
                                borderRadius: BorderRadius.circular(8.0)),
                            margin: const EdgeInsets.symmetric(
                                vertical: 10.0, horizontal: 10.0),
                            child: Column(
                              children: [
                                Container(
                                    margin:
                                        EdgeInsets.symmetric(vertical: 12.0),
                                    child: Text(
                                      'Load DCFG',
                                      style: textTheme.bodyText1,
                                    )),
                                Container(
                                  margin: EdgeInsets.only(bottom: 12.0),
                                  child: AutoSizeText(
                                    'Selected FileName : ${_viewModel.loadDcfgFileName}',
                                    style: textTheme.headline4,
                                    maxLines: 1,
                                  ),
                                ),
                                Padding(
                                  padding: const EdgeInsets.only(bottom: 8.0),
                                  child: Row(
                                    mainAxisAlignment:
                                        MainAxisAlignment.spaceAround,
                                    children: [
                                      RaisedButton(
                                        color: colorScheme.secondary,
                                        child: Text(
                                          'Reset Config',
                                          style: textTheme.headline4,
                                        ),
                                        onPressed: _viewModel.resetLoadSettings,
                                      ),
                                      loadDcfg(colorScheme, textTheme),
                                    ],
                                  ),
                                )
                              ],
                            ),
                          ),
                          _buildPortraitLayout(),
                        ],
                      ),
                    ),
                  ),
                )
              : Center(child: CircularProgressIndicator()),
        ),
      ),
    );
  }

  Widget loadDcfg(ColorScheme _colorScheme, TextTheme _textTheme) {
    return RaisedButton(
      color: _colorScheme.secondary,
      child: Text(
        'Browse',
        style: _textTheme.headline4,
      ),
      onPressed: () async {
        print('Browse pressed');
        FilePickerResult result = await FilePicker.platform.pickFiles();

        if (result != null) {
          File file;
          file = File(result.files.single.path);
          print('File Path ${file.absolute}');
          _viewModel.loadDcfgFile = file;
          _viewModel.loadDcfgFileName = file.path.split("/").last;

          setState(() {});
        } else {
          print('User canceled the picker');
        }
      },
    );
  }

  CardSettings _buildPortraitLayout() {
    return CardSettings.sectioned(
      showMaterialonIOS: false,
      labelWidth: 150,
      contentAlign: TextAlign.right,
      cardless: false,
      children: <CardSettingsSection>[
        CardSettingsSection(
          header: CardSettingsHeader(
            label: 'SpO2',
          ),
          children: <CardSettingsWidget>[
            _buildCardSettingsDuration(SettingsType.ADPD),
          ],
        ),
        CardSettingsSection(
          header: CardSettingsHeader(
            label: 'PPG',
          ),
          children: <CardSettingsWidget>[
            _buildCardSettingsDuration(SettingsType.ADXL),
          ],
        ),
        CardSettingsSection(
          header: CardSettingsHeader(
            label: 'ECG',
          ),
          children: <CardSettingsWidget>[
            _buildCardSettingsDuration(SettingsType.ECG),
          ],
        ),
        CardSettingsSection(
          header: CardSettingsHeader(
            label: 'EDA',
          ),
          children: <CardSettingsWidget>[
            _buildCardSettingsDuration(SettingsType.EDA),
          ],
        ),
      ],
    );
  }

  CardSettingsListPicker _buildCardSettingsSampleRate(
      SettingsType settingsType) {
    return CardSettingsListPicker(
      key: getFreqKey(settingsType),
      label: 'Sample Rate',
      initialValue: getFrequency(settingsType),
      hintText: 'Select One',
      autovalidateMode: _autoValidateMode,
      options: sampleRateOptions,
      values: sampleRateValues,
      validator: (String value) {
        if (value == null || value.isEmpty)
          return 'You must pick a sample rate.';
        return null;
      },
      onSaved: (value) => setFrequency(settingsType, value),
      onChanged: (value) {
        setState(() {
          setFrequency(settingsType, value);
        });
      },
    );
  }

  CardSettingsFilePicker _buildCardSettingsFilePicker(
      SettingsType settingsType) {
    return CardSettingsFilePicker(
      maxThumbnailHeight: 100.0,
      showMaterialonIOS: true,
      unattachDialogTitle: "Do you want to cancel selection ?",
      unattachDialogConfirm: "Yes",
      unattachDialogCancel: "No",
      label: 'Load DCFG',
      fileType: FileType.custom,
      allowedExtensions: ['dcfg'],
      onChanged: (value) async {
        print('onchanged filepicker $value');
      },
    );
  }

  CardSettingsRadioPicker _buildCardSettingsFilter(SettingsType settingType) {
    return CardSettingsRadioPicker(
      key: getFilterKey(settingType),
      label: 'Filter',
      initialValue: getFilter(settingType),
      hintText: 'Select One',
      autovalidateMode: _autoValidateMode,
      options: filterOptions,
      validator: (String value) {
        if (value == null || value.isEmpty) return 'You must pick a filter.';
        return null;
      },
      onSaved: (value) => setFilter(settingType, value),
      onChanged: (value) {
        setState(() {
          setFilter(settingType, value);
        });
      },
    );
  }

  CardSettingsNumberPicker _buildCardSettingsTapCount(SettingsType settingType,
      {TextAlign labelAlign}) {
    return CardSettingsNumberPicker(
      key: getTapCountKey(settingType),
      label: 'Tap Count',
      labelAlign: labelAlign,
      initialValue: 8,
      min: 1,
      max: 128,
      stepInterval: 1,
      onSaved: (value) => setTapCount(settingType, value),
      onChanged: (value) {
        setState(() {
          setTapCount(settingType, value);
        });
      },
    );
  }

  CardSettingsInt _buildCardSettingsDuration1(SettingsType settingType) {
    return CardSettingsInt(
      key: getDurationKey(settingType),
      label: 'Duration',
      unitLabel: '(sec)',
      initialValue: getDuration(settingType),
      autovalidateMode: _autoValidateMode,
      validator: (value) {
        print('Duration : $value');
        if (value == null) {
          return 'duration shouldn\'t be empty';
        }
        return null;
      },
      onSaved: (value) => setDuration(settingType, value),
      onChanged: (value) {
        setState(() {
          setDuration(settingType, value);
        });
      },
    );
  }

  CardSettingsNumberPicker _buildCardSettingsDuration(SettingsType settingType,
      {TextAlign labelAlign}) {
    return CardSettingsNumberPicker(
      key: getDurationKey(settingType),
      label: 'X-Axis Range (Sec)',
      labelAlign: labelAlign,
      initialValue: getDuration(settingType),
      min: 1,
      max: 5,
      stepInterval: 1,
      onSaved: (value) => setDuration(settingType, value),
      onChanged: (value) {
        setState(() {
          setDuration(settingType, value);
        });
      },
    );
  }

  void showErrors(BuildContext context) {
    showDialog<String>(
      context: context,
      builder: (BuildContext context) {
        return AlertDialog(
          title: Text('Form has validation errors'),
          content: Text('Please fix all errors before submitting the form.'),
          actions: <Widget>[
            FlatButton(
              child: Text("Close"),
              onPressed: () {
                Navigator.of(context).pop();
              },
            ),
          ],
        );
      },
    );
  }

  FocusNode getFocusNode(SettingsType settingType) {
    switch (settingType) {
      case SettingsType.ADPD:
        return _adpdDurationNode;
        break;
      case SettingsType.ADXL:
        return _adxldurationNode;
        break;
      case SettingsType.ECG:
        return _ecgDurationNode;
        break;
      case SettingsType.EDA:
        return _edaDurationNode;
        break;
      default:
        return _adpdDurationNode;
    }
  }

  Key getFreqKey(SettingsType settingType) {
    switch (settingType) {
      case SettingsType.ADPD:
        return _adpdFreqKey;
        break;
      case SettingsType.ADXL:
        return _adxlFreqKey;
        break;
      case SettingsType.ECG:
        return _ecgFreqKey;
        break;
      default:
        return _adpdFreqKey;
    }
  }

  Key getFilterKey(SettingsType settingType) {
    switch (settingType) {
      case SettingsType.ADPD:
        return _adpdFilterKey;
        break;
      case SettingsType.ECG:
        return _ecgFilterKey;
        break;
      default:
        return _adpdFilterKey;
    }
  }

  Key getTapCountKey(SettingsType settingType) {
    switch (settingType) {
      case SettingsType.ADPD:
        return _adpdTapCountKey;
        break;
      case SettingsType.ECG:
        return _ecgTapCountKey;
        break;
      default:
        return _adpdTapCountKey;
    }
  }

  Key getDurationKey(SettingsType settingType) {
    switch (settingType) {
      case SettingsType.ADPD:
        return _adpdDurationKey;
        break;
      case SettingsType.ADXL:
        return _adxlDurationKey;
        break;
      case SettingsType.ECG:
        return _ecgDurationKey;
        break;
      case SettingsType.EDA:
        return _edaDurationKey;
        break;
      default:
        return _adpdDurationKey;
    }
  }

  String getFrequency(SettingsType settingType) {
    switch (settingType) {
      case SettingsType.ADPD:
        return _viewModel.adpdSampleRate;
      case SettingsType.ADXL:
        return _viewModel.adxlSampleRate;
      case SettingsType.ECG:
        return _viewModel.ecgSampleRate;
      default:
        return '50';
    }
  }

  void setFrequency(SettingsType settingType, String value) {
    switch (settingType) {
      case SettingsType.ADPD:
        _viewModel.adpdSampleRate = value;
        break;
      case SettingsType.ADXL:
        _viewModel.adxlSampleRate = value;
        break;
      case SettingsType.ECG:
        _viewModel.ecgSampleRate = value;
        break;
      default:
        break;
    }
  }

  String getFilter(SettingsType settingType) {
    switch (settingType) {
      case SettingsType.ADPD:
        return _viewModel.adpdFilter;
      case SettingsType.ECG:
        return _viewModel.ecgFilter;
      default:
        return 'None';
    }
  }

  void setFilter(SettingsType settingType, String value) {
    switch (settingType) {
      case SettingsType.ADPD:
        _viewModel.adpdFilter = value;
        break;
      case SettingsType.ECG:
        _viewModel.ecgFilter = value;
        break;
      default:
        break;
    }
  }

  int getTapCount(SettingsType settingType) {
    switch (settingType) {
      case SettingsType.ADPD:
        return _viewModel.adpdTapCount;
      case SettingsType.ECG:
        return _viewModel.ecgTapCount;
      default:
        return 0;
    }
  }

  void setTapCount(SettingsType settingType, int value) {
    switch (settingType) {
      case SettingsType.ADPD:
        _viewModel.adpdTapCount = value;
        break;
      case SettingsType.ECG:
        _viewModel.ecgTapCount = value;
        break;
      default:
        break;
    }
  }

  int getDuration(SettingsType settingType) {
    switch (settingType) {
      case SettingsType.ADPD:
        return _viewModel.adpdDuration;
      case SettingsType.ADXL:
        return _viewModel.ppgDuration;
      case SettingsType.ECG:
        return _viewModel.ecgDuration;
      case SettingsType.EDA:
        return _viewModel.edaDuration;
      default:
        return 0;
    }
  }

  void setDuration(SettingsType settingType, int value) {
    switch (settingType) {
      case SettingsType.ADPD:
        _viewModel.adpdDuration = value;
        break;
      case SettingsType.ADXL:
        _viewModel.ppgDuration = value;
        break;
      case SettingsType.ECG:
        _viewModel.ecgDuration = value;
        break;
      case SettingsType.EDA:
        _viewModel.edaDuration = value;
        break;
      default:
        break;
    }
  }
}
