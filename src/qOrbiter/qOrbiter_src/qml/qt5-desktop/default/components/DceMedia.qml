import QtQuick 2.0
import AudioVisual 1.0
import QtMultimedia 5.0


Item {
    anchors.fill: parent
    //   property alias videoPlayer:vplayer
    property string currentMediaUrl:dceMediaController.fileUrl
    property bool mediaPlaying:player.playbackState === MediaPlayer.PlayingState
    property string currentStatus:""
    property double volume:0.0
    property double mediaBuffer:0.0
    property bool online:dceMediaController.connected
    property int lmceId:manager.mediaPlayerID

    onOnlineChanged: {
        if(!online && lmceId !==-1){
            setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
        }
    }

    Timer{
        id:reconnect
        running:!online
        repeat: true
        interval: 10000
        onTriggered: {
            console.log("Connecting media player "+lmceId+" at "+manager.m_ipAddress)
            setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
        }
    }


    Connections{
        target: manager
        onMediaPlayerIdChanged:{
            setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
        }
    }

    function setConnectionDetails(deviceID, router){
        dceMediaController.setConnectionDetails(deviceID, router)
    }

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
        source: dceMediaController.fileUrl
        onSourceChanged:{
            console.log("!!!!!!!!!!!!!!!!!!!!!!!!!")
            if(source !==""){play()};
            console.log("New media player source::"+dceMediaController.fileUrl)
        }
        onError: console.log("Error::"+error+" ==>"+errorString)
        onErrorStringChanged: console.log(errorString)
        onPlaybackStateChanged: console.log("New Playback state::"+playbackState)

    }

    VideoOutput{
        id:videoPlane
        anchors.fill: parent
        source:player
    }

    //        Audio{
    //            id:audioPlayer
    //        }


}
