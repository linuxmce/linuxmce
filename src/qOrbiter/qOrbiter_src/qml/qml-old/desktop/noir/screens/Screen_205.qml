import QtQuick 1.1
import "../components"

Rectangle{
    id:requestingregen
    height: appH
    width: appW
     color: style.stageBg
   /*
    Image {
        id: bgimage
        source: "../img/icons/orbiterbg.png"
    }
    BorderImage {
        id: name
        horizontalTileMode: BorderImage.Repeat
        source: "../img/icons/drpshadow.png"
        anchors.fill: regenrequestcontainer
        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
        border { left: 10; top: 10; right: 10; bottom: 10 }
        smooth: true
    }
   */

    Rectangle{
        id:regenrequestcontainer
        height: scaleY(85)
        width:scaleX(50)
        anchors.centerIn: parent
        color: style.darkhighlight
        border.color: style.highlight1
        border.width: 2
        radius: 2.5

        Column{
            id:buttoncolumn
            height: childrenRect.height
            width: parent.width
            spacing: scaleY(2)
            anchors.centerIn: parent

            Text {
                id: label
                text: qsTr("You have recently changed scenarios, floorplans, or other settings. I need to regenerate the graphical screens so you can see the new buttons. This will take from 2 - 20 minutes")
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                width: parent.width * .85
                font.pixelSize: scaleY(2.5)
                anchors.horizontalCenter: parent.horizontalCenter
                color: "aliceblue"
            }

            ButtonSq{
                id:thisorbiter
                height: scaleY(10)
                width:scaleX(10)
                buttontext: "Yes Regenerate this orbiter."
                buttonsqradius: 2.5
                anchors.horizontalCenter: parent.horizontalCenter
            }

            ButtonSq{
                id:allorbiters
                height: scaleY(10)
                width:scaleX(10)
                buttontext: "Regenerate all orbiters in my home."
                buttonsqradius: 2.5
                anchors.horizontalCenter: parent.horizontalCenter
            }

            ButtonSq{
                id:deny
                height: scaleY(10)
                width:scaleX(10)
                buttontext: "No I will do it later."
                buttonsqradius: 2.5
                anchors.horizontalCenter: parent.horizontalCenter
                MouseArea{
                    anchors.fill: parent
                    onClicked: {setCurrentScreen("Screen_1.qml")}
                }
            }

            ButtonSq{
                id:denyall
                height: scaleY(10)
                width:scaleX(10)
                buttontext: "No for all orbiters."
                buttonsqradius: 2.5
                anchors.horizontalCenter: parent.horizontalCenter
                MouseArea{
                    anchors.fill: parent
                    onClicked: {setCurrentScreen("Screen_1.qml")}
                }
            }
        }
    }
}

