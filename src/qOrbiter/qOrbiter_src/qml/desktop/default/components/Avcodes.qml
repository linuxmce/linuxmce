import QtQuick 1.1

Rectangle {
    id:avcodes_rect
    width: scaleX(75)
    height: scaleY(65)
    radius: 10
    border.color: "black"
    border.width: 2
    color: "aliceblue"
    anchors.centerIn: parent
    Component.onCompleted: manager.showAvControl()
    property int selectedDevice
    onSelectedDeviceChanged: deviceCommands.clear()
    MouseArea{
        anchors.fill:avcodes_rect

    }

    Column{
        height: childrenRect.height
        width: childrenRect.width
        anchors.horizontalCenter: parent.horizontalCenter

        spacing: scaleY(1)
        StyledText {
            id: avcodeslabel
            textLabel: qsTr("These are the devices in use. Touch a device name to send it a command, or touch the other cells to resend inputs or jump to advanced")
            width: parent.width * .75
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            font.pixelSize: scaleY(2)
            anchors.horizontalCenter: parent.horizontalCenter
        }


        Rectangle
        {
            id:codelistrect
            height: scaleY(55)
            width: avcodes_rect.width *.85
            color: "#628BC7"
            anchors.horizontalCenter: parent.horizontalCenter
            border.color: "aliceblue"
            border.width: 2
            radius: 20


            ListView
            {
                id:codelist
                height: parent.height - 10
                width: scaleX(15)
                anchors.top:blah.bottom
                model: deviceList
                clip:true
                spacing:5
                delegate: Rectangle{
                    id:av_code_top
                    height: scaleY(8)
                    width: scaleX(20)
                    color: "grey"
                    border.color: "white"
                    border.width: 1
                    Column{
                        spacing: scaleY(1)
                        StyledText{
                            id:av_label
                            textLabel:name
                            fontSize: scaleY(2)
                            isBold: true

                        }

                        StyledText {
                            id: device_number
                            textLabel: qsTr("Device Number:") + devicenumber
                            fontSize: scaleY(2)

                        }
                    }
                    MouseArea{
                        anchors.fill: av_code_top
                        hoverEnabled: true
                        onEntered: av_code_top.color = "white"
                        onExited: av_code_top.color = style.lighthighlight
                        onClicked: {manager.showDeviceCodes(devicenumber);selectedDevice = devicenumber}

                    }
                }
            }

            ListView
            {
                id:device_command_list
                height: parent.height
                width: scaleX(10)
                anchors.top: parent.top
                anchors.left: codelist.right
                anchors.leftMargin: scaleY(5)
                clip: true
                model: deviceCommands
                delegate: Rectangle{
                    id:device_box
                    height: scaleY(8)
                    width: scaleX(20)
                    color: "slateblue"
                    Column{
                        spacing: scaleY(1)
                        StyledText{
                            id:command_label
                            textLabel:name
                            fontSize: scaleY(2)
                        }
                    }
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: device_box.color = "white"
                        onExited: device_box.color = "slateblue"
                        onClicked: manager.resendCode(selectedDevice , commandnumber)
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
            onClicked: avcodes_rect.destroy()
        }
    }
}
