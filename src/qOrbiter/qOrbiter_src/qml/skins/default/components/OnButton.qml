import QtQuick 2.2
import "../components"
import "../."
Item {
    width: Style.appButtonWidth
    height: Style.appButtonHeight


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

        StyledText{
            anchors.centerIn: parent
            text:qsTr("On")
            color:"white"         
        }

        MouseArea{
            id:ex
            anchors.fill: parent
            onPressed: cmdEntry.sendCommand()
        }
    }
}


