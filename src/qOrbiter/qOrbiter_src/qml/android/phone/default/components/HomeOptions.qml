import QtQuick 1.1
//

import "../components"
import "../js/ComponentLoader.js" as MyJs
Item{
    id:optionPanel
    width: parent.width /2
    height:scaleY(25)
    property int buttonH:optionPanel.height / advancedrow.children.length -10
    Rectangle{
        anchors.fill: parent
        color: "black"
        opacity: .85
    }

    opacity: showOptions ? 1 : 0

    Behavior on opacity{
        PropertyAnimation{
            duration:350
        }
    }

    Loader{
        id:optionLoader
        anchors.fill: parent

    }

    states: [
        State {
            name: "home"
            when: manager.currentScreen==="Screen_1.qml"
            PropertyChanges {
                target: optionLoader
                source:"HomeOptions_1.qml"
            }
        }
    ]

}
