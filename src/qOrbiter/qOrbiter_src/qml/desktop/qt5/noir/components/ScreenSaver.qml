import QtQuick 2.0
//import Qt.labs.shaders 1.0
import QtGraphicalEffects 1.0

Rectangle {
    id:ssholder
    width: manager.appWidth
    height: manager.appHeight
    color: "black"

    function changeStuff()
    {
        console.log("Pic Change")
        lower.running =true
        animation_section2.running = false
    }

    function changePic()
    {
        blur.source = undefined
         ssimg.source = "image://listprovider/screensaver/"+securityvideo.timestamp
        console.log(ssimg.sourceSize)
         blur.source = ssimg
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
        source: "image://listprovider/screensaver/"+securityvideo.timestamp
        anchors.centerIn: parent
        smooth: true
        visible: false

        ParallelAnimation {
            id:raise
            running: false
           // PropertyAnimation{ target:ssimg; property: "opacity"; to: "1"; duration: 1500}
            PropertyAnimation{ target:blur; property: "opacity"; to: "1"; duration: 1500}
        }

        SequentialAnimation on opacity {
            id:lower
            running:false
            ParallelAnimation{
               PropertyAnimation{ target:blur; property: "opacity"; to: "0"; duration: 1000}
              //  PropertyAnimation{ target:ssimg; property: "opacity"; to: "0"; duration: 1000}
            }


            ScriptAction {script: changePic()}
        }

        Connections{
            target: screensaver
            onImageChanged:changeStuff()
        }
    }

    GaussianBlur{
        id:blur
        source: ssimg
        anchors.fill: ssimg
        radius: 5
        samples: 10
        deviation: 5
        cached: false

    }

    SequentialAnimation{
        id:animation_section2

        running: false
        PropertyAnimation { target: blur; property: "radius"; to: 10; duration: 250}
        // PropertyAnimation {target: blur; property: "opacity"; to:0 ; duration: 1000 }
        PropertyAnimation { target: blur; property: "radius"; to: 0.0; duration: 2000}
        //  PropertyAnimation {target: blur; property: "opacity"; to:1 ; duration: 1000 }
        // PropertyAnimation { target: blur; property: "radius"; to: .5; duration: 5000}
    }
}
