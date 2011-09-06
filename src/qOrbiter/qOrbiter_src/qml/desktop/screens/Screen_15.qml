import QtQuick 1.0
import "../components"
/*
screen params for this screen are
2 - value of the device
159 - the Screen
*/
Item{
    id:climate

    Rectangle{
        height: style.orbiterH
        width: style.orbiterW
        color: "transparent"
        Component.onCompleted: requestSecurityPic(screenparams.getParam(2), 640, 480)
        Text {
            id: climatelabel
            x: 74
            y: 101
            text: "Single Camera view"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15
            color: style.titletextcolor
        }
        Rectangle{
            id:securitycamrect
            height: scaleY(50)
            width: scaleX(50)
            border.color: "black"
            border.width: 2
            anchors.centerIn: parent

            Image {
                id: securityimage
                fillMode: Image.PreserveAspectFit
                source: "image://securityvideo/"+screenparams.getParam(2)
                anchors.centerIn: parent
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

        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }

}
