import QtQuick 1.1
import "../components"
Rectangle{
    id:reloadrouter
    height: appH
    width: appW
    color: "transparent"

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
                    buttonsqradius: 2.5
                    buttontextitalic: true

                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.QuickReload()
                    }
                }

                ButtonSq{
                    id: deny
                    buttontext: qsTr("Reload Later")
                    buttonsqradius: 2.5
                    buttontextitalic: true
                    MouseArea{
                        anchors.fill: parent
                        onClicked: setCurrentScreen("Screen_1.qml")
                    }
                }
            }
        }
    }

