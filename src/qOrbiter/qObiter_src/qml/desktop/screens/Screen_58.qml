import QtQuick 1.0
import "../components"
Item{
    id:dvdoptions

    Rectangle{
        height: style.orbiterH
        width: style.orbiterW
        color: style.bgcolor
        Text {
            id: dvdoptionslabel
            x: 74
            y: 101
            text: "Dvd Options"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
