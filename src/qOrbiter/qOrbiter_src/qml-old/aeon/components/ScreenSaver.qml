import QtQuick 1.1
import Qt.labs.shaders 1.0
import "../effects"
Rectangle{
    height: style.orbiterH
    width: style.orbiterW
    color: "transparent"
    anchors.fill: parent

    property bool enableEffects: opengl
    property int zoomTime: screensaver.transitionDuration - fadeTime
    property int fadeTime: 3500
    property int currentBGimg: 0
    function getBGimage(){
        return "image://listprovider/screensaver/"+securityvideo.timestamp;
    }
    function changeBGimage(){
        var newSource = getBGimage()
        var currentImg=imgBg1, newImg=imgBg2;
        if (imgBg2.opacity == 1) {
            currentImg = imgBg2;
            newImg = imgBg1;
        }
        newImg.source = newSource;
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
        if (imgBg2.opacity == 1) {
            zoomimgBg1.from = from;
            zoomimgBg1.to = to;
            zoomimgBg1.start();
        } else {
            zoomimgBg2.from = from;
            zoomimgBg2.to = to;
            zoomimgBg2.start();
        }

        newImg.opacity = 1;
        currentImg.opacity = 0;
    }
    Timer { // Simulate a simple PhotoScreensaver
        id: ssTimer;
        interval: screensaver.transitionDuration;
        running: false;
        repeat: true
        onTriggered: changeBGimage()
    }

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
            Behavior on opacity {
                PropertyAnimation{duration:enableEffects?fadeTime:0}
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
            Behavior on opacity {
                PropertyAnimation{duration:enableEffects?fadeTime:0}
            }
            PropertyAnimation { id: zoomimgBg2; target: imgBg2; property: "scale"; from: 1.0; to: 1.25; duration: enableEffects?zoomTime:0 }
        }


    }
    Component.onCompleted: {
        changeBGimage();
        ssTimer.start();
        screensaver.setDuration(30000)
    }
}
