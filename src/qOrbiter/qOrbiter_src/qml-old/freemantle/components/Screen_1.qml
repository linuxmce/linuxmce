import QtQuick 1.0
import ScreenSaverModule 1.0
import "../components"

Item
{

Rectangle {
    id:stage


    signal swapStyle()
    height: style.orbiterH
    width: style.orbiterW
    opacity: 1

    Rectangle{
        id:bgrect
        anchors.fill: parent
        z:0
        color: style.advanced_bg

        SequentialAnimation on color
        {
            running:true
            loops:Animation.Infinite

        ColorAnimation  { from: style.advanced_bg ; to: "steelblue"; duration: 5000 }
        ColorAnimation  { from: "steelblue" ; to: "slategrey"; duration: 5000 }
        ColorAnimation { from:  "slategrey"; to: "aliceblue"; duration: 5000 }
        ColorAnimation  { from: "aliceblue" ; to: style.advanced_bg; duration: 5000 }
        }

        ScreenSaverModule{}
    }

Column{
    id:maindisplay
    anchors.fill: parent
    spacing: 0
    HomeLightingRow{id:lightRow }
    HomeMediaRow{id: mediaRow}
    HomeClimateRow{id: climateRow}
    HomeTelecomRow{id: telecomRow}
    HomeSecurityRow{id: securityRom}

}
BottomPanel{id: advanced; color:style.rowbgColor; anchors.bottom: stage.bottom}
    }
}

