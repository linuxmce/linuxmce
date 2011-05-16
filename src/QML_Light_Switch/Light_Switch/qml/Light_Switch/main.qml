import QtQuick 1.0

Rectangle {
    width: 360
    height: 360
    Text {
        text: "QML Light Switch Panel"
        anchors.centerIn: parent
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }
}
