import QtQuick 2.2
import "../components"
import "../."
Rectangle {
    id:cmdEntry
    height: Style.appButtonHeight
    width: parent.width
    color: "black"
    border.color: "white"
    border.width: 1
    radius:5
    StyledText {
        anchors.left: parent.left
        text: "On/Off"
        fontSize: Style.appFontSize_description
    }
    Row {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: childrenRect.width
        spacing: 10
        StyledButton {
            height: Style.appButtonHeight
            width: Style.appButtonWidth /2
            textSize: Style.appFontSize_description
            anchors.verticalCenter: parent.verticalCenter
            label:qsTr( "On")
            onActivated: sendCommand(cmdNumber, []);
        }
        StyledButton {
            anchors.verticalCenter: parent.verticalCenter
            height: Style.appButtonHeight
            fontSize: Style.appFontSize_description
            width: Style.appButtonWidth /2
            label: "Off"
            onActivated: sendCommand("193", []);
        }
    }
    
}
