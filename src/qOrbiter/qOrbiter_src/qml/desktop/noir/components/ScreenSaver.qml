import QtQuick 1.0
import Qt.labs.shaders 1.0
import "../effects"

Rectangle {
    id:ssholder
    anchors.fill: parent
    color: "black"

    function changeStuff()
    {

            lower.running =true
            animation_section2.running = false



    }

    function changePic()
    {

            ssimg.scale= 1
            ssimg.source = "image://listprovider/screensaver/"+securityvideo.timestamp
            raise.running = true
            animation_section2.running = true

    }

    function startZoom()
    {

    }

    Image {
        id: ssimg
        height: manager.appHeight
        width: manager.appWidth
        source: "image://listprovider/screensaver"+screensaver.timestamp
        anchors.centerIn: parent
        smooth: true

        ParallelAnimation {

            id:raise
            running: false
            PropertyAnimation{ target:ssimg; property: "opacity"; to: "1"; duration: 1500}
            PropertyAnimation{ target:ssimg; property: "scale"; to:1.15; duration: 6000}

        }

        SequentialAnimation on opacity {

            id:lower
            running:false
            PropertyAnimation{ target:ssimg; property: "opacity"; to: "0"; duration: 1000}
            ScriptAction {script: changePic()}
        }

        Connections{
            target: screensaver
            onImageChanged:changeStuff()
        }
    }
/*
    EffectBillboard{
        id: billboard
        anchors.fill: ssimg
        divider: false
        dividerValue: 1.0

        targetHeight: ssimg.height
        targetWidth: ssimg.width
        source: ssimg
        grid:6.0

        //source: ShaderEffectSource { sourceItem: ssimg; hideSource: true }
        SequentialAnimation{
            id:animation_section
            loops: Animation.Infinite
            running: true
           // PropertyAnimation {target: blur; property: "opacity"; to:0 ; duration: 1000 }
            PropertyAnimation { target: billboard; property: "grid"; to: 3.0; duration: 5000}
          //  PropertyAnimation {target: blur; property: "opacity"; to:1 ; duration: 1000 }
            PropertyAnimation { target: billboard; property: "grid"; to: 6.0; duration: 5000}
             PropertyAnimation { target: billboard; property: "grid"; to: 3.0; duration: 5000}
        }
    }
*/
    EffectGaussianBlur{
        id: blur
        anchors.fill: ssimg
        divider: false
        dividerValue: 1.0
        opacity: 1
        radius: 0.75
        targetHeight: ssimg.height
        targetWidth: ssimg.width
        source: ShaderEffectSource { sourceItem: ssholder; hideSource: false }

    }
    SequentialAnimation{
        id:animation_section2

        running: false
        PropertyAnimation { target: blur; property: "radius"; to: 0.75; duration: 250}
       // PropertyAnimation {target: blur; property: "opacity"; to:0 ; duration: 1000 }
        PropertyAnimation { target: blur; property: "radius"; to: 0.0; duration: 2000}
      //  PropertyAnimation {target: blur; property: "opacity"; to:1 ; duration: 1000 }
        PauseAnimation { duration: screensaver.transitionDuration -2000 }
        PropertyAnimation { target: blur; property: "radius"; to: 0.75; duration: 1000}
        // PropertyAnimation { target: blur; property: "radius"; to: .5; duration: 5000}
    }
}
