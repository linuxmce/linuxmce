import QtQuick 2.0


Rectangle {
    width: appW
    height: appH

    Text {
        id: label
        text: qsTr("Regenerating your Configuration data...")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        font.bold: true
        font.pixelSize: scaleY(3)
    }

}
