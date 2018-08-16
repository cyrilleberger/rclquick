import QtQuick 2.0
import QtQuick.Window 2.0
import QtLocation 5.6
import QtPositioning 5.6
import Rcl 1.0
import Rcl.Controls.Maps 1.0

Window {
  width: 512
  height: 512
  visible: true

  GpsPlot
  {
    anchors.fill: parent
    topicName: "gps"
  }
}
