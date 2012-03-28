import QtQuick 1.0

Rectangle {
    id:powerrect
    width: scaleX(40)
    height: scaleY(40)
    clip: true
    color: style.darkhighlight
    anchors.centerIn: parent
    border.color: style.lighthighlight
    border.width: 2

    Image {
        id: bg
        fillMode: Image.PreserveAspectCrop
        source: "../img/icons/power.png"
        anchors.fill: parent
    }

    Rectangle{

        width: powerrect.width-1
        height: powerrect.height-1
        color:"transparent"
        clip:true
        anchors.centerIn: parent


        Row{
            anchors.centerIn: parent
            width: childrenRect.width
            height: childrenRect.height
            spacing: scaleX(5)
            Column {
                id: display_on
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
                            dcerouter.displayToggle(1)
                            loadComponent("NullComponent.qml")
                        }
                    }
                }

            }

            Column {
                id: display_off
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
                            dcerouter.displayToggle(0)
                            loadComponent("NullComponent.qml")
                        }
                    }
                }


            }
        }
    }
}

