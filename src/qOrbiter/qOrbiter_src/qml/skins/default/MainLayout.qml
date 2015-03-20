import QtQuick 2.3
import QtGraphicalEffects 1.0
import AudioVisual 1.0
import DceScreenSaver 1.0
import QtMultimedia 5.0
/*! This File is designed to be the main layout that can be switched in and out for various forms */
Item {
    anchors.fill: qmlRoot
    Rectangle{
        anchors.fill: parent
        color:"black"
    }

    DceScreenSaver{
        id:glScreenSaver

        height:parent.height
        width:parent.width

        enableDebug: true
        interval:60*1000
        useAnimation: true
        //onDebugInfoChanged: console.log(debugInfo)
        active:manager.connectedState
        requestUrl:manager.m_ipAddress
        Component.onCompleted: {
            glScreenSaver.setImageList(manager.screensaverImages)
            console.log("Orbiter Consume Screensaver images")
            console.log("Orbiter counts " + glScreenSaver.pictureCount)
        }

        Connections{
            target:manager
            onScreenSaverImagesReady:{
                glScreenSaver.setImageList(manager.screensaverImages)
                console.log("Orbiter Consume Screensaver images")
                console.log("Orbiter counts " + glScreenSaver.pictureCount)
            }
        }

        MouseArea{
            anchors.fill: parent
            onClicked: {
                if(!uiOn){
                    console.log("screensaver revive")
                    uiOn=true
                }
            }
        }

    }

    Text{
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        text:"Main Layout"
        color:"white"
        font.pointSize: 65
    }

    Rectangle{
        width: parent.width *.75
        height: parent.height *.25
        anchors{
            bottom:parent.bottom
            horizontalCenter: parent.horizontalCenter
        }

        radius:10
        color:"grey"
        Text{
            anchors.centerIn: parent
            text:"Switch to Newtwork Skins"
            color:"white"
            font.pointSize: 65
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            width: parent.width
        }
        MouseArea{
            anchors.fill: parent
            onClicked: {
                manager.setUseLocalSkins(!manager.useLocalSkins)
                settings.destroySettingsData();

            }
        }
    }
}
