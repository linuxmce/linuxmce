import QtQuick 1.1

import "../components"
import "../js/ComponentLoader.js" as MyJs
Item{
    id:optionPanel
    width:manager.isProfile ? parent.width /2 : parent.width /3
    clip:true
    property int buttonH:optionPanel.height / advancedrow.children.length -10
    function setCurrentSource(s){

        if(s!==""){
            console.log("Setting options to "+s)
            optionLoader.source=s
        } else {
            optionLoader.source = "HomeOptionsGeneric.qml"
        }

    }

    opacity: showOptions ? 1 : 0

    Behavior on opacity{
        PropertyAnimation{
            duration:350
        }
    }

    Loader{
        id:optionLoader
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        source:"HomeOptionsGeneric.qml"
        onStatusChanged: {
            if (optionLoader.status===Loader.Error){
                source = "HomeOptionsGeneric.qml"
            }
        }
        height: manager.appHeight-hdr.height-ftr.height
    }

}
