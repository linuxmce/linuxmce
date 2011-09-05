import QtQuick 1.0
import "../components"

   Item {
    id:asktoresume
    width: 450
    height: 300

            Rectangle {
            id:containerrect
            anchors.fill:parent
            color: style.bgcolor
            opacity: 0.5
             }

            Text {
                id: text1
                x: 26
                y: 99
                text: "Would you Like to Auto resume your media?"
                z: 2
                font.family: "Droid Sans"
                font.italic: false
                horizontalAlignment: Text.AlignHCenter
                font.bold: false
                wrapMode: Text.WrapAnywhere
                font.pixelSize: 18
                anchors.centerIn: containerrect
            }
            HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
        }

