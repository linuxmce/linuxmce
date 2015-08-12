import QtQuick 2.2

import "." //used to import the style object as singleton

Item{
    id:qmlRoot
    height: manager.appHeight
    width: manager.appWidth
    focus:true
    property bool screensaverActive:false
    signal screenSaverActivated()
    signal resetTimeout()
    signal unhandledKey(int key)
    property int lastKey:-1
    onResetTimeout: {
        screenSaverTimeout.restart()
        screensaverActive=false
    }


    onScreenSaverActivated: {
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
        height: manager.appHeight
        width: manager.appWidth
        anchors.centerIn: parent
    }
    MouseArea{
        anchors.fill: parent
        onPressed: {
            console.log("pressed")
            if(!screensaverActive)
                mouse.accepted=false


            qmlRoot.resetTimeout()
        }
    }
}
