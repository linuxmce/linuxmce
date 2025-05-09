import QtQuick 1.0
import Qt.labs.shaders 1.0

Rectangle {
    id:ssholder
    width: appW
    height: appH
    color: "black"

    Component.onCompleted: screensaver.transitionDuration = 60000



    function changeStuff()
    {
        lower.running =true
        animation_section2.running = false
    }

    function changePic()
    {
            ssimg.source = "image://listprovider/screensaver/"+securityvideo.timestamp
            raise.running = true
            animation_section2.running = true
    }

    function startZoom()
    {

    }

    Image {
        id: ssimg
        height: appH
        width: appW
        source: "image://listprovider/screensaver"+screensaver.timestamp
        anchors.centerIn: parent
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
        id:animation_section2

        running: false
        PropertyAnimation { target: blur; property: "radius"; to: 0.75; duration: 250}
        // PropertyAnimation {target: blur; property: "opacity"; to:0 ; duration: 1000 }
        PropertyAnimation { target: blur; property: "radius"; to: 0.0; duration: screensaver.transitionDuration-10000}
        //  PropertyAnimation {target: blur; property: "opacity"; to:1 ; duration: 1000 }
        PauseAnimation { duration: 5500 }
        PropertyAnimation { target: blur; property: "radius"; to: 0.75; duration: 1000}
        // PropertyAnimation { target: blur; property: "radius"; to: .5; duration: 5000}
    }
}
