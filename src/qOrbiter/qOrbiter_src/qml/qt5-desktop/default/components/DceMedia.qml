import QtQuick 2.2
import AudioVisual 1.0
import QtMultimedia 5.0


Item {
    anchors.fill: parent
    //   property alias videoPlayer:vplayer
    focus:true
    property string currentMediaUrl:qmlPlayer.fileUrl
    property bool mediaPlaying:qmlPlayer.playbackState === MediaPlayer.PlayingState
    property string currentStatus:""
    property double volume:0.0
    property double mediaBuffer:0.0
    property bool online:lmceData.connected
    property int lmceId:manager.mediaPlayerID
    property bool dceConnected:manager.connectedState
    property bool mediaPlayerConnected:lmceData.connected
    property int mediaPlayerId:manager.mediaPlayerID
    property bool active:lmceData.mediaPlaying


    function setConnectionDetails(deviceID, router){
        lmceData.setConnectionDetails(deviceID, router)
    }

    onCurrentStatusChanged:logger.logMediaMessage("Media Player Status::"+dceplayer.currentStatus)
    onMediaBufferChanged: console.log("media buffer change:"+mediaBuffer)
    onMediaPlayingChanged: console.log("Media Playback status changed locally to "+dceplayer.mediaPlaying)
    onVolumeChanged:console.log(volume)

//    Keys.onVolumeDownPressed: manager.adjustVolume("-1")
//    Keys.onVolumeUpPressed:  manager.adjustVolume("+1")
//    Keys.onTabPressed: ftr.forceActiveFocus()

    Keys.onPressed: {
        hideUiTimer.restart()
        switch(event.key){
        case Qt.Key_Back:
            manager.changedPlaylistPosition((mediaplaylist.currentIndex+1));
            break;
        case Qt.Key_Forward:
            manager.changedPlaylistPosition((mediaplaylist.currentIndex+1))
            break;
        case 16777347: /* Keycode Track forward */
            manager.changedPlaylistPosition((mediaplaylist.currentIndex+1));
            break;
        case 16777346: /* Keycode Track Backwards */
            manager.changedPlaylistPosition((mediaplaylist.currentIndex-1))
            break;
        case Qt.Key_Plus: /*Plus sign */
            manager.adjustVolume(+1)
            break;
        case Qt.Key_VolumeMute:
            manager.mute()
            break;
        case Qt.Key_M:
            manager.mute()
            break;
        case Qt.Key_Minus: /* Minus Sign */
            manager.adjustVolume(-1)
            break;
        case Qt.Key_T:
            showMetaData()
            break;
        case Qt.Key_Tab:
            swapFocus()
            break;
        case Qt.Key_S:
            manager.stopMedia()
            break;
        case Qt.Key_Menu:
            manager.setCurrentScreen("Screen_1.qml")
            break;
        case Qt.Key_Pause:
            manager.pauseMedia()
            break;
        case Qt.Key_P:
            manager.pauseMedia()
            break;

        case Qt.Key_PageUp:
            manager.changedPlaylistPosition(mediaplaylist.currentIndex-1)
            break;

        case Qt.Key_PageDown:
            manager.changedPlaylistPosition(mediaplaylist.currentIndex+1)
            break;
        default:
            console.log(manager.dumpKey(event.key))
            break
        }

        event.accepted=true;
    }

    function restart(){
        lmceData.reInit();
    }

    MediaManager{
        id:lmceData
        onMediaPlayingChanged: {
            if(!mediaPlaying && qmlPlayer.source!==""){
                qmlPlayer.stop()
                videoPlane.visible = false
            } else {

            }

        }

        /* TODO Add reconnect handler in cpp and here */
        /* TODO Change android url to streaming url, generic for other devices. */
        onPluginUrlUpdated:{
            console.log("New QML media Url ==>"+pluginUrl)
            if(pluginUrl.length > 4){
                console.log("URL ok!")
                qmlPlayer.source = lmceData.pluginUrl
            }
            else{
                console.log("Url Malformed!")
                console.log("Url::"+pluginUrl)

            }
        }

        onMutedChanged: {
            if(qmlPlayer.muted)
                qmlPlayer.muted=false
            else
                qmlPlayer.muted=true
        }

        onPluginVolumeUp:{
            console.log("dceplayer::vol up")
            if(qmlPlayer.volume < 1.0){
                qmlPlayer.volume+.10
            }


        }

        onPluginVolumeDown:{
            console.log("dceplayer::vol down")
            if(qmlPlayer.volume > 0.0){
                qmlPlayer.volume-.10
            }
        }

        onPauseMedia:{
            console.log("dceplayer::pause")
            if(qmlPlayer.playbackState == MediaPlayer.PausedState)
                qmlPlayer.play()
            else
                qmlPlayer.pause()
        }

        onUpdatePluginSeek:{
            console.log("dceplayer::seek to"+pos)
            qmlPlayer.seek(pos*1000);
        }

        Component.onCompleted: {
            if(manager.mediaPlayerID!==-1){
                lmceData.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
                console.log("onCompleted::initializing qml media player::"+manager.mediaPlayerID+"::"+manager.m_ipAddress)
            }
        }

        Connections{
            target:manager
            onMediaPlayerIdChanged:{
                if(manager.mediaPlayerID!==-1){
                    console.log("onIdChanged::initializing qml media player::"+manager.mediaPlayerID)
                    lmceData.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
                }
            }
            onConnectedStateChanged:{
                if(manager.connectedState ){
                    //lmceData.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
                    dcePlayer.reInit();
                }
            }
        }
    }

    MediaPlayer{
        id:qmlPlayer
        autoPlay: true
        autoLoad: true

        onStopped: {
            if(duration==position){

                lmceData.qmlPlaybackEnded(false)
            }
        }

        onMediaObjectChanged: {
            console.log(JSON.stringify(metaData, null, "\t"))

        }


        onDurationChanged: {
            if(!duration)
                return;

            console.log("Track duration "+duration)
            lmceData.setQmlTotalTime(Number(duration))
            //   lmceData.setTotalTime(duration)
        }
        onErrorChanged: {
            console.log("QML Media player error::"+error)
        }

        onPositionChanged: {
            if(!position)
                return;

            lmceData.processTimeCode(position);
        }

        onHasVideoChanged: {
            if(hasVideo){
                videoPlane.visible=true
            } else {
                videoPlane.visible=false
            }
        }

    }

    VideoOutput{
        id:videoPlane
        anchors.fill: parent
        source:qmlPlayer
    }

}
