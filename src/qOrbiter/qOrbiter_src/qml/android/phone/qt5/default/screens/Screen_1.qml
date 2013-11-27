import QtQuick 2.0
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
            popExit.opacity = 1;
            exitTimer.start()
            break;
        case Qt.Key_Menu :
            showOptions = !showOptions
            console.log("toggle menu")
            break;
        case Qt.Key_M:
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
        anchors.top: parent.top
        anchors.left: manager.b_orientation ? parent.horizontalCenter : parent.left
        anchors.leftMargin: manager.b_orientation ? (fs_npButton.width / 2) * -1 : scaleX(2)
    }


    DroidHomeSelector{
        id:home_selector;
        anchors.top: manager.b_orientation ? fs_npButton.bottom : parent.top
        anchors.left: manager.b_orientation ? stage.left : fs_npButton.right
        anchors.leftMargin: manager.b_orientation ? scaleX(15) : scaleX(1)
    }
    ScenarioSelector{
        id:scenarioPopup
    }

    HomeOptions {
        id: advancedrow
    }

    Item{
        id:popExit
        height: scaleY(35)
        width: scaleX(85)
        opacity: 0

        anchors.centerIn: parent

        Behavior on opacity{

            PropertyAnimation{
                duration:500
            }
        }

        Timer{
            id:exitTimer
            interval: 1250
            onTriggered: popExit.opacity = 0
        }

        Rectangle{
            anchors.fill: parent
            color: ms.pressed ? androidSystem.greenStandard : androidSystem.redStandard
        }
        StyledText{
            text:"Click Again to exit"
            font.weight: Font.DemiBold
            color: "White"
            width: parent.width
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            fontSize: scaleY(8)
            anchors.centerIn: parent
        }

        MouseArea{
            id:ms
            anchors.fill: parent
            enabled: parent.opacity != 0
            onReleased: manager.closeOrbiter()
        }

    }

}


