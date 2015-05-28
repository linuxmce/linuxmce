import QtQuick 2.2
import "../components"
import "../."
Rectangle {
    id:cmdEntry
    height: Style.appButtonHeight
    width: cmdView.width
    color: "black"
    border.color: "white"
    border.width: 1
    radius:5
    function sendFanCmd(param){
        sendCommand(cmdNumber, [{'paramno': 5, 'val': param }]);
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
            label: "Auto"
            onActivated: sendTempCmd("0");
        }
        StyledButton {
            anchors.verticalCenter: parent.verticalCenter
            label: "On"
            onActivated: sendTempCmd("1");
        }
    }
    
}
