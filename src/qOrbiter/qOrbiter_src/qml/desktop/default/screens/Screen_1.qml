import QtQuick 1.0
import "../components"
import "../../lib/effects"

/*!
 *\class Screen_1
 *\brief Screen 1 component.
 *
 *\ingroup qml_desktop_default
 *In default, this screen is used a place to have all scenarios set in horizontal rows.
 */
Item
{
    anchors.centerIn: parent
    anchors.fill: parent
    Rectangle {
        id:stage

        signal swapStyle()
      anchors.fill: parent
        color: "transparent"
        Column{
            id:maindisplay

            height: childrenRect.height
            width: scaleX(100)
            HomeLightingRow{id:lightRow }
            HomeMediaRow{id: mediaRow}
            HomeClimateRow{id: climateRow}
            HomeTelecomRow{id: telecomRow}
            HomeSecurityRow{id: securityRom}
        }
        BottomPanel{id: advanced; anchors.top: maindisplay.bottom}
    }
}

