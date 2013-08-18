import QtQuick 2.0

Rectangle {
    id:welcome
    height: manager.appHeight
    width: manager.appWidth
    color: wait.running ? "darkyellow": "darkgreen"

    Keys.onReleased: {

            switch(event.key){
            case Qt.Key_Back:
                Qt.quit()
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
        interval: 3500
        onTriggered: {mainContent.source = "http://"+window.router+"/lmce-admin/skins/android/qt5/Splash.qml"; }
        running:false
        Component.onCompleted:
        {
            forceActiveFocus()
            wait.start()
            if(androidSystem.updateBuildInformation() && androidSystem.updateExternalStorageLocation()){
                wait.start()
                logger.setLogLocation(androidSystem.externalStorageLocation)
            }
        }
    }

    Text {
        id: loading
        text: qsTr("Connecting to "+window.router+", please be patient \n" )
        anchors.top: parent.top
        font.pixelSize: 28
        width: parent.width
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: "white"
        anchors.horizontalCenter: parent.horizontalCenter
        font.weight: Font.Light
    }

    Text {
        id: splashtxt
        text: qsTr("LinuxMCE for your " + androidSystem.deviceName )
        anchors.bottom: parent.bottom
        font.pixelSize: 28
        width: parent.width
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: "green"
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
                                 wait.restart()
                             }
            //                         else if (mainContent.status != Loader.Loading){
            //                             loading.text = "Loading, please wait \n" + progress +"% \n"+sourceComponent.errorString()

            //                         }
                             else  if (mainContent.status === Loader.Ready){
                                 mainContent.opacity = .01
                                 loading.visible= false
                                 loading.text = "Content Loaded, one moment"
                             }

        }



    }
