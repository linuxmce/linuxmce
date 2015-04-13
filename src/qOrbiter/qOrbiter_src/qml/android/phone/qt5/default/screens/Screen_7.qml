import QtQuick 2.2
import "../components"
Item{
    id:misc

    Rectangle{
        height: style.orbiterH
        width: style.orbiterW
        color: style.bgcolor
        Text {
            id: misclabel
            x: 74
            y: 101
            text: "Misc"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
