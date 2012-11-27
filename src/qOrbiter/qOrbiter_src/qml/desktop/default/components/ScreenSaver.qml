import QtQuick 1.0
import Qt.labs.shaders 1.0
import "../effects"

Rectangle {
    id:ssholder
    width: manager.appWidth
    height: manager.appHeight
    color: "black"

    property int currentXoffset:100
    property int currentYoffset:100
    clip:false

    function changeStuff()
    {
        lower.running =true
        fadeAndBlur.running = false
        currentYoffset = manager.appHeight *.25

    }

    function changePic()
    {
        ssimg.source = "image://listprovider/screensaver/"+securityvideo.timestamp
        blur.scale = 1.25
        blur.x = currentXoffset
        blur.y = currentYoffset
        raise.running = true
        fadeAndBlur.running = true
        burnsEffect.running = true
    }

    function startZoom()
    {

    }

    Image {
        id: ssimg
        width: manager.appWidth
        height: manager.appHeight
        source: "image://listprovider/screensaver"+screensaver.timestamp      
        smooth: true

        ParallelAnimation {
            id:raise
            running: false

            PropertyAnimation{ target:ssimg; property: "opacity"; to: "1"; duration: 1500}


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
        dividerValue: 1
        opacity: 1
        radius: 0.75

        targetHeight: ssimg.height
        targetWidth: ssimg.width
        source: ShaderEffectSource { sourceItem: ssimg; hideSource: true }
    }

    SequentialAnimation{
        id:fadeAndBlur
        running: false
        PropertyAnimation { target: blur; property: "radius"; to: 0.75; duration: 250}
        // PropertyAnimation {target: blur; property: "opacity"; to:0 ; duration: 1000 }
        PropertyAnimation { target: blur; property: "radius"; to: 0.0; duration: 2000}
        //  PropertyAnimation {target: blur; property: "opacity"; to:1 ; duration: 1000 }
        PauseAnimation { duration: screensaver.transitionDuration - 5000 }
        PropertyAnimation { target: blur; property: "radius"; to: 0.75; duration: 1000}
        // PropertyAnimation { target: blur; property: "radius"; to: .5; duration: 5000}
    }

   ParallelAnimation{
       id:burnsEffect
       running:false
       PropertyAnimation{target:blur; property:"scale";to:1; duration: screensaver.transitionDuration }
       PropertyAnimation{target: blur; property: "x"; to:0; duration: screensaver.transitionDuration }
       PropertyAnimation{target: blur; property: "y"; to: 0; duration: screensaver.transitionDuration }
   }
}
