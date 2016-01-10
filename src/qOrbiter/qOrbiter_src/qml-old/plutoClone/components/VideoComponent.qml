// import QtQuick 2.0 // to target S60 5th Edition or Maemo 5
import QtQuick 2.0
//import AudioVisual 1.0
Rectangle {
    id:videoPlayerContainer
    width: requestedWidth
    height: requestedHeight
    property alias player:vplayer
    property int scaling:1
    property int requestedHeight:320
    property int  requestedWidth:480
    property int mediaType:manager.i_current_mediaType
 clip: true

    function startPlayback(path){
        vplayer.setSource(path)
    }

//    VideoPlayer{
//        id:vplayer
//        height: requestedHeight
//        width: requestedWidth
//        scale:scaling
//        playerType:videoPlayerContainer.mediaType
//        //  mediaType: 5
//        onMediaSourceChanged: {playItem(mediaSource)
//           console.log(vplayer.mediaSource)
//       }

//    }
}
