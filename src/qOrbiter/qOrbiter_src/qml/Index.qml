import QtQuick 2.3
import QtGraphicalEffects 1.0
import AudioVisual 1.0
import DceScreenSaver 1.0
/*
  Root Item.
  */
Item {
    id:qml_index
   height: manager.appHeight
   width: manager.appWidth
    state:"preinit"
    Component.onCompleted: {
        console.log("Index Loaded")
        if(window.orbiterInitialized){
            state="init"
        } else {
            state="preinit"
        }
    }

    Timer{
        id:pusher
        interval: 2000
        onTriggered: bootStrap.source="splash/Splash.qml"
        running:true
    }

    IndexContent {
        id: indexContent
    }

    Loader{
        id:bootStrap
        anchors.fill: parent

    }


    states: [
        State {
            name: "preinit"
            PropertyChanges {
                target: indexContent
                // visible:true

            }

        },
        State {
            name: "init"
            PropertyChanges {
                target: indexContent
                //visible:false
            }
        }
    ]
}
