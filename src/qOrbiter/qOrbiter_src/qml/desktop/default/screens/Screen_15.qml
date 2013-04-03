import QtQuick 1.1
import "../components"
/*!
  \brief single camera view for security camera.
\note screen params for this screen are
@param 2 - value of the device
@param 159 - the Screen

\note access Screen Parameters via
\code manager.getParam(int param) \endcode
*\ingroup qml_desktop_default
*/

Item{
    id:singlecameraview
    property int camera:screenparams.getParam(2) /*!< \brief the camera device number in this screen. */
    height: manager.appHeight
    width: manager.appWidth
    Connections{
        target: securityvideo
        onImageUpdated:{
            "image://listprovider/securityimage/"+camera+"/"+securityvideo.timestamp
            manager.requestSecurityPic(screenparams.getParam(2), 640, 480)
        }
    }



    Rectangle{
        anchors.fill: parent
        color: "transparent"
        Component.onCompleted: requestSecurityPic(screenparams.getParam(2), 640, 480)
        anchors.centerIn: parent
        StyledText {
            id: singlecamlabel
            x: 74
            y: 101
            text: "Single Camera view"
            font.family: "Droid Sans"
            font.bold: false
            font.pointSize: 15

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
                source: "image://listprovider/securityimage/"+camera+"/"+securityvideo.timestamp
                anchors.centerIn: parent
                height: scaleY(50)
                width: scaleX(50)
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
