import QtQuick 1.0
import "../components"
Item{
    id:activecallsitem

    Rectangle{
        height: 430
        width: 800
        color: style.bgcolor
        Text {
            id: activecallstext
            x: 74
            y: 101
            text: "Active Calls"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
