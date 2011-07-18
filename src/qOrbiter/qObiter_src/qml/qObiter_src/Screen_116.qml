import QtQuick 1.0
import "components"
Rectangle {
    Style{id:style}
    width: 640
    height: 480
    color: style.advanced_bg
    Text {
        id: text1
        x: 196
        y: 139
        text: "pnp Hard Drive Screen"
        font.family: "Droid Sans Fallback"
        font.pointSize: 24
        font.pixelSize: 12
    }

    ButtonSq {
        id: buttonsq1
        height: 50
        color: "#85eae9"
        buttontext: "Home"
        width: 50
        x: 574
        y: 25
        MouseArea{
            anchors.fill: parent
            onClicked:gotoQScreen("Screen_1.qml")
        }
    }
}
