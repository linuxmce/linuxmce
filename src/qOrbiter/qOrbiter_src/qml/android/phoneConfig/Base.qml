import QtQuick 1.1

Item {
    id:rootItem
    anchors.fill: parent
    focus:true

    Connections{
        target: window
        onBeginLoading:{
            bootStap.source="BaseLoaded.qml"
        }
    }



    Rectangle{
        anchors.fill: parent
        color: "green"
    }

    Rectangle{
        anchors.fill: parent
        opacity:.85
        gradient: Gradient{
            GradientStop{
                position: 0.0
                color:"transparent"
            }
            GradientStop{
                position: .56
                color:"black"
            }
        }
    }

    Keys.onReleased: {
        switch(event.key){
        case Qt.Key_Back:
            manager.exitApp()
            break;
        case Qt.Key_MediaPrevious:
            Qt.quit()
            break;
        default:
            console.log(event.key)
            break;

        }
        event.accepted=true
    }

    Text{
        anchors.centerIn: parent
        font.pixelSize: 24
        text:"width: "+rootItem.width+" X height "+rootItem.height+"\n LinuxMCE is loading\n Please Wait."
        font.bold: true
        color: "white"

    }

    Loader{
        id:bootStap
        anchors.fill: parent
        source:window.orbiterInitialized ? "BaseLoaded.qml" : ""
    }

    Timer{
        id:pusher
        interval: 3000
        onTriggered: bootStap.source="BaseLoaded.qml"
        running:true
    }

}
