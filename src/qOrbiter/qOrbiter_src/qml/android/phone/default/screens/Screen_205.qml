import QtQuick 1.0
import "../components"

Rectangle {

    width: appW
    height: appH
    color: "transparent"


    Rectangle{
        height: scaleY(30)
        width: scaleX(30)
        color: "navajowhite"
        anchors.centerIn: parent
        radius:5

        ButtonSq{
            radius: 2
            buttontext: "Regen Now"
            anchors.left: parent.left
            MouseArea{
                anchors.fill:parent
                onClicked: manager.quickReload()
            }
        }

        ButtonSq{
            radius: 2
            buttontext: "Later"
            anchors.right: parent.right
            MouseArea{
                anchors.fill: parent
                onClicked:setCurrentScreen("Screen_1.qml")
            }
        }
    }

}
