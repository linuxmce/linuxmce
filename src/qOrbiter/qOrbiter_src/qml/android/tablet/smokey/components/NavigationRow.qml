import QtQuick 1.0

import "../../lib/handlers"


Item{
    id:nav_row
    height: scaleY(8)
    width:parent.width
    property alias navigation:nav
    property string navSource:""

    Loader{
        id:nav
        sourceComponent: ScenarioComponent{}
        height: parent.height
        width: parent.width
    }

    Connections{
        target: roomList
        onCurrentRoomChanged: {current_scenario_model = []; refresh.restart() }
    }
    Timer{
        id:refresh
        running:false
        interval: 750
        onTriggered:{
            if(manager.currentScreen==="Screen_1.qml"){
                if(scenarioList.currentModelName==="currentRoomLights")
                    current_scenario_model = currentRoomLights
                else if(scenarioList.currentModelName==="currentRoomMedia")
                    current_scenario_model = currentRoomMedia
                else if(scenarioList.currentModelName==="currentRoomClimate")
                    current_scenario_model = currentRoomClimate
                else if(scenarioList.currentModelName==="currentRoomTelecom")
                    current_scenario_model=currentRoomTelecom
                else if(scenarioList.currentModelName==="currentRoomSecurity")
                    current_scenario_model = currentRoomSecurity
            }else if (manager.currentScreen==="Screen_47"){
                if(name==="Attribute")
                    console.log("attribute selected")
            } else {
                current_scenario_model = []
            }
        }
    }


    StyledButton{
        buttonText.text:"Advanced"
        visible: manager.currentScreen === "Screen_1.qml"
    }
    StyledButton {
        id: exit_label
        buttonText.text: qsTr("Exit")
        hitArea.onReleased: manager.exitApp()
        visible:manager.currentScreen ==="Screen_1.qml"
    }
    StyledButton {
        id: home_label
        buttonText.text: qsTr("Home")
        hitArea.onReleased: manager.gotoQScreen("Screen_1.qml")
        visible: manager.currentScreen !=="Screen_1.qml"
    }
    StyledButton{
        id:media_goback
        buttonText.text: "Back"
        hitArea.onReleased:
        {
            if(manager.i_current_mediaType !== 5){
                manager.goBackGrid();
            }
            else{
                
                pageLoader.item.state="selection"
                manager.goBackGrid()
                mediatypefilter.reset()
            }
            
            
        }
        visible: manager.currentScreen==="Screen_47.qml"
    }
}
