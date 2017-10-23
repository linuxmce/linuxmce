import QtQuick 2.2
import "../components"
import "../."
Rectangle {
    id:cmdEntry
    height: appStyle.appButtonHeight
    width: cmdView.width
    color: "black"
    border.color: "white"
    border.width: 1
    radius:5
    function sendCmdHeatCool(param){
        sendCommand(cmdNumber, [{'paramno': 8, 'val': param }]);
    }
    StyledText {
        anchors.left: parent.left
        text: cmdName
    }
    Row {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: childrenRect.width
        spacing: 10
        StyledButton {
            anchors.verticalCenter: parent.verticalCenter
            label: "Heat"
            onActivated: sendCmdHeatCool("H");
        }
        StyledButton {
            anchors.verticalCenter: parent.verticalCenter
            label: "Cool"
            onActivated: sendCmdHeatCool("C");
        }
        StyledButton {
            anchors.verticalCenter: parent.verticalCenter
            label: "Auto"
            onActivated: sendCmdHeatCool("A");
        }
    }
}
