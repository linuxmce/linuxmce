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
        color: "transparent"

        Rectangle{
            id:bgrect
            anchors.fill: parent
            z:0
            color: "transparent"

        }

        Column{
            id:maindisplay
            anchors.fill: parent
            spacing: 5
            HomeLightingRow{id:lightRow }
            HomeMediaRow{id: mediaRow}
            HomeClimateRow{id: climateRow}
            HomeTelecomRow{id: telecomRow}
            HomeSecurityRow{id: securityRom}

        }
        BottomPanel{id: advanced; color:style.rowbgColor; anchors.bottom: stage.bottom}
    }
}

