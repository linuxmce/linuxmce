import QtQuick 1.0
import "../components"
/*
screen params for this screen are
2 - value of the device
159 - the Screen
*/
StyledScreen{
    id:singlecameraview
    property int camera:screenparams.getParam(2)
    Timer{
        id:securitytimer
        repeat: true
        interval: 1000
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
        height: childrenRect.height
        width: childrenRect.width
        anchors.centerIn: parent

        headerTitle: "Single Camera View"

        Image {
            id: securityimage
            fillMode: Image.PreserveAspectFit
            source: "image://listprovider/securityimage/"+camera+"/"+securityvideo.timestamp
            anchors.centerIn: parent
            width: scaleX(85)
        }
    }


}
