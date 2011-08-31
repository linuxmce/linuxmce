import QtQuick 1.0
import "../components"

Item
{
    anchors.centerIn: parent
    Rectangle {
        id:stage
        Connections{
            target: screensaver

        }

        signal swapStyle()
        height: style.orbiterH
        width: style.orbiterW
        opacity: 1

        Rectangle{
            id:bgrect
            anchors.fill: parent
            z:0
            color: "transparent"


            Image {
                id: ssimg2
                source: screensaver.transition_image
                opacity: 1


            }

            Image {
                id: ssimg
                fillMode: Image.PreserveAspectCrop
                source: screensaver.current_image
                opacity: 1

                SequentialAnimation on opacity{

                    running: true
                    loops: Animation.Infinite
                    PauseAnimation { duration: 2500 }
                    PropertyAnimation
                    {   id:picswitch
                        target: ssimg
                        property: "opacity"
                        to: 0
                        duration: 3000

                    }
                    PauseAnimation { duration: 2500 }

                    PropertyAnimation
                    {   id:picswitch2
                        target: ssimg
                        property: "opacity"
                        to: 1
                        duration: 3000

                    }
                }


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

