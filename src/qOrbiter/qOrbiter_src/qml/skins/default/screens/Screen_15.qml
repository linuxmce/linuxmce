import QtQuick 2.2
import "../components"
import "../"
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
        height: appStyle.scaleY(75)
        width: appStyle.scaleX(75)
        border.color: "black"
        border.width: 2
        anchors.centerIn: parent
        color:"black"

        Image {
            id: securityimage
            fillMode: Image.PreserveAspectFit
            source: "image://listprovider/securityimage/"+camera+"/"+securityvideo.timestamp
          anchors.fill: parent
        }

        Row{
            height: appStyle.scaleY(10)
            width: appStyle.scaleX(15)
            anchors.top: securityimage.bottom
            anchors.bottom: securitycamrect.bottom
            StyledButton {
                label: "Left"
                onActivated: manager.moveLeft(camera)
            }
            StyledButton {
                label: "Right"
                onActivated: manager.moveRight(camera)
            }
            StyledButton {
                label: "Up"
                onActivated: manager.moveUp(camera)
            }
            StyledButton {
                label: "Down"
                onActivated: manager.moveDown(camera)
            }
        }
    }
}
