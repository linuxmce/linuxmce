import QtQuick 1.1
import "../components"
Item{
    id:security
    Rectangle{
        width: skinStyle.orbiterW
        height: skinStyle.orbiterH
        color: "black"
        opacity:  .8
    }
    Rectangle{
        width: skinStyle.orbiterW
        height: skinStyle.orbiterH
        color: "transparent"
        Text {
            id: unknownscreen
            x: 0
            y: 131
            text: "Error, unable to load screen file: " + screenfile
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
            color: "white"
        }
        Rectangle{ x: 5; y: 5; width: 75; height: 75; smooth: true; border.width: 1; border.color: "black"
            Text{text: "Home"; anchors.centerIn: parent}
            MouseArea{
                anchors.fill: parent
                onClicked:setCurrentScreen("Screen_1.qml")
            }
        }
    }

}
