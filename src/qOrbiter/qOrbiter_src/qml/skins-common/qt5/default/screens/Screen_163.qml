import QtQuick 2.2
import "../components"
/*!
  Device Reload Screen
  */
StyledScreen{
  id:new_device_reload

  StyledText{
          text: qsTr("I finished installing the software for your new devices. You will need to restart the router before you can use them.")
          font.pointSize: 18
          anchors.centerIn: parent
          font.bold: true
      }

}


