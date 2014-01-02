/*!
 *\brief LABEL ME
 *
 *\ingroup qml_desktop_default
 *\bug needs better description
 *
 */

import QtQuick 1.1

import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../effects"

Rectangle {

    // property alias synText:
    id: storedvideoremote
    height: manager.appHeight
    width: manager.appWidth
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
        Component.onCompleted: {
            manager.setBoundStatus(true)
        }

        Connections{
            target: dcenowplaying
            onImageChanged: updateTimer.start()// "image://listprovider/updateobject/"+securityvideo.timestamp;
        }

        Timer{
            id:updateTimer
            interval: 100
            running: true
            onTriggered: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp;
        }
    }

    StoredVideoMetadata {
        id: textrect
        anchors.right: parent.right
        anchors.top: gradientheader.bottom
    }

    Image {
        id: transitPreview
        source: ""
        width: dcenowplaying.aspect=="wide"? scaleX(10) : scaleX(7)
        height:dcenowplaying.aspect=="wide"? scaleY(10) : scaleY(17)
        anchors.bottom: media_transit.top
        anchors.horizontalCenter: parent.horizontalCenter
        visible: dceTimecode.playbackSpeed !=1000 ? true : false
        Connections{
            target: manager
            onMediaScreenShotReady: transitPreview.source="image://listprovider/screenshot/"+securityvideo.timestamp
        }
        Timer{
            id:previewTimer
            interval: 1000
            repeat: true
            running: dceTimecode.playbackSpeed !=1000 ? true : false
            onTriggered: manager.getVideoFrame()
        }

    }


   // MediaScrollBar{id:media_transit; anchors.bottom: controlrow.top; anchors.horizontalCenter: controlrow.horizontalCenter}

    StoredVideoControls {
        id: controlrow
    }

}


