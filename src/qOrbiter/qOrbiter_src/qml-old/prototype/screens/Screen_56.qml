import QtQuick 1.0
import "../components"
Item{
    id:orbiters

    Rectangle{
        height: manager.appHeight
        width: manager.appWidth
        color: "transparent"
        Text {
            id: orbiterslabel
            x: 74
            y: 101
            text: "Orbiters"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
