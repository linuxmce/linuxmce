import QtQuick 1.0
import "../components"
Item{
    id:security

    Rectangle{
        height: 430
        width: 800
        color: style.bgcolor
        Text {
            id: securitylabel
            x: 74
            y: 101
            text: "Security Floorplan"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
