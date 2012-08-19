import QtQuick 1.0
import "../components"
import "../effects"
import Qt.labs.shaders 1.0
import GoogleWeatherPlugin 1.0

Item
{
    anchors.centerIn: parent
    GoogleWeather{
        id:weatherSource
        Component.onCompleted: postalCode = ("90043")

    }

    Rectangle {
        id:stage

        signal swapStyle()
        height: appH
        width: appW
        color: "transparent"

        Image {
            id: headerbg
            source: "../img/ui3/header.png"
            anchors.top: stage.top
            height: scaleY(7)
            width: scaleX(99)
            MouseArea{
                       anchors.fill: headerbg
                       onClicked: weatherSource.getWeather(weatherSource.postalCode)
                   }
        }

        Text{
            id:connectstatus
            text: weatherSource.currentTemp ==0 ? weatherSource.statusMessage +weatherSource.postalCode : qsTr("Orbiter")+ iPK_Device + qsTr(" Is Connected") + " and the current temp is:" + weatherSource.currentTemp
            color: "aliceblue"
            font.letterSpacing: 2
            anchors.left: parent.left
            anchors.leftMargin: scaleX(5)
            anchors.verticalCenter: headerbg.verticalCenter

        }
        Image {
            id: weatherImage
            source: weatherSource.currentIcon
            anchors.left: connectstatus.right
            anchors.verticalCenter: connectstatus.verticalCenter
            anchors.leftMargin: 20
            opacity: .75
            height:25
            fillMode: Image.PreserveAspectFit
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

