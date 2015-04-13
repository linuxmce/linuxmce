import QtQuick 2.2

Rectangle {
    id:powerrect
    width: scaleX(50)
    height: scaleY(40)
    clip: true
    color: style.darkhighlight
    anchors.centerIn: parent
    border.color: style.lighthighlight
    border.width: 2
    opacity: 1

    function openAimation()
    {

    }

    function closeAnimation()
    {

    }

    Image {
        id: bg
        fillMode: Image.PreserveAspectCrop
        source: ""
        anchors.fill: parent
    }

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
                ButtonSq{
                    id:pwron
                    buttontext: qsTr("Display On")
                    buttonsqradius: 20
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            manager.displayToggle(1)
                            loadComponent("NullComponent.qml")
                        }
                    }
                }

                ButtonSq {
                    id: mdonlabel
                    buttontext:  qsTr("Media Director On")
                    buttonsqradius: 20
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
                ButtonSq {
                    id: offlabel
                    buttontext: qsTr("Display Off")
                    buttonsqradius: 20
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            manager.toggleDisplay(false)
                           loadComponent("NullComponent.qml")
                        }
                    }
                }

                ButtonSq{
                    id: mdofflabel
                    buttontext: "Media Director Off"
                    buttonsqradius: 20
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

