import QtQuick 1.0

Rectangle {
    width: appW
    height: appH
    color: "darkgrey"
    anchors.centerIn: parent
    function changeStuff()
    {
        lower.running =true
    }

    function changePic()
    {
        ssimg.scale=1
        ssimg.source = "image://listprovider/screensaver/"+securityvideo.timestamp
        raise.running = true
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
            loops: 1
            PropertyAnimation{ target:ssimg; property: "opacity"; to: "1"; duration: 2000}
            PropertyAnimation{ target:ssimg; property: "scale"; to:1.25; duration: 5000}

        }

        SequentialAnimation on opacity {

            id:lower
            running:false
            loops: 1
            PropertyAnimation{ target:ssimg; property: "opacity"; to: "0"; duration: 2000}
            ScriptAction {script: changePic()}
        }

        Connections{
            target: screensaver
            onImageChanged:changeStuff()

        }
    }
}
