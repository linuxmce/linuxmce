import QtQuick 2.0
import "../components"


Rectangle {
    id:stage

    signal swapStyle()
    height: manager.appHeight
    width: manager.appWidth
    color: "transparent"
    Component.onCompleted: if(dcenowplaying.b_mediaPlaying) {screenChange(dcenowplaying.qs_screen) }
    //focus:true
}



