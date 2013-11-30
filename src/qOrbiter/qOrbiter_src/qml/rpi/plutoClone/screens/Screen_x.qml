import QtQuick 2.0
import "../components"
Item{
    id:security

    Rectangle{
        height: manager.appHeight
        width: manager.appWidth
        color: "transparent"
        StyledText {
            id: unknownscreen
            x: 0
            y: 131
            text: "Error: Screen file not found: " + screenfile
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
