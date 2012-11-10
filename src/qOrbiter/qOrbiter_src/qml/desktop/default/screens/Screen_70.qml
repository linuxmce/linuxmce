import QtQuick 1.0
import Qt.labs.shaders 1.0


import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../effects"

Rectangle {

    // property alias synText:
    id: storedvideoremote
    height: appH
    width: appW
    radius: 0
    opacity: 1
    color: "transparent"
    z:5

    MediaDetailHeader {
        id: gradientheader
    }
    Column{
        id:envControls
        anchors.left: parent.left
        anchors.leftMargin: scaleX(1)
        anchors.top: gradientheader.bottom
        Remote_lighting_controls{ id: remote_lighting_controls1; }
        Remote_Audio_controls{ id: remote1; }
    }

    NonEPGPlaylist{
        id:playlist
        anchors.left: envControls.right
        anchors.rightMargin: scaleX(1)
        anchors.top: gradientheader.bottom
    }
    Image {
        id: nowplayingimage
        anchors.top:gradientheader.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: scaleY(.5)
        width: dcenowplaying.aspect=="wide"? scaleX(30) : scaleX(20)
        height:dcenowplaying.aspect=="wide"? scaleY(30) : scaleY(50)
        source: ""
        Component.onCompleted: manager.setBoundStatus(true)
        Timer{
            interval: 500
            running: true
            onTriggered: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp;
        }
    }

    StoredVideoMetadata {
        id: textrect
        anchors.right: parent.right
        anchors.top: gradientheader.bottom
    }



    MediaScrollBar{id:media_transit; anchors.bottom: controlrow.top; anchors.horizontalCenter: controlrow.horizontalCenter}

    StoredVideoControls {
        id: controlrow

    }

}


