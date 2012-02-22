import QtQuick 1.0

Rectangle {
    id:avcodes
    width: scaleX(75)
    height: scaleY(65)
    radius: 10
    border.color: "black"
    border.width: 2
    color: "aliceblue"
    anchors.centerIn: parent
    Component.onCompleted: dcerouter.showAdvancedButtons()
    MouseArea{
        anchors.fill:avcodes

    }

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
        ListView
        {
            id:codelist
            height: parent.height
            width: scaleX(15)
            anchors.top:blah.bottom
            model: buttonList
            delegate: Rectangle{
                id:av_code_top
                height: scaleY(5)
                width: parent.width *(.25)
                color: "grey"
                Text{
                    id:av_label
                    text:qs_device
                }
                MouseArea{
                    anchors.fill: av_code_top
                    hoverEnabled: true
                    onEntered: av_code_top.color = "white"
                    onExited: av_code_top.color = style.lighthighlight

                }
            }
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
