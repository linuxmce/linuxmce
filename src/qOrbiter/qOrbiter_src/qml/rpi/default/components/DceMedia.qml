import QtQuick 2.0
import QtMultimedia 5.0
import AudioVisual 1.0

Item {
    width: 100
    height: 62
    //   property alias videoPlayer:vplayer

    MediaManager{
        id:dceMediaController

    }
    Connections{
        target:manager
        onOrientationChanged:dceplayer.setWindowSize(manager.appHeight, manager.appWidth)
        onMediaPlayerIdChanged:{
            console.log("initializing media player"+manager.mediaPlayerID)
            dceplayer.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
        }
    }

       VideoOutput{
        id:videoPlane
       }

       Audio{
           id:audioPlayer
       }


}
