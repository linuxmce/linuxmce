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
    Component.onCompleted: requestSecurityPic(screenparams.getParam(2), 640, 480)

    Timer{
        id:securitytimer
        repeat: true
        interval: 2500
        triggeredOnStart: true
        running: true
        onTriggered:{
            "image://listprovider/securityimage/"+camera+"/"+securityvideo.timestamp
            console.log("requesting new security image")
            manager.requestSecurityPic(screenparams.getParam(2), 640, 480)
        }
    }

    StyledText {
        id: singlecamlabel
        text: "Single Camera view"
        anchors{
            top:parent.top
            left:parent.left
        }
    }


    Rectangle{

        id:securitycamrect
        height: scaleY(50)
        width: scaleX(50)
        border.color: "black"
        border.width: 2
        anchors.centerIn: parent
        color:"black"

        Image {
            id: securityimage
            fillMode: Image.PreserveAspectFit
            source: "image://listprovider/securityimage/"+camera+"/"+securityvideo.timestamp
            anchors.centerIn: parent
            height: scaleY(50)
            width: scaleX(50)
        }

        Row{
            height: scaleY(10)
            width: scaleX(15)
            anchors.top: securityimage.bottom
            anchors.bottom: securitycamrect.bottom

            Text {
                id: buttonplaceholder
                text: qsTr("Camera label and controls go here")
            }
        }
    }
}
