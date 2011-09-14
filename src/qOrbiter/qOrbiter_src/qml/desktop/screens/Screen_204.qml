import QtQuick 1.0
import "../components"
Item{
    id:reloadrouter

    Rectangle{
        height: style.orbiterH
        width: style.orbiterW
        color: style.darkhighlight

        Rectangle{
            id:headerblock
            height:scaleY(5)
            width: scaleX(50)
            color: style.warncolor
            anchors.top: parent.top
            anchors.topMargin: scaleY(2)
            anchors.horizontalCenter: parent.horizontalCenter
            radius: 10
            Text {
                id: message
                text: qsTr("The router has requested a reload")
                width: parent.width
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                font.pixelSize: 14
                anchors.centerIn: parent
            }
        }

        Rectangle{
            id:reloadroutercontainer
            height: childrenRect.height + 5
            width:childrenRect.width + 5
            anchors.centerIn: parent
            color:style.accentcolor
            radius: 20
            clip:true

            Row{
                id:buttonrow
                spacing:scaleX(2)
                width: childrenRect.width
                height: childrenRect.height
                anchors.centerIn: parent

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
}
