import QtQuick 1.1
import "../components"
StyledScreen{

    MediaPlaybackBase {
        id:livetv_screen
        playListComponent: TvChannelView{}
    }
}


