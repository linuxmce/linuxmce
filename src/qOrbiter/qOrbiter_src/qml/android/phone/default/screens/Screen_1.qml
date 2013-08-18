import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs


Item {
    id:stage
    anchors.centerIn: parent
    signal swapStyle()
    property bool showOptions:false
    width:manager.appWidth
    height:manager.appHeight
    Component.onCompleted: forceActiveFocus()

    focus:true
    Keys.onReleased: {
        event.accepted=true
        switch(event.key){
        case Qt.Key_Back:
            console.log("show exit")
            break;
        case Qt.Key_Menu :
            showOptions = !showOptions
            break;
        case Qt.Key_MediaPrevious:
            console.log("Media previous")
            break;
        default:
            console.log(event.key)
            break;
        }
    }

    NowPlayingButton{
        id:fs_npButton
        anchors.top: spaceholder.bottom
        anchors.left: manager.b_orientation ? parent.horizontalCenter : parent.left
        anchors.leftMargin: manager.b_orientation ? (fs_npButton.width / 2) * -1 : scaleX(2)
    }


    DroidHomeSelector{
        id:home_selector;
        anchors.top: manager.b_orientation ? fs_npButton.bottom : spaceholder.bottom
        anchors.left: manager.b_orientation ? stage.left : fs_npButton.right
        anchors.leftMargin: manager.b_orientation ? scaleX(15) : scaleX(1)
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


