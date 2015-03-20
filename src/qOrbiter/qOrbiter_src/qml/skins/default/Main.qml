import QtQuick 2.3

import "." //used to import the style object as singleton

Item{
    id:qmlRoot
    height: manager.appHeight
    width: manager.appWidth
    focus:true
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
        anchors.fill: parent
    }
}
