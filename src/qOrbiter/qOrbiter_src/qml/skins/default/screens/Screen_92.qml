import QtQuick 2.2
import "../components"
StyledScreen{
    screen:qsTr("MythTV")
    keepHeader: false
    BroadcastMediaBase {
        id:livetv_screen
        //playListComponent: TvChannelView{}
    }
}


