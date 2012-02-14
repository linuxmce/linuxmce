import QtQuick 1.0
import "../components"

Rectangle {

    // property alias synText:
    id: storedVideoRemote
    height: appH
    width: appW
    Timer{
        id:singleshot
        repeat: false
        interval: 2000
        triggeredOnStart: false
        running: true

        onTriggered: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
    }

    Connections{
        target:dcenowplaying
        onPlayListPositionChanged: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
        onImageChanged:nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
    }
    Component.onCompleted:setNowPlayingData()

    NowPlayingBox
    {
        id:np_box
        x: 74
        y: 11
    }


    Image{
        id:nowplayingimage
fillMode: Image.PreserveAspectCrop
source:"image://listprovider/updateobject/"+securityvideo.timestamp
        anchors.fill: np_box
    }

    AudioRemote{id:controls;anchors.bottom: parent.bottom; anchors.horizontalCenter: parent.horizontalCenter}
    color: "transparent"

    //main 'now playing rect containing all the other items
    // Remote_lighting_controls{ id: remote_lighting_controls1; x: 344; y: 351; width: 93; height: 219; anchors.topMargin: 179;anchors.top: nowplayingbox1.baseline}
    // Remote_Audio_controls{ id: remote1; x: 200; y: 351; anchors.rightMargin: 71; z: 45; anchors.right: remote_lighting_controls1.left}
    HaControls{ x: 0; y: 5;}
    HomeButton{ id:home; anchors.left: storedAudioRemote.left; anchors.top:storedAudioRemote.top}


}

