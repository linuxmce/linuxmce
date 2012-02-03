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
        source: "../images/power.png"
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

                            powerrect.destroy()
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

                            powerrect.destroy()
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

                            powerrect.destroy()
                        }
                    }
                }

                ButtonSq{
                    id: mdofflabel
                    buttontext : qsTr("Media Director Off")
                    buttonsqradius: 20
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {

                            powerrect.destroy()
                        }
                    }
                }
            }
        }
    }
}

