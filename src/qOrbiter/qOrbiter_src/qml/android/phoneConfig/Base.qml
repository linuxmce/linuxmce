import QtQuick 1.1

Item {
    id:rootItem
    anchors.fill: parent
    focus:true

    Connections{
        target: orbiterWindow
        onBeginLoading:{
            bootStrap.source="BaseLoaded.qml"
        }
    }

    Image{
        fillMode: Image.PreserveAspectCrop
        source: "qrc:../img/Splash.png"
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
        case 16777220:
            console.log("close input panel")
        default:
            console.log(event.key)
            break;

        }
        event.accepted=true
    }

    Text{
        anchors.centerIn: parent
        font.pixelSize: 24
        text:"LinuxMCE is loading\n Please Wait."
        font.bold: true
        color: "white"
        visible:bootStrap.source===""

    }

    Loader{
        id:bootStrap
        anchors.fill: parent
        source:orbiterWindow.orbiterInitialized ? "BaseLoaded.qml" : ""
    }

    Timer{
        id:pusher
        interval: 3000
        onTriggered: bootStrap.source="BaseLoaded.qml"
        running:true
    }

}
