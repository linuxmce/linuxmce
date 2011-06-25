import QtQuick 1.0

Item
{

Rectangle {

    id:stage
    Style1{id: style}
    signal swapStyle()

    width: style.orbiterW
    height: style.orbiterH
    color: style.stage_bg

    NotificationBar {id:mnu;  color: style.not_color}

    HomeLightingRow{id:lightRow; anchors.top: mnu.bottom; anchors.topMargin: 0;}
    HomeMediaRow{id: mediaRow; anchors.top: lightRow.bottom}
    HomeClimateRow{id: climateRow; anchors.top: mediaRow.bottom; anchors.topMargin: 0}
    HomeTelecomRow{id: telecomRow; anchors.top:climateRow.bottom; anchors.topMargin: 0}
    HomeSecurityRow{id: securityRom; anchors.top: telecomRow.bottom; anchors.topMargin: 0}

    BottomPanel{id: advanced; color:style.advanced_bg; bottomPanelTextColor: style.advanced_bg_text; anchors.bottom: parent.bottom}

    }
}

