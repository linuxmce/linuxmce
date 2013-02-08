import QtQuick 2.0
import "../components"
Item{
    id:climate

    Rectangle{
        height: style.orbiterH
        width: style.orbiterW
        color: "transparent"
        StyledText {
            id: climatelabel
            x: 74
            y: 101
            text: "Quad View Cameras"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
            color:style.titletextcolor
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
