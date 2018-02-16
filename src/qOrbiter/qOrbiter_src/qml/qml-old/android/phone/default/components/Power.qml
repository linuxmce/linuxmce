import QtQuick 1.0

Item {
    id:powerrect
    width: scaleX(75)
    height: scaleY(55)
    clip: true
    anchors.centerIn: parent

    Rectangle{
        anchors.fill: parent
        border.color: appstyle.button_system_text_color
        color:appstyle.toolbarBgColor
        border.width: 2
    }

    Rectangle{
        id:exit
        anchors{
            left:parent.left
            right:parent.right
            margins: 5
            top:powerrect.top
        }

        height: parent.height*.20
        gradient: appstyle.buttonGradient
        StyledText {
            id: exit_label
            text: qsTr("Close")
            anchors.centerIn: parent
            color:appstyle.highlight2
        }
        MouseArea{
            anchors.fill: parent
            onClicked: loadComponent("NullComponent.qml")
        }
    }

    Row{
        anchors{
            top:exit.bottom
            bottom:parent.bottom
            left:parent.left
            right:parent.right
        }
        spacing: scaleX(5)
        Column {
            id: on
            width: parent.width/2 - 10
            height: parent.height
            spacing: scaleY(10)

            StyledButton{
                id:pwron
                buttonText: qsTr("Display ON")

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
                buttonText:  qsTr("Media \n Director ON")
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
            width: parent.width/2 - 10
            height: parent.height
            spacing: scaleY(10)
            StyledButton {
                id: offlabel
                buttonText: qsTr("Display OFF")

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
                buttonText: "Media \n Director OFF"

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


