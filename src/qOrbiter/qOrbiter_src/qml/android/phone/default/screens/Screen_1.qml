import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs

Item {
    id:stage
    signal swapStyle()
    property bool showOptions:false
    property bool showSecondary:false
    anchors{
        top:parent.top
        left:parent.left
        right:parent.right
        bottom:parent.bottom
    }

    Component.onCompleted: forceActiveFocus()
    property int itemH:manager.b_orientation ? homeSelectionLayout.height /3 :homeSelectionLayout.height /3
    property int itemW:manager.b_orientation ? homeSelectionLayout.width/ 2 : homeSelectionLayout.width / 2
    focus:true
    Keys.onReleased: {
        event.accepted=true
        switch(event.key){
        case Qt.Key_Back:
            if(showSecondary){
                showSecondary = !showSecondary
            }else{
                console.log("show exit")
            }
            break;
        case Qt.Key_Menu :
            showOptions = !showOptions
            break;
        case Qt.Key_MediaPrevious:
            if(secondaryModel.visible){
                showSecondary = false
            } else {
                console.log("show exit")
            }

            console.log("Media previous")
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
            bottom:ftr.top
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
                        {scenarioPopup.currentModel = currentRoomSecurity; scenarioPopup.floorplanType = 4} //can also be 1?
                        else if (modelName==="advanced"){ manager.gotoQScreen("Screen_44.qml") }

                    }
                    showSecondary = true
                }
            }
        }
    }


    GridView{
        id:secondaryModel
        visible:showSecondary
        property int floorplanType: scenarioPopup.floorplanType = 4
        clip:true
        anchors{
            top: fs_npButton.bottom
            left:parent.left
            right:parent.right
            bottom:ftr.top
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
}


