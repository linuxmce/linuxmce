import QtQuick 1.0
import com.nokia.android 1.1
//import "../androidComponents"
import "../components"

Item
{
    id:stage
    anchors.centerIn: parent
    height: manager.appHeight
    width: manager.appWidth
    signal swapStyle()

    Column{
        id:maindisplay
        anchors.top:parent.top
        height: childrenRect.height
        width: scaleX(100)
        ScenarioTemplate{
            id:lightrow
            scenariomodel: currentRoomLights
            floorplantype: 2
            bgimgSource: "../img/ui3/lightingbig.png"
        }
        ScenarioTemplate{
            id:mediaRow
            scenariomodel: currentRoomMedia
            floorplantype: 3
            bgimgSource: "../img/ui3/mediabig.png"
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
        }
        ScenarioTemplate{
            id:securityRow
            scenariomodel: currentRoomSecurity
            floorplantype: 7
            bgimgSource: "../img/ui3/securitybig.png"

        }


    }
    BottomPanel{id: advanced; anchors.bottom: stage.bottom}

}

