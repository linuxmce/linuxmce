import QtQuick 1.1
import Linuxmce.UIElements 1.0

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
            height:50
            width:100
            color:"green"
            borderColor: "white"
            borderWidth: 1
            onPressed:console.log("Pressed!")
            onClicked: console.log("clicked")
            onReleased: console.log("released")

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
