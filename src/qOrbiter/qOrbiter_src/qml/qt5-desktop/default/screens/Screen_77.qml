import QtQuick 2.1
import "../components"
Item {
    id:screen_fiddy_fo
    height: scaleY(100)
    width: scaleX(100)
    Component.onCompleted:setNowPlayingTv()
    onActiveFocusChanged: {
        if(activeFocus){
            console.log("Screen 63 has focus, transferring it to template.")
            playback.forceActiveFocus()

        }
    }

    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity: parent.activeFocus ? .65 : .25
    }


    MediaPlaybackTemplate{
        id:playback
        focus:true
        onActiveFocusChanged: { manager.getLiveTVPlaylist();console.log("Getting tv playlist") }
    }
}
