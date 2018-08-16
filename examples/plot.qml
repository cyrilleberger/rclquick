// Example of use: ROS_ARGUMENTS="data:=/pressure _field:=fluid_pressure"  qmlscene plot.qml

import Rcl 1.0
import QtQuick 2.0
import QtQuick.Window 2.0
import Rcl.Controls.Charts 1.0
  
Window {
  width: 512
  height: 512
  visible: true
  
  LinePlot
  {
    anchors.fill: parent
    topics: [ { topicName: "/data", field: Ros.getParam("~field") } ]
  }
}
