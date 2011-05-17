import QtQuick 1.0


Rectangle {
    width: 360
    height: 360
    color: "black"
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
