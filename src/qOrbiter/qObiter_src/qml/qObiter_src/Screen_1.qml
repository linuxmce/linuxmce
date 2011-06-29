import QtQuick 1.0
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
    spacing: 5
    height: childrenRect.height
    width: childrenRect.width
    NotificationBar {id:mnu;  color: style.not_color}
    HomeLightingRow{id:lightRow }
    HomeMediaRow{id: mediaRow}
    HomeClimateRow{id: climateRow}
    HomeTelecomRow{id: telecomRow}
    HomeSecurityRow{id: securityRom}
    BottomPanel{id: advanced; color:style.advanced_bg}
}

    }
}

