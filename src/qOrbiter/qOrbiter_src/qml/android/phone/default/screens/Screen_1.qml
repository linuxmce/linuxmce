import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs


Item {
    id:stage
    signal swapStyle()
    property bool showOptions:false
    property bool showSecondary:false
    width:manager.appWidth
    height:manager.appHeight
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

    //    DroidHomeSelector{
    //        id:home_selector;
    //        anchors.top: manager.b_orientation ? fs_npButton.bottom : spaceholder.bottom
    //        anchors.left: manager.b_orientation ? stage.left : fs_npButton.right
    //        anchors.leftMargin: manager.b_orientation ? scaleX(15) : scaleX(1)
    //    }

    Row{
        id:ftr
        height: manager.b_orientation ? scaleY(8) : scaleY(12)
        anchors{
            bottom:parent.bottom
            left:parent.left
            right:parent.right
        }
        Rectangle{
            anchors.fill: parent
            color: "black"
            opacity: .75
        }
        StyledButton{
            buttonText: manager.sPK_User
            anchors{
                left:parent.left
                verticalCenter: parent.verticalCenter
            }
            onActivated: {loadComponent("UserSelector.qml");}

        }

        Clock{
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    ScenarioSelector{
        id:scenarioPopup
        currentModel:undefined
        anchors.centerIn: parent
    }

    HomeOptions {
        id: advancedrow
    }
}


