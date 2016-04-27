import QtQuick 1.1
import "../js/ComponentLoader.js" as MyJs
//FocusScope{
//    height: scenarios.height
//    width: scenarios.width


//    onFocusChanged: console.log("Focus for Scenario Container is now "+focus)
//    onActiveFocusChanged: console.log("ActiveFocus for Scenario Container is now " +activeFocus)

Row{
    id:modelRow
    height: scenarios.height
    width: buttonWidth*5
    spacing: 10
   // anchors.horizontalCenter: parent.horizontalCenter
    property alias sModel: scenariosModel
    
    VisualItemModel{
        id:scenariosModel

        ScenarioTrigger {
            id: lightingTrigger
            triggerLabel: "Lights"
            targetModel: currentRoomLights
        }
        
        ScenarioTrigger{
            id:mediaTrigger
            triggerLabel: "Media"
            targetModel: currentRoomMedia
        }
        
        ScenarioTrigger{
            id:climateTrigger
            triggerLabel: "Climate"
            targetModel: currentRoomClimate
        }
        
        ScenarioTrigger{
            id:securityTrigger
            triggerLabel:"Security"
            targetModel: currentRoomSecurity
        }
        
        ScenarioTrigger{
            id:telecomTrigger
            triggerLabel: "Telecom"
            targetModel: currentRoomTelecom
        }
    }
}
