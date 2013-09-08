import QtQuick 1.1
import Linuxmce.UIElements 1.0
import "../../../skins-common/lib/effects"

Rectangle {
    id:display
    width: scaleX(85)
    height: scaleY(85)
    anchors.centerIn: parent

    Rectangle{
        anchors.fill: parent
        color: "grey"
        opacity: .75
    }


    Column{
        id:buttonCol
        height: parent.height /2
        width: scaleX(15)
        anchors.top: display.top
        spacing:scaleY(2)

        QtButton{
            id:button
            height:scaleY(8)
            width:scaleX(10)
            radius:8
            color:isPressed ? "green" : "yellow"
            borderColor: "white"
            borderWidth: 2
            onPressed:console.log("Pressed!")
            onClicked: console.log("clicked")
            onReleased: console.log("released")
            smooth: true
            paintImage:"/src/src/qOrbiter/qOrbiter_src/qml/desktop/default/"+"img/bg_bluegradient.jpg"

        }
    }

    ButtonSq{
        anchors.bottom: parent.bottom
        MouseArea{
            anchors.fill: parent
            onClicked: display.destroy()
        }
    }
}
