import QtQuick 2.0
import "../components"
/*
  screen params
  ------------------------------
Map  Param:  10  Value:
Map  Param:  159  Value:  screen
Map  Param:  165  Value:  housemode
Map  Param:  166  Value:  HouseModeTime
Map  Param:  167  Value:  Exit Delay
Map  Param:  168  Value: Alerts
Map  Param:  251  Value:  0
Map  Param:  252  Value:  0
Map  Param:  253  Value:  0
  */

Stage{
    id:securitystatus

    Rectangle{
        id:mainrect
        height: manager.appHeight
        width: manager.appWidth
        color: style.lighthighlight
        anchors.centerIn: securitystatus
        Column{
            anchors.centerIn: parent

            StyledText {
                id: modetime

                text: qsTr("Change at time: ")+ screenparams.getParam(166)
                font.family: "Droid Sans"
                font.bold: false
                font.pointSize: 18
            }
            StyledText {
                id: statussec

                text: qsTr("House Mode Changed to: ")+ screenparams.getParam(165)
                font.family: "Droid Sans"
                font.bold: false
                font.pointSize: 18
            }
            StyledText {
                id: exitdelay

                text: qsTr("Exit Delay: ")+ screenparams.getParam(167)
                font.family: "Droid Sans"
                font.bold: false
                font.pointSize: 18
            }
            StyledText {
                id: alerts

                text: qsTr("Alerts: ")+ screenparams.getParam(168)
                font.family: "Droid Sans"
                font.bold: false
                font.pointSize: 18
            }
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}


