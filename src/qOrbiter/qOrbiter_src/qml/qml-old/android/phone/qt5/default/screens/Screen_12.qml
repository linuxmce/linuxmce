import QtQuick 2.2
import "../components"
Item{
    id:callplutouser

    Rectangle{
        height: style.orbiterH
        width: style.orbiterW
        color: style.bgcolor
        Text {
            id: callplutolabel
            x: 74
            y: 101
            text: "Make Call LinuxMCE User"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
