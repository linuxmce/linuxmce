import QtQuick 1.1
import "../../../skins-common/lib/effects"

Rectangle{
    id:optionsDisplay
    height: scaleY(10)
    width: scaleX(95)
    color: "transparent"
    state: "hidden"

    Rectangle{
        id:mask
        anchors.fill: parent
        color: "black"
        opacity: .25
    }

   Loader{
       id:optionsLoader
       source: ""
       anchors.centerIn: parent
       onSourceChanged: console.log("Source Changed::"+source.toString())
   }

    states: [
        State {
            name: "exposed"
            PropertyChanges {
                target: mask
                opacity:.50
            }
            PropertyChanges {
                target: optionsLoader
                source:"MediaOptions"+manager.i_current_mediaType+".qml"

            }
            AnchorChanges{
                target: optionsDisplay
                anchors.top: mediaPlaybackBase.top
                anchors.bottom: undefined
            }
        },
        State {
            name: "hidden"
            PropertyChanges {
                target: optionsLoader
                source:""
            }
            AnchorChanges{
                target:optionsDisplay
                anchors.top: undefined
                anchors.bottom: mediaPlaybackBase.top
            }
        }
    ]

    transitions: [
        Transition {
            AnchorAnimation{
                targets: optionsDisplay
                duration: 1000
            }

        }
    ]
}
