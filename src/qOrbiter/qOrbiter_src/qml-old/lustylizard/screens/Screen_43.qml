import QtQuick 1.0
import "../components"
Item{
    id:securitystatus

    Rectangle{
        height: appH
        width: appW
        color: "transparent"
        Text {
            id: statussec
            x: 74
            y: 101
            text: "PC File Tools"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
