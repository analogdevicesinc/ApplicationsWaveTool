import 'package:mp_chart/mp/controller/line_chart_controller.dart';
import 'package:vsm/business_logic/utils/enumerations.dart';

abstract class ChartService {
  void initChartController(Sensors sensor);
  void deinitChartController(Sensors sensor);
  void initChartLineItems(Sensors sensor);
  void deinitChartLineItems(Sensors sensor);
  void updateChartValues(Sensors sensor, Object data);
  LineChartController getChartController(Sensors sensor);
}
