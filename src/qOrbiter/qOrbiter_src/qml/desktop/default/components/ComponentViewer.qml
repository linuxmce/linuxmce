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
        height: parent.height - scaleY(2)
        width: scaleY(15)

                Button{
                    id:button
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
