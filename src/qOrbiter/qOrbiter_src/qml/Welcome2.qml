import QtQuick 2.2
import QtGraphicalEffects 1.0
Item {
    anchors.fill: parent

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
        onTriggered: bootStrap.source="BaseLoaded.qml"
        running:true
    }

    Image {
        id: bg
        source: "qrc:../img/Splash.png"
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
        text: qsTr("QOrbiter for Android")
        anchors{
            top:parent.top
            horizontalCenter: parent.horizontalCenter
        }
    }

    Text{
        id:loadingText
        text:"Please Wait, Loading"
        color:"white"
        anchors.centerIn: parent
        font.pointSize: 32
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
