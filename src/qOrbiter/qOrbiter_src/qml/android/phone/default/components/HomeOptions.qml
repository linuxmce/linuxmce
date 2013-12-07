import QtQuick 1.1
//

import "../components"
import "../js/ComponentLoader.js" as MyJs
Item{
    id:optionPanel
    width:manager.b_orientation ? parent.width /2 : parent.width /3
    clip:true
    property int buttonH:optionPanel.height / advancedrow.children.length -10

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

    states: [
        State {
            name: "home"
            when: manager.currentScreen==="Screen_1.qml"
            PropertyChanges {
                target: optionLoader
                source:"HomeOptions_1.qml"
            }
        },
        State {
            name: "media"
            when:manager.currentScreen==="Screen_47.qml"
            PropertyChanges {
                target: optionLoader
                source:"HomeOptions_47.qml"
            }
        }
    ]

}
