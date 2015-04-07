import QtQuick 2.0
import DceScreenSaver 1.0
import "../../../../skins-common/lib/handlers"

Item{
    id:qml_root
    height: manager.appHeight
    width: manager.appWidth

    function scaleX(x){
        return x/100*qml_root.width
    }
    function scaleY(y){
        return y/100*qml_root.height
    }

    Style{
        id:appStyle
    }

    //Required function. How handle it is up to to. It provides a simple string like "Screen_1.qml"
    function screenchange(screenname ){

    }

    DceScreenSaver{
        id:glScreenSaver
        anchors{
            top:qml_root.top
            bottom:qml_root.bottom
            left: qml_root.left
            right: qml_root.right
        }

        interval:30000

        requestUrl:manager.m_ipAddress
        Component.onCompleted: {
            glScreenSaver.setImageList(manager.screensaverImages)
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
             manager.exitApp()
            }
        }

    }
}
