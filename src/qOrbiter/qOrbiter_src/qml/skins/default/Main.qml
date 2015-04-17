import QtQuick 2.2

import "." //used to import the style object as singleton

Item{
    id:qmlRoot
    height: manager.appHeight
    width: manager.appWidth
    focus:true

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
        ListElement{
            name:"Advanced"
            modelName:"advancedMenu"
            floorplantype:-1
        }
    }


    Component.onCompleted: {
        manager.writeConfig()
    }

    Keys.onPressed: {
        switch(event.key){
        case Qt.Key_F5: console.log("Reloading qml"); manager.qmlReload(); break;
        case Qt.Key_R: console.log("Rotating"); manager.setDesiredOrientation(manager.isProfile ? Qt.LandscapeOrientation : Qt.PortraitOrientation); break;
        default:console.log("RootObject::Key "+event.key+" is unhandled."); break;
        }

    }

    MainLayout{
        id:layout
        height: manager.appHeight
        width: manager.appWidth
        anchors.centerIn: parent
    }
}
