import QtQuick 1.0

Rectangle {
    id:powerrect
    width: scaleX(75)
    height: scaleY(55)
    clip: true
    color: skinStyle.darkhighlight
    anchors.centerIn: parent
    border.color: skinStyle.lighthighlight
    border.width: 2
    opacity: 1

    Rectangle{

        width: powerrect.width-1
        height: powerrect.height-1
        color:"transparent"
        clip:true
        anchors.centerIn: parent

        Rectangle{
            id:exit
            width: parent.width
            height: scaleX(15)
            Text {
                id: exit_label
                text: qsTr("Exit")
                font.bold: true
                font.pixelSize: scaleY(3)
            }
            MouseArea{
                anchors.fill: parent
                onClicked: loadComponent("NullComponent.qml")
            }
        }

        Row{
            anchors.top: exit.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            width: childrenRect.width
            height: childrenRect.height
            spacing: scaleX(5)
            Column {
                id: on
                width: childrenRect.width
                height: childrenRect.height
                spacing: 10
                StyledButton{
                    id:pwron
                    buttonText: qsTr("Display On")

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            manager.displayToggle(1)
                            loadComponent("NullComponent.qml")
                        }
                    }
                }

                StyledButton {
                    id: mdonlabel
                    buttonText:  qsTr("Media Director\n On")
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {

                            loadComponent("NullComponent.qml")
                        }
                    }
                }
            }

            Column {
                id: off
                width: childrenRect.width
                height: childrenRect.height
                spacing: 10
                StyledButton {
                    id: offlabel
                    buttonText: qsTr("Display Off")

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            manager.toggleDisplay(false)
                           loadComponent("NullComponent.qml")
                        }
                    }
                }

                StyledButton{
                    id: mdofflabel
                    buttonText: "Media Director\n Off"

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                           loadComponent("NullComponent.qml")
                        }
                    }
                }
            }
        }
    }
}

