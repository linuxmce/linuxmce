import QtQuick 1.0


ButtonSq {
    id: now_playing
    anchors.top: parent.top
    anchors.topMargin: scaleY(2)
    visible: dcenowplaying.b_mediaPlaying ? true : false
    height: style.stdbuttonh
    width: style.stdbuttonw
    radius: style.but_smooth
    buttontext: dcenowplaying.qs_mainTitle +" \n " + dceTimecode.qsCurrentTime
    
    MouseArea{
        anchors.fill: parent
        onClicked:manager.gotoQScreen(dcenowplaying.qs_screen)
    }
}
