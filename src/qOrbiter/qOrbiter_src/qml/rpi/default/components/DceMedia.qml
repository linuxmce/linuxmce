import QtQuick 2.2
import QtMultimedia 5.0
import AudioVisual 1.0

Item {
    width: 100
    height: 62
    //   property alias videoPlayer:vplayer
    property string currentMediaUrl:dceMediaController.fileUrl

    MediaManager{
        id:dceMediaController
        incomingTime:player.position
         onCurrentStatusChanged:logger.logMediaMessage("Media Player Status::"+dceMediaController.currentStatus)
         onMediaBufferChanged: console.log("media buffer change:"+mediaBuffer)
         onMediaPlayingChanged: console.log("Media Playback status changed locally "+mediaBuffer)
        Component.onCompleted: {
                                   console.log("initializing media player "+manager.mediaPlayerID)
                                   dceMediaController.setConnectionDetails(manager.mediaPlayerID, manager.currentRouter)
                               }

    }



    Connections{
        target:manager
        // onOrientationChanged:dceplayer.setorbiterWindowSize(manager.appHeight, manager.appWidth)
        onMediaPlayerIdChanged:{
            console.log("initializing media player"+manager.mediaPlayerID)
            dceMediaController.setConnectionDetails(manager.mediaPlayerID, manager.currentRouter)
        }
    }

    MediaPlayer{
        id:player
        source: dceMediaController.fileUrl
        onSourceChanged: if(source !==""){play()}
    }

    VideoOutput{
        id:videoPlane
    }

    Audio{
        id:audioPlayer
    }


}
