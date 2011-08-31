import QtQuick 1.0
import "../components"

Rectangle {

    // property alias synText:
    id: storedmediaremote

    width: style.orbiterW
    height: style.orbiterH
    radius: 0
    opacity: 1
    color: "transparent"

    Rectangle
    {
        color: "transparent"
        anchors.centerIn: parent
        height: parent.height
        width: parent.width *.50
        //main 'now playing rect containing all the other items
        NowPlayingBox {
            id: nowplayingbox1
            x: 278
            y: 0
        }

        Remote_lighting_controls{ id: remote_lighting_controls1; x: 339; y: 350; width: 93; height: 219; anchors.topMargin: 350;anchors.top: nowplayingbox1.baseline}
        Remote_Audio_controls{ id: remote1; x: 288; y: 350; width: 60; height: 219; anchors.rightMargin: 0; z: 45; anchors.right: remote_lighting_controls1.left}
        HomeButton{anchors.right: parent.right; anchors.top: parent.top}

        VideoControls {
            id: videocontrols1
            x: 628
            y: 350
            width: 263
            height: 249
        }

        NonEPGPlaylist{}
    }


}
