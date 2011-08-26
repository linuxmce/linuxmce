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
    Remote_lighting_controls{ id: remote_lighting_controls1; x: 344; y: 351; width: 93; height: 219; anchors.topMargin: 179;anchors.top: nowplayingbox1.baseline}
    Remote_Audio_controls{ id: remote1; x: 200; y: 351; anchors.rightMargin: 71; z: 45; anchors.right: remote_lighting_controls1.left}

    HomeButton{anchors.right: parent.right; anchors.top:parent.top}

    VideoControls {
        id: videocontrols1
        x: 555
        y: 351
        width: 263
        height: 249
    }

    Column {
        id: channelgrid

        width: 200
        height: 500
        clip: true

        Flickable{
            height: parent.height
            width: parent.width
            contentHeight: childrenRect.width
            contentWidth: childrenRect.width
            clip: true

            Repeater { model: 50
            }
}
    }

    Rectangle {
        width:200
        height: 50
        color: "whitesmoke"
        Text {
            text: "I am DG item ,Sroll me!"
        }
    }

    NowPlayingBox {
        id: nowplayingbox1
        x: 278
        y: -11
    }
}
