import QtQuick 2.2
import "../components"
StyledScreen {
    screen: qsTr("Sensors Not Ready")
    id: snrScreen
    property string passwd
    property int mode
    Panel{
        headerTitle: screen

        StyledText {
            id: topText
            anchors.top: parent.headerRect.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
            textLabel: qsTr("Waiting for the following sensors:")
        }
        StyledText {
            anchors.top: topText.bottom
            anchors.bottom: btGrid.top
            anchors.horizontalCenter: parent.horizontalCenter
            textLabel: screenparams.getParam(163)
        }

        Grid {
            id: btGrid
            anchors.top: parent.verticalCenter
            anchors.bottom: parent.bottom
            anchors.margins: 20
            anchors.horizontalCenter: parent.horizontalCenter
            width: childrenRect.width
            rows: 2
            columns: 2
            spacing: 20

            StyledButton {
                label: qsTr("Bypass These Sensors and Arm Now")
                width: snrScreen.width / 2.2
                onActivated: manager.setHouseMode(screenparams.getParam(24), screenparams.getParam(25), "B")
            }
            StyledButton {
                label: qsTr("Try Again")
                width: snrScreen.width / 2.2
                onActivated: manager.setHouseMode(screenparams.getParam(24), screenparams.getParam(25), "R")
            }
            StyledButton {
                label: qsTr("Arm other sensors now, arm these when they clear")
                width: snrScreen.width / 2.2
                onActivated: manager.setHouseMode(screenparams.getParam(24), screenparams.getParam(25), "W")
            }
            StyledButton {
                label: qsTr("Cancel")
                width: snrScreen.width / 2.2
                onActivated: manager.goBacktoQScreen()

            }
        }
    }
}
