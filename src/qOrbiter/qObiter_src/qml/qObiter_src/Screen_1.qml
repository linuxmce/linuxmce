import Qt 4.7
import "components"

Item
{

Rectangle {

    id:stage
    Style1{id: style}
    signal swapStyle()

    width: style.orbiterW
    height: style.orbiterH
    color: style.stage_bg


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

