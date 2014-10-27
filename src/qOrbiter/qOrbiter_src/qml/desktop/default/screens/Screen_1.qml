import QtQuick 1.1
import "../components"
import "../../../skins-common/lib/effects"

/*!
 *\class Screen_1
 *\brief Screen 1 component.
 *
 *\ingroup qml_desktop_default
 *In default, this screen is used a place to have all scenarios set in horizontal rows.
 */
Item{
    id:screen_1
    anchors{
        top:parent.top
        left:parent.left
        right:parent.right
        bottom:parent.bottom
    }




    Item {
        id:stage
        signal swapStyle()
        anchors.fill: parent

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
        BottomPanel{
            id: advanced;
            anchors.bottom: parent.bottom
        }
    }
}

