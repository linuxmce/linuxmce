import QtQuick 1.0
import com.nokia.android 1.1
//import "../androidComponents"
import "../components"

Item
{
    anchors.centerIn: parent

    Rectangle {
        id:stage

        signal swapStyle()
        height: manager.appHeight
        width: manager.appWidth
        color:"transparent"


        Image {
            id: headerbg
            source: "../img/ui3/header.png"
            anchors.top: stage.top
            height: scaleY(7)
            width: scaleX(99)

        }

        Label{
            id:connectstatus
            text: "Orbiter "+ deviceid + " is connected."
            font.family: "Droid Sans"
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
            ScenarioTemplate{
                id:lightrow
                scenariomodel: currentRoomLights
                floorplantype: 2
                bgimgSource: "../img/ui3/lightingbig.png"
                rowFillColor.opacity: .25
                rowFillColor.color: "slategrey"
            }
            ScenarioTemplate{
                id:mediaRow
                scenariomodel: currentRoomMedia
                floorplantype: 3
                bgimgSource: "../img/ui3/mediabig.png"
                rowFillColor.color: "pink"

            }
            ScenarioTemplate{
                id:climateRow
                scenariomodel: currentRoomClimate
                floorplantype: 4
                bgimgSource: "../img/ui3/climatebig.png"

            }
            ScenarioTemplate{
                id:telecomRow
                scenariomodel: currentRoomTelecom
                floorplantype: 6
                bgimgSource: "../img/ui3/telephonebig.png"
                rowFillColor.color: "yellow"
            }
            ScenarioTemplate{
                id:securityRow
                scenariomodel: currentRoomSecurity
                floorplantype: 7
                bgimgSource: "../img/ui3/securitybig.png"
                rowFillColor.color:  "red"
            }


        }
        BottomPanel{id: advanced; anchors.top: maindisplay.bottom}
    }
}

