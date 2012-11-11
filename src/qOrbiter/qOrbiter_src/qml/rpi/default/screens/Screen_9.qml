import QtQuick 2.0
import "../components"
Item{
    id:makecallphonebook

    Rectangle{
        height: manager.appHeight
        width: manager.appWidth
        color: "transparent"
        StyledText {
            id: phonebookcall
            x: 74
            y: 101
            text: "Make Call from Phone Book"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
