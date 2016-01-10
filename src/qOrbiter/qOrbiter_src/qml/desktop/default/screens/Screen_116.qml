import QtQuick 1.1
import "../components"
/*!
 *\brief Hard Drive detection popup
 *
 *\ingroup qml_desktop_default
 *This screen shows when a new drive is detected, allowing the user to select the desired actions.
 */
Rectangle {

    height: style.orbiterH
    width: style.orbiterW
    color: style.advanced_bg
    Text {
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
        Text {
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
        Text {
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
