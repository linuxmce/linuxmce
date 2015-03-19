import QtQuick 2.3
import QtGraphicalEffects 1.0
import AudioVisual 1.0
import DceScreenSaver 1.0
import QtMultimedia 5.0
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

    Rectangle{
        anchors.fill: parent
        color:"black"
    }

    Text{
       anchors.horizontalCenter: parent.horizontalCenter
       anchors.top: parent.top
       text:"Main.qml"
       color:"white"
       font.pointSize: 65
    }



}
