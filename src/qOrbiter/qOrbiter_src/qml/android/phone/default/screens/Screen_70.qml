import QtQuick 1.0
import "../components"

Rectangle {

    // property alias synText:
    id: storedVideoRemote
    height: 480
    width: 320

    NowPlayingBox
    {
        id:np_box
        x: 74
        y: 11
    }

    AudioRemote{ x: 25; y: 393; anchors.verticalCenterOffset: 193; anchors.horizontalCenterOffset: 5;anchors.centerIn: parent}
    color: "transparent"

    //main 'now playing rect containing all the other items
    // Remote_lighting_controls{ id: remote_lighting_controls1; x: 344; y: 351; width: 93; height: 219; anchors.topMargin: 179;anchors.top: nowplayingbox1.baseline}
    // Remote_Audio_controls{ id: remote1; x: 200; y: 351; anchors.rightMargin: 71; z: 45; anchors.right: remote_lighting_controls1.left}
    HaControls{ x: 0; y: 5;}
    HomeButton{ x: 275; y: 160; anchors.leftMargin: 275; anchors.topMargin: 160;anchors.left: storedAudioRemote.left; anchors.top:parent.top}


}

