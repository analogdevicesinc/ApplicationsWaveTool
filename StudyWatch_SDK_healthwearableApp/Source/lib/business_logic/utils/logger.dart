/*
Logger is not supported for this project now.
*/

import 'package:logger/logger.dart';

class SimpleLogPrinter extends LogPrinter {
  final String className;
  SimpleLogPrinter(this.className);

  @override
  List<String> log(LogEvent event) {
    var color = PrettyPrinter.levelColors[event.level];
    var emoji = PrettyPrinter.levelEmojis[event.level];
    return [color('$emoji $className - ${event.message}')];
  }
}

Logger getLogger(String className) {
  return Logger(printer: SimpleLogPrinter(className));
}

// var logger = Logger(
//   printer: PrettyPrinter(),
// );

// var loggerNoStack = Logger(
//   printer: PrettyPrinter(methodCount: 0),
// );

void main() {
  print(
      'Run with either `dart example/main.dart` or `dart --enable-asserts example/main.dart`.');
  demo();
}

void demo() {
  final logger = getLogger('PostService');

  logger.d('Log message with 2 methods');

  logger.i('Info message');

  logger.w('Just a warning!');

  logger.e('Error! Something bad happened', 'Test Error');

  logger.v({'key': 5, 'value': 'something'});
}
