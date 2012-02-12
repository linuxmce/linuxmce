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

        Image {
            id: headerbg
            source: "../../img/ui3/header.png"
            anchors.top: stage.top
            height: scaleY(7)
            width: scaleX(99)
        }

        Text{
            id:connectstatus
            text: qsTr("Orbiter")+ iPK_Device + qsTr(" Is Connected")
            color: "aliceblue"
            font.letterSpacing: 2
            anchors.left: parent.left
            anchors.leftMargin: scaleX(5)
            anchors.verticalCenter: headerbg.verticalCenter
        }

        Clock{
            id:screen1time
            anchors.right: headerbg.right
            anchors.rightMargin: scaleX(25)
            anchors.verticalCenter: headerbg.verticalCenter
        }

        Column{
            id:maindisplay
            anchors.top:headerbg.bottom
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

