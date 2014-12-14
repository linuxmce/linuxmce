import QtQuick 2.2

Item {
    anchors.fill: parent

    Component.onCompleted: {
        if(window.orbiterInitialized){
            state="init"
        } else {
            state="preinit"
        }
    }


    Timer{
        id:pusher
        interval: 3000
        onTriggered:state="init"
        running:true
    }

    Image {
        id: bg
        source: "qrc:../img/Splash.png"
        anchors.fill: parent
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

   Item{
    id:infoPanel
    anchors.centerIn: parent
    width: parent.width/2
    height: parent.width /2

   Column{
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
           text:"Manufacturer: "+androidSystem.deviceManufacturer
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
        },
        State {
            name: "init"
            PropertyChanges {
                target: splashText
                text:"Connection Details"
            }
            PropertyChanges{
                target: loadingText
                visible:false
            }

            PropertyChanges {
                target: infoPanel
                visible: true
            }
        }
    ]
}
