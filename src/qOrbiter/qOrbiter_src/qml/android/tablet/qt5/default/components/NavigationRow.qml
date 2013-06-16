import QtQuick 2.0

import "../../../lib/handlers"


Row{
    id:nav_row
    height: scaleY(8)
    width:parent.width
    spacing:scaleY(2)
    ListView{
        id:scenarioList
        height:scaleY(7)
        width:  (scaleX(10)*8)
        spacing: scaleY(2)
        model:current_header_model
        clip:true
        orientation:ListView.Horizontal
        anchors.verticalCenter: parent.verticalCenter
        delegate: Item{
            height: childrenRect.height
            width: childrenRect.width
            StyledButton{
                id:dummy
                buttonText.text: name
                buttonText.color: "antiquewhite"
                hitArea.onReleased: {
                    if(manager.currentScreen==="Screen_1.qml"){
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
                    
                    
                }
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
