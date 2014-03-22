import QtQuick 2.0
import AudioVisual 1.0
import QtMultimedia 5.0


Item {
    anchors.fill: parent
    //   property alias videoPlayer:vplayer
    property string currentMediaUrl:dceMediaController.fileUrl


//    Keys.onPressed:{

//        if(!activeFocus)
//            return

//        switch(event.key){
//        case Qt.Key_Back:
//            manager.changedPlaylistPosition((mediaplaylist.currentIndex-++1));
//            break;
//        case Qt.Key_Forward:
//            manager.changedPlaylistPosition((mediaplaylist.currentIndex+1))
//            break;
//        case 16777347: /* Keycode Track forward */
//            manager.changedPlaylistPosition((mediaplaylist.currentIndex+1));
//            break;
//        case 16777346: /* Keycode Track Backwards */
//            manager.changedPlaylistPosition((mediaplaylist.currentIndex-1))
//            break;
//        case Qt.Key_Plus: /*Plus sign */
//            manager.adjustVolume(+1)
//            break;

//        case Qt.Key_VolumeMute:
//            manager.mute()
//            break;

//        case Qt.Key_M:
//            manager.mute()
//            break;

//        case Qt.Key_Minus: /* Minus Sign */
//            manager.adjustVolume(-1)
//            break;
//        case Qt.Key_T:
//            if(playlist.state==="showing")
//                playlist.state="hidden"
//            else
//                playlist.state = "showing"

//            break;

//        case Qt.Key_S:
//            manager.stopMedia()
//            break;

//        case Qt.Key_Pause:
//            manager.pauseMedia()
//            break;
//        case Qt.Key_P:
//            manager.pauseMedia()
//            break;

//        case Qt.Key_PageUp:
//            manager.changedPlaylistPosition(mediaplaylist.currentIndex-1)
//            break;

//        case Qt.Key_PageDown:
//            manager.changedPlaylistPosition(mediaplaylist.currentIndex+1)
//            break;
//        default:
//            console.log(event.key)
//            break
//        }

//    }

    MediaManager{
        id:dceMediaController
        incomingTime:player.position
        onCurrentStatusChanged:logger.logMediaMessage("Media Player Status::"+dceMediaController.currentStatus)
        onMediaBufferChanged: console.log("media buffer change:"+mediaBuffer)
        onMediaPlayingChanged: console.log("Media Playback status changed locally "+mediaBuffer)
        Component.onCompleted: {
            console.log("initializing media player "+manager.mediaPlayerID)
            dceMediaController.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
        }
        onFileUrlChanged: {
            player.source=Qt.resolvedUrl(dceMediaController.fileUrl)
        }
    }

    Connections{
        target:manager
        // onOrientationChanged:dceplayer.setWindowSize(manager.appHeight, manager.appWidth)
        onMediaPlayerIdChanged:{
            console.log("initializing media player"+manager.mediaPlayerID)
            dceMediaController.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
        }
    }

    MediaPlayer{
        id:player
        source: "http://us.ah.fm:9000" //dceMediaController.fileUrl
        onSourceChanged:{
            if(source !==""){play()};
            console.log("New media player source::"+dceMediaController.fileUrl)
        }
        onError: console.log("Error::"+error+" ==>"+errorString)
        onErrorStringChanged: console.log(errorString)

    }

        VideoOutput{
            id:videoPlane
            anchors.fill: parent
            source:player
        }

    //    Audio{
    //        id:audioPlayer
    //    }


}
