import QtQuick 2.3
import QtGraphicalEffects 1.0
import AudioVisual 1.0
import DceScreenSaver 1.0
import QtMultimedia 5.0
import "."
import "components"
/*! This File is designed to be the main layout that can be switched in and out for various forms */
Item {
    id:layout
    anchors.fill: qmlRoot
    Rectangle{
        anchors.fill: parent
        color:"black"
    }
    property bool uiOn:true

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
              uiOn=!uiOn
            }
        }

    }

    PageLoader {
        id: pageLoader
        anchors{
            top:header.bottom
            left:layout.left
            right:layout.right
            bottom:layout.bottom
        }
    }

    DefaultHeader {
        id: header
    }

}
