import QtQuick 1.1

Item {
    anchors.fill: parent
    property string routerAddress:manager.usingExternal ? manager.externalHost : manager.internalHost

    function refresh(){
        if(manager.usingExternal)
            routerAddress=manager.externalHost
        else
            routerAddress=manager.internalHost
    }

    onRouterAddressChanged: {
     orbiterWindow.router=routerAddress
      wait.start()
    }

    Component.onCompleted: {
        console.log("BaseLoaded")
        forceActiveFocus();
        wait.start()
        if(androidSystem.updateBuildInformation() && androidSystem.updateExternalStorageLocation()){
            logger.setLogLocation(androidSystem.externalStorageLocation)
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

    Timer{
        id:wait
        interval: 1000
        onTriggered: {
            mainContent.source = "http://"+routerAddress+"/lmce-admin/skins/android/splash/Splash.qml";
        }
        running:false
        repeat:true
    }

    Item{
        id:contentRect
        anchors.centerIn: parent
        height: parent.height
        width: parent.width *.85

        Text {
            id: loading
            text: qsTr("Currently Connecting to"+routerAddress+"\n, please be patient \n" )
            anchors.top: parent.top
            anchors.left: parent.left
            font.pixelSize: 22
            width: parent.width/2
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            color: "white"
            anchors.horizontalCenter: parent.horizontalCenter
            font.weight: Font.Light
        }
        Text {
            id: selectOther
            text:"Connection Points.\n Last Using: "+manager.usingExternal ? "External connection" : "Internal connection"
            font.pixelSize: 22
            color: "white"
            font.weight: Font.Light
            anchors.top: loading.bottom
            anchors.right: parent.right
            anchors.topMargin: 15
        }


        Column{
            id:addresses
            width: childrenRect.width
            anchors{
                right:parent.right
                top:selectOther.bottom
            }
            height: parent.height*.25
            spacing:parent.height*.05

            Text {
                id: internalAddy
                text:"Home Address: "+manager.internalHost
                font.pixelSize: manager.internalHost===routerAddress ? 28: 22
                color: "white"
                font.weight: Font.Light
                MouseArea{
                    anchors.fill: parent
                    onReleased: {
                        routerAddress=manager.internalHost;
                        manager.usingExternal = false
                        manager.writeConfig()
                    }
                }
            }
            Text {
                id: externalAddy
                text:"Mobile Address: "+manager.externalHost
                font.pixelSize:manager.externalHost === routerAddress ?28 : 22
                color: "white"
                font.weight: Font.Light
                MouseArea{
                    anchors.fill: parent
                    onReleased:{ routerAddress=manager.externalHost;  manager.usingExternal = true }
                }
            }
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

    }





    Loader{
        id:mainContent
        height: manager.appHeight
        width: manager.appWidth
        source:""
        opacity: 0
        onOpacityChanged: PropertyAnimation {target:mainContent; property: "opacity"; to:1 ; duration: 1500}
        onStatusChanged: if(mainContent.status === Loader.Error){
                             var es = String(mainContent.sourceComponent.errorString())
                             if(es.match("-1")){
                                 loading.text="It Seems we timed out connecting to "+routerAddress+"\nPlease:\n*Choose Another Connection Below\n*Check your wireless connections\n*Ensure you have the correct\naddress"
                             }

                            // loading.text = qsTr("Im Sorry I couldnt connect to a LinuxMCE Server at "+routerAddress+"\n Please ensure you can reach your core. \n I will continue trying. \n"+ mainContent.sourceComponent.errorString())
                             console.log(mainContent.sourceComponent.errorString())
                             wait.stop()
                             configOptions.state = "editing"
                         }
                         else  if (mainContent.status === Loader.Ready){
                             mainContent.opacity = .01
                             loading.visible= false
                             loading.text = "Content Loaded, one moment"
                             configOptions.visible=false
                             fil.visible=false;
                             addresses.visible=false;
                             contentRect.visible=false;
                         }

    }

    FirstRunOptions{
        id:configOptions

    }

}
