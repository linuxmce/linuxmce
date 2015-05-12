import QtQuick 1.1
import "../components"
Item {
    width: scaleX(45)
    height: scaleY(20)


    Item{
        id:button
        anchors{
            top:parent.top
            left:parent.left
            bottom:parent.bottom
            right:parent.right
            margins: 5
        }

        Rectangle{
            anchors.fill: parent
            color: ex.pressed ? "grey" : "green"
            radius: 5
            border.color: "white"
            border.width: 1
        }

        Text{
            anchors.centerIn: parent
            text:qsTr("Off")
            color:"white"
            font.pointSize: 18
        }

        MouseArea{
            id:ex
            anchors.fill: parent
            onPressed: cmdEntry.sendCommand()
        }
    }
}


