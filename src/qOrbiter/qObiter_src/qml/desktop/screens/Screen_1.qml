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
        opacity: 1

        Rectangle{
            id:bgrect
            anchors.fill: parent
            z:0
            color: "transparent"
            Text {
                id: name
                text: screensaver.testtext
                font.pixelSize: 24
                color: "black"
                anchors.centerIn: parent
            }
            Image {
                id: ssimg
                fillMode: Image.PreserveAspectCrop
                source: screensaver.current_image
            }
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

