import QtQuick 1.0

Rectangle {
    width: appW
    height: appH
    color:"slategrey"

    Rectangle{
        height: scaleY(75)
        width: scaleX(65)
        anchors.centerIn: parent

        Text {
            id: status
            text: qsTr("Choose Existing Orbiter")
            font.pixelSize: scaleY(4)
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }



}
