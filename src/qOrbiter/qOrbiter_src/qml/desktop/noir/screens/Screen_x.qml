import QtQuick 1.1
import "../components"
Item{
    id:security

    Rectangle{
        height: appH
        width: appW
        color: "transparent"
        Text {
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
