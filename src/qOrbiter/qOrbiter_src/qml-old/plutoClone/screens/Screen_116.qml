import QtQuick 2.0
import "../components"
Rectangle {

    height: style.orbiterH
    width: style.orbiterW
    color: style.advanced_bg
    StyledText {
        id: text1
        x: 196
        y: 139
        text: "pnp Hard Drive Screen"
        font.family: "Droid Sans Fallback"
        font.pointSize: 24
        font.pixelSize: 12
        anchors.bottom: ignoreonce.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Rectangle{
        id:ignoreonce
        height:scaleY(10)
        width: scaleX(10)
        y:scaleY(15)
        anchors.centerIn: parent
        StyledText {
            id: screen116label1
            text: qsTr("Ignore just this time")
        }
    }

    Rectangle{
        id:ignorealways
        height:scaleY(10)
        width: scaleX(10)
        y:scaleY(15)
        anchors.left: ignoreonce.left
        anchors.leftMargin: 20
        StyledText {
            id: screen116label2
            text: qsTr("Ignore Always")
        }

        Rectangle{
            id:optbox
            height: scaleY(20)
            width: scaleX(15)
            color: style.not_color
            anchors.top: ignoreonce.bottom
            anchors.horizontalCenter: parent.horizontalCenter
        }

    }

    HomeButton{anchors.right: parent.right}
}
