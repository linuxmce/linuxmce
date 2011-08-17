import QtQuick 1.0
import "../components"
Item{
    id:security

    Rectangle{
        width: style.orbiterW
        height: style.orbiterH
        color: style.bgcolor
        Text {
            id: unknownscreen
            x: 0
            y: 131
            text: "Error: Screen file not found: " + screenfile
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
        }
        Rectangle{ x: 5; y: 5; width: 75; height: 75; smooth: true; border.width: 1; border.color: "black"
            Text{text: "Home"; anchors.centerIn: parent}
            MouseArea{
                anchors.fill: parent
                onClicked:gotoQScreen("Screen_1.qml")
            }
        }
    }

}
