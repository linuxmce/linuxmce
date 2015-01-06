
import QtQuick 2.2
import "../components"
/*!
  Device Reload Screen
  */
StyledScreen{
    id:message

    Panel{
        headerTitle: qsTr("System Message")
        StyledText{
            text: screenparams.getParam(9)
            font.pointSize: largeFontSize
            anchors.centerIn: parent
            font.bold: true
        }
    }

}
