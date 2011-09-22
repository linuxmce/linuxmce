import QtQuick 1.0
import "../components"
Rectangle{
    id:reloadrouter
    height: style.orbiterH
    width: style.orbiterW
   // color: style.highlight2
    gradient: Gradient {
        GradientStop {
            position: 0.00;
            color: style.higlight2;
        }
        GradientStop {
            position: 1.00;
            color: style.highlight1;
        }
    }

    Rectangle{
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
                id: message
                text: qsTr("The router has requested a reload")
                width: parent.width
                height: scaleY(5)
                font.italic: false
                horizontalAlignment: Text.AlignHCenter
                font.bold: false
                color: "aliceblue"
                wrapMode: Text.WrapAnywhere
                font.pixelSize: 18
                anchors.centerIn: containerrect
            }
            Row{
                id:buttonrow
                spacing:scaleX(2)
                width: childrenRect.width
                height: childrenRect.height
               anchors.horizontalCenter: parent.horizontalCenter
               anchors.top: message.bottom
               anchors.topMargin: scaleY(2)

                ButtonSq{
                    id: confirm
                    buttontext: qsTr("Reload Now")
                    buttonsqradius: 20
                    buttontextitalic: true

                    MouseArea{
                        anchors.fill: parent
                        onClicked: reloadrouter()
                    }
                }

                ButtonSq{
                    id: deny
                    buttontext: qsTr("Reload Later")
                    buttonsqradius: 20
                    buttontextitalic: true
                    MouseArea{
                        anchors.fill: parent
                        onClicked: gotoQScreen("Screen_1.qml")
                    }
                }
            }
        }
       HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

