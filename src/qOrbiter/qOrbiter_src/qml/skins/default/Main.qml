import QtQuick 2.3

import "." //used to import the style object as singleton

Item{
    id:qmlRoot
    height: manager.appHeight
    width: manager.appWidth
    focus:true

    function createPopup(comp){
        comp.createObject(qmlRoot)
    }

    property ListModel scenarios:ListModel{
        id:scenariosList
        ListElement{
            name:qsTr("Lights")
            modelName:0
            floorplantype:2
        }
        ListElement{
            name:qsTr("Media")
            modelName:2
            floorplantype:5
        }
        ListElement{
            name:qsTr("Climate")
            modelName:1
            floorplantype:3
        }
        ListElement{
            name:qsTr("Telecom")
            modelName:3
            floorplantype:3
        }
        ListElement{
            name:qsTr("Security")
            modelName:"currentRoomSecurity"
            floorplantype:4
        }
        ListElement{
            name:qsTr("Advanced")
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
        default:console.log("Key "+event.key+" is unhandled."); break;
        }
    }

    MainLayout{
        id:layout
        height: manager.appHeight
        width: manager.appWidth
        anchors.centerIn: parent
    }
}
