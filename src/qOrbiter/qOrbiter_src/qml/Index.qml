import QtQuick 2.5
import QtGraphicalEffects 1.0
import QtMultimedia 5.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1


import "splash"
import "skins"
/*
  Root Item.
  */
Window {
    id:qml_root_item
    height: manager.appHeight
    width: manager.appWidth
    onWidthChanged: console.log("window size"+manager.appWidth+ "w x "+manager.appHeight+"h")


    color:"transparent"
    property string appEntryQml: manager.skinEntryFile
    property int currentStage:deviceSettingsReady+orbiterReady

   // Component.onCompleted: console.log(manager.selectPath("IndexContent.qml"))

    //startup properties
    property int deviceSettingsReady: settings.ready ? 1 : 0
    property int orbiterReady:manager.uiReady ? 1 : 0

    Text {
        id: deviceSettings
        text: qsTr("Device Settings")
        color:settings.ready ? "green" : "red"
    }

    Connections{
        target:manager
        onUiReadyChanged:{ if(manager.uiReady) {appContent.state="app"} else {appContent.state="regen"}  }
        onSplashReady:{appContent.state = "splash"; console.log("SPLASH SCREEN UP!!!!!!!!!!!!") }
    }


    Component{
        id:indexRootComponent
        IndexContent{

        }
    }

    Component{
        id:regenComponent
        RegenScreen{

        }
    }

    Component{
        id:splashComponent
        Splash{

        }
    }

    Item{
        id:appContent
        anchors.fill: parent
        focus:true
        Keys.onPressed: {
            switch(event.key){

            case Qt.Key_F5: manager.qmlReload(); break
            default: console.log(event.key);break;
            }
        }
        state:"preinit"
        //        Component.onCompleted: {
        //            console.log("Index Loaded")
        //            if(orbiterWindow.orbiterInitialized){
        //                state="init"
        //            } else {
        //                state="preinit"
        //            }
        //        }



        //        Timer{
        //            id:pusher
        //            interval: 2000
        //            onTriggered: bootStrap.source="splash/Splash.qml"
        //            //running:true
        //        }

        Loader{
            id:app
            anchors.fill: parent
            onSourceChanged: {
                console.log(source)
            }
        }


        Loader{
            id:bootStrap
            anchors.fill: parent

        }


        states: [
            State {
                name: "preinit"
                PropertyChanges {
                    target: bootStrap
                    sourceComponent:indexRootComponent
                }
                PropertyChanges {
                    target: app
                    source:""
                }

            },
            State {
                name: "splash"
                PropertyChanges {
                    target: bootStrap
                    sourceComponent:splashComponent
                }
                PropertyChanges {
                    target: app
                    source:""
                }
            },
            State {
                name: "app"

                PropertyChanges {
                    target: app
                    source:manager.selectPath(appEntryQml)
                }

                StateChangeScript{
                    script:manager.clearSkinCache()
                }

                PropertyChanges {
                    target: bootStrap
                    source:""
                }


            },
            State {
                name: "regen"
                PropertyChanges {
                    target: bootStrap
                    sourceComponent:undefined
                }

                PropertyChanges {
                    target: app
                    sourceComponent:regenComponent
                }
            }
        ]
    }
}
