import QtQuick 1.1
import "../components"
StyledScreen{
    screen:qsTr("Live TV")
    MediaPlaybackBase {
        id:livetv_screen
        playListComponent: TvChannelView{}
    }
}


