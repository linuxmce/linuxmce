import QtQuick 1.0
import "../components"

Item
{
    anchors.centerIn: parent
    Rectangle {
        id:stage

        signal swapStyle()
        height: style.orbiterH
        width: style.orbiterW
        color: "black"

        Rectangle{
            id:bgrect
            anchors.fill: parent
            z:0
            color: "transparent"

        }
        Rectangle{
            id:status
            height: 25
            width: parent.width
            color:"aliceblue"

        }

        Column{
            id:maindisplay

            spacing: 3
            anchors.top:status.bottom
            anchors.topMargin: 2
            height: scaleY(75)
            width: scaleX(100)
            HomeLightingRow{id:lightRow }
            HomeMediaRow{id: mediaRow}
            HomeClimateRow{id: climateRow}
            HomeTelecomRow{id: telecomRow}
            HomeSecurityRow{id: securityRom}

        }
        BottomPanel{id: advanced; anchors.bottom: stage.bottom}
    }
}

