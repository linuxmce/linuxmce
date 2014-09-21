import QtQuick 1.0
import "../components"
/*
screen params for this screen are
2 - value of the device
159 - the Screen
*/
StyledScreen{
    id:singlecameraview
    property int currentInterval:0
    onScreenOpening: {
        currentInterval=glScreenSaver.interval
        glScreenSaver.interval=300000000
    }

    onScreenClosing: {
        glScreenSaver.interval=currentInterval
        currentInterval=0
    }
    property int camera:screenparams.getParam(2)
    Timer{
        id:securitytimer
        repeat: true
        interval: 1500
        triggeredOnStart: true
        running: true
        onTriggered:{

            "image://listprovider/securityimage/"+camera+"/"+securityvideo.timestamp
            manager.requestSecurityPic(screenparams.getParam(2), 640, 480)
        }
    }

    Connections{
        target: securityvideo
        onImageUpdated:{
            "image://listprovider/securityimage/"+camera+"/"+securityvideo.timestamp
            manager.requestSecurityPic(screenparams.getParam(2), 640, 480)
        }
    }



    Panel{
        id:securitycamrect
        anchors.fill: parent

        headerTitle: "Single Camera View"

        Image {
            id: securityimage
            fillMode: Image.PreserveAspectFit
            source: "image://listprovider/securityimage/"+camera+"/"+securityvideo.timestamp
           anchors.fill: parent
        }
    }


}
