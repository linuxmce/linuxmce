import QtQuick 1.1
import "../components"
Item{
    id:advancedMediaOptions

    Rectangle{
        height: appH
        width: appW
        color: "transparent"
        Text {
            id: statussec
            x: 74
            y: 101
            text: "Advanced Media Options"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
