import QtQuick 2.2
import "../components"
import "../."
Item {
    width: appStyle.appButtonWidth
    height: appStyle.appButtonHeight


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
            text:qsTr("Off")
            color:"white"           
        }

        MouseArea{
            id:ex
            anchors.fill: parent
            onPressed: cmdEntry.sendCommand()
        }
    }
}


