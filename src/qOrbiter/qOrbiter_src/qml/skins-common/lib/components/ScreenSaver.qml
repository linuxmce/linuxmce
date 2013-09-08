import QtQuick 1.1
import Qt.labs.shaders 1.0
/*!
 *\class ScreenSaver
 *\brief This is a general screensaver component for LinuxMCE QML based orbiters
 *
 *\ingroup lib_components
 *
 * This component is a general screen saver component for any skin to use. It will have a common
 * interface that provides a simple screen saver for designers to add effects or other treatments to in qml
 *\warn This component requires Opengl and the Qt.labs.shaders 1.0 module
 *\namespace Qt.labs.shaders 1.0
 *
 */
Rectangle{
    id:screensaver_root
    color: "transparent"
    anchors.fill: parent

    property bool enableEffects: opengl
    property int zoomTime: screensaver.transitionDuration - fadeTime
    property int fadeTime: 3500
    property int currentBGimg: 0

    Connections{
        target:screensaver
        onImageChanged:changeBGimage()
    }

    function changeBGimage(){
        if(imgBg1.active){
            imgBg2.source = "image://listprovider/screensaver/"+securityvideo.timestamp;
            imgBg2.active = true
            imgBg1.active= false

        }else{
            imgBg1.source= "image://listprovider/screensaver/"+securityvideo.timestamp;
            imgBg1.active = true
            imgBg2.active = false
        }
        //simulated random zoom function
        //origin seems to be the bottom middle of the scaled image (not the image element itself)
        //can maybe do some x,y move here to simulate a pan, but it will have to be within the contsraints of the scaled image
        //to avoid negative clipping (panning the edge of the image into the image element bounds)
        var temp = Math.random() * .25 + 1; // max scale factor is 1.25
        var from, to;
        if(Math.random() > .5) { // 50% choice between zoom in and out
            from = 1.0;
            to = temp;
        } else {
            from = temp;
            to = 1.0;
        }

        //setting the active image to zoom
        if (imgBg2.opacity == 1) {

            zoomimgBg1.from = from;
            zoomimgBg1.to = to;
            zoomimgBg1.start();
        } else {
            zoomimgBg2.from = from;
            zoomimgBg2.to = to;
            zoomimgBg2.start();
        }

        if(imgBg1.active){
            imgBg1.opacity = 1;
            imgBg2.opacity=0

        }else
        {
            imgBg2.opacity = 1;
            imgBg1.opacity = 0

        }


    }
    //    Timer { // Simulate a simple PhotoScreensaver
    //        id: ssTimer;
    //        interval: screensaver.transitionDuration;
    //        running: false;
    //        repeat: true
    //        onTriggered: changeBGimage()
    //    }


    Rectangle{
        id:bgrect
        anchors.fill: parent
        z:0
        color: "black"
        Image {
            id: imgBg1
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectCrop
            anchors.fill: parent
            opacity: 1
            smooth: true
            cache:false
            property bool active:true
            Behavior on opacity
            {
                SequentialAnimation{
                    PropertyAnimation{duration:enableEffects?fadeTime:0}
                   ScriptAction{script:if(opacity==0){imgBg1.source=""}}
                }
            }
            PropertyAnimation { id: zoomimgBg1; target: imgBg1; property: "scale"; from: 1.0; to: 1.25; duration: enableEffects?zoomTime:0 }
        }

        Image {
            id: imgBg2
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectCrop
            anchors.fill: parent
            opacity: 0
            smooth: true
            property bool active:false
            cache:false
            Behavior on opacity {
                SequentialAnimation{
                    PropertyAnimation{duration:enableEffects?fadeTime:0}
                   ScriptAction{script:if(imgBg2.opacity==0){ imgBg2.source=""}}
                }
            }
            PropertyAnimation { id: zoomimgBg2; target: imgBg2; property: "scale"; from: 1.0; to: 1.25; duration: enableEffects?zoomTime:0 }
        }

        Component.onCompleted: {
            changeBGimage();
            screensaver.setDuration(30000)
        }
    }
}
