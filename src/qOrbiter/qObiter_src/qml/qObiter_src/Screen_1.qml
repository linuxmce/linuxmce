import QtQuick 1.0
import ScreenSaverModule 1.0
import "components"

Item
{

Rectangle {
    id:stage
    Style1{id: style}
    signal swapStyle()

    Rectangle{
        id:bgrect
        anchors.fill: parent
        z:0
        color: "whitesmoke"
        Image {
            id: name
            fillMode: Image.PreserveAspectCrop
            source: "../../img/lmcesplash.jpg"
            anchors.fill: parent
            smooth: true
        }

        SequentialAnimation on color
        {
            running:true
            loops:Animation.Infinite

        ColorAnimation  { from: "whitesmoke" ; to: "steelblue"; duration: 5000 }
        ColorAnimation  { from: "steelblue" ; to: "slategrey"; duration: 5000 }
        ColorAnimation { from:  "slategrey"; to: "lightgrey"; duration: 5000 }
        ColorAnimation  { from: "lightgrey" ; to: "whitesmoke"; duration: 5000 }
        }

        ScreenSaverModule{}
    }

    width: style.orbiterW
    height: style.orbiterH
    opacity: 1

Column{
    anchors.fill: parent
    spacing: 5
    HomeLightingRow{id:lightRow }
    HomeMediaRow{id: mediaRow}
    HomeClimateRow{id: climateRow}
    HomeTelecomRow{id: telecomRow}
    HomeSecurityRow{id: securityRom}

}
BottomPanel{id: advanced; color:style.advanced_bg; anchors.bottom: stage.bottom}
    }
}

