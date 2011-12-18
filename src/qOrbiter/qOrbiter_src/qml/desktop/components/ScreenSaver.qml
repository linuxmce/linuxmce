import QtQuick 1.0

Rectangle {
    width: style.orbiterH
    height: style.orbiterW
    color: "transparent"

    function changeStuff()
    {
        flash.running = true
        if (flash.complete)
        {
        ssimg.source = "image://listprovider/screensaver/"+securityvideo.timestamp
        }
    }

    Image {
        id: ssimg
        height: style.height
        width: style.width
        source: "image://listprovider/screensaver"+screensaver.timestamp
        anchors.centerIn: parent
        smooth: true


    }

    Connections{
        target: screensaver
        onImageChanged:changeStuff()

    }

    Rectangle{
        id:flashbox
        height: style.orbiterH
        width: style.orbiterW
        color: "black"

        SequentialAnimation on color {

            id:flash
            running: true
            loops: Animation.complete
            // PauseAnimation { duration: 900 }

            PropertyAnimation{ target:flashbox; property: "color"; to: "black"; duration: 1000}
            PropertyAnimation{ target:flashbox; property: "color"; to: "transparent"; duration: 2000}
        }
    }
}
