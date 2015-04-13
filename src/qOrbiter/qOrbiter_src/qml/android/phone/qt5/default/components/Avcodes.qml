import QtQuick 2.2

Rectangle {
    id:avcodes
    width: scaleX(75)
    height: scaleY(65)
    radius: 10
    border.color: "black"
    border.width: 2
    color: "aliceblue"
    anchors.centerIn: parent
    Component.onCompleted: manager.showAdvancedButtons()
Column{
    height: childrenRect.height
    width: avcodes.width* .85
    anchors.horizontalCenter: parent.horizontalCenter

    spacing: scaleY(1)
    Text {
        id: avcodeslabel
        text: qsTr("These are the devices in use. Touch a device name to send it a command, or touch the other cells to resend inputs or jump to advanced")
        width: parent.width * .75
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        font.pixelSize: scaleY(2)
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Rectangle
    {
        id:codelistrect
        height: scaleY(55)
        width: avcodes.width *.85
        color: "#628BC7"
        anchors.horizontalCenter: parent.horizontalCenter
        border.color: "aliceblue"
        border.width: 2
        radius: 20

        Text {
            id: blah
            text: qsTr("List model grid goes here")
        }
    }
}

    ButtonSq
    {
        height: scaleY(5)
        width: scaleX(5)
        buttontext: "Close"
        buttonsqradius: 10
        anchors.top: parent.top
        anchors.topMargin: scaleY(1)
        anchors.right: parent.right
        anchors.rightMargin: scaleX(1)
        MouseArea{
            anchors.fill: parent
            onClicked: avcodes.destroy()
        }
    }
}
