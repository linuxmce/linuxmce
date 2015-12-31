import QtQuick 2.2


Rectangle {
    width: manager.appWidth
    height: manager.appHeight

    Text {
        id: label
        text: qsTr("Regenerating your Configuration data...")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        font.bold: true
        font.pixelSize: scaleY(3)
    }

}
