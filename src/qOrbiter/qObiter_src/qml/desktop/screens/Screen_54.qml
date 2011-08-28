import QtQuick 1.0
import "../components"

Rectangle {

    // property alias synText:
    id: pvrRemote


    height: style.orbiterH
    width: style.orbiterW
    radius: 0
    opacity: 1
    color: style.advanced_bg

    //main 'now playing rect containing all the other items
    Remote_lighting_controls{ id: remote_lighting_controls1; x: 360; y: 352; width: 93; height: 219 }
    Remote_Audio_controls{ id: remote1; x: 272; y: 361; anchors.rightMargin: 12; z: 45; anchors.right: remote_lighting_controls1.left}

    HomeButton{anchors.right: parent.right; anchors.top:parent.top}

    VideoControls {
        id: videocontrols1
        x: 555
        y: 351
        width: 263
        height: 249
    }

    ListView{
        height:500
        width: 200

        clip: true
        interactive: true
        flickableDirection: "VerticalFlick"
        model: dcenowplaying.currentPlaylist

        delegate:
            Rectangle {
            width:200
            height: 50
            color: "whitesmoke"
            Text {
                text: title
            }
        }
    }




    NowPlayingBox {
        id: nowplayingbox1
        x: 278
        y: -11
    }
}
