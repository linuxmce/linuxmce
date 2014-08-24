import QtQuick 2.0
import QtMultimedia 5.0
import AudioVisual 1.0
Item{
    id:mediaPlayerRoot

    MediaManager{
        id:lmceData
        onMediaPlayingChanged: {
            if(!mediaPlaying)
                androidPlayer.stop()
        }


        onAndroidUrlUpdated:{
            console.log("NEW ANDROID URL")
            if(androidUrl.length > 4){
                console.log("URL ok!")
                androidPlayer.source = lmceData.androidUrl
                androidPlayer.play()

            }
            else{
                console.log("Url Malformed!")
                console.log("Url::"+androidUrl)
            }
        }

        //  onFocusChanged: console.log("DCEPlayer Internal focus::"+focus)


        Component.onCompleted: {
            if(manager.mediaPlayerID !== -1){
                lmceData.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
               // androidSystem.startAudioService(lmceData.callbackAddress);
                console.log("initializing qml media player::"+manager.mediaPlayerID)

            }
        }

        Connections{
            target:manager
            onMediaPlayerIdChanged:{
                console.log("initializing qml media player::"+manager.mediaPlayerID)
                lmceData.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)

            }
        }


    }

    MediaPlayer{
        id:androidPlayer

    }

    VideoOutput{
        id:videoPlane
        anchors.fill: parent
        source:androidPlayer.hasVideo ? androidPlayer : null
        visible: source != null ? true : false
    }

}
