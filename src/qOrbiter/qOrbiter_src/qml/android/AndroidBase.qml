import QtQuick 2.2
import QtGraphicalEffects 1.0
import QtMultimedia 5.0
Item {
    height: manager.appHeight
    width: manager.appWidth

    Component.onCompleted: {
        state="init"

    }

    Timer{
        id:pusher
        interval: 3000
        onTriggered:state="splashscreen"
        running:false
    }

    Image {
        id: bg
        fillMode: Image.PreserveAspectCrop
        source: "qrc:../img/Splash.png"
        anchors.fill: parent
    }


    Rectangle{
        anchors.fill: parent
        opacity:.85
        gradient: Gradient{
            GradientStop{
                id:top
                position: 0.0
                color:"black"
            }
            GradientStop{
                id:mid
                position: 0
                color:"transparent"
                Behavior on position {
                    PropertyAnimation{
                        duration: 350

                    }
                }
            }
            GradientStop{
                id:btm
                position: .75
                color:"black"
            }
        }
    }

    Text {
        id: splashText
        text: qsTr("QOrbiter for Android")
        color:"white"

        anchors{
            top:parent.top
            horizontalCenter: parent.horizontalCenter
        }
    }

    Text{
        id:loadingText
        text:qsTr("One Moment please...")
        color:"white"
        anchors.centerIn: parent
        font.pointSize: 32
    }

    Item{
        id:infoPanel
        anchors.centerIn: parent
        width: parent.width/3
        height: parent.width /3

        Column{
            id:device_data_column
            anchors.fill: parent
            Text{
                color:"white"
                text:"Device: "+androidSystem.deviceName
            }
            Text{
                text:"Storage Location: "+androidSystem.externalStorageLocation
                color:"white"
            }
            Text{
                color:"white"
                text:"Api Level: "+androidSystem.apiLevel
            }
            Text{
                color:"white"
                text:"Brand: "+androidSystem.deviceBrand
            }
            Text{
                color:"white"
                text:"Logs & Config: "+androidSystem.externalStorageLocation+"/Linuxmce/"
            }
        }

    }
    Loader{
        id:splashTarget
        anchors.fill: parent
        visible:false
        onStatusChanged: {
            if(source!=="" && status==Loader.Ready){
                loadingText.text=""
            }
        }
    }
    states: [
        State {
            name: "preinit"
            PropertyChanges {
                target: splashText
                visible:true
            }

            PropertyChanges {
                target: loadingText
                visible:true
            }
            PropertyChanges {
                target: infoPanel
                visible:false
            }
            PropertyChanges {
                target: splashTarget
                visible:false
            }
        },
        State {
            name: "init"
            PropertyChanges {
                target: splashText
                text:"Connection Details"
            }
            PropertyChanges{
                target: loadingText
                text:"Setting up:" +androidSystem.deviceName
            }
            PropertyChanges {
                target: mid
                position:.56
            }
            PropertyChanges{
                target:pusher
                running:true
            }

            PropertyChanges {
                target: infoPanel
                visible: true
            }
            StateChangeScript{
                script: {
                    if(androidSystem.externalStorageLocation!==""){
                        logger.setLogLocation(androidSystem.externalStorageLocation)
                    }
                }
            }
        }, State{
            name:"splashscreen"
            extend:"init"
            PropertyChanges {
                target: infoPanel
                visible:false
            }
            PropertyChanges{
                target: loadingText
                text:"Connecting to :" +manager.internalHost
            }
            PropertyChanges {
                target: splashTarget
                visible:true
                source:"http://"+manager.internalHost+"/lmce-admin/skins/android/tablet/qt5/splash/Splash.qml";
            }
            PropertyChanges {
                target: splashText
                visible:false
            }
        },
        State{
            name:"errorstate"
            PropertyChanges{
                target:infoPanel
                visible:true
            }
            PropertyChanges{
                target: device_data_column
                visible:false
            }
        }

    ]
}
