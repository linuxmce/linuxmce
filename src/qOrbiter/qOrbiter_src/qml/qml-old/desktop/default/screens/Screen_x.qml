import QtQuick 1.1
import "../components"
Item{
    id:error_screen

    height: manager.appHeight
    width: manager.appWidth
    Rectangle{
     width: parent.width
     height: parent.height /3
     anchors.verticalCenter: parent.verticalCenter
        color: "grey"
        Text {
            id: unknownscreen
            x: 0
            y: 131
            text: "Error: Screen file not found: "
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
