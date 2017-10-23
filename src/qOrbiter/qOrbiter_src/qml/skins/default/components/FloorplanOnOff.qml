import QtQuick 2.2
import "../components"
import "../."
Rectangle {
    id:cmdEntry
    height: appStyle.appButtonHeight
    width: parent.width
    color: "black"
    border.color: "white"
    border.width: 1
    radius:5
    StyledText {
        anchors.left: parent.left
        text: "On/Off"
        fontSize: appStyle.appFontSize_description
    }
    Row {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: childrenRect.width
        spacing: 10
        StyledButton {
            height: appStyle.appButtonHeight
            width: appStyle.appButtonWidth /2
            textSize: appStyle.appFontSize_description
            anchors.verticalCenter: parent.verticalCenter
            label:qsTr( "On")
            onActivated: sendCommand(cmdNumber, []);
        }
        StyledButton {
            anchors.verticalCenter: parent.verticalCenter
            height: appStyle.appButtonHeight
            fontSize: appStyle.appFontSize_description
            width: appStyle.appButtonWidth /2
            label: "Off"
            onActivated: sendCommand("193", []);
        }
    }
    
}
