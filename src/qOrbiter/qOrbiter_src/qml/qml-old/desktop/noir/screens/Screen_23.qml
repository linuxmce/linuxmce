import QtQuick 1.1
import "../components"
Item{
    id:intercomitem

    Rectangle{
        height: appH
        width: appW
        color: "transparent"
        Text {
            id: intercomtext
            x: 74
            y: 101
            text: "Intercom"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
