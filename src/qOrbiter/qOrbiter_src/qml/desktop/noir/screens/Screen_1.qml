import QtQuick 1.1
import "../components"


Rectangle {
    id:stage

    signal swapStyle()
    height: manager.appHeight
    width: manager.appWidth
    color: "transparent"

    Timer{
        id:bind_timer
        running: false
        interval: 1000
        onTriggered: dcenowplaying.b_mediaPlaying ? manager.setBoundStatus(true) : manager.setBoundStatus(false)
    }


    Rectangle{
        visible: dcenowplaying.b_mediaPlaying ? true : false
        id:nowPlayingBox
        width: scaleX(85)
        height: scaleY(75)
        color: "transparent"
        anchors.centerIn: parent

        NowPlayingImage {
            Component.onCompleted: bind_timer.start()
        }
    }

    //focus:true



}



