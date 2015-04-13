import QtQuick 2.2
import "../components"

Item {
    id:screen_fiddy_fo
    height: scaleY(100)
    width: scaleX(100)
    onActiveFocusChanged: {
        if(activeFocus){
            console.log("Screen" + parent.id+", transferring it to template.")
            playback.forceActiveFocus()
        }
    }




    MediaPlaybackTemplate{
        id:playback
        focus:true
    }
}
