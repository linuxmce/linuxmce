import QtQuick 2.2
import "../components"
StyledScreen{
    screen:qsTr("Live TV")
    MediaPlaybackBase {
        id:livetv_screen
        playListComponent: TvChannelView{}
    }
}


