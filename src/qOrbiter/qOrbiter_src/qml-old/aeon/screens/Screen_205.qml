import QtQuick 1.1
import "../components"
Item{
    id:security
    Rectangle{
        width: style.orbiterW
        height: style.orbiterH
        color: "black"
        opacity:  .8
    }
    Rectangle{
        width: style.orbiterW
        height: style.orbiterH
        color: "transparent"
        Text {
            x: 0
            y: 131
            text: "Orbiter needs regen; not yet implemented"
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
