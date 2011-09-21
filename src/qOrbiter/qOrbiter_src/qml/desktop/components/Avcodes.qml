import QtQuick 1.0

Rectangle {
    id:avcodes
    width: scaleX(50)
    height: scaleY(50)
    radius: 10
    border.color: "black"
    border.width: 2
    color: "aliceblue"
    anchors.centerIn: parent

    Text {
        id: avcodeslabel
        text: qsTr("These are the devices in use. Touch a device name to send it a command, or touch the other cells to resend inputs or jump to advanced")
        width: parent.width
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        height: scaleY(3)
        font.pixelSize: scaleY(1)
    }

    Rectangle
    {

        id:codelistrect
        height: scaleY(45)
        width: scaleX(45)
        color: "#628BC7"
        anchors.top: avcodeslabel.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        border.color: "aliceblue"
        border.width: 2

        Text {
            id: blah
            text: qsTr("List model grid goes here")
        }
    }
}
