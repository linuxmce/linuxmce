import QtQuick 2.0
import QtMultimedia 5.0
import AudioVisual 1.0
Item{
    id:mediaPlayerRoot

    property bool dceConnected:manager.connectedState
    property bool mediaPlayerConnected:lmceData.connected
    property int mediaPlayerId:manager.mediaPlayerID

    onDceConnectedChanged: {
        if(!dceConnected && manager.mediaPlayerID!==-1){
            lmceData.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
        }
    }



    Timer{
        id:reconnect
        running:!dceConnected && manager.connectedState
        repeat: true
        interval: 10000
        onTriggered: {
            console.log("Connecting media player "+lmceId+" at "+manager.m_ipAddress)
            setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
        }
    }


    function restart(){
         lmceData.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
    }

    MediaManager{
        id:lmceData
        onMediaPlayingChanged: {
            if(!mediaPlaying && androidPlayer.source!=="")
                androidPlayer.stop()
        }

        /* TODO Add reconnect handler in cpp and here */
        /* TODO Change android url to streaming url, generic for other devices. */
        onAndroidUrlUpdated:{
            console.log("NEW ANDROID URL")
            if(androidUrl.length > 4){
                console.log("URL ok!")
                androidPlayer.source = lmceData.androidUrl
            }
            else{
                console.log("Url Malformed!")
                console.log("Url::"+androidUrl)
            }
        }

        //  onFocusChanged: console.log("DCEPlayer Internal focus::"+focus)


        Component.onCompleted: {
            if(manager.mediaPlayerID!==-1){
                lmceData.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
                // androidSystem.startAudioService(lmceData.callbackAddress);
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
                }
    }

    MediaPlayer{
        id:androidPlayer
        autoPlay: true
        autoLoad: true
        onDurationChanged: {
            lmceData.setTotalTime(duration)
        }
        onErrorChanged: {

        }

        onPositionChanged: {
            //lmceData.processTimeCode(position); console.log(position)
        }

    }

    VideoOutput{
        id:videoPlane
        anchors.fill: parent
        source:androidPlayer.hasVideo ? androidPlayer : null
        visible: source != null ? true : false
    }

}
