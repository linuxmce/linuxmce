import QtQuick 2.2
import QtGraphicalEffects 1.0
import QtMultimedia 5.0
import QtQuick.Window 2.0
import QtQuick.Controls 1.2
import "splash"
import "skins"
/*
  Root Item.
  */
ApplicationWindow {
    id:qml_root_item
    height: manager.appHeight
    width: manager.appWidth

    visible: true
    color:"transparent"
    property string appEntryQml:  manager.skinEntryFile

    Connections{
        target:manager
        onUiReadyChanged:appContent.state="app"
    }


    menuBar: MenuBar{

        Menu{
            title: qsTr("&File")
        }

    }

    Component{
        id:indexRootComponent
        IndexContent{

        }
    }

    Component{
        id:splashRootCompnent
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
        //            if(window.orbiterInitialized){
        //                state="init"
        //            } else {
        //                state="preinit"
        //            }
        //        }

        onWidthChanged: console.log("qml_index width::"+width)

        //        Timer{
        //            id:pusher
        //            interval: 2000
        //            onTriggered: bootStrap.source="splash/Splash.qml"
        //            //running:true
        //        }

        Loader{
            id:app
            anchors.fill: parent
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
                    sourceComponent:splashRootCompnent
                }
                PropertyChanges {
                    target: app
                    source:""
                }
            },
            State {
                name: "app"
                PropertyChanges {
                    target: bootStrap
                    sourceComponent:undefined
                }

                PropertyChanges {
                    target: app
                    source:appEntryQml
                }

            }
        ]
    }
}
