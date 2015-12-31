import QtQuick 2.2
import QtGraphicalEffects 1.0
import AudioVisual 1.0
import DceScreenSaver 1.0

Item {
    anchors.fill: parent
    height: 720
    width: 1280
    Component.onCompleted: {
        if(orbiterWindow.orbiterInitialized){
            state="init"

        } else {
            state="preinit"
        }
    }



    Timer{
        id:pusher
        interval: 3000
        onTriggered: bootStrap.source = "http://192.168.80.1/lmce-admin/skins/Index.qml"
        running:true
    }
    Image{
        source: "images/splash-bg.png"
        anchors.fill:parent
    }


    Rectangle{
        anchors.fill: parent
        opacity:.85
        gradient: Gradient{
            GradientStop{
                position: 0.0
                color:"black"
            }
            GradientStop{
                position: .56
                color:"transparent"
            }
            GradientStop{
                position: .75
                color:"black"
            }
        }
    }

    Loader{
        id:bootStrap
        anchors.fill: parent
        source:orbiterWindow.orbiterInitialized ? "BaseLoaded.qml" : ""
    }

    Text {
        id: splashText
        text: qsTr("QOrbiter for Linux")
        color:"white"
        font.pointSize: 42
        anchors{
            top:parent.top
            horizontalCenter: parent.horizontalCenter
        }
    }

    Text{
        id:loadingText
        text:"Please Wait"
        color:"white"
        anchors.centerIn: parent
        font.pointSize: 32
        font.weight: Font.Light
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
        },
        State {
            name: "init"
            PropertyChanges {
                target: splashText
                text:"Connection Details"
            }
        }
    ]
}
