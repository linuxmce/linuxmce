import QtQuick 1.1
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

    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity: parent.activeFocus ? .65 : .25
    }


    MediaPlaybackTemplate{
        id:playback
        focus:true
    }
}
