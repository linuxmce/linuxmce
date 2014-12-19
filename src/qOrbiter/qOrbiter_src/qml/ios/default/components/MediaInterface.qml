import QtQuick 2.0
import QtMultimedia 5.0
import AudioVisual 1.0
Item{
    id:mediaPlayerRoot
    height: manager.appHeight
    width: manager.appWidth

    property bool dceConnected:manager.connectedState
    property bool mediaPlayerConnected:lmceData.connected
    property int mediaPlayerId:manager.mediaPlayerID
    property bool active:lmceData.mediaPlaying

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
        onAndroidUrlUpdated:{
            console.log("NEW ANDROID URL")
            if(androidUrl.length > 4){
                console.log("URL ok!")
                qmlPlayer.source = lmceData.androidUrl
            }
            else{
                console.log("Url Malformed!")
                console.log("Url::"+androidUrl)
            }
        }

        onAndroidVolumeUp:{
            console.log("dceplayer::vol up")
            qmlPlayer.volume= qmlPlayer.volume+1
        }

        onAndroidVolumeDown:{
            console.log("dceplayer::vol down")
            qmlPlayer.volume= qmlPlayer.volume-1
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
                    lmceData.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
                    dceplayer.reInit();
                }
            }
        }
    }

    MediaPlayer{
        id:qmlPlayer
        autoPlay: true
        autoLoad: true

        onStopped: {
            if(duration===position){
                lmceData.qmlPlaybackEnded(true)
            }
        }

        onDurationChanged: {
            if(!duration)
                return;

            lmceData.setQmlTotalTime(duration)
            //   lmceData.setTotalTime(duration)
        }
        onErrorChanged: {
            console.log(error)
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
        height: manager.appHeight
        width: manager.appWidth
        source:qmlPlayer
    }

}
