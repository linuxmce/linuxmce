import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs

StyledScreen {
    id:stage
    signal swapStyle()

    property bool showSecondary:false


    Component.onCompleted:{ forceActiveFocus() ; hdr.state="open"}
    property int itemH:manager.isProfile ? homeSelectionLayout.height /3 :homeSelectionLayout.height /3
    property int itemW:manager.isProfile ? homeSelectionLayout.width/ 2 : homeSelectionLayout.width / 2
    focus:true
    Keys.onReleased: {

        switch(event.key){
        case Qt.Key_Back:
            if(showSecondary){
                showSecondary = !showSecondary
            }else{
                console.log("show exit")
            }
            event.accepted=true
            break;
        case Qt.Key_Menu :
            showOptions = !showOptions
            event.accepted=true
            break;
        case Qt.Key_MediaPrevious:
            if(secondaryModel.visible){
                showSecondary = false
            } else {
                console.log("show exit")
            }

            console.log("Media previous")
            event.accepted=true
            break;
        default:
            console.log(event.key)

            break;
        }
    }

    NowPlayingButton{
        id:fs_npButton
    }

    Flow{
        id:homeSelectionLayout
        visible: !showSecondary
        anchors{
            top: fs_npButton.bottom
            left:parent.left
            right:parent.right
            bottom:parent.bottom
            leftMargin: 10
        }

        Repeater{
            model:scenarios

            delegate:
                StyledButton{
                id:btn
                height: itemH -10
                width: itemW - 10
                buttonText:name
                textSize: 38
                onHeld: {
                    if(modelName==="currentRoomLights")
                    {scenarioPopup.floorplanType = 2}
                    else if(modelName==="currentRoomMedia")
                    { scenarioPopup.floorplanType = 5}
                    else if(modelName==="currentRoomClimate")
                    {scenarioPopup.floorplanType  = 3}
                    else if(modelName==="currentRoomTelecom")
                    { scenarioPopup.floorplanType = 7}
                    else if(modelName==="currentRoomSecurity")
                    {scenarioPopup.floorplanType = 1} //can also be 1?
                    else if (modelName==="advanced"){ manager.setCurrentScreen("Screen_44.qml") }
                    manager.showfloorplan(floorplantype)
                    manager.setFloorplanType(floorplantype)
                  //  manager.setCurrentScreen("Screen_"+scenarioPopup.floorplanType+".qml")
                }

                onActivated: {
                    if(manager.currentScreen==="Screen_1.qml"){
                        if(modelName==="currentRoomLights")
                        {scenarioPopup.currentModel =currentRoomLights; scenarioPopup.floorplanType = 2}
                        else if(modelName==="currentRoomMedia")
                        { scenarioPopup.currentModel = currentRoomMedia; scenarioPopup.floorplanType = 5}
                        else if(modelName==="currentRoomClimate")
                        {scenarioPopup.currentModel = currentRoomClimate; scenarioPopup.floorplanType  = 3}
                        else if(modelName==="currentRoomTelecom")
                        {scenarioPopup.currentModel = currentRoomTelecom; scenarioPopup.floorplanType = 7}
                        else if(modelName==="currentRoomSecurity")
                        {scenarioPopup.currentModel = currentRoomSecurity; scenarioPopup.floorplanType = 1} //can also be 1?
                        else if (modelName==="advanced"){ manager.setCurrentScreen("Screen_44.qml") }

                    }
                    showSecondary = true
                }
            }
        }
    }


    GridView{
        id:secondaryModel
        visible:showSecondary
        property int floorplanType: scenarioPopup.floorplanType = 5
        clip:true
        anchors{
            top: fs_npButton.bottom
            left:parent.left
            right:parent.right
            bottom:parent.bottom
            leftMargin: 10
        }
        cellHeight: itemH-10
        cellWidth: itemW-10
        model:scenarioPopup.currentModel
        delegate:
            StyledButton{
            id:gridBtn
            height: itemH -10
            width: itemW - 10
            buttonText:title
            textSize: 38
            onActivated: {
                manager.execGrp(params)
                showSecondary = false

            }
        }
    }
    ScenarioSelector{
        id:scenarioPopup
        currentModel:undefined
        anchors.centerIn: parent
    }
}


