import QtQuick 1.0


Rectangle {
    width: 360
    height: 360
    color: "black"
    opacity: 0.75
    SwitchButton {}
    Text {
        id: switchpanel
        text: "DCE / QML Light Switch Panel"
        color: "white"
        anchors.centerIn: parent
    }
    MouseArea {
        height: 50; width: 360;
        anchors.bottom: switchpanel.bottom
        onClicked: {
            Qt.quit();
        }
    }
}
