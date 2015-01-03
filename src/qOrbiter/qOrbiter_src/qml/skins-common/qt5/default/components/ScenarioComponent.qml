import QtQuick 2.2



ListView{
    id:scenarioList
    height:scaleY(7)
    width:  (scaleX(10)*8)
    visible:manager.currentScreen==="Screen_1.qml"
    spacing: scaleY(2)
    model:current_header_model
    clip:true
    orientation:ListView.Horizontal
    anchors.verticalCenter: parent.verticalCenter
    property string currentModelName:""
    delegate:
        StyledButton{
            id:dummy
            buttonText: name
            txtObj.color: "antiquewhite"
            txtObj.isBold: false
            anchors.verticalCenter: parent.verticalCenter
            hitArea.onPressAndHold:{
                manager.showfloorplan(floorplantype)
                manager.setFloorplanType(floorplantype)
            }

            hitArea.onReleased: {
                console.log(manager.currentScreen)
                if(manager.currentScreen=="Screen_1.qml"){
                    if(modelName==="currentRoomLights")
                        current_scenario_model = currentRoomLights
                    else if(modelName==="currentRoomMedia")
                        current_scenario_model = currentRoomMedia
                    else if(modelName==="currentRoomClimate")
                        current_scenario_model = currentRoomClimate
                    else if(modelName==="currentRoomTelecom")
                        current_scenario_model=currentRoomTelecom
                    else if(modelName==="currentRoomSecurity")
                        current_scenario_model = currentRoomSecurity
                }else if (manager.currentScreen==="Screen_47"){
                    if(name==="Attribute")
                        console.log("attribute selected")
                }
                else{
                    console.log("Manager currentScreen is not Screen 1::"+manager.currentScreen)
                    current_scenario_model = []
                }
                 scenarioList.currentModelName = modelName
                console.log(scenarioList.currentModelName)
            }
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
}
