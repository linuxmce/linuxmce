import QtQuick 1.0
import "../components"

   Rectangle {
    id:asktoresume
    width: style.orbiterW
    height: style.orbiterH
    color: style.highlight2

            Rectangle {
            id:containerrect
            height:scaleY(50)
            width: scaleX(60)
            color: style.bgcolor
            clip: true
            border.color: style.highlight1
            border.width: 2
            anchors.centerIn: parent
            radius: 10

            Text {
                id: text1
                width: parent.width -10
                height: scaleY(5)
                text: "Would you Like to Auto resume your media?"
                z: 2
                font.family: "Droid Sans"
                font.italic: false
                horizontalAlignment: Text.AlignHCenter
                font.bold: false
                color: "aliceblue"
                wrapMode: Text.WrapAnywhere
                font.pixelSize: 18
                anchors.centerIn: containerrect
            }

            Row
            {
                height: childrenRect.height + scaleY(1)
                width: childrenRect.width
                spacing: scaleX(1)
                anchors.top: text1.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                ButtonSq{
                    buttontext: "Yes Resume"
                    buttonsqradius: 10
                }
                ButtonSq{
                    buttontext: "No thank you"
                    buttonsqradius: 10
                }

                ButtonSq{
                    buttontext: "Yes Always Resume"
                    buttonsqradius: 10
                }

                ButtonSq{
                    buttontext: "No thank you, Never resume"
                    buttonsqradius: 10
                }

            }
            }

            HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
        }

