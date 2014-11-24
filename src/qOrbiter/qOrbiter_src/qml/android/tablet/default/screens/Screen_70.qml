import QtQuick 1.1
import "../components"
/*!
  This is screen 70, the stored video file remote screen.
  It has
  *Playlist
    -Playlist Manipulation controls.
  *Media Playback controls
  *Media Image
  *Lighting Control
  *Audio Control
  *Advanced Device Controls
  */
StyledScreen{
    onScreenOpening: {
       // screen_70.playlistSource=manager.getDataGridModel("Playlist", 18)
    }

    keepHeader: false
    MediaPlaybackBase{
        id:screen_70
        headerTitle: manager.translateMediaType(manager.i_current_mediaType)
    }
}


