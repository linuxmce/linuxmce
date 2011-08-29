import QtQuick 1.0
import "../components"

Rectangle {

    // property alias synText:
    id: pvrRemote


    width: 800
    height: 600
    radius: 0
    opacity: 1
    color: style.advanced_bg

    //main 'now playing rect containing all the other items
    NowPlayingBox {
        id: nowplayingbox1
        x: 278
        y: 0
    }
    Remote_lighting_controls{ id: remote_lighting_controls1; x: 348; y: 350; width: 93; height: 219; anchors.topMargin: 179;anchors.top: video_now_playing.baseline}
    Remote_Audio_controls{ id: remote1; x: 288; y: 350; width: 60; height: 219; anchors.rightMargin: 0; z: 45; anchors.right: remote_lighting_controls1.left}

    HomeButton{anchors.right: parent.right; anchors.top: parent.top}

    VideoControls {
        id: videocontrols1
        x: 537
        y: 231
        width: 263
        height: 249
    }

    NonEPGPlaylist{}
}
