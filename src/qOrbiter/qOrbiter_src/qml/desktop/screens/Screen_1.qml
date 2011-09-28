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
        color: style.bgcolor

        Rectangle{
            id:bgrect
            anchors.fill: parent
            z:0
            color: "transparent"

        }
        Rectangle{
            id:status
            height: scaleY(2.5)
            width: parent.width
            color: connectedState == true ? style.darkhighlight :red


            Text{
                id:connectstatus
                text: qsTr("Orbiter")+ iPK_Device + qsTr(" Is Connected")
               height: status.height; anchors.left: parent.left; anchors.leftMargin: scaleX(5)
            }

            Text{
                id:dcemessages
                text:dcemessage
               height: status.height; anchors.left: parent.left; anchors.leftMargin: scaleX(5)
            }

            Clock{id:screen1time; height: status.height; anchors.right: parent.right; anchors.rightMargin: scaleX(25) }

        }

        Column{
            id:maindisplay
            spacing: 0
            anchors.top:status.bottom
            anchors.topMargin: 2
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

