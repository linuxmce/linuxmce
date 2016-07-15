import QtQuick 2.2
import QtQuick.Window 2.1
//import QtQuick.Dialogs 1.2
import "." //used to import the style object as singleton

Item{
    id:qmlRoot
    anchors.fill: parent


    property bool screensaverActive:false
    signal screenSaverActivated()
    signal resetTimeout()
    signal clearDialogs()
    signal unhandledKey(int key)
    property int lastKey:-1
    onResetTimeout: {
        console.log("timer reset")
        screenSaverTimeout.restart()
        screensaverActive=false
    }

    onScreenSaverActivated: {
        console.log("Screen saver is active")
        screensaverActive=true
    }

    Timer{
        id:screenSaverTimeout
        interval: manager.screenSaverTimeout*1000
        running: true
        repeat: true
        onTriggered: {
            screenSaverActivated()
        }
    }

    function createPopup(comp){
        comp.createObject(layout)
    }


    property ListModel scenarios:ListModel{
        id:scenariosList
        ListElement{
            name:"Lights"
            modelName:0
            floorplantype:2
        }
        ListElement{
            name:"Media"
            modelName:2
            floorplantype:5
        }
        ListElement{
            name:"Climate"
            modelName:1
            floorplantype:1
        }
        ListElement{
            name:"Telecom"
            modelName:3
            floorplantype:3
        }
        ListElement{
            name:"Security"
            modelName:"currentRoomSecurity"
            floorplantype:4
        }
        //        ListElement{
        //            name:"Advanced"
        //            modelName:"advancedMenu"
        //            floorplantype:-1
        //        }
    }


    Component.onCompleted: {
        manager.writeConfig()
//         manager.setActiveRoom(manager.room , manager.entertainArea);
//        console.log( String("Setting Ea to %1 and Room to %2").arg(manager.room).arg(manager.entertainArea) )
    }

    Keys.onPressed: {
        switch(event.key){
        case Qt.Key_F5: console.log("Reloading qml"); manager.qmlReload(); break;
        case Qt.Key_R: console.log("Rotating"); manager.setDesiredOrientation(manager.isProfile ? Qt.LandscapeOrientation : Qt.PortraitOrientation); break;
        case Qt.Key_Backspace: console.log("Back Button Hard Key"); manager.goBacktoQScreen();break;
        case Qt.Key_Back: console.log("Back Key pressed"); manager.goBacktoQScreen(); event.accepted=true; break;

        default:console.log("qmlRoot::Key "+event.key+" is unhandled."); qmlRoot.unhandledKey( event.key ); break;
        }

    }

    MainLayout{
        id:layout
        anchors.centerIn: parent
    }



    MouseArea{
        anchors.fill: parent
        onPressed: {

            if(!screensaverActive){
                mouse.accepted=false
                console.log("event filter ignored click event")
            }   else {
                qmlRoot.resetTimeout()
                console.log("event filter ignored click event")
            }
            // console.log("Click Event Filter Pressed, screensaver state is "+screensaverActive)
        }
    }
}


