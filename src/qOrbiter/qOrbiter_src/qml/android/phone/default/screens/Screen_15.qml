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

    StyledText {
        id: singlecamlabel
        text: "Single Camera view"
        font.pointSize: 15
        anchors{
            top:parent.top
            left:parent.left
        }
    }

    Rectangle{
        id:securitycamrect
        height: childrenRect.height
        width: childrenRect.width
        border.color: "black"
        border.width: 2
        anchors.centerIn: parent
        color: "transparent"

        Image {
            id: securityimage
            fillMode: Image.PreserveAspectFit
            source: "image://listprovider/securityimage/"+camera+"/"+securityvideo.timestamp
            anchors.centerIn: parent
            width: scaleX(85)

        }

        Rectangle{
            height: scaleY(10)
            width: scaleX(15)
            anchors.top: securityimage.bottom
            anchors.horizontalCenter: securitycamrect.horizontalCenter

            Text {
                id: buttonplaceholder
                text: qsTr("Camera label and controls go here")
            }
        }
    }


}
