import QtQuick 2.2
import "../components"
/*!
  This is a base screen 117, the VDR remote screen.
  atm it is a copy of screen 70, to make sure that 
  something happens when we hit TV on VDR based systems.
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
    keepHeader: false
    BroadcastMediaBase{
        id:screen_117
        headerTitle: manager.translateMediaType(manager.i_current_mediaType)
    }
}


