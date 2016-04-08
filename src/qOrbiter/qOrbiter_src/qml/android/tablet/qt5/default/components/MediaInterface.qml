import QtQuick 2.2
import QtMultimedia 5.0
import AudioVisual 1.0
Item{
    id:mediaPlayerRoot
    anchors{
        left:qml_root.left
        top:nav_row.bottom
        right:parent.right
        bottom: info_panel.top
    }

    property bool dceConnected:manager.connectedState
    property bool mediaPlayerConnected:lmceData.connected
    property int mediaPlayerId:manager.mediaPlayerID
    property bool active:lmceData.mediaPlaying


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
                lmceData.setConnectionDetails(manager.mediaPlayerID, manager.currentRouter)
                console.log("onCompleted::initializing qml media player::"+manager.mediaPlayerID+"::"+manager.currentRouter)
            }
        }

        Connections{
            target:manager
            onMediaPlayerIdChanged:{
                if(manager.mediaPlayerID!==-1){
                    console.log("onIdChanged::initializing qml media player::"+manager.mediaPlayerID)
                    lmceData.setConnectionDetails(manager.mediaPlayerID, manager.currentRouter)
                }
            }
            onConnectedStateChanged:{
                if(manager.connectedState ){
                    //lmceData.setConnectionDetails(manager.mediaPlayerID, manager.currentRouter)
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
