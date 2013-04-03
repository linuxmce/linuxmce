import QtQuick 1.1
import "../components"
Item{
    id:securitystatus

    Rectangle{
        height: manager.appHeight
        width: manager.appWidth
        color: "transparent"
        Text {
            id: statussec
            x: 74
            y: 101
            text: "Device Control"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
