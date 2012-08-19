// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import AudioVisual 1.0

Rectangle {
    id:videoPlayerContainer
    width: requestedWidth
    height: requestedHeight
    property alias player:mediaPlayer
    property int scaling:1
    property int requestedHeight:320
    property int  requestedWidth:480
    property int mediaType:dcerouter.i_current_mediaType
    clip: true
    function startPlayback(path){
        mediaPlayer.setSource(path)
    }

    VideoPlayer{
        id:mediaPlayer
        height: requestedHeight
        width: requestedWidth
        scale:scaling
        playerType:videoPlayerContainer.mediaType
        //  mediaType: 5
        onMediaSourceChanged: play()
    }
}
