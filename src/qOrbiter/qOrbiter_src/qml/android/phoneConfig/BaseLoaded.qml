import QtQuick 1.1

Item {
    anchors.fill: parent
    property string routerAddress:window.router
    Component.onCompleted: {
        console.log("BaseLoaded")
        forceActiveFocus();
        wait.start()
        if(androidSystem.updateBuildInformation() && androidSystem.updateExternalStorageLocation()){
            logger.setLogLocation(androidSystem.externalStorageLocation)
        }
    }
Rectangle{
    color:"black"
    anchors.fill: parent
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

    Timer{
        id:wait
        interval: 1000
        onTriggered: {mainContent.source = "http://"+routerAddress+"/lmce-admin/skins/android/splash/Splash.qml"; console.log("conecting")}
        running:false
        repeat:true
    }

    Text {
        id: loading
        text: qsTr("Connecting to "+routerAddress+", please be patient \n" )
        anchors.top: parent.top
        font.pixelSize: 22
        width: parent.width
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: "white"
        anchors.horizontalCenter: parent.horizontalCenter
        font.weight: Font.Light
    }

    Item{
        id:fil
        anchors.centerIn: parent
        height: 45
        width: 400

        Rectangle{
            anchors.fill: parent
            color:"green"
            border.color: "white"
            border.width: 1
            radius:5
            opacity:switched ? 1 : 0
            property bool switched:false
            Text {
                id: splashtxt
                text:  androidSystem.deviceName
                anchors.bottom: parent.bottom
                font.pixelSize: 28
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
                font.weight: Font.Light
            }

            Timer{
                id:sw
                running:true
                interval:800

                onTriggered: {
                    parent.switched = !parent.switched
                }
                repeat: true
            }

            Behavior on opacity{
                PropertyAnimation{
                    duration: 750
                }
            }
        }
    }

    Loader{
        id:mainContent
        height: manager.appHeight
        width: manager.appWidth
        source:""
        opacity: 0
        onOpacityChanged: PropertyAnimation {target:mainContent; property: "opacity"; to:1 ; duration: 1500}
        onStatusChanged: if(mainContent.status === Loader.Error){
                             loading.text = qsTr("Im Sorry I couldnt connect to a LinuxMCE Server at "+window.router+" Please ensure you can reach your core. \n I will continue trying. \n"+ mainContent.sourceComponent.errorString())
                             console.log(mainContent.sourceComponent.errorString())
                             wait.stop()
                             configOptions.state = "editing"
                         }
                         else  if (mainContent.status === Loader.Ready){
                             mainContent.opacity = .01
                             loading.visible= false
                             loading.text = "Content Loaded, one moment"
                         }

    }

    FirstRunOptions{
        id:configOptions

    }

}
