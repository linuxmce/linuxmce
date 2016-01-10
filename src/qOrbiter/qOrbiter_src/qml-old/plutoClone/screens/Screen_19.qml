import QtQuick 2.0
import "../components"
Item{
    id:securitystatus

    Rectangle{
        height: style.orbiterH
        width: style.orbiterW
        color: "transparent"
        StyledText {
            id: statussec
            x: 74
            y: 101
            text: "File Save"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
